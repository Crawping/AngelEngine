
#include "../Public/PostProcess.h"
#include"../../../AngelTools/AngelGlobalTools.h"
#include"../../../AngelSubSystem/Public/SubSystemsResource.h"
#include"../Public/VertexTypes.h"
#include"../Public/Shader.h"
#include"../Public/RWTexture.h"
#include"../Public/LightManager.h"
#include"../../Public/SubSystemManager.h"
#include"../Public/TerrainManager.h" // for debug porpose should delete
#include<Source\Include.h>

AngelCore::AngelRendering::PostProcess * AngelCore::AngelRendering::PostProcess::m_instance = nullptr;

AngelCore::AngelRendering::PostProcess * AngelCore::AngelRendering::PostProcess::GetInstance()
{
	if (!m_instance)
	{
		m_instance = new PostProcess();
	}
	return m_instance;
}

void AngelCore::AngelRendering::PostProcess::DestroyInstance()
{
	if (m_instance)
	{
		delete m_instance;
	}
}

AngelCore::AngelRendering::PostProcess::~PostProcess()
{
	/*for (int i = 0;i < 3;i++)
	{
		m_renderTargetView[i]->Release();
		

		m_renderTargetViewTexture[i]->Release();
		m_renderTargetViewTexture[i] = nullptr;

		m_srvRtv[i]->Release();
		m_srvRtv[i] = nullptr;
	}*/

	/*COM_RELEASE(m_srvDsv);
	COM_RELEASE(m_dsv);
	COM_RELEASE(m_vertexBuffer);
	COM_RELEASE(m_indexBuffer);

	for (int i = 0; i< NUM_HELPER_TEXTURES;i++)
	{
		COM_RELEASE(m_helperTexture[i]);
		COM_RELEASE(m_helperSrv[i]);
	}
*/
	//delete					m_shader;

	/*m_cbDownSampling.Release();
	m_cbkernelTexel.Release();
	m_cbScreenSize.Release();

	delete				m_rwTexture;
	delete				m_shaderBlurHorizontal;
	delete				m_shaderBlurVertical;
	delete				m_shaderDownSample;
	delete				m_shaderDOF;
	delete				m_shaderMotionBlur;
	delete				m_shaderBrightPass;
	delete				m_shaderBloomVertical;
	delete				m_shaderBloomHorizontal;
	delete				m_shaderCombine;*/
}

