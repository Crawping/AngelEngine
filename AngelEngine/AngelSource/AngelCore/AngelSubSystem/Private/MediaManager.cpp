#include"../Public/MediaManager.h"
#include"../../AngelSubSystem/Public/TimeManager.h"
#include"../../AngelSubSystem/Public/FileSystemManager.h"
#include"../../AngelSubSystem/Public/SubSystemManager.h"
#include<assert.h>
#include<mfapi.h>

bool AngelCore::AngelSubSystem::MediaManager::StartUp(...)
{
	m_play = false;
	ComPtr<IMFAttributes> spAttributes;
	

	HR(MFStartup(MF_VERSION));

	UINT resetToken;
	HR(
		MFCreateDXGIDeviceManager(&resetToken, &m_DXGIManager)
		);

	HR(
		m_DXGIManager->ResetDevice(
			AngelCore::AngelSubSystemResources::GraphicDeviceResources::Device.Get()
			, resetToken)
		);


	// Create the class factory for the Media Engine.
	HR(
		CoCreateInstance(CLSID_MFMediaEngineClassFactory, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&
			m_factory))
		);

	ComPtr<IDXGIFactory1> DXGIFactory;
	HR(
		CreateDXGIFactory1(IID_PPV_ARGS(&DXGIFactory))
		);

	ComPtr<IDXGIAdapter> spAdapter;
	(
		DXGIFactory->EnumAdapters(0, &spAdapter)
		);

	ComPtr<IDXGIOutput> spOutput;
	HR(
		spAdapter->EnumOutputs(0, &m_spDXGIOutput)
		);

	return true;
}

bool AngelCore::AngelSubSystem::MediaManager::ShutDown()
{
	return true;
}

void AngelCore::AngelSubSystem::MediaManager::Update()
{
	
}
