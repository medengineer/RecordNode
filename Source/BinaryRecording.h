#ifndef BINARYRECORDING_H
#define BINARYRECORDING_H

#include <chrono>

#include "RecordEngine.h"
#include "SequentialBlockFile.h"

class BinaryRecording : public RecordEngine
{
public:
	BinaryRecording();
	~BinaryRecording();

	String getEngineID() const override;

	void openFiles(File rootFolder, int experimentNumber, int recordingNumber) override;
	void closeFiles() override;
	void resetChannels() override;
	void writeData(int writeChannel, int realChannel, const float* buffer, int size) override;
	//TODO:
	void writeEvent(int eventIndex, const MidiMessage& event) override;
	//TODO:
	void addSpikeElectrode(int index, const SpikeChannel* elec) override;
	//TODO:
	void writeSpike(int electrodeIndex, const SpikeEvent* spike) override;
	void writeTimestampSyncText(uint16 sourceID, uint16 sourceIdx, int64 timestamp, float, String text) override;
	void setParameter(EngineParameter& parameter) override;

private:

	void createChannelMetaData(const MetaDataInfoObject* channel, DynamicObject* jsonObject);
	HeapBlock<float> m_scaledBuffer;
	HeapBlock<int16> m_intBuffer;
	HeapBlock<int64> m_tsBuffer;
	int m_bufferSize;

	OwnedArray<SequentialBlockFile> m_DataFiles;
	Array<unsigned int> m_channelIndexes;
	Array<unsigned int> m_fileIndexes;

	static String jsonTypeValue(BaseType type);
	static String getProcessorString(const InfoObjectCommon* channelInfo);
	
	ScopedPointer<FileOutputStream> m_syncTextFile;

	int m_recordingNum;
	Array<int64> m_startTS;

	const int samplesPerBlock{ 4096 };


};
#endif