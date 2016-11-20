#include"../Public/LightManager.h"
#include"../../Public/TimeManager.h"
#include<time.h>
#include"../../../AngelCore/AngelSubSystem/AngelRendering/Public/BaseCamera.h"
#include"../../Public/InputManager.h"
#include"../../Public/SubSystemManager.h"
#include"../Public/TerrainManager.h"
#define Radius 500
AngelCore::AngelRendering::LightManager *		AngelCore::AngelRendering::LightManager::m_instance = nullptr;

AngelCore::AngelRendering::LightManager * AngelCore::AngelRendering::LightManager::GetInstance()
{
	if (!m_instance)
	{
		m_instance = new LightManager();
		return m_instance;
	}

	return m_instance;
}

void AngelCore::AngelRendering::LightManager::Initialize()
{
	for (unsigned int i = 0;i <MAX_LIGHT;i++)
	{
		Light * l = new Light();
		m_cbLight.Const.l[i] = *l;
	}
#pragma region Cubic
	//
	// Cubemap is a special texture array with 6 elements.
	//
	const int CubeMapSize = 1024;

	//
	// Cubemap is a special texture array with 6 elements.
	//

	D3D11_TEXTURE2D_DESC texDesc;
	texDesc.Width = CubeMapSize;
	texDesc.Height = CubeMapSize;
	texDesc.MipLevels = 0;
	texDesc.ArraySize = 6;
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;
	texDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	texDesc.Usage = D3D11_USAGE_DEFAULT;
	texDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	texDesc.CPUAccessFlags = 0;
	texDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS | D3D11_RESOURCE_MISC_TEXTURECUBE;

	Microsoft::WRL::ComPtr<ID3D11Texture2D> cubeTex;
	HR(AngelCore::AngelSubSystemResources::GraphicDeviceResources::GetDevice3D()
		->CreateTexture2D(&texDesc, 0, cubeTex.GetAddressOf()));

	//
	// Create a render target view to each cube map face 
	// (i.e., each element in the texture array).
	// 

	D3D11_RENDER_TARGET_VIEW_DESC rtvDesc;
	rtvDesc.Format = texDesc.Format;
	rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DARRAY;
	rtvDesc.Texture2DArray.ArraySize = 1;
	rtvDesc.Texture2DArray.MipSlice = 0;

	for (int i = 0; i < 6; ++i)
	{
		rtvDesc.Texture2DArray.FirstArraySlice = i;
		HR(AngelCore::AngelSubSystemResources::GraphicDeviceResources::GetDevice3D()
			->CreateRenderTargetView(cubeTex.Get(), &rtvDesc, &m_Rtv[i]));
	}

	//
	// Create a shader resource view to the cube map.
	//

	/*D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	srvDesc.Format = texDesc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
	srvDesc.TextureCube.MostDetailedMip = 0;
	srvDesc.TextureCube.MipLevels = -1;

	HR(md3dDevice->CreateShaderResourceView(cubeTex, &srvDesc, &mDynamicCubeMapSRV));*/

	COM_RELEASE(cubeTex);

	//
	// We need a depth texture for rendering the scene into the cubemap
	// that has the same resolution as the cubemap faces.  
	//

	D3D11_TEXTURE2D_DESC depthTexDesc;
	depthTexDesc.Width = CubeMapSize;
	depthTexDesc.Height = CubeMapSize;
	depthTexDesc.MipLevels = 1;
	depthTexDesc.ArraySize = 6;
	depthTexDesc.SampleDesc.Count = 1;
	depthTexDesc.SampleDesc.Quality = 0;
	depthTexDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;
	depthTexDesc.Usage = D3D11_USAGE_DEFAULT;
	depthTexDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	depthTexDesc.CPUAccessFlags = 0;
	depthTexDesc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;

	Microsoft::WRL::ComPtr<ID3D11Texture2D> depthTex;
	HR(AngelCore::AngelSubSystemResources::GraphicDeviceResources::GetDevice3D()
		->CreateTexture2D(&depthTexDesc, 0, &depthTex));

	// Create the depth stencil view for the entire cube
	D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc;
	dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	dsvDesc.Flags = 0;
	dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DARRAY;
	dsvDesc.Texture2DArray.ArraySize = 1;
	dsvDesc.Texture2DArray.MipSlice = 1;
	dsvDesc.Texture2D.MipSlice = 0;
	for (int i = 0; i < 6; ++i)
	{
		dsvDesc.Texture2DArray.FirstArraySlice = i;
		HR(AngelCore::AngelSubSystemResources::GraphicDeviceResources::GetDevice3D()
			->CreateDepthStencilView(depthTex.Get(), &dsvDesc, &m_Dsv[i]));
	}

	D3D11_SHADER_RESOURCE_VIEW_DESC dsrvDesc;
	dsrvDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
	dsrvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
	dsrvDesc.TextureCube.MostDetailedMip = 0;
	dsrvDesc.TextureCube.MipLevels = -1;

	HR(AngelCore::AngelSubSystemResources::GraphicDeviceResources::GetDevice3D()
		->CreateShaderResourceView(depthTex.Get(), &dsrvDesc, &m_SrvDsv));

	COM_RELEASE(depthTex);
#pragma endregion

	const int shadowMapSize = 8192;

#pragma region Directional And Spot
	this->m_viewPort.Height = shadowMapSize;
	this->m_viewPort.MaxDepth = 1.0f;
	this->m_viewPort.MinDepth = 0.0f;
	this->m_viewPort.TopLeftX = 0.0f;
	this->m_viewPort.TopLeftY = 0.0f;
	this->m_viewPort.Width = shadowMapSize;


	D3D11_TEXTURE2D_DESC desc;
	desc.ArraySize = 1;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_DEPTH_STENCIL;
	desc.CPUAccessFlags = 0;

	desc.Format = DXGI_FORMAT_R24G8_TYPELESS;
	desc.Height = shadowMapSize;
	desc.MipLevels = 1;
	desc.MiscFlags = 0;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.Width = shadowMapSize;;

	ID3D11Texture2D * depthMap;
	HR(AngelCore::AngelSubSystemResources::GraphicDeviceResources::GetDevice3D()
		->CreateTexture2D(&desc, nullptr, &depthMap));

	D3D11_DEPTH_STENCIL_VIEW_DESC dsvdesc;
	dsvdesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	dsvdesc.Texture2D.MipSlice = 0;
	dsvdesc.Flags = 0;
	dsvdesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;

	HR(AngelCore::AngelSubSystemResources::GraphicDeviceResources::GetDevice3D()
		->CreateDepthStencilView(depthMap, &dsvdesc, &this->m_DsvDirectional));
	HR(AngelCore::AngelSubSystemResources::GraphicDeviceResources::GetDevice3D()
		->CreateDepthStencilView(depthMap, &dsvdesc, &this->m_DsvSpot));

	D3D11_SHADER_RESOURCE_VIEW_DESC srvdesc;
	srvdesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvdesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
	srvdesc.Texture2D.MipLevels = desc.MipLevels;
	srvdesc.Texture2D.MostDetailedMip = 0;

	HR(AngelCore::AngelSubSystemResources::GraphicDeviceResources::GetDevice3D()
		->CreateShaderResourceView(depthMap, &srvdesc, &this->m_SrvDirectional));
	HR(AngelCore::AngelSubSystemResources::GraphicDeviceResources::GetDevice3D()
		->CreateShaderResourceView(depthMap, &srvdesc, &this->m_SrvSpot));
#pragma endregion

	
}
void AngelCore::AngelRendering::LightManager::BuildCubicShadowMap(float x, float y, float z)
{
	using namespace DirectX;
	XMFLOAT3 center(x, y, z);
	XMFLOAT3 worldUp(0.0f, 1.0f, 0.0f);
	XMFLOAT3 targets[6] =
	{
		XMFLOAT3(x + 1.0f, y, z), // +X
		XMFLOAT3(x - 1.0f, y, z), // -X
		XMFLOAT3(x, y + 1.0f, z), // +Y
		XMFLOAT3(x, y - 1.0f, z), // -Y
		XMFLOAT3(x, y, z + 1.0f), // +Z
		XMFLOAT3(x, y, z - 1.0f)  // -Z
	};

	// Use world up vector (0,1,0) for all directions except +Y/-Y.  In these cases, we
	// are looking down +Y or -Y, so we need a different "up" vector.
	XMFLOAT3 ups[6] =
	{
		XMFLOAT3(0.0f, 1.0f, 0.0f),  // +X
		XMFLOAT3(0.0f, 1.0f, 0.0f),  // -X
		XMFLOAT3(0.0f, 0.0f, -1.0f), // +Y
		XMFLOAT3(0.0f, 0.0f, +1.0f), // -Y
		XMFLOAT3(0.0f, 1.0f, 0.0f),	 // +Z
		XMFLOAT3(0.0f, 1.0f, 0.0f)	 // -Z
	};

	for (int i = 0;i < 6;i++)
	{
		XMStoreFloat4x4(&m_views[i],XMMatrixLookAtRH(
			XMVectorSet(center.x
			, center.y,
			center.z, 
				1.0f),
			XMVectorSet(targets[i].x
				, targets[i].y,
				targets[i].z,
				1.0f),
			XMVectorSet(ups[i].x,
				ups[i].y, ups[i].z, 
				1.0f)));
		XMStoreFloat4x4(&m_projections[i]
			,
			XMMatrixPerspectiveFovRH(0.5*XM_PI,
				1.0f, 0.1f, 1000.0f));
	}
}

