#include"../Public/DebugCollisionShape.h"
#include<d3d11.h>
#include"../Public/VertexTypes.h"
#include"../../Public/SubSystemsResource.h"


void AngelCore::AngelRendering::DebugCollisionShape::Create()
{
	VertexTypesStruct::Position v[] =
	{
		VertexTypesStruct::Position(-1.0f, -1.0f, -1.0f),
		VertexTypesStruct::Position(-1.0f, +1.0f, -1.0f),
		VertexTypesStruct::Position(+1.0f, +1.0f, -1.0f),
		VertexTypesStruct::Position(+1.0f, -1.0f, -1.0f),
		VertexTypesStruct::Position(-1.0f, -1.0f, +1.0f),
		VertexTypesStruct::Position(-1.0f, +1.0f, +1.0f),
		VertexTypesStruct::Position(+1.0f, +1.0f, +1.0f),
		VertexTypesStruct::Position(+1.0f, -1.0f, +1.0f),
	};

	DWORD indices[] = {
		// front face
		0, 1, 2,
		0, 2, 3,

		// back face
		4, 6, 5,
		4, 7, 6,

		// left face
		4, 5, 1,
		4, 1, 0,

		// right face
		3, 2, 6,
		3, 6, 7,

		// top face
		1, 5, 6,
		1, 6, 2,

		// bottom face
		4, 0, 3,
		4, 3, 7
	};

	D3D11_BUFFER_DESC indexBufferDesc;
	ZeroMemory(&indexBufferDesc, sizeof(indexBufferDesc));

	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(DWORD) * 12 * 3;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA iinitData;

	iinitData.pSysMem = indices;
	HR(AngelSubSystemResources::GraphicDeviceResources::Device->CreateBuffer(&indexBufferDesc, &iinitData, &m_indexBuffer));

	D3D11_BUFFER_DESC vertexBufferDesc;
	ZeroMemory(&vertexBufferDesc, sizeof(vertexBufferDesc));

	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(VertexTypesStruct::Position) * 8;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA vbData;
	vbData.pSysMem = v;
	vbData.SysMemPitch = vbData.SysMemSlicePitch = 0;
	HR(AngelSubSystemResources::GraphicDeviceResources::Device->CreateBuffer
		(&vertexBufferDesc, &vbData, m_vertexBuffer.GetAddressOf()));
	m_shader = new Shader();
	Shader::LoadShader(ShaderType::VertexShader, "VertexDebugCollision.cso", VertexTypes::Position, m_shader);
	Shader::LoadShader(ShaderType::PixelShader, "PixelDebugCollision.cso", VertexTypes::NOP, m_shader);

	m_cbColor.Load();
	m_cbColor.Const.color = DirectX::XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);
	m_cbWVP.Load();
}

