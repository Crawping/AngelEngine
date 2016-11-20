
#include "../Public/DefferedRenderTexture.h"
#include"../../AngelLog/Public/AngelLog.h"
#include"../Public/LightManager.h"
#include"../../AngelRendering/Public/BaseCamera.h"
#include"../../Public/InputManager.h"

AngelCore::AngelRendering::DefferedRenderTexture::DefferedRenderTexture()
{

}

void AngelCore::AngelRendering::DefferedRenderTexture::Initialize(ID3D11Device1 * device,int height, int width, float screenDepth, float screenNear)
{
	
	for (int i = 0;i < BUFFER_COUNT;i++)
	{
		this->m_renderTargetView[i] = nullptr;
		this->m_renderTargetViewTexture[i] = nullptr;
		this->m_shaderResourceView[i] = nullptr;
	}

	m_depthStencilBuffer = nullptr;
	m_depthStencilView = nullptr;

	HRESULT hr;
	D3D11_TEXTURE2D_DESC td;
	ZeroMemory(&td, sizeof(D3D11_TEXTURE2D_DESC));
	td.ArraySize = 1;
	td.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	td.CPUAccessFlags = 0;
	td.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	td.Height = height;
	td.MipLevels = 1;
	td.MiscFlags = 0;
	td.SampleDesc.Count = 1;
	td.SampleDesc.Quality = 0;
	td.Usage = D3D11_USAGE_DEFAULT;
	td.Width = width;
	for (int i = 0;i < BUFFER_COUNT;i++)
		hr = device->CreateTexture2D(&td, nullptr, &this->m_renderTargetViewTexture[i]);

	if (FAILED(hr))
	{
		AngelCore::AngelSubSystem::AngelLog::WriteErrorMessage("Failed creating rendertexture texture");
	}

	D3D11_RENDER_TARGET_VIEW_DESC rtvd;
	rtvd.Format = td.Format;
	rtvd.Texture2D.MipSlice = 0;
	rtvd.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;

	for (int i = 0;i < BUFFER_COUNT;i++)
		HR(device->CreateRenderTargetView(
			this->m_renderTargetViewTexture[i], &rtvd, &this->m_renderTargetView[i]));

	
	D3D11_SHADER_RESOURCE_VIEW_DESC srvd;
	srvd.Format = td.Format;
	srvd.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvd.Texture2D.MipLevels = 1;
	srvd.Texture2D.MostDetailedMip = 0;

	for (int i = 0;i < BUFFER_COUNT;i++)
		HR(device->CreateShaderResourceView(
			this->m_renderTargetViewTexture[i], &srvd, &this->m_shaderResourceView[i]));



	D3D11_TEXTURE2D_DESC depthBufferDesc;
	// Initialize the description of the depth buffer.
	ZeroMemory(&depthBufferDesc, sizeof(depthBufferDesc));

	// Set up the description of the depth buffer.
	depthBufferDesc.Width = width;
	depthBufferDesc.Height = height;
	depthBufferDesc.MipLevels = 1;
	depthBufferDesc.ArraySize = 1;
	depthBufferDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;
	depthBufferDesc.SampleDesc.Count = 1;
	depthBufferDesc.SampleDesc.Quality = 0;
	depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	depthBufferDesc.CPUAccessFlags = 0;
	depthBufferDesc.MiscFlags = 0;

	// Create the texture for the depth buffer using the filled out description.
	HR(device->CreateTexture2D(&depthBufferDesc, NULL, &m_depthStencilBuffer));

	D3D11_SHADER_RESOURCE_VIEW_DESC drvd;
	drvd.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
	drvd.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	drvd.Texture2D.MipLevels = 1;
	drvd.Texture2D.MostDetailedMip = 0;

	HR(device->CreateShaderResourceView(this->m_depthStencilBuffer, &drvd, &this->m_depthTexture));

	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
	// Initailze the depth stencil view description.
	ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));

	// Set up the depth stencil view description.
	depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Texture2D.MipSlice = 0;

	// Create the depth stencil view.
	HR(device->CreateDepthStencilView(m_depthStencilBuffer, &depthStencilViewDesc, &m_depthStencilView));
	

	// Setup the viewport for rendering.
	m_viewport.Width = (float)width;
	m_viewport.Height = (float)height;
	m_viewport.MinDepth = 0.0f;
	m_viewport.MaxDepth = 1.0f;
	m_viewport.TopLeftX = 0.0f;
	m_viewport.TopLeftY = 0.0f;

	LightManager::Light * l;
	l = new LightManager::Light();
	//l->color = DirectX::XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);
	//l->direction = DirectX::XMFLOAT3(0.5f, 0.2f, 0.2f);
	//l->position = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
	//l->type = LightManager::LIGHT_TYPE::Directional;
	//l->cbLight.Load();

	m_cbGBuffer.Load();

	InitializeQuad();

}

