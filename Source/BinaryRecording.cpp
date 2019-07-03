#include "BinaryRecording.h"

#define MAX_BUFFER_SIZE 40960

BinaryRecording::BinaryRecording()
{
	m_scaledBuffer.malloc(MAX_BUFFER_SIZE);
	m_intBuffer.malloc(MAX_BUFFER_SIZE);
	m_tsBuffer.malloc(MAX_BUFFER_SIZE);		
}

BinaryRecording::~BinaryRecording() {}

String BinaryRecording::getEngineID() const
{
	return "RAWBINARY";
}

String BinaryRecording::getProcessorString(const InfoObjectCommon* channelInfo)
{
	String fName = (channelInfo->getCurrentNodeName().replaceCharacter(' ', '_') + "-" +
		String(channelInfo->getCurrentNodeID()));
	if (channelInfo->getCurrentNodeID() == channelInfo->getSourceNodeID())
		// found the channel source
	{
		fName += "." + String(channelInfo->getSubProcessorIdx());
	}
	else
	{
		fName += "_" + String(channelInfo->getSourceNodeID()) + "." +
			String(channelInfo->getSubProcessorIdx());
	}
	fName += File::separatorString;
	return fName;
}

void BinaryRecording::openFiles(File rootFolder, int experimentNumber, int recordingNumber)
{

	String basepath = rootFolder.getFullPathName() + rootFolder.separatorString + "experiment" + String(experimentNumber) + File::separatorString + "recording" + String(recordingNumber + 1) + File::separatorString;
	String contPath = basepath + "continuous" + File::separatorString;

	//Open channel files
	m_channelIndexes.insertMultiple(0, 0, getNumRecordedChannels());
	m_fileIndexes.insertMultiple(0, 0, getNumRecordedChannels());

	Array<unsigned int> indexedChannelCount;
	Array<var> jsonContinuousfiles;
	Array<var> jsonChannels;
	StringArray continousFileNames;
	int lastId = 0;

	//Removed need to loop through processors 
	const RecordProcessorInfo& pInfo = getProcessorInfo(0);
	//int recChans = getNumRecordedChannels();
	int recChans = pInfo.recordedChannels.size();

	for (int chan = 0; chan < recChans; chan++)
	{
		const DataChannel* channelInfo = getDataChannel(chan);
		int sourceId = channelInfo->getSourceNodeID();
		int sourceSubIdx = channelInfo->getSubProcessorIdx();
		bool found = false;
		DynamicObject::Ptr jsonChan = new DynamicObject();
		jsonChan->setProperty("channel_name", channelInfo->getName());
		jsonChan->setProperty("description", channelInfo->getDescription());
		jsonChan->setProperty("identifier", channelInfo->getIdentifier());
		jsonChan->setProperty("history", channelInfo->getHistoricString());
		jsonChan->setProperty("bit_volts", channelInfo->getBitVolts());
		jsonChan->setProperty("units", channelInfo->getDataUnits());
		jsonChan->setProperty("source_processor_index", channelInfo->getSourceIndex());
		jsonChan->setProperty("recorded_processor_index", channelInfo->getCurrentNodeChannelIdx());
		createChannelMetaData(channelInfo, jsonChan);

		String datPath = getProcessorString(channelInfo);
		continousFileNames.add(contPath + datPath + "continuous.dat");
		m_channelIndexes.set(chan, 0);
		indexedChannelCount.add(1);

		Array<var> jsonChanArray;
		jsonChanArray.add(var(jsonChan));
		jsonChannels.add(var(jsonChanArray));
		DynamicObject::Ptr jsonFile = new DynamicObject();
		jsonFile->setProperty("folder_name", datPath.replace(File::separatorString, "/")); 
		jsonFile->setProperty("sample_rate", channelInfo->getSampleRate());
		jsonFile->setProperty("source_processor_name", channelInfo->getSourceName());
		jsonFile->setProperty("source_processor_id", channelInfo->getSourceNodeID());
		jsonFile->setProperty("source_processor_sub_idx", channelInfo->getSubProcessorIdx());
		jsonFile->setProperty("recorded_processor", channelInfo->getCurrentNodeName());
		jsonFile->setProperty("recorded_processor_id", channelInfo->getCurrentNodeID());
		jsonContinuousfiles.add(var(jsonFile));
	}

	int nFiles = continousFileNames.size();
	for (int i = 0; i < nFiles; i++)
	{
		int numChannels = jsonChannels.getReference(i).size();
		ScopedPointer<SequentialBlockFile> blockFile = new SequentialBlockFile(numChannels, samplesPerBlock);
		if (blockFile->openFile(continousFileNames[i]))
			m_DataFiles.add(blockFile.release());
		else
			m_DataFiles.add(nullptr);
		DynamicObject::Ptr jsonFile = jsonContinuousfiles.getReference(i).getDynamicObject();
		jsonFile->setProperty("num_channels", numChannels);
		jsonFile->setProperty("channels", jsonChannels.getReference(i));
	}

	int nChans = getNumRecordedChannels();
	
	//TODO:Timestamps
	//TODO:Events
	//TODO:Spikes

	File syncFile = File(basepath + "sync_messages.txt");
	Result res = syncFile.create();
	if (res.failed())
		std::cerr << "Error creating sync text file: " << res.getErrorMessage() << std::endl;
	else
		m_syncTextFile = syncFile.createOutputStream();

	m_recordingNum = recordingNumber;

	DynamicObject::Ptr jsonSettingsFile = new DynamicObject();
	jsonSettingsFile->setProperty("GUI version", CoreServices::getGUIVersion());
	jsonSettingsFile->setProperty("continuous", jsonContinuousfiles);
	//TODO: Events, spikes...
	FileOutputStream settingsFileStream(File(basepath + "structure.oebin"));

	int indentLevel = 2;
	bool allOnOneLine = false;
	jsonSettingsFile->writeAsJSON(settingsFileStream, indentLevel, allOnOneLine);

}

