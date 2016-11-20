#pragma once
#include"../../AngelCore/AngelSubSystem/AngelLog/Public/AngelLog.h"

#include"AngelMathHelper\Public\Vector.h"

#include<d3d11_1.h>
#include<comdef.h>
#include<d2d1.h>
#include<dwrite_1.h>

#define MB_ERROR MB_OK | MB_ICONERROR



#pragma region Typdef
using I8 = __int8;

using I16 = __int16;

using I32 = __int32;

using I64 = __int64;

#ifndef __VECTOR_TYPE
#define __VECTOR_TYPE
using Vector2 = AngelCore::Vector<2>;
						 
using Vector3 = AngelCore::Vector<3>;
						 
using Vector4 = AngelCore::Vector<4>;
#endif

#pragma endregion

#pragma region ErrorChecking
#if defined(DEBUG) | defined(_DEBUG)
#ifndef HR
#define HR(x)																				\
		{																					\
		HRESULT hr = x;																		\
		if(FAILED(hr))																		\
					{																		\
				_com_error er(hr);															\
				std::wstring errStr= er.ErrorMessage();										\
				std::wstring errWstr(errStr.begin(),errStr.end());							\
				const wchar_t * message = errWstr.c_str();									\
				std::wstring messageW = message;											\
				std::string messageS(messageW.begin(), messageW.end());						\
				messageS += "\nAt:";														\
				messageS += __FILE__;														\
				messageS += "\nAt:";														\
				messageS += std::to_string(__LINE__);										\
				MessageBoxA(NULL, messageS.c_str(), "Error", MB_ERROR);						\
				AngelCore::AngelSubSystem::AngelLog::WriteErrorMessage(messageS.c_str());	\
				std::terminate(); 															\
					}																				\
		}
#endif

#else
#ifndef HR
#define HR(x) x;
#endif
#endif 


#if defined (DEBUG) | defined(_DEBUG)
#ifndef ANGEL_ERROR
#define ANGEL_ERROR																	\
	{																				\
		std::wstring messageW = L"Error Occured\n";									\
		std::string messageS(messageW.begin(), messageW.end());						\
		messageS += "\nAt:";														\
		messageS += __FILE__;														\
		messageS += "\nAt:";														\
		messageS += std::to_string(__LINE__);										\
		MessageBoxA(NULL, messageS.c_str(), "Error", MB_ERROR);						\
		AngelCore::AngelSubSystem::AngelLog::WriteErrorMessage(messageS.c_str());	\
		std::terminate(); 															\
	}
#endif
#endif

#pragma endregion

#define COM_RELEASE(x)			{ if (x != nullptr) { auto _x = x.GetAddressOf(); (*_x)->Release();	(*_x) = nullptr; x = nullptr; } }


inline D2D1::Matrix3x2F& Cast(DWRITE_MATRIX& matrix)
{
	// DWrite's matrix, D2D's matrix, and GDI's XFORM
	// are all compatible.
	return *reinterpret_cast<D2D1::Matrix3x2F*>(&matrix);
}

inline float GetDeterminant(DWRITE_MATRIX const& matrix)
{
	return matrix.m11 * matrix.m22 - matrix.m12 * matrix.m21;
}

inline DWRITE_MATRIX& Cast(D2D1::Matrix3x2F& matrix)
{
	return *reinterpret_cast<DWRITE_MATRIX*>(&matrix);
}

inline int RoundToInt(float x)
{
	return static_cast<int>(floor(x + .5));
}

inline double DegreesToRadians(float degrees)
{
	return degrees * DirectX::XM_PI * 2.0f / 360.0f;
}
////////////////////////////////////////
// COM help.


// Releases a COM object and nullifies pointer.
template <typename InterfaceType>
inline void SafeRelease(InterfaceType** currentObject)
{
	if (*currentObject != NULL)
	{
		(*currentObject)->Release();
		*currentObject = NULL;
	}
}


// Acquires an additional reference, if non-null.
template <typename InterfaceType>
inline InterfaceType* SafeAcquire(InterfaceType* newObject)
{
	if (newObject != NULL)
		newObject->AddRef();

	return newObject;
}