void AngelCore::AngelRendering::DefferedRenderTexture::InitializeQuad()
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
		::GetDevice3D()->CreateBuffer(&vertexBufferDesc, &vertexBufferData, &this->vertexBuffer);
	//ThrowIfFailed(hr);

#pragma endregion

#pragma region Create Index Buffer

	const unsigned short Indices[] = { 0, 1, 2, 0, 3, 1 };
	this->indicesSize = ARRAYSIZE(Indices);

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

	hr = AngelCore::AngelSubSystemResources::GraphicDeviceResources
		::GetDevice3D()->CreateBuffer(&indexBufferDesc, &indexBufferData, &this->indexBuffer);
	//ThrowIfFailed(hr);

#pragma endregion

	this->shader = new AngelRendering::Shader();
	AngelRendering::Shader::LoadShader(VertexShader, "VertexDefferedRendererFinal.cso", VertexTypes::PositionTexCoord, this->shader);
	AngelRendering::Shader::LoadShader(PixelShader, "PixelDefferedRendererFinal.cso", VertexTypes::NOP, this->shader);
}

void AngelCore::AngelRendering::DefferedRenderTexture::SetRenderTarget(ID3D11DeviceContext1 * deviceContext, ID3D11DepthStencilView *depthStencilView, ID3D11BlendState *blendState)
{
	deviceContext->OMSetRenderTargets(BUFFER_COUNT, m_renderTargetView, m_depthStencilView);

	// Set the viewport.
	deviceContext->RSSetViewports(1, &m_viewport);

	//deviceContext->OMSetBlendState(blendState, blendFactor, 0xffffffff);
}

