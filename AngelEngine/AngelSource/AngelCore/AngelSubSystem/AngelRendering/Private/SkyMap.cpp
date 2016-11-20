// ***********************************************************************
// FileName         : SkyMap.cpp
// Author           : Peyman Tahghighi
//
// ***********************************************************************
//     Copyright (c) Peyman Tahghighi . All rights reserved.
//
// <summary> Loading and using skymap,also rendering a sphere  </summary>
// ***********************************************************************

#include "../Public/SkyMap.h"
#include"../../../AngelSubSystem/Public/SubSystemsResource.h"
#include"../../../AngelSubSystem/Public/SubSystemManager.h"
#include"../../../AngelSubSystem/Public/WorldManager.h"
#include"../../AngelRendering/Public/DDSTextureLoader.h"
#include<fstream>
#include"../Public/LightManager.h"

void AngelCore::AngelRendering::SkyMap::Initialize()
{
#pragma region Create Plane
	int skyPlaneResolution, textureRepeat;
	float skyPlaneWidth, skyPlaneTop, skyPlaneBottom;
	bool result;


	// Set the sky plane parameters.
	skyPlaneResolution = 1000;
	skyPlaneWidth = 5950.0f;
	skyPlaneTop = 100.5f;
	skyPlaneBottom = -500.0f;
	textureRepeat = 10;

	// Set the brightness of the clouds.
	m_brightness = 0.55f;

	// Setup the cloud translation speed increments.
	m_translationSpeed[0] = 0.000030f;   // First texture X translation speed.
	m_translationSpeed[1] = 0.0001f;      // First texture Z translation speed.
	m_translationSpeed[2] = 0.00015f;  // Second texture X translation speed.
	m_translationSpeed[3] = 0.0002f;      // Second texture Z translation speed.

									   // Initialize the texture translation values.
	m_textureTranslation[0] = 0.0f;
	m_textureTranslation[1] = 0.0f;
	m_textureTranslation[2] = 0.0f;
	m_textureTranslation[3] = 0.0f;



	// Create the sky plane.
	result = InitializeSkyPlane(skyPlaneResolution, skyPlaneWidth, skyPlaneTop, skyPlaneBottom, textureRepeat);
	if (!result)
	{
		return;
	}

#pragma endregion

	std::ifstream fin;
	char input;
	int i;
	std::string dataS = "skyDome.txt";
	std::wstring path = AngelCore::AngelSubSystem::FileSystemManager::GetContentDirectory();
	std::string pathS(path.begin(), path.end());

	m_skyMapTextures = new SkyMapTexture[2];
	dataS = pathS +"\\AngelData\\Model\\"+ dataS;
	// Open the model file.
	fin.open(dataS.c_str());

	// If it could not open the file then exit.
	if (fin.fail())
	{
		AngelCore::AngelSubSystem::AngelLog::WriteErrorMessage("Failed Find Model SkyDome.txt");
		return;
	}

	// Read up to the value of vertex count.
	fin.get(input);
	while (input != ':')
	{
		fin.get(input);
	}

	// Read in the vertex count.
	fin >> m_vertexCount;

	// Set the number of indices to be the same as the vertex count.
	m_indexCount = m_vertexCount;

	// Create the model using the vertex count that was read in.
	m_model = new ModelType[m_vertexCount];
	if (!m_model)
	{
		return;
	}

	// Read up to the beginning of the data.
	fin.get(input);
	while (input != ':')
	{
		fin.get(input);
	}
	fin.get(input);
	fin.get(input);

	// Read in the vertex data.
	for (i = 0; i<m_vertexCount; i++)
	{
		fin >> m_model[i].x >> m_model[i].y >> m_model[i].z;
		fin >> m_model[i].tu >> m_model[i].tv;
		fin >> m_model[i].nx >> m_model[i].ny >> m_model[i].nz;
	}

	// Close the model file.
	fin.close();

	VertexTypesStruct::SkyVertex* vertices;
	unsigned long* indices;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;

	// Create the vertex array.
	vertices = new VertexTypesStruct::SkyVertex[m_vertexCount];
	if (!vertices)
	{
		return;
	}

	// Create the index array.
	indices = new unsigned long[m_indexCount];
	if (!indices)
	{
		return;
	}

	// Load the vertex array and index array with data.
	for (i = 0; i<m_vertexCount; i++)
	{
		vertices[i].pos = DirectX::XMFLOAT3(m_model[i].x, m_model[i].y, m_model[i].z);
		indices[i] = i;
	}

	// Set up the description of the vertex buffer.
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(VertexTypesStruct::SkyVertex) * m_vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the vertex data.
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// Now finally create the vertex buffer.
	 HR(AngelCore::AngelSubSystemResources::GraphicDeviceResources::Device->
		 CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer));
	

	// Set up the description of the index buffer.
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * m_indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the index data.
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// Create the index buffer.
	HR(AngelCore::AngelSubSystemResources::GraphicDeviceResources::Device->
		CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer));

	// Release the arrays now that the vertex and index buffers have been created and loaded.
	delete[] vertices;
	vertices = 0;

	delete[] indices;
	indices = 0;

	this->m_shader = new AngelCore::AngelRendering::Shader();


	AngelCore::AngelRendering::Shader::LoadShader(ShaderType::VertexShader, "VertexSkyMap.cso", VertexTypes::SkyVertex, this->m_shader);
	AngelCore::AngelRendering::Shader::LoadShader(ShaderType::PixelShader, "PixelSkyMap.cso", VertexTypes::NOP, this->m_shader);

	D3D11_DEPTH_STENCIL_DESC dssDesc;
	ZeroMemory(&dssDesc, sizeof(D3D11_DEPTH_STENCIL_DESC));
	dssDesc.DepthEnable = true;
	dssDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	dssDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
	auto device = AngelCore::AngelSubSystemResources::GraphicDeviceResources::Device;
	HR(device->CreateDepthStencilState(&dssDesc, this->m_depthStencilState.GetAddressOf()));

	D3D11_RASTERIZER_DESC wireframeDesc;
	ZeroMemory(&wireframeDesc, sizeof(D3D11_RASTERIZER_DESC));
	wireframeDesc.FillMode = D3D11_FILL_SOLID;
	wireframeDesc.CullMode = D3D11_CULL_NONE;
	wireframeDesc.FrontCounterClockwise = false;
	wireframeDesc.DepthClipEnable = true;

	AngelCore::AngelSubSystemResources::GraphicDeviceResources::Device->CreateRasterizerState(&wireframeDesc,
		this->m_rasterizer.GetAddressOf());

	m_curBlendAmount = 1;
	
	this->m_worldBuffer.Load();
	this->m_cbPerFrame.Load();
	this->m_uvBuffer.Load();

	m_dynamicCloud = false;

	this->m_cbGradiant.Load();

	m_cbGradiant.Const.apexColor = DirectX::XMFLOAT4(0.15f, 0.15f, 0.66f, 1.0f);
	m_cbGradiant.Const.centerColor = DirectX::XMFLOAT4(0.81f, 0.38f, 0.66f, 1.0f);
}

