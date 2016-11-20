#include"../Public/IGUI3D.h"
#include"../../../Core/AngelGUI.h"
#include"../../3D/Public/CBuffer.h"

AngelGUISystem::GUI3D::IGUI3D::IGUI3D()
{
	m_position = DirectX::XMFLOAT3(0.0f, 0.0f, 0.f);
	m_scale = DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f);
	m_rotation = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);

	m_rectangle.top = 0;
	m_rectangle.left = 0;
	m_rectangle.right = 780;
	m_rectangle.bottom = 640;

	m_defaultColor = DirectX::XMFLOAT4(0.5f, 0.8f, 0.5f, 0.7f);
	m_onHoverColor = DirectX::XMFLOAT4(0.6f, 0.85f, 0.7f, 0.7f);
	m_onClickColor = DirectX::XMFLOAT4(0.6f, 0.9f, 0.7f, 0.7f);
	m_currentColor = m_defaultColor;

	m_selected = false;
	m_hovered = false;

	m_recreate = false;
	m_active = true;

	m_renderFromResource = false;

	m_refCount = 1;
	m_billboard = false;

	using namespace DirectX;
#pragma region Create Vertex Buffer

	const VertexType Vertices[] =
	{
		{ XMFLOAT3(1.0f, 1.0f, 0.0f), XMFLOAT2(1.0f, 0.0f) },
		{ XMFLOAT3(-1.0f, -1.0f, 0.0f), XMFLOAT2(0.0f, 1.0f) },
		{ XMFLOAT3(-1.0f, 1.0f, 0.0f), XMFLOAT2(0.0f, 0.0f) },
		{ XMFLOAT3(1.0f, -1.0f, 0.0f), XMFLOAT2(1.0f, 1.0f) },
	};



	XMFLOAT3 vMinf3(+5, +5, +5);
	XMFLOAT3 vMaxf3(-5, -5, -5);

	XMVECTOR vMin = XMLoadFloat3(&vMinf3);
	XMVECTOR vMax = XMLoadFloat3(&vMaxf3);

	XMVECTOR v[4];
	v[0] = XMLoadFloat3(&XMFLOAT3(1.0f, 1.0f, 0.0f));
	v[1] = XMLoadFloat3(&XMFLOAT3(-1.0f, -1.0f, 0.0f));
	v[2] = XMLoadFloat3(&XMFLOAT3(-1.0f, 1.0f, 0.0f));
	v[3] = XMLoadFloat3(&XMFLOAT3(1.0f, -1.0f, 0.0f));

	for (int i = 0;i < 4;i++)
	{
		vMin = XMVectorMin(vMin, v[i]);
		vMax = XMVectorMax(vMax, v[i]);
	}

	XMStoreFloat3(&m_boundingBox.Center, 0.5f*(XMVectorAdd(vMax, vMin)));
	XMStoreFloat3(&m_boundingBox.Extents, 0.5f*(XMVectorSubtract(vMax, vMin)));

	D3D11_BUFFER_DESC vertexBufferDesc = { 0 };
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * ARRAYSIZE(Vertices);
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
	HR(AngelGUISystem::AngelGUI::GetDevice3D()
		->CreateBuffer(&vertexBufferDesc, &vertexBufferData, &this->m_vertexBuffer));

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

	HR(AngelGUI::GetDevice3D()->CreateBuffer
		(&indexBufferDesc, &indexBufferData, &this->m_indexBuffer));

#pragma endregion

	m_shader = new AngelGUISystem::GUI3D::GUI3DRendering::Shader();
	m_shader->Load();

	this->m_cbWorld.Load(AngelGUI::GetDevice3D().Get(),AngelGUI::GetDeviceContext3D().Get());
	this->m_cbColor.Load(AngelGUI::GetDevice3D().Get(), AngelGUI::GetDeviceContext3D().Get());

	ID3D11Texture2D *texture;
	RECT rect = {};
	GetClientRect(AngelGUI::GetHWND(), &rect);
	D2D1_SIZE_U d2dSize = D2D1::SizeU(rect.right, rect.bottom);

	D3D11_TEXTURE2D_DESC texDesc;
	texDesc.ArraySize = 1;
	texDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	texDesc.CPUAccessFlags = 0;
	texDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	texDesc.Height = d2dSize.height;
	texDesc.Width = d2dSize.width;
	texDesc.MipLevels = 1;
	texDesc.MiscFlags = 0;
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;
	texDesc.Usage = D3D11_USAGE_DEFAULT;

	HR(AngelGUI::GetDevice3D()->
		CreateTexture2D(&texDesc, NULL, &texture));

	IDXGISurface1 *pDxgiSurface = NULL;

	texture->QueryInterface(IID_PPV_ARGS(&pDxgiSurface));

	D2D1_RENDER_TARGET_PROPERTIES props =
		D2D1::RenderTargetProperties(
			D2D1_RENDER_TARGET_TYPE_DEFAULT,
			D2D1::PixelFormat(DXGI_FORMAT_UNKNOWN, D2D1_ALPHA_MODE_PREMULTIPLIED),
			96,
			96
			);


	(AngelGUI::GetFactory()->
		CreateDxgiSurfaceRenderTarget(
			pDxgiSurface, props, m_renderTarget2D.GetAddressOf()));

	D3D11_SHADER_RESOURCE_VIEW_DESC srvd;
	srvd.Format = texDesc.Format;
	srvd.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvd.Texture2D.MipLevels = 1;
	srvd.Texture2D.MostDetailedMip = 0;

	HR(AngelGUI::GetDevice3D()->CreateShaderResourceView
		(texture, &srvd, m_shaderResourceView.GetAddressOf()));
}