template <typename TO, typename FROM>
void dataToVar(var& dataTo, const void* dataFrom, int length)
{
	const FROM* buffer = reinterpret_cast<const FROM*>(dataFrom);
	for (int i = 0; i < length; i++)
	{
		dataTo.append(static_cast<TO>(*(buffer + i)));
	}
}

String BinaryRecording::jsonTypeValue(BaseType type)
{
	switch (type)
	{
	case BaseType::CHAR:
		return "string";
	case BaseType::INT8:
		return "int8";
	case BaseType::UINT8:
		return "uint8";
	case BaseType::INT16:
		return "int16";
	case BaseType::UINT16:
		return "uint16";
	case BaseType::INT32:
		return "int32";
	case BaseType::UINT32:
		return "uint32";
	case BaseType::INT64:
		return "int64";
	case BaseType::UINT64:
		return "uint64";
	case BaseType::FLOAT:
		return "float";
	case BaseType::DOUBLE:
		return "double";
	default:
		return String::empty;
	}
}


void BinaryRecording::createChannelMetaData(const MetaDataInfoObject* channel, DynamicObject* jsonFile)
{
	int nMetaData = channel->getMetaDataCount();
	if (nMetaData < 1) return;

	Array<var> jsonMetaData;
	for (int i = 0; i < nMetaData; i++)
	{
		const MetaDataDescriptor* md = channel->getMetaDataDescriptor(i);
		const MetaDataValue* mv = channel->getMetaDataValue(i);
		DynamicObject::Ptr jsonValues = new DynamicObject();
		MetaDataDescriptor::MetaDataTypes type = md->getType();
		unsigned int length = md->getLength();
		jsonValues->setProperty("name", md->getName());
		jsonValues->setProperty("description", md->getDescription());
		jsonValues->setProperty("identifier", md->getIdentifier());
		jsonValues->setProperty("type", jsonTypeValue(type));
		jsonValues->setProperty("length", (int)length);
		var val;
		if (type == MetaDataDescriptor::CHAR)
		{
			String tmp;
			mv->getValue(tmp);
			val = tmp;
		}
		else
		{
			const void* buf = mv->getRawValuePointer();
			switch (type)
			{
			case MetaDataDescriptor::INT8:
				dataToVar<int, int8>(val, buf, length);
				break;
			case MetaDataDescriptor::UINT8:
				dataToVar<int, uint8>(val, buf, length);
				break;
			case MetaDataDescriptor::INT16:
				dataToVar<int, int16>(val, buf, length);
				break;
			case MetaDataDescriptor::UINT16:
				dataToVar<int, uint16>(val, buf, length);
				break;
			case MetaDataDescriptor::INT32:
				dataToVar<int, int32>(val, buf, length);
				break;
				//A full uint32 doesn't fit in a regular int, so we increase size
			case MetaDataDescriptor::UINT32:
				dataToVar<int64, uint8>(val, buf, length);
				break;
			case MetaDataDescriptor::INT64:
				dataToVar<int64, int64>(val, buf, length);
				break;
				//This might overrun and end negative if the uint64 is really big, but there is no way to store a full uint64 in a var
			case MetaDataDescriptor::UINT64:
				dataToVar<int64, uint64>(val, buf, length);
				break;
			case MetaDataDescriptor::FLOAT:
				dataToVar<float, float>(val, buf, length);
				break;
			case MetaDataDescriptor::DOUBLE:
				dataToVar<double, double>(val, buf, length);
				break;
			default:
				val = "invalid";
			}
		}
		jsonValues->setProperty("value", val);
		jsonMetaData.add(var(jsonValues));
	}
	jsonFile->setProperty("channel_metadata", jsonMetaData);
}

