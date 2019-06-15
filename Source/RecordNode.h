//This prevents include loops. We recommend changing the macro to a name suitable for your plugin
#ifndef RECORDNODE_H_DEFINED
#define RECORDNODE_H_DEFINED

#include <chrono>
#include <math.h>

#include <ProcessorHeaders.h>
#include "RecordNodeEditor.h"

#define MAX_BUFFER_SIZE 40960
#define NIDAQ_BIT_VOLTS 0.001221f
#define NPX_BIT_VOLTS	0.195f
#define CHANNELS_PER_THREAD 384

class RecordNode : public GenericProcessor
{

public:

	RecordNode();

	~RecordNode();

	AudioProcessorEditor* createEditor() override;

	bool hasEditor() const override { return true; }

	void prepareToPlay(double sampleRate, int estimatedSamplesPerBlock);

	void process(AudioSampleBuffer& buffer) override;

	void setParameter(int parameterIndex, float newValue) override;

	int64 scaleTime;
	int64 convertTime;
	int64 writeTime;

private:

	int numChannels;
	int numSamples;

	AudioSampleBuffer rawBufferCopy;

	float scaleFactor;
	HeapBlock<float> scaledBuffer;  
	HeapBlock<int16> intBuffer;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(RecordNode);

};

#endif