void AngelGUISystem::GUI3D::IGUI3D::Render()
{
	ID3D11DeviceContext1 *deviceContext =
		AngelGUI::GetDeviceContext3D().Get();
	this->m_cbWorld.Update();
	this->m_shader->SetConstantBuffer(0, 1, m_cbWorld.getBuffer());
	this->m_cbColor.Update();
	m_cbColor.Const.color = DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);

	this->m_shader->SetConstantBuffer(1, 1, m_cbColor.getBuffer());
	this->m_shader->SetSampler
		(0, 1,
			AngelGUI::GetSamplerState().GetAddressOf());
	if(!m_renderFromResource)
		this->m_shader->BindSRV(0, 1, this->m_shaderResourceView.Get(),
			AngelGUISystem::GUI3D::GUI3DRendering::ShaderType::PixelShader);
	else
		this->m_shader->BindSRV(0, 1, this->m_resourceSRV.Get(),
			AngelGUISystem::GUI3D::GUI3DRendering::ShaderType::PixelShader);

	this->m_stride = sizeof(VertexType);
	this->m_offset = 0;
	this->m_shader->Apply();
	{
		deviceContext->IASetVertexBuffers(0, 1,
			this->m_vertexBuffer.GetAddressOf(), 
			&this->m_stride, &this->m_offset);
		deviceContext->IASetIndexBuffer(this->m_indexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0);
		deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		deviceContext->DrawIndexed(6, 0, 0);
	}
	m_shader->UnBindSRV(0, 1,
		AngelGUISystem::GUI3D::GUI3DRendering::ShaderType::PixelShader);
	m_shader->UnBindSRV(1, 1,
		AngelGUISystem::GUI3D::GUI3DRendering::ShaderType::PixelShader);
	m_shader->Disable();
}

void AngelGUISystem::GUI3D::IGUI3D::Update(float dt, DirectX::CXMMATRIX view, DirectX::CXMMATRIX projection)
{
	using namespace DirectX;
	//we use constant scale divide here.

	float xscale = m_rectangle.right / 500;
	float yscale = m_rectangle.bottom / 500;
	if (m_billboard)
	{
		DirectX::XMMATRIX inverse = DirectX::XMMatrixInverse(&DirectX::XMMatrixDeterminant(view), view);
		DirectX::XMFLOAT3 cameraPosition = DirectX::XMFLOAT3(inverse.r[3].m128_f32[0], inverse.r[3].m128_f32[1], inverse.r[3].m128_f32[2]);

		float angle = atan2(m_position.x - cameraPosition.x, (m_position.z - cameraPosition.z)) * (180.0f / XM_PI);
		float rotation = (float)angle * 0.0174532925f;
		DirectX::XMStoreFloat4x4(&m_worldMatrix,
			DirectX::XMMatrixScaling(
				m_scale.x,
				m_scale.y,
				m_scale.z) *
			DirectX::XMMatrixRotationY(rotation)
			* DirectX::XMMatrixTranslation(m_position.x, m_position.y, m_position.z));

		m_rotation.y = rotation;
	}
	else
	{
		DirectX::XMStoreFloat4x4(&m_worldMatrix,
			DirectX::XMMatrixScaling(
				xscale * m_scale.x,
				yscale * m_scale.y,
				m_scale.z) *
			DirectX::XMMatrixRotationRollPitchYaw(m_rotation.x,m_rotation.y,m_rotation.z)
			* DirectX::XMMatrixTranslation(m_position.x, m_position.y, m_position.z));

	}

	XMStoreFloat4x4(&this->m_cbWorld.Const.WVP,
	XMMatrixTranspose(DirectX::XMLoadFloat4x4(
		&(m_worldMatrix))
			*view
			*projection));

	
	DirectX::XMStoreFloat4x4(&m_invWorldMatrix,
		XMMatrixInverse(&XMMatrixDeterminant(DirectX::XMLoadFloat4x4(&m_worldMatrix)), 
			DirectX::XMLoadFloat4x4(&m_worldMatrix)));
	

	DirectX::XMStoreFloat4x4(&m_view, view);
	DirectX::XMStoreFloat4x4(&m_projection, projection);
}