bool AngelCore::AngelRendering::SkyMap::InitializeSkyPlane(
	int skyPlaneResolution, 
	float skyPlaneWidth, 
	float skyPlaneTop, 
	float skyPlaneBottom, 
	int textureRepeat)
{
	{
		float quadSize, radius, constant, textureDelta;
		int i, j, index;
		float positionX, positionY, positionZ, tu, tv;


		// Create the array to hold the sky plane coordinates.
		m_skyPlane = new SkyPlaneType[(skyPlaneResolution + 1) * (skyPlaneResolution + 1)];
		if (!m_skyPlane)
		{
			return false;
		}

		// Determine the size of each quad on the sky plane.
		quadSize = skyPlaneWidth / (float)skyPlaneResolution;

		// Calculate the radius of the sky plane based on the width.
		radius = skyPlaneWidth / 2.0f;

		// Calculate the height constant to increment by.
		constant = (skyPlaneTop - skyPlaneBottom) / (radius * radius);

		// Calculate the texture coordinate increment value.
		textureDelta = (float)textureRepeat / (float)skyPlaneResolution;

		// Loop through the sky plane and build the coordinates based on the increment values given.
		for (j = 0; j <= skyPlaneResolution; j++)
		{
			for (i = 0; i <= skyPlaneResolution; i++)
			{
				// Calculate the vertex coordinates.
				positionX = (-0.5f * skyPlaneWidth) + ((float)i * quadSize);
				positionZ = (-0.5f * skyPlaneWidth) + ((float)j * quadSize);
				positionY = skyPlaneTop - (constant * ((positionX * positionX) + (positionZ * positionZ)));

				// Calculate the texture coordinates.
				tu = (float)i * textureDelta;
				tv = (float)j * textureDelta;

				// Calculate the index into the sky plane array to add this coordinate.
				index = j * (skyPlaneResolution + 1) + i;

				// Add the coordinates to the sky plane array.
				m_skyPlane[index].x = positionX;
				m_skyPlane[index].y = positionY;
				m_skyPlane[index].z = positionZ;
				m_skyPlane[index].tu = tu;
				m_skyPlane[index].tv = tv;
			}
		}
	}
	//Initialize Buffers
	{
		VertexType* vertices;
		unsigned long* indices;
		D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
		D3D11_SUBRESOURCE_DATA vertexData, indexData;
		HRESULT result;
		int i, j, index, index1, index2, index3, index4;
		// Calculate the number of vertices in the sky plane mesh.
		m_PlanevertexCount = (skyPlaneResolution + 1) * (skyPlaneResolution + 1) * 6;

		// Set the index count to the same as the vertex count.
		m_PlaneindexCount = m_PlanevertexCount;

		// Create the vertex array.
		vertices = new VertexType[m_PlanevertexCount];
		if (!vertices)
		{
			return false;
		}

		// Create the index array.
		indices = new unsigned long[m_PlaneindexCount];
		if (!indices)
		{
			return false;
		}

		// Initialize the index into the vertex array.
		index = 0;

		// Load the vertex and index array with the sky plane array data.
		for (j = 0; j<skyPlaneResolution; j++)
		{
			for (i = 0; i<skyPlaneResolution; i++)
			{
				index1 = j * (skyPlaneResolution + 1) + i;
				index2 = j * (skyPlaneResolution + 1) + (i + 1);
				index3 = (j + 1) * (skyPlaneResolution + 1) + i;
				index4 = (j + 1) * (skyPlaneResolution + 1) + (i + 1);

				// Triangle 1 - Upper Left
				vertices[index].position = DirectX::XMFLOAT3(m_skyPlane[index1].x, m_skyPlane[index1].y, m_skyPlane[index1].z);
				vertices[index].texture = DirectX::XMFLOAT2(m_skyPlane[index1].tu, m_skyPlane[index1].tv);
				indices[index] = index;
				index++;

				// Triangle 1 - Upper Right
				vertices[index].position = DirectX::XMFLOAT3(m_skyPlane[index2].x, m_skyPlane[index2].y, m_skyPlane[index2].z);
				vertices[index].texture = DirectX::XMFLOAT2(m_skyPlane[index2].tu, m_skyPlane[index2].tv);
				indices[index] = index;
				index++;

				// Triangle 1 - Bottom Left
				vertices[index].position = DirectX::XMFLOAT3(m_skyPlane[index3].x, m_skyPlane[index3].y, m_skyPlane[index3].z);
				vertices[index].texture = DirectX::XMFLOAT2(m_skyPlane[index3].tu, m_skyPlane[index3].tv);
				indices[index] = index;
				index++;

				// Triangle 2 - Bottom Left
				vertices[index].position = DirectX::XMFLOAT3(m_skyPlane[index3].x, m_skyPlane[index3].y, m_skyPlane[index3].z);
				vertices[index].texture = DirectX::XMFLOAT2(m_skyPlane[index3].tu, m_skyPlane[index3].tv);
				indices[index] = index;
				index++;

				// Triangle 2 - Upper Right
				vertices[index].position = DirectX::XMFLOAT3(m_skyPlane[index2].x, m_skyPlane[index2].y, m_skyPlane[index2].z);
				vertices[index].texture = DirectX::XMFLOAT2(m_skyPlane[index2].tu, m_skyPlane[index2].tv);
				indices[index] = index;
				index++;

				// Triangle 2 - Bottom Right
				vertices[index].position = DirectX::XMFLOAT3(m_skyPlane[index4].x, m_skyPlane[index4].y, m_skyPlane[index4].z);
				vertices[index].texture = DirectX::XMFLOAT2(m_skyPlane[index4].tu, m_skyPlane[index4].tv);
				indices[index] = index;
				index++;
			}
		}

		// Set up the description of the vertex buffer.
		vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		vertexBufferDesc.ByteWidth = sizeof(VertexType) * m_PlanevertexCount;
		vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vertexBufferDesc.CPUAccessFlags = 0;
		vertexBufferDesc.MiscFlags = 0;
		vertexBufferDesc.StructureByteStride = 0;

		// Give the subresource structure a pointer to the vertex data.
		vertexData.pSysMem = vertices;
		vertexData.SysMemPitch = 0;
		vertexData.SysMemSlicePitch = 0;

		// Now finally create the vertex buffer.
		result = AngelCore::AngelSubSystemResources::GraphicDeviceResources::GetDevice3D()->CreateBuffer(&vertexBufferDesc, &vertexData, &m_PlanevertexBuffer);
		if (FAILED(result))
		{
			return false;
		}

		// Set up the description of the index buffer.
		indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
		indexBufferDesc.ByteWidth = sizeof(unsigned long) * m_PlaneindexCount;
		indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		indexBufferDesc.CPUAccessFlags = 0;
		indexBufferDesc.MiscFlags = 0;
		indexBufferDesc.StructureByteStride = 0;

		// Give the subresource structure a pointer to the index data.
		indexData.pSysMem = indices;
		indexData.SysMemPitch = 0;
		indexData.SysMemSlicePitch = 0;

		// Create the index buffer.
		result = AngelCore::AngelSubSystemResources::GraphicDeviceResources::GetDevice3D()->CreateBuffer(&indexBufferDesc, &indexData, &m_PlaneindexBuffer);
		if (FAILED(result))
		{
			return false;
		}

		// Release the arrays now that the vertex and index buffers have been created and loaded.
		delete[] vertices;
		vertices = 0;

		delete[] indices;
		indices = 0;

	}
	m_cloudTexture1 = LoadPlaneTexture(L"cloud001.dds");
	m_cloudTexture2 = LoadPlaneTexture(L"cloud002.dds");

	this->m_Planeshader = new AngelCore::AngelRendering::Shader();


	AngelCore::AngelRendering::Shader::LoadShader(ShaderType::VertexShader, 
		"VertexSkyMapPlane.cso", VertexTypes::PlaneVertex, this->m_Planeshader);
	AngelCore::AngelRendering::Shader::LoadShader(ShaderType::PixelShader, 
		"PixelSkyMapPlane.cso", VertexTypes::NOP, this->m_Planeshader);

	m_cbPlane.Load();
	m_cbWorldPlane.Load();
	return true;
}