void AngelCore::AngelRendering::LightManager::BuildOrthographicShadowMap(int i)
{
	using namespace DirectX;
	XMVECTOR lightPos = XMVectorSet(this->m_lights.at(i)->PositionWS.x,
		this->m_lights.at(i)->PositionWS.y,
		this->m_lights.at(i)->PositionWS.z - 5, 0.0f);;
	lightPos = XMVectorMultiply(XMVectorSet
		(this->m_lights.at(i)->PositionWS.x,
			this->m_lights.at(i)->PositionWS.y, 
			this->m_lights.at(i)->PositionWS.z, 0.0f),
		XMVectorSet(this->m_lights.at(i)->DirectionWS.x, this->m_lights.at(i)->DirectionWS.y,
			this->m_lights.at(i)->DirectionWS.z, 0.0f));

	XMVECTOR lightTarget = XMLoadFloat3(&XMFLOAT3(0.0f, 0.0f, 0.0f));
	XMVECTOR lightUp = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	XMMATRIX V = XMMatrixLookAtRH(lightPos, lightTarget, lightUp);
	XMMATRIX P = XMMatrixIdentity();
	XMFLOAT3 sphereCenterLS;
	XMStoreFloat3(&sphereCenterLS, XMVector3TransformCoord(lightTarget, V));

	float  l = sphereCenterLS.x - Radius;
	float  b = sphereCenterLS.y - Radius;
	float  n = sphereCenterLS.z - Radius;
	float  r = sphereCenterLS.x + Radius;
	float  t = sphereCenterLS.y + Radius;
	float  f = sphereCenterLS.z + 2*Radius;

	P = XMMatrixOrthographicOffCenterRH(l, r, b, t, n, f);

	XMStoreFloat4x4(&m_views[0], V);
	XMStoreFloat4x4(&m_projections[0], P);
}

