#include"../Public/VideoRenderer.h"
#include"../Public/Transform.h"
#include"../../../Public/SubSystemManager.h"
#include"../../../Public/FileSystemManager.h"
#include"../../Public/Actor.h"//GetOwner
#include<mfapi.h>


AngelCore::AngelWorldSystem::VideoRenderer::VideoRenderer() : m_2dRendering(true) , m_3dRendering(false)
{
	SetRectangle(D2D1::RectF(0, 0, 100, 100));
}

AngelCore::AngelWorldSystem::VideoRenderer::VideoRenderer(const VideoRenderer &that)
{
	
}

AngelCore::AngelWorldSystem::VideoRenderer::VideoRenderer(VideoRenderer &&that)
{
	
}

AngelCore::AngelWorldSystem::VideoRenderer & AngelCore::AngelWorldSystem::VideoRenderer::operator=(VideoRenderer &&that)
{
	return *this;
}

AngelCore::AngelWorldSystem::VideoRenderer & AngelCore::AngelWorldSystem::VideoRenderer::operator=(const VideoRenderer &that)
{
	return *this;
}

AngelCore::AngelWorldSystem::VideoRenderer::~VideoRenderer()
{
	m_MediaEngine->Pause();
	COM_RELEASE(m_MediaEngine);
}