void AngelCore::AngelRendering::PostProcess::Initialize(ID3D11Device1 * device)
{
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

	HR(device->CreateTexture2D(&td, nullptr, &this->m_renderTargetViewTexture[0]));
	HR(device->CreateTexture2D(&td, nullptr, &this->m_renderTargetViewTexture[2]));
	td.Format = DXGI_FORMAT_R16G16_FLOAT;
	HR(device->CreateTexture2D(&td, nullptr, &this->m_renderTargetViewTexture[1]));
	td.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	for (int i = 0;i < NUM_HELPER_TEXTURES;i++)
	{
		td.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		if (i == 1)
		{
			td.Format = DXGI_FORMAT_R16G16_FLOAT;
		}	
		HR(device->CreateTexture2D(&td, nullptr, this->m_helperTexture[i].GetAddressOf()));
	}

	D3D11_RENDER_TARGET_VIEW_DESC rtvd;
	rtvd.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	rtvd.Texture2D.MipSlice = 0;
	rtvd.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;

	HR(device->CreateRenderTargetView(this->m_renderTargetViewTexture[0],
		&rtvd, &this->m_renderTargetView[0]));
	HR(device->CreateRenderTargetView(this->m_renderTargetViewTexture[2],
		&rtvd, &this->m_renderTargetView[2]));
	rtvd.Format = DXGI_FORMAT_R16G16_FLOAT;
	HR(device->CreateRenderTargetView(this->m_renderTargetViewTexture[1],
		&rtvd, &this->m_renderTargetView[1]));

	D3D11_SHADER_RESOURCE_VIEW_DESC srvd;
	srvd.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	srvd.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvd.Texture2D.MipLevels = 1;
	srvd.Texture2D.MostDetailedMip = 0;

	HR(device->CreateShaderResourceView(this->m_renderTargetViewTexture[0],
		&srvd, &this->m_srvRtv[0]));
	HR(device->CreateShaderResourceView(this->m_renderTargetViewTexture[2],
		&srvd, &this->m_srvRtv[2]));
	srvd.Format = DXGI_FORMAT_R16G16_FLOAT;
	(device->CreateShaderResourceView(this->m_renderTargetViewTexture[1],
		&srvd, &this->m_srvRtv[1]));
	srvd.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	for (int i = 0;i < NUM_HELPER_TEXTURES;i++)
	{
		srvd.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		if (i == 1)
		{
			//We need texture1 for motion blur
			srvd.Format = DXGI_FORMAT_R16G16_FLOAT;
		}
		HR(device->CreateShaderResourceView(this->m_helperTexture[i].Get(),
			&srvd, this->m_helperSrv[i].GetAddressOf()));
	}

	D3D11_TEXTURE2D_DESC desc;

	desc.ArraySize = 1;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_DEPTH_STENCIL;
	desc.CPUAccessFlags = 0;

	desc.Format = DXGI_FORMAT_R24G8_TYPELESS;
	desc.Height = AngelCore::AngelSubSystemResources::WindowProperties::GetHeight();
	desc.MipLevels = 1;
	desc.MiscFlags = 0;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.Width = AngelCore::AngelSubSystemResources::WindowProperties::GetWidth();;

	ComPtr<ID3D11Texture2D> depthMap;
	HR(AngelCore::AngelSubSystemResources::GraphicDeviceResources::GetDevice3D()
		->CreateTexture2D(&desc, nullptr, &depthMap));

	D3D11_DEPTH_STENCIL_VIEW_DESC dsvdesc;
	dsvdesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	dsvdesc.Texture2D.MipSlice = 0;
	dsvdesc.Flags = 0;
	dsvdesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;

	HR(AngelCore::AngelSubSystemResources::GraphicDeviceResources::GetDevice3D()
		->CreateDepthStencilView(depthMap.Get(), &dsvdesc, this->m_dsv.GetAddressOf()));

	D3D11_SHADER_RESOURCE_VIEW_DESC srvdesc;
	srvdesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvdesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
	srvdesc.Texture2D.MipLevels = desc.MipLevels;
	srvdesc.Texture2D.MostDetailedMip = 0;

	HR(AngelCore::AngelSubSystemResources::GraphicDeviceResources::GetDevice3D()
		->CreateShaderResourceView(depthMap.Get(), &srvdesc, this->m_srvDsv.GetAddressOf()));
	COM_RELEASE(depthMap);

	this->m_rwTexture = new RWTexture();
	this->m_rwTexture->Load();
	this->m_shader = new AngelRendering::Shader();
	AngelRendering::Shader::LoadShader(VertexShader, "VertexPostProcess.cso",
		VertexTypes::PositionTexCoord, this->m_shader);
	AngelRendering::Shader::LoadShader(PixelShader, "PixelPostProcess.cso",
		VertexTypes::NOP, this->m_shader);


	//Load each post process technique shader
	this->m_shaderBlurHorizontal = new AngelRendering::Shader();
	AngelRendering::Shader::LoadShader(ShaderType::ComputeShader, "BlurHorizontal.cso",
		VertexTypes::NOP, this->m_shaderBlurHorizontal);
	this->m_shaderBlurVertical = new AngelRendering::Shader();
	AngelRendering::Shader::LoadShader(ComputeShader, "BlurVertical.cso",
		VertexTypes::NOP, this->m_shaderBlurVertical);
	this->m_shaderDownSample = new AngelRendering::Shader();
	AngelRendering::Shader::LoadShader(ComputeShader, "DownSample.cso",
		VertexTypes::NOP, this->m_shaderDownSample);
	this->m_shaderDOF = new AngelRendering::Shader();
	AngelRendering::Shader::LoadShader(ComputeShader, "DepthOfField.cso",
		VertexTypes::NOP, this->m_shaderDOF);
	this->m_shaderMotionBlur = new AngelRendering::Shader();
	AngelRendering::Shader::LoadShader(ComputeShader, "MotionBlur.cso",
		VertexTypes::NOP, this->m_shaderMotionBlur);
	this->m_shaderBrightPass = new AngelRendering::Shader();
	AngelRendering::Shader::LoadShader(ComputeShader, "BrightPass.cso",
		VertexTypes::NOP, this->m_shaderBrightPass);
	this->m_shaderBloomVertical = new AngelRendering::Shader();
	AngelRendering::Shader::LoadShader(ComputeShader, "BloomVertical.cso",
		VertexTypes::NOP, this->m_shaderBloomVertical);
	this->m_shaderBloomHorizontal = new AngelRendering::Shader();
	AngelRendering::Shader::LoadShader(ComputeShader, "BloomHorizontal.cso",
		VertexTypes::NOP, this->m_shaderBloomHorizontal);
	this->m_shaderCombine = new AngelRendering::Shader();
	AngelRendering::Shader::LoadShader(ComputeShader, "CombinePass.cso",
		VertexTypes::NOP, this->m_shaderCombine);

	m_cbDownSampling.Load();
	m_cbkernelTexel.Load();
	m_cbScreenSize.Load();
	InitalizeQuad();

	m_enableMotionBlur =false;
	m_enableDepthOfField = false;
	m_enableBloom = false;

	
}