void AngelCore::AngelRendering::LightManager::BuildPerspectiveShadowMap(int i)
{
	using namespace DirectX;
	//mLightView = g_Obj[2].m_mWorld;
	//XMFLOAT3 vPos(m_lights[0].PositionWS.x
	//	._41, mLightView._42, mLightView._43);  // Offset z by -2 so that it's closer to headlight
	//XMFLOAT4 Dir = XMFLOAT4(0.0f, 0.0f, -1.0f, 1.0f);  // In object space, car is facing -Z
	//mLightView._41 = mLightView._42 = mLightView._43 = 0.0f;  // Remove the translation
	//D3DXVec4Transform(&vDir, &vDir, &mLightView);  // Obtain direction in world space
	//vDir.w = 0.0f;  // Set w 0 so that the translation part below doesn't come to play
	//D3DXVec4Normalize(&vDir, &vDir);
	//vPos.x += vDir.x * 4.0f;  // Offset the center by 4 so that it's closer to the headlight
	//vPos.y += vDir.y * 4.0f;
	//vPos.z += vDir.z * 4.0f;
	//vDir.x += vPos.x;  // vDir denotes the look-at point
	//vDir.y += vPos.y;
	//vDir.z += vPos.z;
	//D3DXVECTOR3 vUp(0.0f, 1.0f, 0.0f);
	//D3DXMatrixLookAtLH(&mLightView, &vPos, (D3DXVECTOR3*)&vDir, &vUp);
}
void AngelCore::AngelRendering::LightManager::RenderLightsShadowMap()
{
	for (unsigned int i = 0;i < m_lights.size();i++)
	{
		if (m_lights.at(i)->castShadow == 1 && m_lights.at(i)->Type == 0)
		{
			DirectX::XMMATRIX view =  
				AngelCore::AngelSubSystemResources::BaseCameraProperties::BCamera->GetView();
			DirectX::XMMATRIX proj =
				AngelCore::AngelSubSystemResources::BaseCameraProperties::BCamera->GetProjection();
			for (int j = 0;j < 6;j++)
			{
				AngelCore::AngelSubSystemResources::GraphicDeviceResources::GetDeviceContext3D()->
					OMSetRenderTargets(1,
						m_Rtv[j].GetAddressOf()
						, m_Dsv[j].Get());
				AngelCore::AngelSubSystemResources::GraphicDeviceResources::GetDeviceContext3D()
					->ClearDepthStencilView(m_Dsv[j].Get()
						, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL
						, 1.0f, 0);
				BuildCubicShadowMap(
					m_lights.at(i)->PositionWS.x,
					m_lights.at(i)->PositionWS.y,
					m_lights.at(i)->PositionWS.z);
				AngelCore::AngelSubSystemResources::BaseCameraProperties::BCamera->SetView(m_views[j]);
				AngelCore::AngelSubSystemResources::BaseCameraProperties::BCamera->SetProjection(m_projections[j]);
				AngelCore::AngelSubSystem::SubSystemManager::GetInstance()->GetActorFactory()->RenderGeometry();
			}
			AngelCore::AngelSubSystemResources::BaseCameraProperties::BCamera->SetView(view);
			AngelCore::AngelSubSystemResources::BaseCameraProperties::BCamera->SetProjection(proj);
		}
		if (m_lights.at(i)->castShadow == 1 && m_lights.at(i)->Type == 2)
		{
			DirectX::XMMATRIX view =
				AngelCore::AngelSubSystemResources::BaseCameraProperties::BCamera->GetView();
			DirectX::XMMATRIX proj =
				AngelCore::AngelSubSystemResources::BaseCameraProperties::BCamera->GetProjection();
			BuildOrthographicShadowMap(i);
			ID3D11RenderTargetView *nullRenderTarget[1] = { 0 };
			AngelCore::AngelSubSystemResources::GraphicDeviceResources::GetDeviceContext3D()->RSSetViewports(1
				, &m_viewPort);
			AngelCore::AngelSubSystemResources::GraphicDeviceResources::GetDeviceContext3D()->
				OMSetRenderTargets(1,
					nullRenderTarget
					, m_DsvDirectional.Get());
			AngelCore::AngelSubSystemResources::GraphicDeviceResources::GetDeviceContext3D()
				->ClearDepthStencilView(m_DsvDirectional.Get()
					, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL
					, 1.0f, 0);
			AngelCore::AngelSubSystemResources::BaseCameraProperties::BCamera->SetView(m_views[0]);
			AngelCore::AngelSubSystemResources::BaseCameraProperties::BCamera->SetProjection(m_projections[0]);
			//AngelRendering::TerrainManager::GetInstance()->Update();
			//AngelRendering::TerrainManager::GetInstance()->Render();
			AngelCore::AngelSubSystem::SubSystemManager::GetInstance()->GetActorFactory()->RenderGeometry();
		
			AngelCore::AngelSubSystemResources::BaseCameraProperties::BCamera->SetView(view);
			AngelCore::AngelSubSystemResources::BaseCameraProperties::BCamera->SetProjection(proj);
		}

		if (m_lights.at(i)->castShadow == 1 && m_lights.at(i)->Type == 1)
		{
			DirectX::XMMATRIX view =
				AngelCore::AngelSubSystemResources::BaseCameraProperties::BCamera->GetView();
			DirectX::XMMATRIX proj =
				AngelCore::AngelSubSystemResources::BaseCameraProperties::BCamera->GetProjection();
			BuildPerspectiveShadowMap(i);
			ID3D11RenderTargetView *nullRenderTarget[1] = { 0 };
			AngelCore::AngelSubSystemResources::GraphicDeviceResources::GetDeviceContext3D()->RSSetViewports(1
				, &m_viewPort);
			AngelCore::AngelSubSystemResources::GraphicDeviceResources::GetDeviceContext3D()->
				OMSetRenderTargets(1,
					nullRenderTarget
					, m_DsvSpot.Get());
			AngelCore::AngelSubSystemResources::GraphicDeviceResources::GetDeviceContext3D()
				->ClearDepthStencilView(m_DsvSpot.Get()
					, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL
					, 1.0f, 0);
			AngelCore::AngelSubSystemResources::BaseCameraProperties::BCamera->SetView(m_views[0]);
			AngelCore::AngelSubSystemResources::BaseCameraProperties::BCamera->SetProjection(proj);
			AngelCore::AngelSubSystem::SubSystemManager::GetInstance()->GetActorFactory()->RenderGeometry();
			AngelCore::AngelSubSystemResources::BaseCameraProperties::BCamera->SetView(view);
			AngelCore::AngelSubSystemResources::BaseCameraProperties::BCamera->SetProjection(proj);
		}
	}
}