// Sets a new COM object, releasing the old one.
template <typename InterfaceType>
inline void SafeSet(InterfaceType** currentObject, InterfaceType* newObject)
{
	SafeAcquire(newObject);
	SafeRelease(currentObject);
	*currentObject = newObject;
}


// Releases a COM object and nullifies pointer.
template <typename InterfaceType>
inline InterfaceType* SafeDetach(InterfaceType** currentObject)
{
	InterfaceType* oldObject = *currentObject;
	*currentObject = NULL;
	return oldObject;
}


// Sets a new COM object, acquiring the reference.
template <typename InterfaceType>
inline void SafeAttach(InterfaceType** currentObject, InterfaceType* newObject)
{
	SafeRelease(currentObject);
	*currentObject = newObject;
}

template <typename InterfaceChain>
	class ComBase : public InterfaceChain
	{
	public:
		explicit ComBase() throw()
			: refValue_(0)
		{ }

		// IUnknown interface
		IFACEMETHOD(QueryInterface)(IID const& iid, OUT void** ppObject)
		{
			*ppObject = NULL;
			InterfaceChain::QueryInterfaceInternal(iid, ppObject);
			if (*ppObject == NULL)
				return E_NOINTERFACE;

			AddRef();
			return S_OK;
		}

		IFACEMETHOD_(ULONG, AddRef)()
		{
			return InterlockedIncrement(&refValue_);
		}

		IFACEMETHOD_(ULONG, Release)()
		{
			ULONG newCount = InterlockedDecrement(&refValue_);
			if (newCount == 0)
				delete this;

			return newCount;
		}

		virtual ~ComBase()
		{ }

	protected:
		ULONG refValue_;

	private:
		// No copy construction allowed.
		ComBase(const ComBase& b);
		ComBase& operator=(ComBase const&);
	};

	struct QiListNil
	{
	};


	// When the QueryInterface list refers to itself as class,
	// which hasn't fully been defined yet.
	template <typename InterfaceName, typename InterfaceChain>
	class QiListSelf : public InterfaceChain
	{
	public:
		inline void QueryInterfaceInternal(IID const& iid, OUT void** ppObject) throw()
		{
			if (iid != __uuidof(InterfaceName))
				return InterfaceChain::QueryInterfaceInternal(iid, ppObject);

			*ppObject = static_cast<InterfaceName*>(this);
		}
	};


	// When this interface is implemented and more follow.
	template <typename InterfaceName, typename InterfaceChain = QiListNil>
	class QiList : public InterfaceName, public InterfaceChain
	{
	public:
		inline void QueryInterfaceInternal(IID const& iid, OUT void** ppObject) throw()
		{
			if (iid != __uuidof(InterfaceName))
				return InterfaceChain::QueryInterfaceInternal(iid, ppObject);

			*ppObject = static_cast<InterfaceName*>(this);
		}
	};


	// When the this is the last implemented interface in the list.
	template <typename InterfaceName>
	class QiList<InterfaceName, QiListNil> : public InterfaceName
	{
	public:
		inline void QueryInterfaceInternal(IID const& iid, OUT void** ppObject) throw()
		{
			if (iid != __uuidof(InterfaceName))
				return;

			*ppObject = static_cast<InterfaceName*>(this);
		}
	};

	inline bool IsSurrogate(UINT32 ch) throw()
	{
		// 0xD800 <= ch <= 0xDFFF
		return (ch & 0xF800) == 0xD800;
	}


	inline bool IsHighSurrogate(UINT32 ch) throw()
	{
		// 0xD800 <= ch <= 0xDBFF
		return (ch & 0xFC00) == 0xD800;
	}


	inline bool IsLowSurrogate(UINT32 ch) throw()
	{
		// 0xDC00 <= ch <= 0xDFFF
		return (ch & 0xFC00) == 0xDC00;
	}

#ifndef HINST_THISCOMPONENT
	EXTERN_C IMAGE_DOS_HEADER __ImageBase;
#define HINST_THISCOMPONENT ((HINSTANCE)&__ImageBase)
#endif

static float RandF()
{
	return (float)std::rand() / (float)(RAND_MAX);
}

static float RandF(float a, float b)
{
	return a + RandF()*(b - a);
}