void AngelCore::AngelWorldSystem::VideoRenderer::Initialize(Actor* _owner, ...)
{
	m_play = false;
	ComPtr<IMFAttributes> spAttributes;
	Microsoft::WRL::ComPtr<AngelCore::AngelSubSystem::MediaEngineNotify> spNotify;
	spNotify = new AngelCore::AngelSubSystem::MediaEngineNotify();
	// Set configuration attribiutes.
	HR(
		MFCreateAttributes(&spAttributes, 1)
		);

	HR(
		spAttributes->SetUnknown(MF_MEDIA_ENGINE_DXGI_MANAGER, (IUnknown*)
			AngelCore::AngelSubSystem::SubSystemManager::GetInstance()->GetMediaManager()->
			GetDXGIManager().Get())
		);

	HR(
		spAttributes->SetUnknown(MF_MEDIA_ENGINE_CALLBACK, (IUnknown*)spNotify.Get())
		);

	HR(
		spAttributes->SetUINT32(MF_MEDIA_ENGINE_VIDEO_OUTPUT_FORMAT, DXGI_FORMAT_B8G8R8A8_UNORM)
		);
	this->m_owner = _owner;

	const DWORD flags = MF_MEDIA_ENGINE_WAITFORSTABLE_STATE;
	HR(
		AngelCore::AngelSubSystem::SubSystemManager::GetInstance()->GetMediaManager()->GetFactory()
		->CreateInstance(flags, spAttributes.Get(), &m_MediaEngine)
		);
	spNotify->MediaEngineNotifyCallback(this);

	m_sprite = new AngelGUISystem::GUI3D::AGSprite();
	m_sprite->Initialize();
	m_sprite->SetPosition(GetOwner()->GetComponent<AngelCore::AngelWorldSystem::Transform>()->GetPosition()->GetX()
		, GetOwner()->GetComponent<AngelCore::AngelWorldSystem::Transform>()->GetPosition()->GetY()
		, GetOwner()->GetComponent<AngelCore::AngelWorldSystem::Transform>()->GetPosition()->GetZ());
	m_sprite->SetScale(GetOwner()->GetComponent<AngelCore::AngelWorldSystem::Transform>()->GetScalling()->GetX()
		, GetOwner()->GetComponent<AngelCore::AngelWorldSystem::Transform>()->GetScalling()->GetY()
		, GetOwner()->GetComponent<AngelCore::AngelWorldSystem::Transform>()->GetScalling()->GetZ());
	m_sprite->SetRotation(
		GetOwner()->GetComponent<AngelCore::AngelWorldSystem::Transform>()->GetRotation()->GetX()
		, GetOwner()->GetComponent<AngelCore::AngelWorldSystem::Transform>()->GetRotation()->GetY()
		, GetOwner()->GetComponent<AngelCore::AngelWorldSystem::Transform>()->GetRotation()->GetZ());

	m_sprite->SetDefaultColor(0, 0, 0, 0);

	ComPtr<ID3D11Texture2D> spTextureDst;
	HR(
		AngelSubSystemResources::GraphicDeviceResources::SwapChain
		->GetBuffer(0, IID_PPV_ARGS(&spTextureDst))
		);
	D3D11_TEXTURE2D_DESC desc;
	spTextureDst->GetDesc(&desc);
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
	HR(AngelSubSystemResources::GraphicDeviceResources::Device->CreateTexture2D(&desc, nullptr, &m_textureDst));
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	srvDesc.Format = desc.Format;
	srvDesc.Texture2D.MipLevels = 1;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION::D3D11_SRV_DIMENSION_TEXTURE2D;
	HR(AngelSubSystemResources::GraphicDeviceResources::Device->CreateShaderResourceView(m_textureDst.Get(), &srvDesc, m_srv.GetAddressOf()));

	using namespace DirectX;
	{
		m_buffersInitialized = true;
		D3D11_BUFFER_DESC vertexBufferDesc = { 0 };
		vertexBufferDesc.ByteWidth = sizeof(VertexTypesStruct::PositionTexCoord) *(4);
		vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		vertexBufferDesc.MiscFlags = 0;
		vertexBufferDesc.StructureByteStride = 0;

		Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer;
		HRESULT hr = AngelCore::AngelSubSystemResources::GraphicDeviceResources
			::GetDevice3D()->CreateBuffer(&vertexBufferDesc, nullptr, &this->m_vertexBufferImage);


		this->m_indicesSizeImage = 6;

		D3D11_BUFFER_DESC indexBufferDesc;
		indexBufferDesc.ByteWidth = sizeof(unsigned short) * (6);
		indexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		indexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		indexBufferDesc.MiscFlags = 0;
		indexBufferDesc.StructureByteStride = 0;


		HR(AngelCore::AngelSubSystemResources::GraphicDeviceResources
			::GetDevice3D()->CreateBuffer(&indexBufferDesc, nullptr, &this->m_indexBufferImage));

		//Initialize Textures and RenderTargetViews
		D3D11_TEXTURE2D_DESC td;
		ZeroMemory(&td, sizeof(D3D11_TEXTURE2D_DESC));
		td.ArraySize = 1;
		td.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
		td.CPUAccessFlags = 0;
		td.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		td.Height = AngelCore::AngelSubSystemResources::WindowProperties::GetHeight();
		td.MipLevels = 1;
		td.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;
		td.SampleDesc.Count = 1;
		td.SampleDesc.Quality = 0;
		td.Usage = D3D11_USAGE_DEFAULT;
		td.Width = AngelCore::AngelSubSystemResources::WindowProperties::GetWidth();

		HR(
			AngelCore::AngelSubSystemResources::GraphicDeviceResources::Device
			->CreateTexture2D(&td, nullptr, &this->m_renderTargetViewTexture));

		D3D11_RENDER_TARGET_VIEW_DESC rtvd;
		rtvd.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		rtvd.Texture2D.MipSlice = 0;
		rtvd.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;

		HR(AngelCore::AngelSubSystemResources::GraphicDeviceResources::Device
			->CreateRenderTargetView(this->m_renderTargetViewTexture,
				&rtvd, &this->m_renderTargetView));

		D3D11_SHADER_RESOURCE_VIEW_DESC srvd;
		srvd.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		srvd.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvd.Texture2D.MipLevels = 1;
		srvd.Texture2D.MostDetailedMip = 0;

		HR(AngelCore::AngelSubSystemResources::GraphicDeviceResources::Device
			->CreateShaderResourceView(this->m_renderTargetViewTexture,
				&srvd, &this->m_srvRtv));


		m_shaderImage = new AngelRendering::Shader();
		AngelRendering::Shader::LoadShader(ShaderType::VertexShader, "VertexGUI.cso", VertexTypes::PositionTexCoord, m_shaderImage);
		AngelRendering::Shader::LoadShader(ShaderType::PixelShader, "PixelGUI.cso", VertexTypes::NOP, m_shaderImage);
	}
	const VertexTypesStruct::PositionTexCoord VerticesImageData[] =
	{
		{ XMFLOAT3(1.0f, 1.0f, 0.0f), XMFLOAT2(1.0f, 0.0f) },
		{ XMFLOAT3(-1.0f, -1.0f, 0.0f), XMFLOAT2(0.0f, 1.0f) },
		{ XMFLOAT3(-1.0f, 1.0f, 0.0f), XMFLOAT2(0.0f, 0.0f) },
		{ XMFLOAT3(1.0f, -1.0f, 0.0f), XMFLOAT2(1.0f, 1.0f) },
	};

	VertexTypesStruct::PositionTexCoord * verticesImage = new VertexTypesStruct::PositionTexCoord[4];
	for (int i = 0;i < 4;i++)
	{
		verticesImage[i].pos = VerticesImageData[i].pos;
		verticesImage[i].uv = VerticesImageData[i].uv;
	}

	const unsigned short IndicesImage[] = { 0, 1, 2, 0, 3, 1 };

	D3D11_MAPPED_SUBRESOURCE vtx_resource, idx_resource;
	HR(AngelCore::AngelSubSystemResources::GraphicDeviceResources::DeviceContext
		->Map(m_vertexBufferImage.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &vtx_resource));

	HR(AngelCore::AngelSubSystemResources::GraphicDeviceResources::DeviceContext
		->Map(m_indexBufferImage.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &idx_resource));


	VertexTypesStruct::PositionTexCoord * vertices = (VertexTypesStruct::PositionTexCoord *)vtx_resource.pData;
	unsigned short * indices = (unsigned short *)idx_resource.pData;

	memcpy(vertices, &verticesImage[0], 4 * sizeof(VertexTypesStruct::PositionTexCoord));
	memcpy(indices, &IndicesImage[0], 6 * sizeof(unsigned short));
	AngelCore::AngelSubSystemResources::GraphicDeviceResources::DeviceContext->Unmap(m_vertexBufferImage.Get(), 0);
	AngelCore::AngelSubSystemResources::GraphicDeviceResources::DeviceContext->Unmap(m_indexBufferImage.Get(), 0);
	delete[] verticesImage;

	m_cbVertexTranslation.Load();
}

