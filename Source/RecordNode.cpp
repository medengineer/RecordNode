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

	
void RecordNode::process(AudioSampleBuffer& buffer)
{

	int64 processStart = Time::getHighResolutionTicks();

	if (!numChannels || !numSamples)
	{
		numChannels = buffer.getNumChannels();
		numSamples = buffer.getNumSamples();
		dataQueue->setChannels(numChannels);

		for (int ch = 0; ch < numChannels; ch++)
			channelMap.add(ch);
		recordThread->setChannelMap(channelMap);

	}

	printf("Acquisition status: %d", CoreServices::getAcquisitionStatus());
	if (!CoreServices::getAcquisitionStatus())
	{
		printf("******Called thread should exit!\n");
		recordThread->signalThreadShouldExit();
	}
	else if (!recordThread->isThreadRunning())
		recordThread->startThread();

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

	processTime = Time::getHighResolutionTicks() - processStart;
}





