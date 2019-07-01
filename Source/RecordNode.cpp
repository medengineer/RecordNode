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
	//recordThread->setChannelMap(channelMap); //probably dont need this anymore

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


	if (CoreServices::getAcquisitionStatus() == false)
	{
		recordThread->stopThread(1);
	}
	else if (!recordThread->isThreadRunning())
		recordThread->startThread();

	if (!numChannels || !numSamples)
	{
		numChannels = buffer.getNumChannels();
		numSamples = buffer.getNumSamples();
		dataQueue->setChannels(numChannels);

		for (int ch = 0; ch < numChannels; ch++)
		{
			channelMap.add(ch);
		}

	}

	int64 t1 = Time::getHighResolutionTicks();
	int timestamp = t1;

	for (int ch = 0; ch < numChannels; ch++)
	{
		//printf("Processing channel %d\n", ch); 
		dataQueue->writeChannel(buffer, ch, numSamples);
	}

	if (!setFirstBlock)
	{
		recordThread->setFirstBlockFlag(true);
		setFirstBlock = true;
	}

	scaleTime = Time::getHighResolutionTicks() - t1;

	//for (int channel = 0; channel < numChannels; channel++)
	//{
	//	dataQueue->writeChannel(buffer, channel, numSamples);
	//}

	/*
	for (int i = 0; i < recordThreads.size(); i++)
	{
		recordThreads[i]->setNumSamples(buffer.getNumSamples());
		recordThreads[i]->setNumChannels(CHANNELS_PER_THREAD);
		recordThreads[i]->setChannelOffset(CHANNELS_PER_THREAD * i);
		recordThreads[i]->setBuffer(buffer);
		recordThreads[i]->startThread();
	}
	*/
}





