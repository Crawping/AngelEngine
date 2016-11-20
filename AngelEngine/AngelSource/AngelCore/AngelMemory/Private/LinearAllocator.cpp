#include"../Public/LinearAllocator.h"
#include<stdlib.h>
#include<memory>
#include<assert.h>

AngelCore::AngelMemory::LinearAllocator::LinearAllocator()
{
}

void AngelCore::AngelMemory::LinearAllocator::Init(size_t size)
{
	this->m_currentPos = malloc(size);
	this->m_size = size;
	this->m_numAllocation = 0;
	this->m_usedMemory = 0;
}

void *AngelCore::AngelMemory::LinearAllocator::Allocate(size_t size)
{
	assert(this->m_usedMemory + size < this->m_size);
	uintptr_t address = (uintptr_t)m_currentPos;
	m_currentPos = (void*)(address + size);
	this->m_usedMemory += size;
	this->m_numAllocation++;
	void * temp = (void*)address;
	return (void*)address;
}

void AngelCore::AngelMemory::LinearAllocator::FreeAll()
{
	this->m_currentPos = nullptr;
}