void AngelCore::AngelRendering::DefferedRenderTexture::ClearRenderTarget(ID3D11DeviceContext1 * deviceContext, ID3D11DepthStencilView *depthStencilView, float color[4])
{
	// Clear the render target buffers.
	for (unsigned int i = 0; i<BUFFER_COUNT; i++)
	{
		deviceContext->ClearRenderTargetView(m_renderTargetView[i], color);
	}

	// Clear the depth buffer.
	deviceContext->ClearDepthStencilView(m_depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

}

void AngelCore::AngelRendering::DefferedRenderTexture::RenderCurrentFrameToTexture(ID3D11DeviceContext1 *deviceContext, ID3D11DepthStencilView * depthStencil,ID3D11BlendState *blendState)
{
	SetRenderTarget(deviceContext, depthStencil,blendState);
	float color[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	ClearRenderTarget(deviceContext, depthStencil, color);
	
}
void AngelCore::AngelRendering::DefferedRenderTexture::RenderLights()
{
	//LightManager::GetInstance()->Update(shader);

}

void AngelCore::AngelRendering::DefferedRenderTexture::Render()
{
	//if (AngelCore::AngelSubSystem::inputManager->IsKeyDown(AngelCore::AngelSubSystem::InputManager::
	//	Keys::RIGHT))
	//{
	//	x += amount / 100;
	//}
	//if (AngelCore::AngelSubSystem::inputManager->IsKeyDown(AngelCore::AngelSubSystem::InputManager::
	//	Keys::LEFT))
	//{
	//	x -= amount / 100;
	//}
	//if (AngelCore::AngelSubSystem::inputManager->IsKeyDown(AngelCore::AngelSubSystem::InputManager::
	//	Keys::UP))
	//{
	//	y += amount / 100;
	//}
	//if (AngelCore::AngelSubSystem::inputManager->IsKeyDown(AngelCore::AngelSubSystem::InputManager::
	//	Keys::DOWN))
	//{
	//	y -= amount / 100;
	//}
	//if (AngelCore::AngelSubSystem::inputManager->IsKeyDown(AngelCore::AngelSubSystem::InputManager::
	//	Keys::NUMPAD0))
	//{
	//	z -= amount / 100;
	//}
	//if (AngelCore::AngelSubSystem::inputManager->IsKeyDown(AngelCore::AngelSubSystem::InputManager::
	//	Keys::NUMPAD1))
	//{
	//	z += amount / 100;
	//}

	//if (AngelCore::AngelSubSystem::inputManager->IsKeyDown(AngelCore::AngelSubSystem::InputManager::
	//	Keys::P))
	//{
	//	r += amount;
	//}
	//if (AngelCore::AngelSubSystem::inputManager->IsKeyDown(AngelCore::AngelSubSystem::InputManager::
	//	Keys::O))
	//{
	//	r -= amount;
	//}

	//using namespace DirectX;
	//DirectX::XMMATRIX view =
	//	(AngelSubSystemResources::BaseCameraProperties::BCamera.GetView());
	//XMVECTOR det = XMMatrixDeterminant(view);

	//XMMATRIX invView = DirectX::XMMatrixInverse
	//	(&det
	//		, AngelSubSystemResources::BaseCameraProperties::BCamera.GetView());

	//XMStoreFloat4x4(&m_cbGBuffer.Const.ViewInv, invView);

	//XMFLOAT4 projectionParams;
	//projectionParams.x = 1 /
	//	AngelSubSystemResources::BaseCameraProperties::BCamera.GetProjection().r[0].m128_f32[0];
	//projectionParams.y = 1 /
	//	AngelSubSystemResources::BaseCameraProperties::BCamera.GetProjection().r[1].m128_f32[1];

	//projectionParams.z =
	//	AngelSubSystemResources::BaseCameraProperties::BCamera.GetProjection().r[2].m128_f32[3];

	//projectionParams.w = -
	//	AngelSubSystemResources::BaseCameraProperties::BCamera.GetProjection().r[2].m128_f32[2];


	//m_cbGBuffer.Const.PerspectiveValues = projectionParams;



	////x += 0.2f * sign;
	////y += 0.1* sign;
	////z += 0.3f* sign;

	//////r += 0.01f;

	////if (x > 10) sign = -sign;
	////if (x < -10) sign = -sign;
	//float blendFactor[] = { 0.0f, 0.0f, 0.0f, 0.0f };
	//auto context = AngelCore::AngelSubSystemResources::GraphicDeviceResources::DeviceContext;
	//context->OMSetBlendState(AngelCore::AngelSubSystemResources::GraphicDeviceResources::AdditiveBlending.Get()
	//	, blendFactor, 0xffffffff);
	//countr += sign;
	//if (countr % 250 == 0)
	//{
	//	b = !b;
	//	
	//}
	//	
	//for (int i = 0;i < MAX_LIGHT;i++)
	//{
	//	
	//	float a = (float)rand() / RAND_MAX;
	//	if (b)
	//	{
	//		LightManager::GetInstance()->GetLight(i)->PositionWS.z += a/10;

	//		float r = (float)rand() / RAND_MAX;
	//		//srand(time(NULL));
	//		float g = (float)rand() / RAND_MAX;
	//		//srand(time(NULL));
	//		float b = (float)rand() / RAND_MAX;
	//		//LightManager::GetInstance()->GetLight(i)->color = DirectX::XMFLOAT4(r, g, b, 1.0f);
	//	}
	//	if (!b)
	//	{
	//		LightManager::GetInstance()->GetLight(i)->PositionWS.z -= a/10;
	//		float r = (float)rand() / RAND_MAX;
	//		//srand(time(NULL));
	//		float g = (float)rand() / RAND_MAX;
	//		//srand(time(NULL));
	//		float b = (float)rand() / RAND_MAX;
	//		//LightManager::GetInstance()->GetLight(i)->color = DirectX::XMFLOAT4(r, g, b, 1.0f);
	//	}	

	//	
	//}
	//	LightManager::GetInstance()->GetLight(0)->position = DirectX::XMFLOAT3(x, y, z);

		//LightManager::GetInstance()->GetLight(0)->cbLight.Const.range = 1 / r;
		//LightManager::GetInstance()->GetLight(0)->color = DirectX::XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);
		ID3D11DeviceContext1 *deviceContext = AngelCore::AngelSubSystemResources::GraphicDeviceResources
			::GetDeviceContext3D().Get();
		//this->shader->SetTexture2D(0, 1, this->texture2D);
		this->shader->SetSampler(0, 1, AngelCore::AngelSubSystemResources::GraphicDeviceResources::Samplers.at(0).GetAddressOf());
		this->shader->BindSRV(0, 1, m_shaderResourceView[0], ShaderType::PixelShader);
		this->shader->BindSRV(1, 1, m_shaderResourceView[1], ShaderType::PixelShader);
		this->shader->BindSRV(2, 1, m_shaderResourceView[2], ShaderType::PixelShader);
		this->shader->BindSRV(3, 1, m_depthTexture, ShaderType::PixelShader);

		//LightManager::GetInstance()->GetLight(0)->cbLight.Update();
		//this->shader->SetConstantBuffer(0, 1, LightManager::GetInstance()->GetLight(0)->cbLight.getBuffer());

		m_cbGBuffer.Update();
		this->shader->SetConstantBuffer(1, 1, m_cbGBuffer.getBuffer());
		this->stride = sizeof(VertexTypesStruct::PositionTexCoord);
		this->offset = 0;
		this->shader->Apply();
		{
			deviceContext->IASetVertexBuffers(0, 1, this->vertexBuffer.GetAddressOf(), &this->stride, &this->offset);
			deviceContext->IASetIndexBuffer(this->indexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0);
			deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
			deviceContext->DrawIndexed(this->indicesSize, 0, 0);
		}
		this->shader->UnBindSRV(0, 1, ShaderType::PixelShader);
		this->shader->UnBindSRV(1, 1, ShaderType::PixelShader);
		this->shader->UnBindSRV(2, 1, ShaderType::PixelShader);
		this->shader->UnBindSRV(3, 1, ShaderType::PixelShader);
		this->shader->UnBindSampler(0, 1);
		this->shader->Disable();
	
	//auto context = AngelCore::AngelSubSystemResources::GraphicDeviceResources::DeviceContext;
	/*context->OMSetBlendState(nullptr
		, blendFactor, 0xffffffff);*/
}