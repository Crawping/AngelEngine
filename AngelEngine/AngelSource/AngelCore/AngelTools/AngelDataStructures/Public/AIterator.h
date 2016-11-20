#pragma once
#include<Windows.h>
#include<memory>

namespace AngelCore
{
	namespace AngelTools
	{
		namespace AngelDataStructures
		{
			template<typename T>
			class AIterator : public std::iterator<std::random_access_iterator_tag, T, ptrdiff_t>
			{
			public:
				AIterator() { this->m_data = new T(); }

				AIterator(T * ptr)
				{
					this->m_data = ptr;
				}

				AIterator(const AIterator &that)
				{
					this->m_data = that.m_data;
				}

				AIterator &operator =(T *ptr)
				{
					this->m_data = ptr;
					return *this;
				}

				AIterator & operator =(const AIterator &that)
				{
					this->m_data = that.m_data;
					return *this;
				}


				AIterator &operator =(AIterator &&that)
				{
					this->m_data = std::move(that.m_data);
					return *this;
				}

				operator bool() const
				{
					if (this->m_data)
						return true;
					return false;
				}

				bool operator ==(const AIterator &rhs) const
				{
					if (this->m_data == rhs.m_data)
						return true;
					return false;
				}

				bool operator !=(const AIterator &rhs) const
				{
					if (this->m_data != rhs.m_data)
						return true;
					return false;
				}

				AIterator &operator +=(const ptrdiff_t &size)
				{
					this->m_data += size;
					return *this;
				}

				AIterator &operator -=(const ptrdiff_t &size)
				{
					this->m_data -= size;
					return *this;
				}

				AIterator &operator +(const  ptrdiff_t &size)
				{
					this->m_data += size;
					return *this;
				}

				AIterator &operator -(const ptrdiff_t &size)
				{
					this->m_data -= size;
					return *this;
				}

				AIterator &operator++()
				{
					this->m_data+=ptrdiff_t;
					return *this;
				}

				AIterator &operator ++(ptrdiff_t )
				{
					this->m_data = this->m_data->m_next;
					
 					return *this;
				}

				T & operator *()
				{
					return *m_data;
				}

				T * operator->()
				{
					return this->m_data;
				}

				T * operator &()
				{
					return this->m_data;
				}


			private:
				T *										m_data;
			};
		}
	}
}
