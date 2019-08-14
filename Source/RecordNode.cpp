#include "RecordNode.h"

using namespace std::chrono;

RecordNode::RecordNode() 
	: GenericProcessor("Record Node"),
	newDirectoryNeeded(true),
	timestamp(0),
	setFirstBlock(false),
	numChannels(0),
	numSamples(0),
	experimentNumber(0),
	recordingNumber(2),
	isRecording(false),
	hasRecorded(false),
	settingsNeeded(false),
	scaleTime(0),
	convertTime(0),
	writeTime(0)
{
	setProcessorType(PROCESSOR_TYPE_FILTER);

	dataQueue = new DataQueue(WRITE_BLOCK_LENGTH, DATA_BUFFER_NBLOCKS);
	eventQueue = new EventMsgQueue(EVENT_BUFFER_NEVENTS);
	spikeQueue = new SpikeMsgQueue(SPIKE_BUFFER_NSPIKES);

	recordThread = new RecordThread(this);
	recordThread->setQueuePointers(dataQueue, eventQueue, spikeQueue);

	validBlocks.clear();
	validBlocks.insertMultiple(0, false, getNumInputs());

}

RecordNode::~RecordNode()
{
}

String RecordNode::generateDirectoryName()
{
	Time calendar = Time::getCurrentTime();

	Array<int> t;
	t.add(calendar.getYear());
	t.add(calendar.getMonth() + 1); // January = 0 
	t.add(calendar.getDayOfMonth());
	t.add(calendar.getHours());
	t.add(calendar.getMinutes());
	t.add(calendar.getSeconds());

	//Any custom text to prepend;
	//String filename = AccessClass::getControlPanel()->getTextToPrepend();
	String filename = "";

	String datestring = "";

	for (int n = 0; n < t.size(); n++)
	{
		if (t[n] < 10)
			datestring += "0";

		datestring += t[n];

		if (n == 2)
			datestring += "_";
		else if (n < 5)
			datestring += "-";
	}

	//AccessClass::getControlPanel()->setDateText(datestring);

	if (filename.length() < 24)
		filename += datestring;
	else
		filename = filename.substring(0, filename.length() - 1);

	//filename += AccessClass::getControlPanel()->getTextToAppend();

	return filename;

}

void RecordNode::createNewDirectory()
{
	LOGD(__FUNCTION__, " Creating new directory.");

	dataDirectory = File::getCurrentWorkingDirectory();

	rootFolder = File(dataDirectory.getFullPathName() + File::separator + generateDirectoryName());

	newDirectoryNeeded = false;

}

String RecordNode::generateDateString() const
{

	Time calendar = Time::getCurrentTime();

	String datestring;

	datestring += String(calendar.getDayOfMonth());
	datestring += "-";
	datestring += calendar.getMonthName(true);
	datestring += "-";
	datestring += String(calendar.getYear());
	datestring += " ";

	int hrs, mins, secs;
	hrs = calendar.getHours();
	mins = calendar.getMinutes();
	secs = calendar.getSeconds();

	datestring += hrs;

	if (mins < 10)
		datestring += 0;

	datestring += mins;

	if (secs < 0)
		datestring += 0;

	datestring += secs;

	return datestring;

}

int RecordNode::getExperimentNumber() const
{
	LOGD(__FUNCTION__, " = ", experimentNumber);
	return experimentNumber;
}

int RecordNode::getRecordingNumber() const
{
	LOGD(__FUNCTION__, " = ", recordingNumber);
	return recordingNumber;
}

AudioProcessorEditor* RecordNode::createEditor()
{
	editor = new RecordNodeEditor(this, true);
	return editor;
}

void RecordNode::prepareToPlay(double sampleRate, int estimatedSamplesPerBlock)
{
	/* This gets called right when the play button is pressed*/
}

/* Use this function to change paramaters while recording...*/
void RecordNode::setParameter(int parameterIndex, float newValue)
{
	editor->updateParameterButtons(parameterIndex);

	if (currentChannel >= 0)
	{
		Parameter* p = parameters[parameterIndex];
		p->setValue(newValue, currentChannel);
	}

}

void RecordNode::updateSettings()
{
	LOGD(__FUNCTION__);
}

void RecordNode::startRecording()
{
	int numChannels = getNumInputs();

	LOGD(__FUNCTION__, " numChannels: ", numChannels, " experimentNumber: ", experimentNumber);

	/* Set number of channels */
	dataQueue->setChannels(numChannels);
	channelMap.clear();
	for (int ch = 0; ch < numChannels; ch++)
		channelMap.add(ch);
	recordThread->setChannelMap(channelMap);

	/* Set write properties */
	createNewDirectory();
	recordThread->setFileComponents(rootFolder, recordingNumber, experimentNumber);
	recordThread->setFirstBlockFlag(false);
	setFirstBlock = false;

	/* Start record thread */
	recordThread->startThread();
	isRecording = true;

}

void RecordNode::stopRecording()
{

	LOGD(__FUNCTION__);
	isRecording = false;
	if (recordThread->isThreadRunning())
	{
		recordThread->signalThreadShouldExit();
		recordThread->waitForThreadToExit(200); //2000
	}

}

void RecordNode::handleEvent(const EventChannel* eventInfo, const MidiMessage& event, int samplePosition)
{

	LOGD(__FUNCTION__);
    if (true)
    {

            if ((*(event.getRawData()+0) & 0x80) == 0) // saving flag > 0 (i.e., event has not already been processed)
            {
				int64 timestamp = Event::getTimestamp(event);
				int eventIndex;
				if (eventInfo)
					eventIndex = getEventChannelIndex(Event::getSourceIndex(event), Event::getSourceID(event), Event::getSubProcessorIdx(event));
				else
					eventIndex = -1;
				if (isRecording)
					eventQueue->addEvent(event, timestamp, eventIndex);
            }
    }
}

void RecordNode::handleTimestampSyncTexts(const MidiMessage& event)
{
	handleEvent(nullptr, event, 0);
}
	
void RecordNode::process(AudioSampleBuffer& buffer)
{

	checkForEvents();

	if (isRecording)
	{

		for (int ch = 0; ch < buffer.getNumChannels(); ch++)
		{
			int numSamples = getNumSamples(ch);
			if (numSamples > 0)
			{
				int64 timestamp = getTimestamp(ch);
				dataQueue->writeChannel(buffer, ch, numSamples, timestamp);
			}
		}

		if (!setFirstBlock)
		{
			recordThread->setFirstBlockFlag(true);
			setFirstBlock = true;
		}

		scaleTime = recordThread->getScaleCount();
		convertTime = recordThread->getConvertCount();
		writeTime = recordThread->getWriteCount();

	}

}