void AngelCore::AngelRendering::PostProcess::CreateVRGUITexture(UINT width,UINT height)
{
	D3D11_TEXTURE2D_DESC texDesc;
	texDesc.ArraySize = 1;
	texDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	texDesc.CPUAccessFlags = 0;
	texDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	texDesc.Height = height;
	texDesc.Width = width;
	texDesc.MipLevels = 1;
	texDesc.MiscFlags = 0;
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;
	texDesc.Usage = D3D11_USAGE_DEFAULT;

	HR(AngelCore::AngelSubSystemResources::GraphicDeviceResources::Device
		->CreateTexture2D(&texDesc, NULL, &m_guiTexture));

	D3D11_SHADER_RESOURCE_VIEW_DESC srvd;
	srvd.Format = texDesc.Format;
	srvd.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvd.Texture2D.MipLevels = 1;
	srvd.Texture2D.MostDetailedMip = 0;

	HR(AngelCore::AngelSubSystemResources::GraphicDeviceResources::Device
		->CreateShaderResourceView
		(m_guiTexture.Get(), &srvd, m_guiSRV.GetAddressOf()));
}
void AngelCore::AngelRendering::PostProcess::InitalizeQuad()
{
	using namespace DirectX;
#pragma region Create Vertex Buffer

	const VertexTypesStruct::PositionTexCoord Vertices[] =
	{
		{ XMFLOAT3(1.0f, 1.0f, 0.0f), XMFLOAT2(1.0f, 0.0f) },
		{ XMFLOAT3(-1.0f, -1.0f, 0.0f), XMFLOAT2(0.0f, 1.0f) },
		{ XMFLOAT3(-1.0f, 1.0f, 0.0f), XMFLOAT2(0.0f, 0.0f) },
		{ XMFLOAT3(1.0f, -1.0f, 0.0f), XMFLOAT2(1.0f, 1.0f) },
	};

	D3D11_BUFFER_DESC vertexBufferDesc = { 0 };
	vertexBufferDesc.ByteWidth = sizeof(VertexTypesStruct::PositionTexCoord) * ARRAYSIZE(Vertices);
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA vertexBufferData;
	vertexBufferData.pSysMem = Vertices;
	vertexBufferData.SysMemPitch = 0;
	vertexBufferData.SysMemSlicePitch = 0;

	Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer;
	HRESULT hr = AngelCore::AngelSubSystemResources::GraphicDeviceResources
		::GetDevice3D()->CreateBuffer(&vertexBufferDesc, &vertexBufferData, 
			this->m_vertexBuffer.GetAddressOf());
	

#pragma endregion

#pragma region Create Index Buffer

	const unsigned short Indices[] = { 0, 1, 2, 0, 3, 1 };
	this->m_indicesSize = ARRAYSIZE(Indices);

	D3D11_BUFFER_DESC indexBufferDesc;
	indexBufferDesc.ByteWidth = sizeof(unsigned short) * ARRAYSIZE(Indices);
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	D3D11_SUBRESOURCE_DATA indexBufferData;
	indexBufferData.pSysMem = Indices;
	indexBufferData.SysMemPitch = 0;
	indexBufferData.SysMemSlicePitch = 0;

	HR(AngelCore::AngelSubSystemResources::GraphicDeviceResources
		::GetDevice3D()->CreateBuffer(&indexBufferDesc, &indexBufferData,
		this->m_indexBuffer.GetAddressOf()));

#pragma endregion

}

void AngelCore::AngelRendering::PostProcess::SetRenderTarget(
	ID3D11DeviceContext1 * deviceContext)
{
	deviceContext->OMSetRenderTargets(3, this->m_renderTargetView, m_dsv.Get());
	
}

