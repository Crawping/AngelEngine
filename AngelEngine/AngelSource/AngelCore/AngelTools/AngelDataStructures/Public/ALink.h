#pragma once

namespace AngelCore
{
	namespace AngelTools
	{
		namespace AngelDataStructures
		{
			
			template<typename ELEMENT>
			struct ALink
			{
			public:
				
				ELEMENT * m_prev;
				ELEMENT * m_next;
			};

			template<typename T>
			struct ANode
			{
				T * Next;
				T * Data;
			};


		}
	}
}