bool AngelGUISystem::GUI3D::IGUI3D::TestIntersects(
	DirectX::XMFLOAT2 * HitPoint2DSpace,
	DirectX::XMFLOAT3 * HitPointLocalSpace, float *dist,
	float mouseX,float mouseY)
{
	//Compute in view space
	using namespace DirectX;

	UINT numOfVP = 1;
	D3D11_VIEWPORT vp;
	AngelGUI::GetDeviceContext3D()->RSGetViewports(&numOfVP, &vp);

	float vx = (+2.0f*mouseX / vp.Width - 1.0f) /
		m_projection(0, 0);
	float vy = (-2.0f*mouseY / vp.Height + 1.0f) /
		m_projection(1, 1);


	XMVECTOR rayOrigin = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
	XMVECTOR rayDir = XMVectorSet(vx, vy, 1.0f, 0.0f);

	//we are now in view space.

	XMMATRIX invView = XMMatrixInverse(&XMMatrixDeterminant(DirectX::XMLoadFloat4x4(&m_view)),
		DirectX::XMLoadFloat4x4(&m_view));

	XMMATRIX toLocal = XMMatrixMultiply(invView, DirectX::XMLoadFloat4x4(&m_invWorldMatrix));

	rayOrigin = XMVector3TransformCoord(rayOrigin,
		toLocal);
	rayDir = XMVector3TransformNormal(rayDir, toLocal);

	// Make the ray direction unit length for the
	//intersection tests.
	rayDir = XMVector3Normalize(rayDir);

	float tmin = 0.0f;
	//OutputDebugStringA("\nX:");
	//OutputDebugStringA(std::to_string(rayOrigin.m128_f32[0]).c_str());
	//OutputDebugStringA("\nY:");
	//OutputDebugStringA(std::to_string(rayOrigin.m128_f32[1]).c_str());
	//OutputDebugStringA("\nZ:");
	//OutputDebugStringA(std::to_string(rayOrigin.m128_f32[2]).c_str());
	if (!DirectX::Internal::XMVector3IsUnit(rayDir))
		return false;
	if (m_boundingBox.Intersects(rayOrigin, rayDir, tmin))
	{
		XMVECTOR hitPosition = rayOrigin + rayDir*tmin;
		if (dist)
			*dist = tmin;
		if (HitPointLocalSpace)
		{
			HitPointLocalSpace->x = hitPosition.m128_f32[0];
			HitPointLocalSpace->y = hitPosition.m128_f32[1];
			HitPointLocalSpace->z = hitPosition.m128_f32[2];
		}

		if (HitPoint2DSpace) {
			HitPoint2DSpace->x = (hitPosition.m128_f32[0] *
				800 / 2) +
				800 / 2;
			float height = 600;
			HitPoint2DSpace->y = (hitPosition.m128_f32[1] *
				-height / 2) +
				height / 2;

			OutputDebugStringA("\nX:");
			OutputDebugStringA(std::to_string(HitPoint2DSpace->x).c_str());
			OutputDebugStringA("\nY:");
			OutputDebugStringA(std::to_string(HitPoint2DSpace->y).c_str());
		}

		return true;
	}
	return false;
}

bool AngelGUISystem::GUI3D::IGUI3D::TestIntersects(
	DirectX::CXMMATRIX invWorld, float *dist,
	float mouseX, float mouseY)
{
	//Compute in view space
	using namespace DirectX;

	UINT numOfVP = 1;
	D3D11_VIEWPORT vp;
	AngelGUI::GetDeviceContext3D()->RSGetViewports(&numOfVP, &vp);

	float vx = (+2.0f*mouseX / vp.Width - 1.0f) /
		m_projection(0, 0);
	float vy = (-2.0f*mouseY / vp.Height + 1.0f) /
		m_projection(1, 1);


	XMVECTOR rayOrigin = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
	XMVECTOR rayDir = XMVectorSet(vx, vy, 1.0f, 0.0f);

	//we are now in view space.

	XMMATRIX invView = XMMatrixInverse(&XMMatrixDeterminant(DirectX::XMLoadFloat4x4(&m_view)),
		DirectX::XMLoadFloat4x4(&m_view));

	XMMATRIX toLocal = XMMatrixMultiply(invView,invWorld);

	rayOrigin = XMVector3TransformCoord(rayOrigin,
		toLocal);
	rayDir = XMVector3TransformNormal(rayDir, toLocal);

	// Make the ray direction unit length for the
	//intersection tests.
	rayDir = XMVector3Normalize(rayDir);

	float tmin = 0.0f;
	//OutputDebugStringA("\nX:");
	//OutputDebugStringA(std::to_string(rayOrigin.m128_f32[0]).c_str());
	//OutputDebugStringA("\nY:");
	//OutputDebugStringA(std::to_string(rayOrigin.m128_f32[1]).c_str());
	//OutputDebugStringA("\nZ:");
	//OutputDebugStringA(std::to_string(rayOrigin.m128_f32[2]).c_str());
	if (!DirectX::Internal::XMVector3IsUnit(rayDir))
		return false;
	if (m_boundingBox.Intersects(rayOrigin, rayDir, tmin))
	{
		XMVECTOR hitPosition = rayOrigin + rayDir*tmin;
		if (dist)
			*dist = tmin;
		return true;
	}
	return false;
}