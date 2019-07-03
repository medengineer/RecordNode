#include "SequentialBlockFile.h"

SequentialBlockFile::SequentialBlockFile(int nChannels, int samplesPerBlock) :
m_file(nullptr),
m_nChannels(nChannels),
m_samplesPerBlock(samplesPerBlock),
m_blockSize(nChannels*samplesPerBlock),
m_lastBlockFill(0)
{
	m_memBlocks.ensureStorageAllocated(blockArrayInitSize);
	for (int i = 0; i < nChannels; i++)
		m_currentBlock.add(-1);
}

SequentialBlockFile::~SequentialBlockFile()
{
	//Ensure that all remaining blocks are flushed in order. Keep the last one. 
	int n = m_memBlocks.size();
	for (int i = 0; i < n - 1; i++)
		m_memBlocks.remove(0);

	m_memBlocks[0]->partialFlush(m_lastBlockFill * m_nChannels);
}

bool SequentialBlockFile::openFile(String filename)
{
	File file(filename);
	Result res = file.create();
	if (res.failed())
	{
		std::cerr << "Error creating file " << filename << ":" << res.getErrorMessage() << std::endl;
		return false;
	}
	m_file = file.createOutputStream(streamBufferSize);
	if (!m_file)
		return false;

	m_memBlocks.add(new FileBlock(m_file, m_blockSize, 0));
	return true;
}

bool SequentialBlockFile::writeChannel(uint64 startPos, int channel, int16* data, int nSamples)
{
	if (!m_file)
		return false;

	int blockIndex = m_memBlocks.size() - 1;
	if ((blockIndex < 0) || (m_memBlocks[blockIndex]->getOffset() + m_samplesPerBlock) < (startPos + nSamples))
		allocateBlocks(startPos, nSamples);

	for (blockIndex = m_memBlocks.size() - 1; blockIndex >= 0; blockIndex--)
	{
		if (m_memBlocks[blockIndex]->getOffset() <= startPos)
			break;
	}
	if (blockIndex < 0)
	{
		std::cerr << "BINARY WRITER: Memory Block unloaded ahead of time for chan " << channel << " start " << startPos << " ns " << nSamples << " first " << m_memBlocks[0]->getOffset() << std::endl;
		for (int i = 0; i < m_nChannels; i++)
			std::cout << "channel " << i << " last block " << m_currentBlock[i] << std::endl;
		return false;
	}

	int writtenSamples = 0;
	int startIdx = startPos - m_memBlocks[blockIndex]->getOffset();
	int startMemPos = startIdx*m_nChannels;
	int dataIdx = 0;
	int lastBlockIdx = m_memBlocks.size() - 1;
	while (writtenSamples < nSamples)
	{
		int16* blockPtr = m_memBlocks[blockIndex]->getData();
		int samplesToWrite = jmin((nSamples - writtenSamples), (m_samplesPerBlock - startIdx));
		for (int i = 0; i < samplesToWrite; i++)
			*(blockPtr + startMemPos + channel + i*m_nChannels) = *(data + dataIdx++);
		writtenSamples += samplesToWrite;

		//update last block fill index
		size_t samplePos = startIdx + samplesToWrite;
		if (blockIndex == lastBlockIdx && samplePos > m_lastBlockFill)
			m_lastBlockFill = samplePos;

		startIdx = 0;
		startMemPos = 0;
		blockIndex++;
	}
	m_currentBlock.set(channel, blockIndex - 1); //store last block channel was written in
	return true;
}

void SequentialBlockFile::allocateBlocks(uint64 startIndex, int numSamples)
{
	//First deallocate full blocks
	//Search for the earliest unused block
	unsigned int minBlock = 0xFFFFFFFF; //large number...
	for (int i = 0; i < m_nChannels; i++)
	{
		if (m_currentBlock[i] < minBlock)
			minBlock = m_currentBlock[i];
	}

	//Update block indexes
	for (int i = 0; i < m_nChannels; i++)
	{
		m_currentBlock.set(i, m_currentBlock[i] - minBlock);
	}

	m_memBlocks.removeRange(0, minBlock);

	//Look for last available position and calculate 
	uint64 lastOffset = m_memBlocks.getLast()->getOffset();
	uint64 maxAddr = lastOffset + m_samplesPerBlock + 1;
	uint64 newSpaceNeeded = numSamples - (maxAddr - startIndex);
	int newBlocks = (newSpaceNeeded + m_samplesPerBlock - 1) / (m_samplesPerBlock); //Fast ceiling division

	for (int i = 0; i < newBlocks; i++)
	{
		lastOffset += m_samplesPerBlock;
		m_memBlocks.add(new FileBlock(m_file, m_blockSize, lastOffset));
	}
	if (newBlocks > 0)
		m_lastBlockFill = 0; //added some new blocks so last one will be empty
}