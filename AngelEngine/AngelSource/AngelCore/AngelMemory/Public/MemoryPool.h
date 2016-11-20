#pragma once

#include<stdio.h>

#include<stdlib.h>

#include<Windows.h>
namespace AngelCore
{
	namespace AngelMemory
	{
		class MemoryPool
		{
		public:
			void Init(unsigned int DataSize,unsigned int PageSize);

			void *Allocate(void);

			void Free(void * p);

			void FreeAll();

		private:
			struct BlockHeader
			{
				BlockHeader *next;

			};
			struct PageHeader
			{
				PageHeader *next;
				BlockHeader *blockes(void)
				{
					return reinterpret_cast<BlockHeader*>(this + 1);
				}
			};

		private:

			unsigned m_dataSize;
			unsigned m_pageSize;
			unsigned m_alignmentSize;
			unsigned m_blockSize;
			unsigned m_blocksPerPage;

			// statistics
			unsigned m_numPages;
			unsigned m_numBlocks;
			unsigned m_numFreeBlocks;

			// the page list
			PageHeader *m_pageList;

			// the free list
			BlockHeader *m_freeList;

		private:
			BlockHeader *NextBlock(BlockHeader*b);


		};
	}
}