void AngelCore::AngelWorldSystem::VideoRenderer::Update(const AngelSubSystem::TimeManager &timer)
{
	if(m_play)
	{
		//if (SUCCEEDED(m_spDXGIOutput->WaitForVBlank()))
		{
			LONGLONG pts;
			if (m_MediaEngine->OnVideoStreamTick(&pts) == S_OK)
			{

				RECT r;
				r.left = 0;
				r.top = 0;
				r.right = AngelCore::AngelSubSystemResources::WindowProperties::GetWidth();
				r.bottom = AngelCore::AngelSubSystemResources::WindowProperties::GetHeight();
				MFARGB          m_bkgColor;
				m_bkgColor.rgbAlpha = 0;
				HR(
					m_MediaEngine->TransferVideoFrame(m_textureDst.Get(), nullptr,
						&r, &m_bkgColor)
					);
				if(m_3dRendering)
					m_sprite->SetResource(m_srv.Get());
			}
		}
	}

	m_sprite->SetPosition(GetOwner()->GetComponent<AngelCore::AngelWorldSystem::Transform>()->GetPosition()->GetX()
		, GetOwner()->GetComponent<AngelCore::AngelWorldSystem::Transform>()->GetPosition()->GetY()
		, GetOwner()->GetComponent<AngelCore::AngelWorldSystem::Transform>()->GetPosition()->GetZ());
	m_sprite->SetScale(GetOwner()->GetComponent<AngelCore::AngelWorldSystem::Transform>()->GetScalling()->GetX()
		, GetOwner()->GetComponent<AngelCore::AngelWorldSystem::Transform>()->GetScalling()->GetY()
		, GetOwner()->GetComponent<AngelCore::AngelWorldSystem::Transform>()->GetScalling()->GetZ());
	m_sprite->SetRotation(
		GetOwner()->GetComponent<AngelCore::AngelWorldSystem::Transform>()->GetRotation()->GetX()
		, GetOwner()->GetComponent<AngelCore::AngelWorldSystem::Transform>()->GetRotation()->GetY()
		, GetOwner()->GetComponent<AngelCore::AngelWorldSystem::Transform>()->GetRotation()->GetZ());
}

