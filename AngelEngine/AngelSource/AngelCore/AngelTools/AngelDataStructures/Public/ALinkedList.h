#pragma once

#include"ALink.h"
#include"AIterator.h"


namespace AngelCore
{
	namespace AngelTools
	{
		namespace AngelDataStructures
		{
			template<typename T>
			class AELinkedList
			{
			public:

				using Iterator = AIterator<T>;

			public:
				AELinkedList()
				{
					m_head = nullptr;
					m_tail = nullptr;
					this->m_size = 0;
				}

				~AELinkedList()
				{
					delete this->m_head;
					delete this->m_tail;
				}

			public:
				///Mainpulate
				void Add(T * _newItem)
				{
					if (!m_head)
					{
						this->m_head = _newItem;
						this->m_tail = _newItem;
						this->m_head->m_prev = this->m_tail;
						this->m_head->m_next = this->m_tail;
						this->m_tail->m_next = this->m_head;
						this->m_tail->m_prev = this->m_head;
						m_size++;
						return;
					}
					T * temp = this->m_head;
					while (temp->m_next != this->m_head)
					{
						temp = temp->m_next;
					}
					temp->m_next = _newItem;
					_newItem->m_prev = temp;
					_newItem->m_next = this->m_head;
					this->m_tail = _newItem;
					
					m_size++;
				}
				T * Find(const T * _item)
				{
					Iterator beg = Begin();
					Iterator end = End();
					while (beg != end)
					{
						if (&beg == _item)
							return &beg;
						beg++;
					}

					if (&end == _item)
						return &end;

					return nullptr;
				}
				T * at(unsigned int i)
				{
					Iterator beg = Begin();
					Iterator end = End();
					while (i != 0)
					{
						beg++;
						i--;
					}
					return &beg;
				}
				void DeleteAt(unsigned int i)
				{
					//assert(i < this->m_size);
					Iterator beg = Begin();
					Iterator end = End();
					while (i != 0)
					{
						beg++;
						i--;
					}
					delete &beg;
				}

				bool IsHaving(const T * _item)
				{
					Iterator beg = Begin();
					Iterator end = End();
					while (beg != end)
					{
						if (&beg == _item)
							return true;
						beg++;
					}

					if (&end == _item)
						return true;

					return false;
				}
				///

			public:
				///Operators
				T * operator[](unsigned int i)
				{
					Iterator beg = Begin();
					Iterator end = End();
					while (i != 0)
					{
						beg++;
						i--;
					}
					return &beg;
				}
				///
				
			public:
				unsigned int Size() const								{ return this->m_size; }

				Iterator Begin()										{ return Iterator(this->m_head); }
				Iterator End()											{ return Iterator(this->m_tail); }

				bool IsEmpty() const									{ return this->m_size == 0 ? true : false; }

			private:

				T *												m_tail;
				T *												m_head;
				unsigned int									m_size;

			private:
				AELinkedList(const AELinkedList &that) {}
				AELinkedList(AELinkedList &&that) {}
			};


			template<typename T>
			class AILinkedList
			{

			public:
				///Iterator
				using Iterator = AIterator<T>;

 				Iterator Begin() { return Iterator(this->m_head); }
				Iterator End() { return Iterator((this->m_tail)); }
				
				///Iterator

			public:

				///Constructor And Desctructors
				AILinkedList() : m_size(0),m_tail(nullptr),m_head(nullptr) {}
				~AILinkedList()
				{
					if (IsEmpty()) return;
					T * temp;
					temp = this->m_head;
					T* tempNext = temp->m_next;
					while (tempNext != this->m_tail)
					{
						tempNext = temp->m_next;
						delete temp;
						temp = tempNext;

					}
					delete tempNext;
				}
				///

				///Mainpulation
				void Add(T * _newItem)
				{
					if (this->m_size == 0)
					{
						this->m_head = _newItem;
						this->m_tail = _newItem;
						this->m_head->m_prev = this->m_tail;
						this->m_head->m_next = this->m_tail;
						this->m_tail->m_next = this->m_head;
						this->m_tail->m_prev = this->m_head;
						m_size++;
						return;
					}
					T * temp;
					_newItem->m_prev = this->m_tail;
					_newItem->m_next = this->m_head;
					temp = this->m_tail;
					temp->m_next = _newItem;
					this->m_tail = _newItem;	
					m_size++;	
				}
				T * Find(const T * _item)
				{
					Iterator beg = Begin();
					Iterator end = End();
					while (beg != end)
					{
						if (&beg == _item)
							return &beg;
						beg++;
					}

					if (&end == _item)
						return &end;

					return nullptr;
				}
				T * at(unsigned int i)
				{
					Iterator beg = Begin();
					Iterator end = End();
					while (i != 0)
					{
						beg++;
						i--;
					}
					return &beg;
				}
				bool IsHaving(const T * _item)
				{
					Iterator beg = Begin();
					Iterator end = End();
					while (beg != end)
					{
						if (&beg == _item)
							return true;
						beg++;
					}

					if (&end == _item)
						return true;

					return false;
				}
				///

			public:
				///Operators
				T * operator[](unsigned int i)
				{
					Iterator beg = Begin();
					Iterator end = End();
					while (i != 0)
					{
						beg++;
						i--;
					}
					return &beg;
				}
				///
			public:
				unsigned int Size()									{ return this->m_size; }

				bool IsEmpty() const								{ return this->m_size == 0 ? true : false; }

			private:
				T *											m_head;
				T *											m_tail;
				unsigned int								m_size;

			};
		}
	}
}