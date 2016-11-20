#pragma once

#include<stdlib.h>
#include<memory>
#include<assert.h>

namespace AngelCore
{
	namespace AngelMemory
	{
		class StackAllocator
		{

			using Marker = uintptr_t;

		public:

			StackAllocator();

			void Init(size_t size);

			void * Allocate(size_t size);

		private:

			unsigned int								m_numAllocations;
			unsigned int								m_usedMemory;
			unsigned int								m_totalSize;
			Marker										m_start;
			Marker										m_end;
			Marker										m_curPosition;
		};
	}
}
