#include "RecordNode.h"

using namespace std::chrono;

RecordNode::RecordNode() : GenericProcessor("Record Node"),
scaleTime(0), convertTime(0), writeTime(0)
{
	setProcessorType(PROCESSOR_TYPE_FILTER);

	dataQueue = new DataQueue(WRITE_BLOCK_LENGTH, DATA_BUFFER_NBLOCKS);

	recordThread = new RecordThread();
	recordThread->setQueuePointers(dataQueue);

	setFirstBlock = false;

	numChannels = 0;
	numSamples = 0;

	processLoopCounter = 0;
	lastProcessStart = -1; 

}

RecordNode::~RecordNode()
{

}

AudioProcessorEditor* RecordNode::createEditor()
{
	editor = new RecordNodeEditor(this, true);
	return editor;
}

void RecordNode::prepareToPlay(double sampleRate, int estimatedSamplesPerBlock)
{
	/* This gets called right when the play button is pressed*/
	File rootFolder = File::getCurrentWorkingDirectory();
	int recordingNumber = 1337;
	int experimentNumber = 1337;
	recordThread->setFileComponents(rootFolder, recordingNumber, experimentNumber);
	recordThread->setFirstBlockFlag(false);
	setFirstBlock = false;

}

void RecordNode::setParameter(int parameterIndex, float newValue)
{
	editor->updateParameterButtons(parameterIndex);

	if (currentChannel >= 0)
	{
		Parameter* p = parameters[parameterIndex];
		p->setValue(newValue, currentChannel);
	}

}

void RecordNode::startRecording()
{
	printf("*******Called start recording!!!\n");
	recordThread->startThread();
}

void RecordNode::stopRecording()
{
	printf("******Called stop recording!!!\n");
	if (recordThread->isThreadRunning())
	{
		recordThread->signalThreadShouldExit();
		recordThread->waitForThreadToExit(2000);
	}
}
	
void RecordNode::process(AudioSampleBuffer& buffer)
{

	if (!numChannels || !numSamples)
	{
		numChannels = buffer.getNumChannels();
		numSamples = buffer.getNumSamples();
		dataQueue->setChannels(numChannels);

		for (int ch = 0; ch < numChannels; ch++)
			channelMap.add(ch);
		recordThread->setChannelMap(channelMap);

	}

	for (int ch = 0; ch < numChannels; ch++)
	{
		int64 timestamp = getTimestamp(ch);
		dataQueue->writeChannel(buffer, ch, numSamples, timestamp);
	}

	if (!setFirstBlock)
	{
		recordThread->setFirstBlockFlag(true);
		setFirstBlock = true;
	}

}