Texture2D * AngelCore::AngelRendering::SkyMap::LoadPlaneTexture(const wchar_t* textureName)
{
	Texture2D* texture = new Texture2D();
	if (!texture)
	{
		return false;
	}
	texture->LoadTexture(
		textureName,false);
	return texture;
}


ID3D11ShaderResourceView* AngelCore::AngelRendering::SkyMap::LoadSkyMapTexture(const char * name,int id)
{
	Microsoft::WRL::ComPtr<ID3D11Resource> resource;
	auto device = AngelCore::AngelSubSystemResources::GraphicDeviceResources::GetDevice3D();
	m_skyMapTextures[id].name = name;
	size_t dataSize;
	std::unique_ptr<uint8_t[]> data;
	int fileState;
	std::string dataS = name;
	std::wstring path = AngelCore::AngelSubSystem::FileSystemManager::GetContentDirectory();
	std::string pathString(path.begin(), path.end());
	dataS = pathString + "\\AngelData\\Texture\\" + dataS;
	std::wstring pathWS(dataS.begin(), dataS.end());
	HR(AngelCore::AngelSubSystem::FileSystemManager::ReadBinaryFile(pathWS, data, &dataSize, &fileState));

	ID3D11ShaderResourceView * srv = nullptr;

	HR(DirectX::CreateDDSTextureFromFileEx(device.Get(),
		pathWS.c_str(),
		0,
		D3D11_USAGE_DEFAULT,
		D3D11_BIND_SHADER_RESOURCE,
		0,
		4,
		false,
		resource.GetAddressOf(),
		m_skyMapTextures[id].srv.GetAddressOf()));

	Microsoft::WRL::ComPtr<ID3D11Texture2D> t;
	resource.As(&t);

	D3D11_TEXTURE2D_DESC desc;
	t->GetDesc(&desc);
	desc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;
	desc.ArraySize = 6;
	desc.Width = desc.Height;



	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;

	srvDesc.Format = desc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
	srvDesc.TextureCube.MipLevels = desc.MipLevels;
	srvDesc.TextureCube.MostDetailedMip = 0;

	HR(device->CreateShaderResourceView(resource.Get(), &srvDesc, m_skyMapTextures[id].srv.GetAddressOf()));
	return m_skyMapTextures[id].srv.Get();
}


