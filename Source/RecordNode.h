//This prevents include loops. We recommend changing the macro to a name suitable for your plugin
#ifndef RECORDNODE_H_DEFINED
#define RECORDNODE_H_DEFINED

#include <chrono>
#include <math.h>

#include <ProcessorHeaders.h>
#include "RecordNodeEditor.h"
#include "DataQueue.h"
#include "RecordThread.h"

#define NIDAQ_BIT_VOLTS			0.001221f
#define NPX_BIT_VOLTS			0.195f

#define MAX_BUFFER_SIZE			40960
#define CHANNELS_PER_THREAD		384
#define WRITE_BLOCK_LENGTH		1024
#define DATA_BUFFER_NBLOCKS		300
#define EVENT_BUFFER_NEVENTS	512
#define SPIKE_BUFFER_NSPIKES	512

class RecordNode : public GenericProcessor
{

public:

	RecordNode();

	~RecordNode();

	AudioProcessorEditor* createEditor() override;

	bool hasEditor() const override { return true; }

	void prepareToPlay(double sampleRate, int estimatedSamplesPerBlock);

	void process(AudioSampleBuffer& buffer) override;

	void startRecording() override;
	void stopRecording() override;

	void setParameter(int parameterIndex, float newValue) override;

	int64 processLoopCounter;
	int64 lastProcessStart;
	int64 processTime;
	int64 scaleTime;
	int64 convertTime;
	int64 writeTime;

private:

	int numChannels;
	int numSamples;

	ScopedPointer<RecordThread> recordThread;
	ScopedPointer<DataQueue> dataQueue;

	std::atomic<bool> setFirstBlock;

	Array<int> channelMap;

	float scaleFactor;
	HeapBlock<float> scaledBuffer;  
	HeapBlock<int16> intBuffer;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(RecordNode);

};

#endif