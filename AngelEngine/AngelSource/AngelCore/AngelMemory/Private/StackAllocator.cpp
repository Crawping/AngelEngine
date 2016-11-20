#include"../Public/StackAllocator.h"
#include<stdlib.h>
#include<assert.h>

AngelCore::AngelMemory::StackAllocator::StackAllocator() : m_numAllocations(0), m_usedMemory(0)
{

}

void AngelCore::AngelMemory::StackAllocator::Init(size_t size)
{
	this->m_start = (Marker)malloc(size);
	this->m_end = this->m_start + size;
	this->m_curPosition = this->m_start;
	this->m_totalSize = size;
}

void * AngelCore::AngelMemory::StackAllocator::Allocate(size_t size)
{
	assert(this->m_usedMemory + size < this->m_totalSize);
	this->m_usedMemory += size;
	void * ret = (void*)this->m_curPosition;
	this->m_curPosition += size;
	return ret;
}