void AngelCore::AngelRendering::DebugCollisionShape::Create(DirectX::XMFLOAT3 * points)
{
	VertexTypesStruct::Position v[] =
	{
		VertexTypesStruct::Position(points[0].x, points[0].y, points[0].z),
		VertexTypesStruct::Position(points[1].x, points[1].y, points[1].z),
		VertexTypesStruct::Position(points[2].x, points[2].y, points[2].z),
		VertexTypesStruct::Position(points[3].x, points[3].y, points[3].z),
		VertexTypesStruct::Position(points[4].x, points[4].y, points[4].z),
		VertexTypesStruct::Position(points[5].x, points[5].y, points[5].z),
		VertexTypesStruct::Position(points[6].x, points[6].y, points[6].z),
		VertexTypesStruct::Position(points[7].x, points[7].y, points[7].z),
	};

	DWORD indices[] = {
		// front face
		0, 1, 2,
		0, 2, 3,

		// back face
		4, 6, 5,
		4, 7, 6,

		// left face
		4, 5, 1,
		4, 1, 0,

		// right face
		3, 2, 6,
		3, 6, 7,

		// top face
		1, 5, 6,
		1, 6, 2,

		// bottom face
		4, 0, 3,
		4, 3, 7
	};

	D3D11_BUFFER_DESC indexBufferDesc;
	ZeroMemory(&indexBufferDesc, sizeof(indexBufferDesc));

	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(DWORD) * 12 * 3;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA iinitData;

	iinitData.pSysMem = indices;
	HR(AngelSubSystemResources::GraphicDeviceResources::Device->CreateBuffer(&indexBufferDesc, &iinitData, &m_indexBuffer));

	D3D11_BUFFER_DESC vertexBufferDesc;
	ZeroMemory(&vertexBufferDesc, sizeof(vertexBufferDesc));

	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(VertexTypesStruct::Position) * 8;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA vbData;
	vbData.pSysMem = v;
	vbData.SysMemPitch = vbData.SysMemSlicePitch = 0;
	HR(AngelSubSystemResources::GraphicDeviceResources::Device->CreateBuffer
		(&vertexBufferDesc, &vbData, m_vertexBuffer.GetAddressOf()));
	m_shader = new Shader();
	Shader::LoadShader(ShaderType::VertexShader, "VertexDebugCollision.cso", VertexTypes::Position, m_shader);
	Shader::LoadShader(ShaderType::PixelShader, "PixelDebugCollision.cso", VertexTypes::NOP, m_shader);

	m_cbColor.Load();
	m_cbColor.Const.color = DirectX::XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);
	m_cbWVP.Load();
}

void AngelCore::AngelRendering::DebugCollisionShape::Render()
{

	AngelCore::AngelSubSystemResources::GraphicDeviceResources::DeviceContext->RSSetState(
		AngelCore::AngelSubSystemResources::GraphicDeviceResources::WireFrameRasterizer.Get());
	AngelSubSystemResources::GraphicDeviceResources::DeviceContext->IASetIndexBuffer(m_indexBuffer.Get(), DXGI_FORMAT_R32_UINT
		, 0);
	AngelSubSystemResources::GraphicDeviceResources::DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	UINT stride = sizeof(VertexTypesStruct::Position);
	UINT offset = 0;
	AngelSubSystemResources::GraphicDeviceResources::DeviceContext->IASetVertexBuffers(0, 1, m_vertexBuffer.GetAddressOf()
		, &stride, &offset);
	m_cbColor.Update();
	m_shader->SetConstantBuffer(1, 1, m_cbColor.getBuffer());
	m_cbWVP.Update();
	m_shader->SetConstantBuffer(0, 1, m_cbWVP.getBuffer());

	m_shader->Apply();
	AngelSubSystemResources::GraphicDeviceResources::DeviceContext->DrawIndexed(36, 0, 0);
	AngelCore::AngelSubSystemResources::GraphicDeviceResources::DeviceContext->RSSetState(
		AngelCore::AngelSubSystemResources::GraphicDeviceResources::SolidRasterizer.Get());


}

void AngelCore::AngelRendering::DebugCollisionShape::Update(
	DirectX::CXMMATRIX view,
	DirectX::CXMMATRIX projection)
{
	using namespace DirectX;
	XMMATRIX world = XMMatrixScaling(m_boundingBox.Extents.x, m_boundingBox.Extents.y, m_boundingBox.Extents.z)
		* XMMatrixRotationQuaternion(XMVectorSet(m_boundingBox.Orientation.x,
			m_boundingBox.Orientation.y,
			m_boundingBox.Orientation.z,
			m_boundingBox.Orientation.w))
		*XMMatrixTranslation(m_boundingBox.Center.x, m_boundingBox.Center.y, m_boundingBox.Center.z);
	/*XMMATRIX tview = XMLoadFloat4x4(&view);
	XMMATRIX tworld = XMLoadFloat4x4(&world);
	XMMATRIX tprojection = XMLoadFloat4x4(&projection);*/
	XMMATRIX wvp = XMMatrixTranspose(world*view*projection);
	XMStoreFloat4x4(&m_cbWVP.Const.WVP, wvp);
}