void AngelCore::AngelRendering::LightManager::AddLight(
	AngelCore::AngelWorldSystem::LightRenderer *l)
{
	Light * light = new Light(l);
	m_lights.push_back(light);
}


void AngelCore::AngelRendering::LightManager::UpdateLight(
	int index, AngelCore::AngelWorldSystem::LightRenderer *l)
{
	m_lights.at(index)->PositionWS = l->GetPosition();
	m_lights.at(index)->castShadow = l->GetCastShadow();
	m_lights.at(index)->Type = l->GetType();
	m_lights.at(index)->DirectionWS = l->GetDirection();
	m_lights.at(index)->Color = l->GetColor();
	m_lights.at(index)->SpotlightAngle = l->GetSpotLightAngle();
	m_lights.at(index)->Range = l->GetRange();
	m_lights.at(index)->Intensity = l->GetIntensity();
	m_lights.at(index)->Enabled = l->GetEnable();
}


void AngelCore::AngelRendering::LightManager::DeleteLight(
	AngelCore::AngelRendering::LightManager::Light *l)
{
	/*int i = 0;
	bool found = false;
	for (i = 0;i < m_lights.size();i++)
	{
		if (*m_lights[i] == *l)
		{
			found = true;
			break;
		}
	}
	if (found)
	{
		std::vector<Light *> tmp;
		for (int j = 0;j < m_lights.size();j++)
		{
			if (j != i)
			{
				tmp.push_back(m_lights[j]);
			}
		}
	}*/
}
void AngelCore::AngelRendering::LightManager::Update()
{
	for (unsigned int i = 0;i < m_lights.size();i++)
	{
		m_lights.at(i)->CameraPosition =
			DirectX::XMFLOAT3(
				AngelCore::AngelSubSystemResources::BaseCameraProperties::BCamera->GetCameraPosition().x,
				AngelCore::AngelSubSystemResources::BaseCameraProperties::BCamera->GetCameraPosition().y,
				AngelCore::AngelSubSystemResources::BaseCameraProperties::BCamera->GetCameraPosition().z);
		m_cbLight.Const.l[i] = *m_lights.at(i);
	}
}

AngelCore::AngelRendering::LightManager::Light * AngelCore::AngelRendering::LightManager::GetLight(int i)
{
	return m_lights.at(i);
}

void AngelCore::AngelRendering::LightManager::ClearLights()
{
	for (int i = 0;i < m_lights.size();i++)
	{
		delete m_lights[i];
	}
	m_lights.clear();
	m_cbLight.Release();
	m_cbLight.Load();
}