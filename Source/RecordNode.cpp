#include "RecordNode.h"

using namespace std::chrono;

RecordNode::RecordNode() : GenericProcessor("Record Node"),
scaleTime(0), convertTime(0), writeTime(0)
{

	setProcessorType(PROCESSOR_TYPE_FILTER);

	rawBufferCopy = AudioSampleBuffer(getTotalNumInputChannels(), MAX_BUFFER_SIZE);

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
	int64 t1 = Time::getHighResolutionTicks();

	numChannels = buffer.getNumChannels();
	numSamples = buffer.getNumSamples();

	for (int ch = 0; ch < numChannels; ch++)
	{
		printf("Procssing channel %d\r", ch); fflush(stdout);
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