void AngelCore::AngelRendering::PostProcess::ClearRenderTarget(
	ID3D11DeviceContext1 * deviceContext, float color[4])
{
	D3D11_VIEWPORT v;
	v.Height = AngelSubSystemResources::WindowProperties::GetHeight();
	v.MaxDepth = 1.0f;
	v.MinDepth = 0.0f;
	v.TopLeftX = 0.0f;
	v.TopLeftY = 0.0f;
	v.Width = AngelSubSystemResources::WindowProperties::GetWidth();
	AngelSubSystemResources::GraphicDeviceResources::DeviceContext->RSSetViewports(1, &v);
	deviceContext->ClearRenderTargetView(this->m_renderTargetView[0], color);
	deviceContext->ClearRenderTargetView(this->m_renderTargetView[1], color);
	deviceContext->ClearRenderTargetView(this->m_renderTargetView[2], color);
	deviceContext->ClearDepthStencilView(m_dsv.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
}

void AngelCore::AngelRendering::PostProcess::RenderCurrentFrameToTexture(
	ID3D11DeviceContext1 *deviceContext)
{
	SetRenderTarget(deviceContext);
	float color[4] = { 0.0f, 0.0f, 1.0f, 0.0f };
	ClearRenderTarget(deviceContext, color);
}
void AngelCore::AngelRendering::PostProcess::RenderFinalImage()
{
	RenderQuad();
}
void AngelCore::AngelRendering::PostProcess::RenderFinalImageCombined()
{
	RenderQuadCombined();
}
void AngelCore::AngelRendering::PostProcess::ProcessTechniques()
{
	DispatchTechniques();
}
void AngelCore::AngelRendering::PostProcess::PushTechnique(
	AngelCore::AngelRendering::PostProcess::POST_PROCESS_TECHNIQUE PPT)
{
	if (PPT == MOTION_BLUR)
	{
		if (!m_enableMotionBlur)
		{
			m_techniques.push_back(MOTION_BLUR);
			m_enableMotionBlur = true;
		}
	}
	if (PPT == DEPTH_OF_FIELD)
	{
		if (!m_enableDepthOfField)
		{
			m_techniques.push_back(DOWN_SAMPLE);
			m_techniques.push_back(DEPTH_OF_FIELD);
			m_enableDepthOfField = true;
		}
	}
	if (PPT == BLOOM)
	{
		if (!m_enableBloom)
		{
			m_techniques.push_back(DOWN_SAMPLE);
			m_techniques.push_back(BLOOM);
			m_enableBloom = true;
		}
	}
}

void AngelCore::AngelRendering::PostProcess::PopTechnique(
	AngelCore::AngelRendering::PostProcess::POST_PROCESS_TECHNIQUE PPT)
{
	if (PPT == MOTION_BLUR)
	{
		if (m_enableMotionBlur)
		{
			std::vector<AngelCore::AngelRendering::PostProcess::POST_PROCESS_TECHNIQUE> tmp;
			for (auto & member : m_techniques)
			{
				if (member != MOTION_BLUR)
					tmp.push_back(member);
			}
			m_techniques = tmp;
			m_enableMotionBlur = false;
		}
	}
	if (PPT == DEPTH_OF_FIELD)
	{
		if (m_enableDepthOfField)
		{
			int occur = 0;
			std::vector<AngelCore::AngelRendering::PostProcess::POST_PROCESS_TECHNIQUE> tmp;
			for (unsigned int i = 0;i < m_techniques.size();i++)
			{
				if (m_techniques[i] == DEPTH_OF_FIELD)
				{
					occur = i;
					break;
				}	
			}

			for (int i = 0;i < m_techniques.size();i++)
			{
				if (i== occur || i==occur-1)
				{
					continue;
				}
				tmp.push_back(m_techniques[i]);
			}
			m_techniques = tmp;
			m_enableDepthOfField = false;
		}
	}
	if (PPT == BLOOM)
	{
		if (m_enableBloom)
		{
			int occur = 0;
			std::vector<AngelCore::AngelRendering::PostProcess::POST_PROCESS_TECHNIQUE> tmp;
			for (int i = 0;i < m_techniques.size();i++)
			{
				if (m_techniques[i] == BLOOM)
				{
					occur = i;
					break;
				}
			}

			for (int i = 0;i < m_techniques.size();i++)
			{
				if (i == occur || i == occur - 1)
				{
					continue;
				}
				tmp.push_back(m_techniques[i]);
			}
			m_techniques = tmp;
			m_enableBloom = false;
		}
	}
}
void AngelCore::AngelRendering::PostProcess::DispatchTechniques()
{
	//Store orignal image
	AngelSubSystemResources::GraphicDeviceResources::DeviceContext->CopyResource(
		m_helperTexture[0].Get(), m_renderTargetViewTexture[0]);
	//Set Screen Size For All Shaders
	m_cbScreenSize.Const.size.x = AngelCore::AngelSubSystemResources::WindowProperties::GetWidth();
	m_cbScreenSize.Const.size.y = AngelCore::AngelSubSystemResources::WindowProperties::GetHeight();
	m_cbScreenSize.Const.frameRate = AngelCore::AngelSubSystem::SubSystemManager::GetInstance()->
		GetTimeManager()->GetElapsedSeconds();
	m_cbScreenSize.Update();
	this->m_shaderBloomHorizontal->SetConstantBuffer(1, 1, m_cbScreenSize.getBuffer());
	this->m_shaderBloomVertical->SetConstantBuffer(1, 1, m_cbScreenSize.getBuffer());
	this->m_shaderBlurHorizontal->SetConstantBuffer(1, 1, m_cbScreenSize.getBuffer());
	this->m_shaderBlurVertical->SetConstantBuffer(1, 1, m_cbScreenSize.getBuffer());
	this->m_shaderBrightPass->SetConstantBuffer(1, 1, m_cbScreenSize.getBuffer());
	this->m_shaderCombine->SetConstantBuffer(1, 1, m_cbScreenSize.getBuffer());
	this->m_shaderDOF->SetConstantBuffer(1, 1, m_cbScreenSize.getBuffer());
	this->m_shaderDownSample->SetConstantBuffer(1, 1, m_cbScreenSize.getBuffer());
	this->m_shaderMotionBlur->SetConstantBuffer(1, 1, m_cbScreenSize.getBuffer());
	for (unsigned int i = 0;i < m_techniques.size();i++)
	{
		if (m_techniques[i] == DEPTH_OF_FIELD)
		{
			UINT groupThreadX = static_cast<UINT>(ceilf(AngelSubSystemResources::WindowProperties::GetWidth() / 256.0f));
			UINT groupThreadY = static_cast<UINT>(ceilf(AngelSubSystemResources::WindowProperties::GetHeight()
				/ 256.0f));
			for (int i = 0;i < 2;i++)
			{
				this->m_shaderBlurHorizontal->BindSRV(0, 1, m_srvRtv[0], ShaderType::ComputeShader);
				this->m_shaderBlurHorizontal->BindUAV(0, 1, *this->m_rwTexture->GetUnorderedAccessView());
				UINT groupThreadY = static_cast<UINT>(ceilf(AngelSubSystemResources::WindowProperties::GetHeight()
					/ 256.0f));
				this->m_shaderBlurHorizontal->Dispatch(AngelSubSystemResources::WindowProperties::GetWidth(),
					groupThreadY, 1);
				this->m_shaderBlurHorizontal->UnBindSRV(0, 1, ShaderType::ComputeShader);
				this->m_shaderBlurHorizontal->UnBindUAV(0, 1);

				this->m_shaderBlurVertical->BindSRV(0, 1, m_srvRtv[0], ShaderType::ComputeShader);
				//this->m_shaderBlurVertical->BindSRV(1, 1, *this->m_rwTexture->GetShaderResourceView(), ShaderType::ComputeShader);
				this->m_shaderBlurVertical->BindUAV(0, 1, *this->m_rwTexture->GetUnorderedAccessView());

				this->m_shaderBlurVertical->Dispatch(groupThreadX, AngelSubSystemResources::WindowProperties::GetHeight(), 1);
				this->m_shaderBlurVertical->UnBindSRV(0, 1, ShaderType::ComputeShader);
				this->m_shaderBlurVertical->UnBindUAV(0, 1);

				AngelSubSystemResources::GraphicDeviceResources::DeviceContext->
					CopyResource(m_renderTargetViewTexture[0], this->m_rwTexture->GetResource().Get());
			}

			//Normal Image
			this->m_shaderDOF->BindSRV(0, 1, m_helperSrv[0].Get(), ShaderType::ComputeShader);
			//Blured Image
			this->m_shaderDOF->BindSRV(1, 1, m_srvRtv[0], ShaderType::ComputeShader);
			//Depth Image
			this->m_shaderDOF->BindSRV(2, 1, m_srvDsv.Get(), ShaderType::ComputeShader);

			this->m_shaderDOF->BindSRV(3, 1, m_srvRtv[2], ShaderType::ComputeShader);

			this->m_shaderDOF->BindUAV(0, 1, *this->m_rwTexture->GetUnorderedAccessView());
			this->m_shaderDOF->Dispatch(groupThreadX, AngelSubSystemResources::WindowProperties::GetHeight(), 1);;


			this->m_shaderDOF->UnBindSRV(0, 1, ShaderType::ComputeShader);
			this->m_shaderDOF->UnBindSRV(1, 1, ShaderType::ComputeShader);
			this->m_shaderDOF->UnBindSRV(3, 1, ShaderType::ComputeShader);
			this->m_shaderDOF->UnBindSRV(2, 1, ShaderType::ComputeShader);
			this->m_shaderDOF->UnBindUAV(0, 1);

			AngelSubSystemResources::GraphicDeviceResources::DeviceContext->
				CopyResource(m_helperTexture[0].Get(), m_rwTexture->GetResource().Get());

		}
		if (m_techniques[i] == MOTION_BLUR)
		{
			UINT groupThreadX =
				static_cast<UINT>(ceilf(AngelSubSystemResources::WindowProperties::GetWidth() / 256.0f));
			//Normal Image
			this->m_shaderMotionBlur->BindSRV(0, 1, m_srvRtv[0], ShaderType::ComputeShader);
			//Current Velocity Image
			this->m_shaderMotionBlur->BindSRV(1, 1, m_srvRtv[1], ShaderType::ComputeShader);
			//Last frame velocity Image
			this->m_shaderMotionBlur->BindSRV(2, 1, m_helperSrv[1].Get(), ShaderType::ComputeShader);
			this->m_shaderMotionBlur->SetSampler(0, 1,
				AngelSubSystemResources::GraphicDeviceResources::Samplers.at(0)
				.GetAddressOf());
			this->m_shaderMotionBlur->BindUAV(0, 1, *this->m_rwTexture->GetUnorderedAccessView());
			this->m_shaderMotionBlur->Dispatch(groupThreadX,
				AngelSubSystemResources::WindowProperties::GetHeight(), 1);;


			this->m_shaderMotionBlur->UnBindSRV(0, 1, ShaderType::ComputeShader);
			this->m_shaderMotionBlur->UnBindSRV(1, 1, ShaderType::ComputeShader);
			this->m_shaderMotionBlur->UnBindSRV(2, 1, ShaderType::ComputeShader);
			this->m_shaderMotionBlur->UnBindUAV(0, 1);

			AngelSubSystemResources::GraphicDeviceResources::DeviceContext->
				CopyResource(m_helperTexture[0].Get(), m_rwTexture->GetResource().Get());
			//Set last frame velocity

		}
		if (m_techniques[i] == DOWN_SAMPLE)
		{
			UINT groupThreadX =
				static_cast<UINT>(ceilf(AngelSubSystemResources::WindowProperties::GetWidth() / 256.0f));
			float width = AngelSubSystemResources::WindowProperties::GetWidth();
			float height = AngelSubSystemResources::WindowProperties::GetHeight();
			for (int i = 0;i < 3;i++)
			{
				DirectX::XMFLOAT4 DSoffsets[9];
				int idx = 0;
				for (int x = -1; x < 2; x++)
				{
					for (int y = -1; y < 2; y++)
					{
						DSoffsets[idx++] = DirectX::XMFLOAT4(
							static_cast<float>(x) / static_cast<float>(
								width),
							static_cast<float>(y) / static_cast<float>(
								height),
							0.0f,   //unused
							0.0f    //unused
							);

					}
				}
				width /= 2;
				height /= 2;
				for (int i = 0;i < 9;i++)
				{
					m_cbDownSampling.Const.tcDSOffsets[i] = DSoffsets[i];
				}
				m_cbDownSampling.Update();
				this->m_shaderDownSample->SetConstantBuffer(0, 1, m_cbDownSampling.getBuffer());
				this->m_shaderDownSample->BindSRV(0, 1, m_srvRtv[0], ShaderType::ComputeShader);
				this->m_shaderDownSample->BindUAV(0, 1, *m_rwTexture->GetUnorderedAccessView());
				this->m_shaderDownSample->SetSampler(0, 1,
					AngelSubSystemResources::GraphicDeviceResources::Samplers.at(0).GetAddressOf());
				this->m_shaderDownSample->Dispatch(groupThreadX,
					AngelSubSystemResources::WindowProperties::GetHeight(), 1);
				this->m_shaderDownSample->UnBindSRV(0, 1, ShaderType::ComputeShader);
				this->m_shaderDownSample->UnBindUAV(0, 1);

				AngelSubSystemResources::GraphicDeviceResources::DeviceContext->
					CopyResource(m_renderTargetViewTexture[0], this->m_rwTexture->GetResource().Get());
			}

			//m_cbDownSampling.Const.tcDSOffsets=DSoffsets;
		}
		if (m_techniques[i] == BLOOM)
		{
			DirectX::XMFLOAT2 PixelKernelV[13] =
			{
				{ 0, -6 },
				{ 0, -5 },
				{ 0, -4 },
				{ 0, -3 },
				{ 0, -2 },
				{ 0, -1 },
				{ 0,  0 },
				{ 0,  1 },
				{ 0,  2 },
				{ 0,  3 },
				{ 0,  4 },
				{ 0,  5 },
				{ 0,  6 },
			};

			DirectX::XMFLOAT2 PixelKernelH[13] =
			{
				{ -6, 0 },
				{ -5, 0 },
				{ -4, 0 },
				{ -3, 0 },
				{ -2, 0 },
				{ -1, 0 },
				{ 0, 0 },
				{ 1, 0 },
				{ 2, 0 },
				{ 3, 0 },
				{ 4, 0 },
				{ 5, 0 },
				{ 6, 0 },
			};
			for (DWORD i = 0; i < 13; ++i)
			{
				PixelKernelV[i].x = PixelKernelV[i].x /
					AngelSubSystemResources::WindowProperties::GetWidth();
				PixelKernelV[i].y = PixelKernelV[i].y /
					AngelSubSystemResources::WindowProperties::GetHeight();
			}
			for (DWORD i = 0; i < 13; ++i)
			{
				this->m_cbkernelTexel.Const.TexelKernel[i].x = PixelKernelV[i].x;
				this->m_cbkernelTexel.Const.TexelKernel[i].y = PixelKernelV[i].y;
			}
			UINT groupThreadX =
				static_cast<UINT>(ceilf(AngelSubSystemResources::WindowProperties::GetWidth() / 256.0f));
			//Down Sampled Image
			this->m_shaderBrightPass->BindSRV(0, 1, m_srvRtv[0], ComputeShader);
			this->m_shaderBrightPass->BindUAV(0, 1, *m_rwTexture->GetUnorderedAccessView());
			this->m_shaderBrightPass->SetSampler(0, 1,
				AngelSubSystemResources::GraphicDeviceResources::Samplers.at(0).GetAddressOf());
			this->m_shaderBrightPass->Dispatch(groupThreadX,
				AngelSubSystemResources::WindowProperties::GetHeight(), 1);
			this->m_shaderBrightPass->UnBindSRV(0, 1, ShaderType::ComputeShader);
			this->m_shaderBrightPass->UnBindUAV(0, 1);

			//Store Bright Pass
			AngelSubSystemResources::GraphicDeviceResources::DeviceContext->
				CopyResource(m_helperTexture[3].Get(), this->m_rwTexture->GetResource().Get());
			AngelSubSystemResources::GraphicDeviceResources::DeviceContext->
				CopyResource(m_renderTargetViewTexture[0], this->m_rwTexture->GetResource().Get());

			////Do vertical Bloom
			this->m_cbkernelTexel.Update();
			this->m_shaderBloomVertical->SetConstantBuffer(0, 1, m_cbkernelTexel.getBuffer());
			this->m_shaderBloomVertical->BindSRV(0, 1, m_srvRtv[0], ComputeShader);
			this->m_shaderBloomVertical->BindUAV(0, 1, *m_rwTexture->GetUnorderedAccessView());
			this->m_shaderBloomVertical->SetSampler(0, 1,
				AngelSubSystemResources::GraphicDeviceResources::Samplers.at(0).GetAddressOf());
			this->m_shaderBloomVertical->Dispatch(groupThreadX,
				AngelSubSystemResources::WindowProperties::GetHeight(), 1);
			this->m_shaderBloomVertical->UnBindSRV(0, 1, ShaderType::ComputeShader);
			this->m_shaderBloomVertical->UnBindUAV(0, 1);

			AngelSubSystemResources::GraphicDeviceResources::DeviceContext->
				CopyResource(m_renderTargetViewTexture[0], this->m_rwTexture->GetResource().Get());

			//Do Horizontal Bloom
			for (DWORD i = 0; i < 13; ++i)
			{
				PixelKernelH[i].x = PixelKernelH[i].x /
					AngelSubSystemResources::WindowProperties::GetWidth();
				PixelKernelH[i].y = PixelKernelH[i].y /
					AngelSubSystemResources::WindowProperties::GetHeight();
			}
			for (DWORD i = 0; i < 13; ++i)
			{
				this->m_cbkernelTexel.Const.TexelKernel[i].x = PixelKernelH[i].x;
				this->m_cbkernelTexel.Const.TexelKernel[i].y = PixelKernelH[i].y;
			}
			this->m_cbkernelTexel.Update();
			this->m_shaderBloomHorizontal->SetConstantBuffer(0, 1, m_cbkernelTexel.getBuffer());
			this->m_shaderBloomHorizontal->BindSRV(0, 1, m_srvRtv[0], ComputeShader);
			this->m_shaderBloomHorizontal->BindUAV(0, 1, *m_rwTexture->GetUnorderedAccessView());
			this->m_shaderBloomHorizontal->SetSampler(0, 1,
				AngelSubSystemResources::GraphicDeviceResources::Samplers.at(0).GetAddressOf());
			this->m_shaderBloomHorizontal->Dispatch(groupThreadX,
				AngelSubSystemResources::WindowProperties::GetHeight(), 1);
			this->m_shaderBloomHorizontal->UnBindSRV(0, 1, ShaderType::ComputeShader);
			this->m_shaderBloomHorizontal->UnBindUAV(0, 1);

			AngelSubSystemResources::GraphicDeviceResources::DeviceContext->
				CopyResource(m_renderTargetViewTexture[0], this->m_rwTexture->GetResource().Get());

			//Combine Bloomed Image With Original
			//Bloomed Image
			this->m_shaderCombine->BindSRV(0, 1, m_srvRtv[0], ComputeShader);
			//Normal Image
			this->m_shaderCombine->BindSRV(1, 1, m_helperSrv[0].Get(), ComputeShader);

			this->m_shaderCombine->BindUAV(0, 1, *m_rwTexture->GetUnorderedAccessView());
			this->m_shaderCombine->SetSampler(0, 1,
				AngelSubSystemResources::GraphicDeviceResources::Samplers.at(0).GetAddressOf());
			this->m_shaderCombine->Dispatch(groupThreadX,
				AngelSubSystemResources::WindowProperties::GetHeight(), 1);
			this->m_shaderCombine->UnBindSRV(0, 1, ShaderType::ComputeShader);
			this->m_shaderCombine->UnBindUAV(0, 1);

			AngelSubSystemResources::GraphicDeviceResources::DeviceContext->
				CopyResource(m_helperTexture[0].Get(), m_rwTexture->GetResource().Get());
		}
		AngelSubSystemResources::GraphicDeviceResources::DeviceContext->
			CopyResource(m_renderTargetViewTexture[0], this->m_rwTexture->GetResource().Get());
		AngelSubSystemResources::GraphicDeviceResources::DeviceContext->
			CopyResource(m_helperTexture[1].Get(), m_renderTargetViewTexture[1]);

	}
}
void AngelCore::AngelRendering::PostProcess::RenderQuad()
{
	float factor[4] = { 0.0f,0.0f,0.0f,0.0f };
	ID3D11DeviceContext1 *deviceContext = AngelCore::AngelSubSystemResources::GraphicDeviceResources
		::GetDeviceContext3D().Get();
	AngelCore::AngelSubSystemResources::GraphicDeviceResources::DeviceContext->
		OMSetBlendState(nullptr
			, 0,
			0xffffffff);

	this->m_shader->SetSampler(
		0, 1,
		AngelCore::AngelSubSystemResources::GraphicDeviceResources::Samplers.at(0).GetAddressOf());

	//Scene texture
	this->m_shader->BindSRV(0, 1, m_srvRtv[0], ShaderType::PixelShader);
	this->m_stride = sizeof(VertexTypesStruct::PositionTexCoord);
	this->m_offset = 0;
	this->m_shader->Apply();
	{
		deviceContext->IASetVertexBuffers(0, 1, this->m_vertexBuffer.GetAddressOf(), &this->m_stride, &this->m_offset);
		deviceContext->IASetIndexBuffer(this->m_indexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0);
		deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		deviceContext->DrawIndexed(this->m_indicesSize, 0, 0);
	}
	this->m_shader->UnBindSRV(0, 1, ShaderType::PixelShader);
	this->m_shader->UnBindSampler(0, 1);
	this->m_shader->Disable();
}

void AngelCore::AngelRendering::PostProcess::RenderQuadCombined()
{
	float factor[4] = { 0.0f,0.0f,0.0f,0.0f };
	ID3D11DeviceContext1 *deviceContext = AngelCore::AngelSubSystemResources::GraphicDeviceResources
		::GetDeviceContext3D().Get();
	AngelCore::AngelSubSystemResources::GraphicDeviceResources::DeviceContext->
		OMSetBlendState(nullptr
			, 0,
			0xffffffff);
	this->m_shader->SetSampler(
		0, 1,
		AngelCore::AngelSubSystemResources::GraphicDeviceResources::Samplers.at(0).GetAddressOf());

	//Scene texture
	this->m_shader->BindSRV(0, 1, m_srvRtv[0], ShaderType::PixelShader);
	//GUI texture
	this->m_shader->BindSRV(1, 1, m_guiSRV.Get(), ShaderType::PixelShader);

	this->m_stride = sizeof(VertexTypesStruct::PositionTexCoord);
	this->m_offset = 0;
	this->m_shader->Apply();
	{
		deviceContext->IASetVertexBuffers(0, 1, this->m_vertexBuffer.GetAddressOf(), &this->m_stride, &this->m_offset);
		deviceContext->IASetIndexBuffer(this->m_indexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0);
		deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		deviceContext->DrawIndexed(this->m_indicesSize, 0, 0);
	}
	this->m_shader->UnBindSRV(0, 1, ShaderType::PixelShader);
	this->m_shader->UnBindSampler(0, 1);
	this->m_shader->Disable();
}


void AngelCore::AngelRendering::PostProcess::DownSample()
{

}