void AngelCore::AngelRendering::SkyMap::Render()
{
	

	using namespace DirectX;
	
	//do blending
	if (m_changeBlendAmount)
	{
		m_curBlendAmount -= m_blendAmountPerUpdate;
	}
	if (m_curBlendAmount < 0)
	{
		m_curBlendAmount = 1;
		m_shadeResourcecurrent = m_shadeResourcenext;
		m_shadeResourcenext = nullptr;
		//std::swap(m_shadeResourcecurrent, m_shadeResourcenext);
		m_changeBlendAmount = false;
	}
	this->m_worldBuffer.Update();
	this->m_shader->SetConstantBuffer(0, 1, this->m_worldBuffer.getBuffer());

	this->m_cbPerFrame.Const.blendAmount = m_curBlendAmount;
	this->m_cbPerFrame.Const.alpha = m_alpha;
	this->m_cbPerFrame.Const.dynamicWeahter = (int)AngelCore::AngelSubSystem::SubSystemManager::GetInstance()->GetWorldManager()->GetDynamicWeather();
	this->m_cbPerFrame.Update();
	this->m_shader->SetConstantBuffer(1, 1, this->m_cbPerFrame.getBuffer());

	if (AngelCore::AngelSubSystem::SubSystemManager::GetInstance()->GetWorldManager()->GetDynamicWeather())
	{
		this->m_cbGradiant.Update();
		this->m_shader->SetConstantBuffer(2, 1, m_cbGradiant.getBuffer());
	}
	
	this->m_shader->BindSRV(0, 1, this->m_shadeResourcecurrent.Get(), ShaderType::PixelShader);
	this->m_shader->BindSRV(1, 1, this->m_shadeResourcenext.Get(), ShaderType::PixelShader);

	this->m_shader->SetSampler(0, 1, AngelCore::AngelSubSystemResources::GraphicDeviceResources::Samplers.at(0).GetAddressOf());

	auto context = AngelCore::AngelSubSystemResources::GraphicDeviceResources::DeviceContext;
	context->RSSetState(this->m_rasterizer.Get());
	UINT stride = sizeof(VertexTypesStruct::SkyVertex);
	UINT offset = 0;
	this->m_shader->Apply();
	context->IASetVertexBuffers(0, 1, this->m_vertexBuffer.GetAddressOf(), &stride, &offset);
	context->IASetIndexBuffer(this->m_indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	
	context->OMSetDepthStencilState(this->m_depthStencilState.Get(), 0);
	context->DrawIndexed(this->m_indexCount, 0, 0);

	context->OMSetDepthStencilState(nullptr, 0);

	m_shader->UnBindSRV(0, 1, ShaderType::PixelShader);
	m_shader->UnBindSRV(1, 1, ShaderType::PixelShader);
	m_shader->UnBindSampler(0, 1);
	this->m_shader->Disable();

	if(m_dynamicCloud)
		RenderSkyPlane();

	if (!AngelCore::AngelSubSystem::SubSystemManager::GetInstance()->GetWorldManager()->GetDynamicWeather())
	{
		m_cbGradiant.Const.apexColor = DirectX::XMFLOAT4(0, 0, 0, 0);
		m_cbGradiant.Const.centerColor = DirectX::XMFLOAT4(0, 0, 0, 0);
	}

}

void AngelCore::AngelRendering::SkyMap::RenderSkyPlane()
{
	float blendFactor[] = { 0.0f, 0.0f, 0.0f, 0.0f };
	auto context = AngelCore::AngelSubSystemResources::GraphicDeviceResources::DeviceContext;
	context->OMSetBlendState(AngelCore::AngelSubSystemResources::GraphicDeviceResources::AdditiveBlending.Get()
		, blendFactor, 0xffffffff);
	unsigned int stride;
	unsigned int offset;

	m_cbWorldPlane.Update();
	m_Planeshader->SetConstantBuffer(0, 1, m_cbWorldPlane.getBuffer());

	m_cbPlane.Update();
	m_Planeshader->SetConstantBuffer(1, 1, m_cbPlane.getBuffer());

	this->m_Planeshader->SetSampler(0, 1, 
		AngelCore::AngelSubSystemResources::GraphicDeviceResources::Samplers.at(0).GetAddressOf());
	m_Planeshader->BindSRV(0, 1, m_cloudTexture1->GetSRV().Get(), ShaderType::PixelShader);
	m_Planeshader->BindSRV(1, 1, m_cloudTexture2->GetSRV().Get(), ShaderType::PixelShader);

	m_Planeshader->Apply();

	// Set vertex buffer stride and offset.
	stride = sizeof(VertexType);
	offset = 0;

	// Set the vertex buffer to active in the input assembler so it can be rendered.
	context->IASetVertexBuffers(0, 1, m_PlanevertexBuffer.GetAddressOf(), &stride, &offset);

	// Set the index buffer to active in the input assembler so it can be rendered.
	context->IASetIndexBuffer(m_PlaneindexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);

	// Set the type of primitive that should be rendered from this vertex buffer, in this case triangles.
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	context->DrawIndexed(m_PlaneindexCount, 0, 0);

	m_Planeshader->UnBindSRV(0, 1, ShaderType::PixelShader);
	m_Planeshader->UnBindSRV(1, 1, ShaderType::PixelShader);
	m_Planeshader->UnBindSampler(0, 1);
	m_Planeshader->Disable();

	context->OMSetBlendState(
		AngelCore::AngelSubSystemResources::GraphicDeviceResources::TransparentBlending.Get()
		, blendFactor, 0xffffffff);
}


void AngelCore::AngelRendering::SkyMap::Update()
{
	//if(m_type==Dynamic)
		//m_curRotation -= m_rotationSpeed/100000;
	//OutputDebugStringA(std::to_string(m_curRotation).c_str());
	//OutputDebugStringA("\n");

	using namespace DirectX;

	XMMATRIX Scale = XMMatrixScaling(5, 5, 5);
	XMMATRIX rotation = XMMatrixRotationX(m_curRotation);
	XMMATRIX Translation = XMMatrixTranslation((
		AngelCore::AngelSubSystemResources::BaseCameraProperties::BCamera->GetCameraPosition().x),
		(AngelCore::AngelSubSystemResources::BaseCameraProperties::BCamera->GetCameraPosition().y),
		(AngelCore::AngelSubSystemResources::BaseCameraProperties::BCamera->GetCameraPosition().z));
	XMMATRIX world = Scale *rotation* Translation;
	XMStoreFloat4x4(&this->m_worldBuffer.Const.WVP,
		XMMatrixTranspose(world
			*AngelCore::AngelSubSystemResources::BaseCameraProperties::BCamera->GetView()
			*AngelCore::AngelSubSystemResources::BaseCameraProperties::BCamera->GetProjection()));

	XMStoreFloat4x4(&this->m_worldBuffer.Const.World, XMMatrixTranspose(world));
	Scale = XMMatrixScaling(5, 5, 5);
	world = Translation;
	XMStoreFloat4x4(&this->m_cbWorldPlane.Const.WVP,
		XMMatrixTranspose(world
			*AngelCore::AngelSubSystemResources::BaseCameraProperties::BCamera->GetView()
			*AngelCore::AngelSubSystemResources::BaseCameraProperties::BCamera->GetProjection()));

	m_textureTranslation[0] += m_translationSpeed[0];
	m_textureTranslation[1] += m_translationSpeed[1];
	m_textureTranslation[2] += m_translationSpeed[2];
	m_textureTranslation[3] += m_translationSpeed[3];

	// Keep the values in the zero to one range.
	if (m_textureTranslation[0] > 1.0f) { m_textureTranslation[0] -= 1.0f; }
	if (m_textureTranslation[1] > 1.0f) { m_textureTranslation[1] -= 1.0f; }
	if (m_textureTranslation[2] > 1.0f) { m_textureTranslation[2] -= 1.0f; }
	if (m_textureTranslation[3] > 1.0f) { m_textureTranslation[3] -= 1.0f; }

	m_cbPlane.Const.brightness = m_brightness;
	m_cbPlane.Const.translation = m_textureTranslation[0];
	m_cbPlane.Const.scale =0.03f;

}

void AngelCore::AngelRendering::SkyMap::SetNextTextureBlend(float blendAmount, 
	ID3D11ShaderResourceView *srvNext)
{
	if (blendAmount < 0.99f)
	{
		m_curBlendAmount = blendAmount;
		m_shadeResourcenext = srvNext;
	}
	else
	{
		m_curBlendAmount = 0.0f;
		m_shadeResourcecurrent = m_shadeResourcenext;
	}
		
}

void AngelCore::AngelRendering::SkyMap::ChangeCenterColor(DirectX::XMFLOAT4 newColor, float speed)
{
	DirectX::XMStoreFloat4(&m_cbGradiant.Const.centerColor,
		DirectX::XMVectorLerp(DirectX::XMLoadFloat4(&m_cbGradiant.Const.centerColor), DirectX::XMLoadFloat4(&newColor), speed));
}

void AngelCore::AngelRendering::SkyMap::ChangeApexColor(DirectX::XMFLOAT4 newColor, float speed)
{
	DirectX::XMStoreFloat4(&m_cbGradiant.Const.apexColor,
		DirectX::XMVectorLerp(DirectX::XMLoadFloat4(&m_cbGradiant.Const.apexColor), DirectX::XMLoadFloat4(&newColor), speed));
}