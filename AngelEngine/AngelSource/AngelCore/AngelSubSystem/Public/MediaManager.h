#pragma once
#include"SubSystemBase.h"
#include<mfmediaengine.h>
#include<memory>

#define __MEDIA_MANAGER

namespace AngelCore
{
	namespace AngelSubSystem
	{

		struct MediaEngineNotifyCallback abstract
		{
		public:
			virtual void OnMediaEngineEvent(DWORD meEvent) = 0;
		};

		class MediaEngineNotify : public IMFMediaEngineNotify
		{
			long m_cRef;
			MediaEngineNotifyCallback* m_pCB;

		public:
			MediaEngineNotify() :  m_cRef(1), m_pCB(nullptr)
			{
			}

			STDMETHODIMP QueryInterface(REFIID riid, void** ppv)
			{
				if (__uuidof(IMFMediaEngineNotify) == riid)
				{
					*ppv = static_cast<IMFMediaEngineNotify*>(this);
				}
				else
				{
					*ppv = nullptr;
					return E_NOINTERFACE;
				}

				AddRef();

				return S_OK;
			}

			STDMETHODIMP_(ULONG) AddRef()
			{
				return InterlockedIncrement(&m_cRef);
			}

			STDMETHODIMP_(ULONG) Release()
			{
				LONG cRef = InterlockedDecrement(&m_cRef);
				if (cRef == 0)
				{
					delete this;
				}
				return cRef;
			}

			// EventNotify is called when the Media Engine sends an event.
			STDMETHODIMP EventNotify(DWORD meEvent, DWORD_PTR param1, DWORD param2)
			{
				if (meEvent == MF_MEDIA_ENGINE_EVENT_NOTIFYSTABLESTATE)
				{
					SetEvent(reinterpret_cast<HANDLE>(param1));
				}
				else
				{
					m_pCB->OnMediaEngineEvent(meEvent);
				}

				return S_OK;
			}

			void MediaEngineNotifyCallback(MediaEngineNotifyCallback* pCB)
			{
				m_pCB = pCB;
			}
		};


		class MediaManager : public ISubSystem
		{
		public:

			bool StartUp(...) override;
			bool ShutDown() override;

			void Update();

		public:
			Microsoft::WRL::ComPtr<IMFDXGIDeviceManager> GetDXGIManager() const { return m_DXGIManager; }
			Microsoft::WRL::ComPtr<IMFMediaEngineClassFactory> GetFactory() const { return m_factory; }

		private:
			Microsoft::WRL::ComPtr<IMFDXGIDeviceManager>        m_DXGIManager;
			CRITICAL_SECTION                        m_critSec;
			bool									m_play;
			Microsoft::WRL::ComPtr<IDXGIOutput>                 m_spDXGIOutput;
			Microsoft::WRL::ComPtr<IMFMediaEngineClassFactory>	m_factory;
			
		};
	}
}