void AngelCore::AngelWorldSystem::VideoRenderer::Render()
{
	m_cbVertexTranslation.Update();
	m_shaderImage->SetConstantBuffer(0, 1, m_cbVertexTranslation.getBuffer());

	m_shaderImage->SetSampler(0, 1,
		AngelCore::AngelSubSystemResources::GraphicDeviceResources::Samplers.at(0).GetAddressOf());
	m_shaderImage->BindSRV(0, 1, m_srv.Get(), ShaderType::PixelShader);
	UINT stride = sizeof(VertexTypesStruct::PositionTexCoord);
	UINT offset = 0;
	AngelCore::AngelSubSystemResources::GraphicDeviceResources::DeviceContext->
		IASetIndexBuffer(m_indexBufferImage.Get()
			, DXGI_FORMAT_R16_UINT, 0);

	AngelCore::AngelSubSystemResources::GraphicDeviceResources::DeviceContext->
		IASetVertexBuffers(0, 1, m_vertexBufferImage.GetAddressOf()
			, &stride, &offset);

	AngelCore::AngelSubSystemResources::GraphicDeviceResources::DeviceContext->
		IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	m_shaderImage->Apply();

	if(m_2dRendering)
		AngelCore::AngelSubSystemResources::GraphicDeviceResources::DeviceContext->
			DrawIndexed(m_indicesSizeImage, 0, 0);
}

void AngelCore::AngelWorldSystem::VideoRenderer::OnMediaEngineEvent(DWORD meEvent)
{
	switch (meEvent)
	{
	case MF_MEDIA_ENGINE_EVENT_LOADEDMETADATA:
	{
		//m_fEOS = FALSE;
	}
	break;
	case MF_MEDIA_ENGINE_EVENT_CANPLAY:
	{
		m_play = true;
		HR(m_MediaEngine->Play());
		m_MediaEngine->SetLoop(true);
	}
	break;
	case MF_MEDIA_ENGINE_EVENT_PLAY:
		//m_fPlaying = TRUE;
		break;
	case MF_MEDIA_ENGINE_EVENT_PAUSE:
		//m_fPlaying = FALSE;
		break;
	case MF_MEDIA_ENGINE_EVENT_ENDED:
		break;
	case MF_MEDIA_ENGINE_EVENT_TIMEUPDATE:
		break;
	case MF_MEDIA_ENGINE_EVENT_ERROR:
		break;
	case MF_MEDIA_ENGINE_EVENT_LOADEDDATA:
		//

		//static float dur = m_MediaEngine->GetDuration();
		//OutputDebugStringA(std::to_string(dur).c_str());
		break;
	case MF_MEDIA_ENGINE_EVENT_LOADSTART:
		break;
	}

	return;
}

void AngelCore::AngelWorldSystem::VideoRenderer::Load(const wchar_t * name)
{
	std::wstring path = AngelCore::AngelSubSystem::SubSystemManager::GetInstance()
		->GetFileSystemManager()->GetContentDirectory();
	path += L"\\AngelData\\Video\\";
	path += name;
	wchar_t * pathW = new wchar_t[path.size() + 1];
	for (unsigned int i = 0;i < path.size();i++)
		pathW[i] = path.at(i);
	pathW[path.size()] = L'\0';
	HR(m_MediaEngine->SetSource(pathW));
	HR(m_MediaEngine->Load());
}

