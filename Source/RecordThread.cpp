//#include "RecordThread.h"
//
//
//RecordThread::RecordThread(AudioSampleBuffer& buffer) : Thread("Record Node")
//{
//	scaleFactor = 1 / (float(0x7fff) * NPX_BIT_VOLTS);
//	//info = new BenchmarkInfo();
//}
//
//RecordThread::~RecordThread()
//{
//
//}
//
//void RecordThread::setNumChannels(int numChannels)
//{
//	this->numChannels = numChannels;
//}
//
//void RecordThread::setNumSamples(int numSamples)
//{
//	this->numSamples = numSamples;
//	scaledBuffer.malloc(numSamples);
//	intBuffer.malloc(numSamples);
//}
//
//void RecordThread::setBuffer(AudioSampleBuffer &buffer)
//{
//	rawBuffer = buffer;
//}
//
//void RecordThread::setChannelOffset(int offset)
//{
//	this->channelOffset = offset;
//}
//
//void RecordThread::scale()
//{
//	FloatVectorOperations::copyWithMultiply(scaledBuffer.getData(), rawBufferPtr, scaleFactor, numSamples);
//}
//
//void RecordThread::convert()
//{
//	//METHOD 1: Use juce implementation
//	//AudioDataConverters::convertFloatToInt16LE(scaledBuffer.getData(), intBuffer.getData(), numSamples);
//
//	//METHOD 2: Try to beat juce implementation 
//	//int16 *intBufferPtr = intBuffer.getData();
//	//float *fltBufferPtr = scaledBuffer.getData();
//	//int count = 0;
//	//while (count < numSamples)
//	//{ 
//	//	*(intBufferPtr)++ = (int)((float(0x7fff) * (*(fltBufferPtr)++) + 32768.5) - 32768);
//	//	count++;
//	//}
//	//return;
//}
//
//void RecordThread::run()
//{
//
//	//int64 t1 = Time::getHighResolutionTicks();
//
//	//for (int ch = channelOffset; ch < channelOffset + numChannels; ch++)
//	//{
//	//	rawBufferPtr = rawBuffer.getWritePointer(ch);
//	//	scale();
//	//}
//	//info->scaleTime = Time::getHighResolutionTicks() - t1;
//
//	//t1 = Time::getHighResolutionTicks();
//
//	//for (int ch = channelOffset; ch < channelOffset + numChannels; ch++)
//	//{
//	//	rawBufferPtr = rawBuffer.getWritePointer(ch);
//	//	convert();
//	//}
//
//	//info->convertTime = Time::getHighResolutionTicks() - t1;
//
//	//printf("\rProcess time:: Scale: %4d Convert: %4d, Write: %4d", info->scaleTime, info->convertTime, 0);
//	//fflush(stdout);
//	////printf("***ALGO CHECK*** Raw: %.6f Scaled: %.6f Int: %d\n", *rawBufferPtr, *scaledBuffer.getData(), *intBuffer.getData());
//}
//
//BenchmarkInfo* RecordThread::getInfo()
//{
//	return info;
//}
//
//
//
