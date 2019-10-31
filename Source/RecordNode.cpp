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

	recordEngine = new BinaryRecording();
	recordEngine->resetChannels();

	recordThread = new RecordThread(this, recordEngine);
	recordThread->setQueuePointers(dataQueue, eventQueue, spikeQueue);

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
	LOGD("Number of channels: ", dataChannelArray.size());

	parChannelMap.clear();
	int subProcIdx = -1;
	std::vector<int> subProcChannels;

	/* Build processor channel map here for now, need to move into startRecording to enable channel selection */
	for (int ch = 0; ch < dataChannelArray.size(); ch++)
	{
		DataChannel* chan = dataChannelArray[ch];
		chan->setRecordState(true);
		if (chan->getSubProcessorIdx() > subProcIdx)
		{
			subProcIdx = chan->getSubProcessorIdx();
			if (subProcChannels.size())
				parChannelMap.push_back(subProcChannels);
			subProcChannels = {ch};
		}
		else
		{
			subProcChannels.push_back(ch);
		}
	}
	parChannelMap.push_back(subProcChannels);

}

void RecordNode::startRecording()
{
	/* Got signal from plugin-GUI to start recording */
	if (newDirectoryNeeded)
	{
		createNewDirectory();
		recordingNumber = 0;
		experimentNumber = 1;
		settingsNeeded = true;
		recordEngine->directoryChanged();
	}
	else
	{
		recordingNumber++; // increment recording number within this directory
	}

	if (!rootFolder.exists())
	{
		rootFolder.createDirectory();
	}
	/* TODO: Do we still need this? 
	if (settingsNeeded)
	{
		String settingsFileName = rootFolder.getFullPathName() + File::separator + "settings" + ((experimentNumber > 1) ? "_" + String(experimentNumber) : String::empty) + ".xml";
		AccessClass::getEditorViewport()->saveState(File(settingsFileName), m_lastSettingsText);
		settingsNeeded = false;
	}
	*/

	recordThread->setFileComponents(rootFolder, recordingNumber, experimentNumber);

	channelMap.clear();
	int totChans = dataChannelArray.size();
	LOGD(__FUNCTION__, " totChans: ", totChans);
	OwnedArray<RecordProcessorInfo> procInfo;
	Array<int> chanProcessorMap;
	Array<int> chanOrderinProc;
	int lastProcessor = -1;
	int procIndex = -1;
	int chanProcOrder = 0;
	for (int ch = 0; ch < totChans; ++ch)
	{
		DataChannel* chan = dataChannelArray[ch];
		if (chan->getRecordState())
		{
			channelMap.add(ch);
			//This is bassed on the assumption that all channels from the same processor are added contiguously
			//If this behaviour changes, this check should be most thorough
			if (chan->getCurrentNodeID() != lastProcessor)
			{
				lastProcessor = chan->getCurrentNodeID();
				RecordProcessorInfo* pi = new RecordProcessorInfo();
				pi->processorId = chan->getCurrentNodeID();
				procInfo.add(pi);
				procIndex++;
				chanProcOrder = 0;
			}
			procInfo.getLast()->recordedChannels.add(channelMap.size() - 1);
			chanProcessorMap.add(procIndex);
			chanOrderinProc.add(chanProcOrder);
			chanProcOrder++;
		}
	}
	std::cout << "Num Recording Processors: " << procInfo.size() << std::endl;
	int numRecordedChannels = channelMap.size();
	
	validBlocks.clear();
	validBlocks.insertMultiple(0, false, getNumInputs());

	//WARNING: If at some point we record at more that one recordEngine at once, we should change this, as using OwnedArrays only works for the first
	recordEngine->setChannelMapping(channelMap, chanProcessorMap, chanOrderinProc, procInfo);
	recordThread->setChannelMap(channelMap);

	dataQueue->setChannels(numRecordedChannels);
	eventQueue->reset();
	spikeQueue->reset();
	recordThread->setFirstBlockFlag(false);

	isRecording = true;
	hasRecorded = true;

	/* Set write properties */
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

	if (true) //TODO: True only when saving events is enabled for this RecordNode.
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

void RecordNode::handleTimestampSyncTexts(const MidiMessage& event)
{
	handleEvent(nullptr, event, 0);
}
	
void RecordNode::process(AudioSampleBuffer& buffer)
{

	checkForEvents();

	if (isRecording)
	{

		for (int ch = 0; ch < channelMap.size(); ch++)
		{
			int nSamples = getNumSamples(ch);
			bool shouldWrite = validBlocks[ch];
			if (!shouldWrite && nSamples > 0)
			{
				shouldWrite = true;
				validBlocks.set(ch, true);
			}
			
			if (shouldWrite)
				dataQueue->writeChannel(buffer, ch, nSamples, getTimestamp(ch));
		}

		if (!setFirstBlock)
		{
			bool shouldSetFlag = true;
			for (int chan = 0; chan < channelMap.size(); ++chan)
			{
				if (!validBlocks[chan])
				{
					shouldSetFlag = false;
					break;
				}
			}
			if (shouldSetFlag)
			{
				recordThread->setFirstBlockFlag(true);
				setFirstBlock = true;
			}
		}

		scaleTime = recordThread->getScaleCount();
		convertTime = recordThread->getConvertCount();
		writeTime = recordThread->getWriteCount();

	}

}





