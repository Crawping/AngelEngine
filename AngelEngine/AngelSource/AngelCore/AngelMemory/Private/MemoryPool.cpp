#include"../Public/MemoryPool.h"
#include<algorithm>
#include<math.h>

void AngelCore::AngelMemory::MemoryPool::Init(unsigned int DataSize, unsigned int PageSize)
{
	this->m_pageList = nullptr;
	this->m_freeList = nullptr;

	this->m_pageSize = PageSize;
	this->m_dataSize = DataSize;

	unsigned maxHeaderData = (sizeof(BlockHeader), DataSize);
	m_alignmentSize =
		(maxHeaderData % 16)
		? (4 - maxHeaderData % 16)
		: (0);

	m_blockSize =
		maxHeaderData + m_alignmentSize;

	m_blocksPerPage =
		(m_pageSize - sizeof(PageHeader)) / m_blockSize;

	PageHeader *newPage = reinterpret_cast<PageHeader*>(new char[this->m_pageSize]);
	++m_numPages;
	this->m_numFreeBlocks += this->m_blocksPerPage;
	this->m_numBlocks += this->m_blocksPerPage;

	if (this->m_pageList)
		newPage->next = this->m_pageList;

	this->m_pageList = newPage;

	BlockHeader *curBlock = newPage->blockes();

	for (unsigned int i = 0;i < this->m_blocksPerPage - 1;i++)
	{
		curBlock->next = NextBlock(curBlock);
		curBlock = NextBlock(curBlock);
	}
	curBlock->next = nullptr;

	this->m_freeList = newPage->blockes();

}

void *AngelCore::AngelMemory::MemoryPool::Allocate()
{
	/*if (!this->m_freeList)
	{
		PageHeader *newPage = reinterpret_cast<PageHeader*>(new char[this->m_pageSize]);
		++m_numPages;
		this->m_numFreeBlocks += this->m_blocksPerPage;
		this->m_numBlocks += this->m_blocksPerPage;

		if (this->m_pageList)
			newPage->next = this->m_pageList;

		this->m_pageList = newPage;

		BlockHeader *curBlock = newPage->blockes();

		for (unsigned int i = 0;i < this->m_blocksPerPage - 1;i++)
		{
			curBlock->next = NextBlock(curBlock);
			curBlock = NextBlock(curBlock);
		}
		curBlock->next = nullptr;

		this->m_freeList = newPage->blockes();
	}*/

	BlockHeader *ret = this->m_freeList;
	this->m_freeList = this->m_freeList->next;
	--m_numFreeBlocks;
	return ret;

}

AngelCore::AngelMemory::MemoryPool::BlockHeader * AngelCore::AngelMemory::MemoryPool::NextBlock(AngelCore::AngelMemory::MemoryPool::BlockHeader * b)
{
	return reinterpret_cast<BlockHeader *>(reinterpret_cast<char *>(b) + this->m_blockSize);
}

void AngelCore::AngelMemory::MemoryPool::Free(void *p)
{
	BlockHeader *header = reinterpret_cast<BlockHeader*>(p);

	header->next = this->m_freeList;
	this->m_freeList = header;
	++this->m_numFreeBlocks;
}


void AngelCore::AngelMemory::MemoryPool::FreeAll()
{
	if (this->m_pageList)
	{
		PageHeader *iterator = this->m_pageList;

		delete[] reinterpret_cast<char*>(iterator);
		// release pointers
		m_pageList = nullptr;
		m_freeList = nullptr;

		// re-init stats
		m_numPages = 0;
		m_numBlocks = 0;
		m_numFreeBlocks = 0;
	}
	

}