void BinaryRecording::closeFiles()
{
	resetChannels();
}

void BinaryRecording::resetChannels()
{
	m_DataFiles.clear();
	m_channelIndexes.clear();
	m_fileIndexes.clear();
	/* //TODO
	m_dataTimestampFiles.clear();
	m_eventFiles.clear();
	m_spikeChannelIndexes.clear();
	m_spikeFileIndexes.clear();
	m_spikeFiles.clear();
	*/
	m_syncTextFile = nullptr;

	m_scaledBuffer.malloc(MAX_BUFFER_SIZE);
	m_intBuffer.malloc(MAX_BUFFER_SIZE);
	m_tsBuffer.malloc(MAX_BUFFER_SIZE);
	m_bufferSize = MAX_BUFFER_SIZE;
	m_startTS.clear();
}

void BinaryRecording::writeData(int writeChannel, int realChannel, const float* buffer, int size)
{
	if (size > m_bufferSize) //shouldn't happen, but if does, this prevents crash...
	{
		std::cerr << "Write buffer overrun, resizing to: " << size << std::endl;
		m_scaledBuffer.malloc(size);
		m_intBuffer.malloc(size);
		m_tsBuffer.malloc(size);
		m_bufferSize = size;
	}

	double multFactor = 1 / (float(0x7fff) * getDataChannel(realChannel)->getBitVolts());

	FloatVectorOperations::copyWithMultiply(m_scaledBuffer.getData(), buffer, multFactor, size);

	AudioDataConverters::convertFloatToInt16LE(m_scaledBuffer.getData(), m_intBuffer.getData(), size);

	int fileIndex = m_fileIndexes[writeChannel];
	m_DataFiles[fileIndex]->writeChannel(
		getTimestamp(writeChannel) - m_startTS[writeChannel],
		m_channelIndexes[writeChannel],
		m_intBuffer.getData(), size);

	if (m_channelIndexes[writeChannel] == 0)
	{
		int64 baseTS = getTimestamp(writeChannel);
		for (int i = 0; i < size; i++)
			m_tsBuffer[i] = baseTS + i;
		/* //TODO:
		m_dataTimestampFiles[fileIndex]->writeData(m_tsBuffer, size*sizeof(int64));
		m_dataTimestampFiles[fileIndex]->increaseRecordCount(size);
		*/

	}
}

void BinaryRecording::writeTimestampSyncText(uint16 sourceID, uint16 sourceIdx, int64 timestamp, float, String text)
{
	if (!m_syncTextFile)
		return;
	m_syncTextFile->writeText(text + "\n", false, false);
}

void BinaryRecording::setParameter(EngineParameter& parameter)
{
	//boolParameter(0, m_saveTTLWords);
}