////This prevents include loops. We recommend changing the macro to a name suitable for your plugin
//#ifndef RECORDTHREAD_H_DEFINED
//#define RECORDTHREAD_H_DEFINED
//
//#include <chrono>
//#include <math.h>
//
//#include <ProcessorHeaders.h>
//#include "RecordNode.h"
//
//#define MAX_BUFFER_SIZE 40960
//#define NIDAQ_BIT_VOLTS 0.001221f
//#define NPX_BIT_VOLTS 0.195f
//
//
//class RecordThread : public Thread
//{
//public:
//
//	RecordThread(AudioSampleBuffer&);
//
//	~RecordThread();
//
//	void setNumChannels(int);
//	void setNumSamples(int);
//	void setBuffer(AudioSampleBuffer&);
//	void setChannelOffset(int offset);
//
//	void scale();
//	void convert();
//	void write();
//
//	void run();
//
//	BenchmarkInfo* getInfo();
//
//private:
//
//	AudioSampleBuffer rawBuffer;
//	float* rawBufferPtr;
//
//	int numSamples;
//	int numChannels;
//
//	int channelOffset;
//
//	float scaleFactor;
//
//	HeapBlock<float> scaledBuffer;  
//	HeapBlock<int16> intBuffer;
//
//	ScopedPointer<BenchmarkInfo> info;
//
//	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(RecordThread);
//
//};
//
//#endif