void AngelCore::AngelWorldSystem::VideoRenderer::Load(std::string &name)
{
	std::wstring path = AngelCore::AngelSubSystem::SubSystemManager::GetInstance()
		->GetFileSystemManager()->GetContentDirectory();
	std::wstring nameW(name.begin(), name.end());
	path += L"\\AngelData\\Video\\";
	path += nameW;
	wchar_t * pathW = new wchar_t[path.size() + 1];
	for (unsigned int i = 0;i < path.size();i++)
		pathW[i] = path.at(i);
	pathW[path.size()] = L'\0';
	HR(m_MediaEngine->SetSource(pathW));
	HR(m_MediaEngine->Load());
}

void AngelCore::AngelWorldSystem::VideoRenderer::SetRectangle(D2D1_RECT_F rect)
{
	float width = AngelCore::AngelSubSystemResources::WindowProperties::GetWidth();
	float height = AngelCore::AngelSubSystemResources::WindowProperties::GetHeight();
	float leftTopX = rect.left / width + rect.left / width;
	float rightBottomX = (width - rect.right) / width + (width - rect.right) / width;
	float leftTopY = rect.top / height + rect.top / height;
	float rightBottomY = (height - rect.bottom) / height + (height - rect.bottom) / height;

	DirectX::XMStoreFloat4x4(&m_cbVertexTranslation.Const.vertexNegative1Negative1Tranlation,
		DirectX::XMMatrixTranslation(+leftTopX, +rightBottomY, 0.0f));
	DirectX::XMStoreFloat4x4(&m_cbVertexTranslation.Const.vertexNegative1Positive1Tranlation,
		DirectX::XMMatrixTranslation(+leftTopX, -leftTopY, 0.0f));
	DirectX::XMStoreFloat4x4(&m_cbVertexTranslation.Const.vertexPositive1Negative1Tranlation,
		DirectX::XMMatrixTranslation(-rightBottomX, +rightBottomY, 0.0f));
	DirectX::XMStoreFloat4x4(&m_cbVertexTranslation.Const.vertexPositive1Positive1Tranlation,
		DirectX::XMMatrixTranslation(-rightBottomX, -leftTopY, 0.0f));
}

void AngelCore::AngelWorldSystem::VideoRenderer::SetRectangle(float &l, float &t, float &r, float &b)
{
	float width = AngelCore::AngelSubSystemResources::WindowProperties::GetWidth();
	float height = AngelCore::AngelSubSystemResources::WindowProperties::GetHeight();
	float leftTopX = l / width + l / width;
	float rightBottomX = (width - r) / width + (width - r) / width;
	float leftTopY = t / height + t / height;
	float rightBottomY = (height - b) / height + (height - b) / height;

	DirectX::XMStoreFloat4x4(&m_cbVertexTranslation.Const.vertexNegative1Negative1Tranlation,
		DirectX::XMMatrixTranslation(+leftTopX, +rightBottomY, 0.0f));
	DirectX::XMStoreFloat4x4(&m_cbVertexTranslation.Const.vertexNegative1Positive1Tranlation,
		DirectX::XMMatrixTranslation(+leftTopX, -leftTopY, 0.0f));
	DirectX::XMStoreFloat4x4(&m_cbVertexTranslation.Const.vertexPositive1Negative1Tranlation,
		DirectX::XMMatrixTranslation(-rightBottomX, +rightBottomY, 0.0f));
	DirectX::XMStoreFloat4x4(&m_cbVertexTranslation.Const.vertexPositive1Positive1Tranlation,
		DirectX::XMMatrixTranslation(-rightBottomX, -leftTopY, 0.0f));
}