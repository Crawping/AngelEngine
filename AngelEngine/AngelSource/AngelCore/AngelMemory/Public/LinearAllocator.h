#pragma once

namespace AngelCore
{
	namespace AngelMemory
	{
		class LinearAllocator
		{
		public:
			LinearAllocator();
			void Init(size_t size);
			void *Allocate(size_t size);

			void FreeAll();

		private:
			void *						m_currentPos;
			size_t						m_size;
			size_t						m_usedMemory;
			size_t						m_numAllocation;
		};
	}
}