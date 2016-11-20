#include"../Public/TerrainManager.h"
#include"../../Public/TimeManager.h"
#include<time.h>
#include"../../../AngelCore/AngelSubSystem/AngelRendering/Public/BaseCamera.h"
#include"../../Public/InputManager.h"
#include"../../Public/SubSystemManager.h"
#include"../Public/VertexTypes.h"
#include"../Public/Shader.h"
#include<stdio.h>
#include"../Public/DebugCollisionShape.h"
#include<fstream>
#include"../Public/Texture2D.h"
#include"../../../AngelTools/AngelMathHelper/Public/MathHelper.h"
#include"../../../AngelTools/AngelGlobalTools.h"
#include"../Public/LightManager.h"
#include<AntTweakBar.h>
#include<Source\Include.h>

#define NUM_CHUNKS 50
#define MAX_DISTANCE 200

AngelCore::AngelRendering::TerrainManager * AngelCore::AngelRendering::TerrainManager::m_instance = nullptr;

AngelCore::AngelRendering::TerrainManager * AngelCore::AngelRendering::TerrainManager::GetInstance()
{
	if (!m_instance)
	{
		m_instance = new TerrainManager();
		return m_instance;
	}

	return m_instance;
}

void AngelCore::AngelRendering::TerrainManager::CreateInstance()
{
	if (!m_instance)
	{
		m_instance = new TerrainManager();
	}
}


AngelCore::AngelRendering::TerrainManager::TerrainManager() : m_heightMap(nullptr) , m_terrainModel(nullptr)
, m_pickedTriangle(-1),m_vertexCount(0),m_vertexBuffer(nullptr) , m_indexBuffer(nullptr),m_vertices(nullptr)
,m_indices(nullptr),m_rawImage(nullptr) , m_colorMap(nullptr) , m_heightThreshold(0.0f), 
m_currentColor(DirectX::XMFLOAT4(1.0f,1.0f,1.0f,1.0f))
, m_applyColor(false), m_applyHeight(false), m_layerMap(nullptr),m_selectedLayer(0),m_applyLayer(false)
,m_addModel(false),m_selectedModel(0),m_mousePosInTerrainSpace(DirectX::XMFLOAT3(0,0,0))
,m_lPointCount(0),m_buffersInitialized(false),m_initialized(false)
{

}

void AngelCore::AngelRendering::TerrainManager::Initialize()
{
	TwBar *bar = TwNewBar("Terrain Editor");
	int k = TwDefine(" options for customizing terrains. "); // Message added to the help bar.
	int barSize[2] = { 224, 320 };
	int j = TwSetParam(bar, NULL, "size", TW_PARAM_INT32, 2, barSize);
	m_terrainHeight = 50;
	m_terrainWidth = 50;
	m_heightScale = 3.0;
	m_terrainFilename = "heightmap.r16";

	TwStructMember pointMembers[] = {
		{ "X", TW_TYPE_FLOAT, offsetof(Vector3, Vector3::m_x), " Min=-100 Max=100 Step=0.01 " },
		{ "Y", TW_TYPE_FLOAT, offsetof(Vector3, Vector3::m_y), " Min=-100 Max=100 Step=0.01 " },
		{ "Z", TW_TYPE_FLOAT, offsetof(Vector3, Vector3::m_z), "	 Min=-100 Max=100 Step=0.01 " } };
	TwType pointType = TwDefineStruct("POINT", pointMembers, 3, sizeof(Vector3), NULL, NULL);

	int i = TwAddVarRW(bar, "Height Threshold", TW_TYPE_FLOAT, &m_heightThreshold, "Min=-100 Max=100 Step=0.01 ");
	i = TwAddVarRW(bar, "Height Scale", TW_TYPE_FLOAT, &m_heightScale, "Min=-100 Max=100 Step=0.01 ");
	
	i = TwAddVarRW(bar, "Color", TW_TYPE_COLOR4F, &m_currentColor, "");

	i = TwAddVarRW(bar, "Apply Color", TW_TYPE_BOOL8, &m_applyColor, "");
	
	i = TwAddVarRW(bar, "Apply Height", TW_TYPE_BOOL8, &m_applyHeight, "");
	
	i = TwAddVarRW(bar, "Apply Layer", TW_TYPE_BOOL8, &m_applyLayer, "");
	
	i = TwAddVarRW(bar, "Apply Model", TW_TYPE_BOOL8, &m_addModel, "");
	
	i = TwAddVarRW(bar, "Selected Layer", TW_TYPE_INT32, &m_selectedLayer, "");
	
	i = TwAddVarRW(bar, "Selected Model", TW_TYPE_INT32, &m_selectedModel, "");
	
	m_collisonShape = new DebugCollisionShape();

	m_indexCount = (m_terrainWidth - 1) * (m_terrainHeight - 1) * 6;;

	LoadRawHeightMap();
	LoadColorMap();
	LoadLayerMap();
	LoadHeightMap("heightmap.bmp");
	BuildTileNeighbors();
	UpdateTileNeighbors();
	SetTerrainCoordinates();
	CalculateNormals();
	BuildTerrainModel();
	UpdateBuffers();
	CreateChunks();

	//Load some models
	TerrainObjects * to1 = new TerrainObjects();
	to1->mesh = new AngelCore::AngelWorldSystem::MeshRenderer();
	to1->mesh->Initialize(nullptr,"tree1.cmo", false, 1, true);
	to1->instances = 0;

	TerrainObjects * to2 = new TerrainObjects();
	to2->mesh = new AngelCore::AngelWorldSystem::MeshRenderer();
	to2->mesh->Initialize(nullptr, "grass.cmo", false, 1, true);
	to2->instances = 0;

	TerrainObjects * to3 = new TerrainObjects();
	to3->mesh = new AngelCore::AngelWorldSystem::MeshRenderer();
	to3->mesh->Initialize(nullptr, "grass2.cmo", false, 1, true);
	to3->instances = 0;
	m_terrainObjects.push_back(to1);
	m_terrainObjects.push_back(to2);
	m_terrainObjects.push_back(to3);
	
	
	m_dirtTexture = new Texture2D();
	m_dirtTexture->LoadTexture(
		L"grass.dds",true);

	m_StoneTexture = new Texture2D();
	m_StoneTexture->LoadTexture(
		L"grass.dds", true);

	m_dirt1Texture = new Texture2D();
	m_dirt1Texture->LoadTexture(
		L"dirt2.dds", true);

	m_dirt2Texture = new Texture2D();
	m_dirt2Texture->LoadTexture(
		L"grass.dds",true);

	m_shader = new Shader();
	Shader::LoadShader(ShaderType::VertexShader, "VertexTerrain.cso", VertexTypes::Terrain, m_shader);
	Shader::LoadShader(ShaderType::PixelShader, "PixelTerrain.cso", VertexTypes::NOP, m_shader);

	m_cbWorld.Load();
	m_cbTextureData.Load();
	/*Microsoft::WRL::ComPtr<ID3D11Texture2D> t;
	m_fullTexture->GetResource().As(&t);

	D3D11_TEXTURE2D_DESC desc;
	t->GetDesc(&desc);

	m_cbTextureData.Const.width = 800;
	m_cbTextureData.Const.height = 600;
	m_cbTextureData.Const.numTileWidth = m_terrainWidth - 1;
	m_cbTextureData.Const.numTileHeight = m_terrainHeight - 1;*/

	m_cbVertexTranslation.Load();

	InitializeFreeDrawing();

	m_enable = false;
	m_initialized = true;

}

bool AngelCore::AngelRendering::TerrainManager::CalculateNormals()
{
	int i, j, index1, index2, index3, index;
	float vertex1[3], vertex2[3], vertex3[3], vector1[3], vector2[3], sum[3], length;
	VectorType* normals;


	// Create a temporary array to hold the face normal vectors.
	normals = new VectorType[(m_terrainHeight - 1) * (m_terrainWidth - 1)];
	if (!normals)
	{
		return false;
	}

	// Go through all the faces in the mesh and calculate their normals.
	for (j = 0; j<(m_terrainHeight - 1); j++)
	{
		for (i = 0; i<(m_terrainWidth - 1); i++)
		{
			index1 = ((j + 1) * m_terrainWidth) + i;      // Bottom left vertex.
			index2 = ((j + 1) * m_terrainWidth) + (i + 1);  // Bottom right vertex.
			index3 = (j * m_terrainWidth) + i;          // Upper left vertex.

														// Get three vertices from the face.
			vertex1[0] = m_heightMap[index1].x;
			vertex1[1] = m_heightMap[index1].y;
			vertex1[2] = m_heightMap[index1].z;

			vertex2[0] = m_heightMap[index2].x;
			vertex2[1] = m_heightMap[index2].y;
			vertex2[2] = m_heightMap[index2].z;

			vertex3[0] = m_heightMap[index3].x;
			vertex3[1] = m_heightMap[index3].y;
			vertex3[2] = m_heightMap[index3].z;

			// Calculate the two vectors for this face.
			vector1[0] = vertex1[0] - vertex3[0];
			vector1[1] = vertex1[1] - vertex3[1];
			vector1[2] = vertex1[2] - vertex3[2];
			vector2[0] = vertex3[0] - vertex2[0];
			vector2[1] = vertex3[1] - vertex2[1];
			vector2[2] = vertex3[2] - vertex2[2];

			index = (j * (m_terrainWidth - 1)) + i;

			// Calculate the cross product of those two vectors to get the un-normalized value for this face normal.
			normals[index].x = (vector1[1] * vector2[2]) - (vector1[2] * vector2[1]);
			normals[index].y = (vector1[2] * vector2[0]) - (vector1[0] * vector2[2]);
			normals[index].z = (vector1[0] * vector2[1]) - (vector1[1] * vector2[0]);

			// Calculate the length.
			length = (float)sqrt((normals[index].x * normals[index].x) + (normals[index].y * normals[index].y) +
				(normals[index].z * normals[index].z));

			// Normalize the final value for this face using the length.
			normals[index].x = (normals[index].x / length);
			normals[index].y = (normals[index].y / length);
			normals[index].z = (normals[index].z / length);
		}
	}

	// Now go through all the vertices and take a sum of the face normals that touch this vertex.
	for (j = 0; j<m_terrainHeight; j++)
	{
		for (i = 0; i<m_terrainWidth; i++)
		{
			// Initialize the sum.
			sum[0] = 0.0f;
			sum[1] = 0.0f;
			sum[2] = 0.0f;

			// Bottom left face.
			if (((i - 1) >= 0) && ((j - 1) >= 0))
			{
				index = ((j - 1) * (m_terrainWidth - 1)) + (i - 1);

				sum[0] += normals[index].x;
				sum[1] += normals[index].y;
				sum[2] += normals[index].z;
			}

			// Bottom right face.
			if ((i<(m_terrainWidth - 1)) && ((j - 1) >= 0))
			{
				index = ((j - 1) * (m_terrainWidth - 1)) + i;

				sum[0] += normals[index].x;
				sum[1] += normals[index].y;
				sum[2] += normals[index].z;
			}

			// Upper left face.
			if (((i - 1) >= 0) && (j<(m_terrainHeight - 1)))
			{
				index = (j * (m_terrainWidth - 1)) + (i - 1);

				sum[0] += normals[index].x;
				sum[1] += normals[index].y;
				sum[2] += normals[index].z;
			}

			// Upper right face.
			if ((i < (m_terrainWidth - 1)) && (j < (m_terrainHeight - 1)))
			{
				index = (j * (m_terrainWidth - 1)) + i;

				sum[0] += normals[index].x;
				sum[1] += normals[index].y;
				sum[2] += normals[index].z;
			}

			// Calculate the length of this normal.
			length = (float)sqrt((sum[0] * sum[0]) + (sum[1] * sum[1]) + (sum[2] * sum[2]));

			// Get an index to the vertex location in the height map array.
			index = (j * m_terrainWidth) + i;

			// Normalize the final shared normal for this vertex and store it in the height map array.
			m_heightMap[index].nx = (sum[0] / length);
			m_heightMap[index].ny = (sum[1] / length);
			m_heightMap[index].nz = (sum[2] / length);
		}
	}

	// Release the temporary normals.
	delete[] normals;
	normals = 0;

	return true;
}

bool AngelCore::AngelRendering::TerrainManager::LoadHeightMap(char* filename)
{
	int error, imageSize, i, j, k, index;
	FILE* filePtr;
	unsigned long long count;
	BITMAPFILEHEADER bitmapFileHeader;
	BITMAPINFOHEADER bitmapInfoHeader;
	unsigned char* bitmapImage;
	unsigned char height;


	// Start by creating the array structure to hold the height map data.
	m_testMap = new float[m_terrainWidth * m_terrainHeight];
	if (!m_heightMap)
	{
		return false;
	}

	// Open the bitmap map file in binary.
	error = fopen_s(&filePtr, filename, "rb");
	if (error != 0)
	{
		return false;
	}

	// Read in the bitmap file header.
	count = fread(&bitmapFileHeader, sizeof(BITMAPFILEHEADER), 1, filePtr);
	if (count != 1)
	{
		return false;
	}

	// Read in the bitmap info header.
	count = fread(&bitmapInfoHeader, sizeof(BITMAPINFOHEADER), 1, filePtr);
	if (count != 1)
	{
		return false;
	}

	// Make sure the height map dimensions are the same as the terrain dimensions for easy 1 to 1 mapping.
	if ((bitmapInfoHeader.biHeight != m_terrainHeight) || (bitmapInfoHeader.biWidth != m_terrainWidth))
	{
		return false;
	}

	// Calculate the size of the bitmap image data.  
	// Since we use non-divide by 2 dimensions (eg. 257x257) we need to add an extra byte to each line.
	imageSize = m_terrainWidth * m_terrainHeight * 3;

	// Allocate memory for the bitmap image data.
	bitmapImage = new unsigned char[imageSize];
	if (!bitmapImage)
	{
		return false;
	}

	// Move to the beginning of the bitmap data.
	fseek(filePtr, bitmapFileHeader.bfOffBits, SEEK_SET);

	// Read in the bitmap image data.
	count = fread(bitmapImage, 1, imageSize, filePtr);
	if (count != imageSize)
	{
		return false;
	}

	// Close the file.
	error = fclose(filePtr);
	if (error != 0)
	{
		return false;
	}

	// Initialize the position in the image data buffer.
	k = 0;

	// Read the image data into the height map array.
	for (j = 0; j<m_terrainHeight; j++)
	{
		for (i = 0; i<m_terrainWidth; i++)
		{
			// Bitmaps are upside down so load bottom to top into the height map array.
			index = (m_terrainWidth * (m_terrainHeight - 1 - j)) + i;

			// Get the grey scale pixel value from the bitmap image data at this location.
			height = bitmapImage[k];

			// Store the pixel value as the height at this point in the height map array.
			m_testMap[index] = (float)height/255;

			// Increment the bitmap image data index.
			k += 3;
		}

		// Compensate for the extra byte at end of each line in non-divide by 2 bitmaps (eg. 257x257).
		k++;
	}

	// Release the bitmap image data now that the height map array has been loaded.
	

	return true;
}

void AngelCore::AngelRendering::TerrainManager::UpdateBuffers()
{
	using namespace DirectX;
	UINT newVertexSize = (m_terrainWidth - 1) * (m_terrainHeight - 1) * 6;
	if (newVertexSize != m_vertexCount || !m_vertexBuffer || !m_indexBuffer)
	{
		m_vertexCount = newVertexSize;
		m_indexCount = m_vertexCount;
		D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
		vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		vertexBufferDesc.ByteWidth = sizeof(VertexTypesStruct::Terrain) * m_vertexCount;
		vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		vertexBufferDesc.MiscFlags = 0;
		vertexBufferDesc.StructureByteStride = 0;

		HR(AngelCore::AngelSubSystemResources::GraphicDeviceResources::Device->
			CreateBuffer(&vertexBufferDesc, nullptr, &m_vertexBuffer));

		indexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		indexBufferDesc.ByteWidth = sizeof(unsigned long) * m_indexCount;
		indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		indexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		indexBufferDesc.MiscFlags = 0;
		indexBufferDesc.StructureByteStride = 0;


		HR(AngelCore::AngelSubSystemResources::GraphicDeviceResources::Device
			->CreateBuffer(&indexBufferDesc, nullptr, &m_indexBuffer));

	}

	int i;
	XMFLOAT4 color;
	// Set the color of the terrain grid.
	color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

	// Calculate the number of vertices in the terrain.
	m_vertexCount = (m_terrainWidth - 1) * (m_terrainHeight - 1) * 6;

	// Set the index count to the same as the vertex count.
	
	if(!m_vertices)
	// Create the vertex array.
	m_vertices = new VertexTypesStruct::Terrain[m_vertexCount];

	if(!m_indices)
	// Create the index array.
	m_indices = new unsigned long[m_indexCount];
	// Load the vertex array and index array with 3D terrain model data.
	for (i = 0; i<m_vertexCount; i++)
	{
		m_vertices[i].Position = XMFLOAT3(m_terrainModel[i].x, m_terrainModel[i].y, m_terrainModel[i].z);
		m_vertices[i].TexCoord = DirectX::XMFLOAT2(m_terrainModel[i].tu, m_terrainModel[i].tv);
		m_vertices[i].Normal = XMFLOAT3(m_terrainModel[i].nx, m_terrainModel[i].ny, m_terrainModel[i].nz);
		m_vertices[i].Color = XMFLOAT3(m_terrainModel[i].r, m_terrainModel[i].g, m_terrainModel[i].b);
		m_vertices[i].Layers[0] = m_terrainModel[i].l1;
		m_vertices[i].Layers[1] = m_terrainModel[i].l2;
		m_vertices[i].Layers[2] = m_terrainModel[i].l3;
		m_vertices[i].Layers[3] = m_terrainModel[i].l4;
		m_vertices[i].TileNumber = m_terrainModel[i].tileNumber;
		m_indices[i] = i;
	}

	D3D11_MAPPED_SUBRESOURCE vtx_resource, idx_resource;
	HR(AngelCore::AngelSubSystemResources::GraphicDeviceResources::DeviceContext
		->Map(m_vertexBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &vtx_resource));

	HR(AngelCore::AngelSubSystemResources::GraphicDeviceResources::DeviceContext
		->Map(m_indexBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &idx_resource));

	VertexTypesStruct::Terrain * vertices = (VertexTypesStruct::Terrain *)vtx_resource.pData;
	unsigned long * indices = (unsigned long *)idx_resource.pData;

	memcpy(vertices, &m_vertices[0], m_vertexCount * sizeof(VertexTypesStruct::Terrain));
	memcpy(indices, &m_indices[0], m_indexCount * sizeof(unsigned long));

	AngelCore::AngelSubSystemResources::GraphicDeviceResources::DeviceContext->Unmap(m_vertexBuffer.Get(), 0);
	AngelCore::AngelSubSystemResources::GraphicDeviceResources::DeviceContext->Unmap(m_indexBuffer.Get(), 0);
}
bool AngelCore::AngelRendering::TerrainManager::LoadLayerMap()
{
	int  i, j, index;
	unsigned long long imageSize;

	imageSize = m_terrainHeight * m_terrainWidth;
	if (!m_layerMap)
	{
		m_layerMap = new DirectX::XMFLOAT4[m_terrainWidth * m_terrainHeight];
		for (j = 0; j<m_terrainHeight; j++)
		{
			for (i = 0; i<m_terrainWidth; i++)
			{
				index = (m_terrainWidth * j) + i;

				// Store the height at this point in the height map array.
				m_layerMap[index] = DirectX::XMFLOAT4(1.0f, 0.0f, 0.0f,0.0f);
			}
		}
		for (j = 0; j<m_terrainHeight; j++)
		{
			for (i = 0; i<m_terrainWidth; i++)
			{
				index = (m_terrainWidth * j) + i;

				// Store the height at this point in the height map array.
				m_heightMap[index].l1 = m_layerMap[index].x;
				m_heightMap[index].l2 = m_layerMap[index].y;
				m_heightMap[index].l3 = m_layerMap[index].z;
				m_heightMap[index].l4 = m_layerMap[index].w;
			}
		}
	}

	if (m_pickedTriangle != -1)
	{
		int mul = m_pickedTriangle / ((m_terrainHeight * 2) - 2);
		UINT vertex = (m_pickedTriangle) / 2 + mul;

		//m_rawImage[vertex] += 3.5f;

		//for (j = 0; j<m_terrainHeight; j++)
		//{
		//	for (i = 0; i<m_terrainWidth; i++)
		//	{
		//		index = (m_terrainWidth * j) + i;

		//		// Store the height at this point in the height map array.
		//		m_heightMap[index].y = (float)m_rawImage[index];
		//	}
		//}
		//m_layerMap[vertex].x = 0;
		//m_layerMap[vertex].y = 0;
		//m_layerMap[vertex].z = 0;
		//m_layerMap[vertex].w = 0;
		if(m_selectedLayer==0)
			m_layerMap[vertex].x += 0.01f;
		if (m_selectedLayer == 1)
			m_layerMap[vertex].y += 0.01f;
		if (m_selectedLayer == 2)
			m_layerMap[vertex].z += 0.01f;
		if (m_selectedLayer == 3)
			m_layerMap[vertex].w += 0.01f;
		m_heightMap[vertex].l1 = m_layerMap[vertex].x;
		m_heightMap[vertex].l2 = m_layerMap[vertex].y;
		m_heightMap[vertex].l3 = m_layerMap[vertex].z;
		m_heightMap[vertex].l4 = m_layerMap[vertex].w;
	}

	// Read in the raw image data.
	//count = fread(rawImage, sizeof(unsigned short), imageSize, filePtr);
	//FILE * newFilePtr = fopen("newH.r16", "wb");
	/*if (!newFilePtr)
	{
	return false;
	}*/
	//fwrite(rawImage, sizeof(unsigned short), imageSize, newFilePtr);
	///*if (count != imageSize)
	//{
	//	return false;
	//}*/

	//// Close the file.
	//error = fclose(filePtr);


	// Copy the image data into the height map array.

	return true;
}
bool AngelCore::AngelRendering::TerrainManager::LoadColorMap()
{
	int  i, j, index;
	FILE* filePtr;
	unsigned long long imageSize, count;

	imageSize = m_terrainHeight * m_terrainWidth;
	if (!m_colorMap)
	{
		m_colorMap = new DirectX::XMFLOAT3[m_terrainWidth * m_terrainHeight];
		for (j = 0; j<m_terrainHeight; j++)
		{
			for (i = 0; i<m_terrainWidth; i++)
			{
				index = (m_terrainWidth * j) + i;

				// Store the height at this point in the height map array.
				m_colorMap[index] = DirectX::XMFLOAT3(1.0f,1.0f,1.0f);
			}
		}
		for (j = 0; j<m_terrainHeight; j++)
		{
			for (i = 0; i<m_terrainWidth; i++)
			{
				index = (m_terrainWidth * j) + i;

				// Store the height at this point in the height map array.
				m_heightMap[index].r = m_colorMap[index].x;
				m_heightMap[index].g = m_colorMap[index].y;
				m_heightMap[index].b = m_colorMap[index].z;
			}
		}
	}

	if (m_pickedTriangle != -1)
	{
		int mul = m_pickedTriangle / ((m_terrainHeight * 2) - 2);
		UINT vertex = (m_pickedTriangle) / 2 + mul;

		//m_rawImage[vertex] += 3.5f;

		//for (j = 0; j<m_terrainHeight; j++)
		//{
		//	for (i = 0; i<m_terrainWidth; i++)
		//	{
		//		index = (m_terrainWidth * j) + i;

		//		// Store the height at this point in the height map array.
		//		m_heightMap[index].y = (float)m_rawImage[index];
		//	}
		//}
		m_colorMap[vertex] = DirectX::XMFLOAT3(
			m_currentColor.x, m_currentColor.y, m_currentColor.z);
		m_heightMap[vertex].r = m_colorMap[vertex].x;
		m_heightMap[vertex].g = m_colorMap[vertex].y;
		m_heightMap[vertex].b = m_colorMap[vertex].z;
	}

	// Read in the raw image data.
	//count = fread(rawImage, sizeof(unsigned short), imageSize, filePtr);
	//FILE * newFilePtr = fopen("newH.r16", "wb");
	/*if (!newFilePtr)
	{
	return false;
	}*/
	//fwrite(rawImage, sizeof(unsigned short), imageSize, newFilePtr);
	///*if (count != imageSize)
	//{
	//	return false;
	//}*/

	//// Close the file.
	//error = fclose(filePtr);


	// Copy the image data into the height map array.

	return true;
}
bool AngelCore::AngelRendering::TerrainManager::LoadRawHeightMap()
{
	int  i, j, index;
	unsigned long long imageSize;
	

	if(!m_heightMap)
	// Create the float array to hold the height map data.
	m_heightMap = new HeightMapType[m_terrainWidth * m_terrainHeight];
	// Open the 16 bit raw height map file for reading in binary.
	// Calculate the size of the raw image data.
	imageSize = m_terrainHeight * m_terrainWidth;
	if (!m_rawImage)
	{
		m_rawImage = new unsigned short[imageSize];
		for (j = 0; j<m_terrainHeight; j++)
		{
			for (i = 0; i<m_terrainWidth; i++)
			{
				index = (m_terrainWidth * j) + i;
				m_rawImage[index] = 0.0f;
			}
		}

		for (j = 0; j<m_terrainHeight; j++)
		{
			for (i = 0; i<m_terrainWidth; i++)
			{
				index = (m_terrainWidth * j) + i;

				// Store the height at this point in the height map array.
				m_heightMap[index].y = (float)m_rawImage[index];
			}
		}
	}

	if (m_pickedTriangle != -1)
	{
		int mul = m_pickedTriangle / ((m_terrainHeight*2)-2);
		UINT vertex = (m_pickedTriangle) / 2 + mul;
		
		m_rawImage[vertex] += m_heightThreshold;
		
		for (j = 0; j<m_terrainHeight; j++)
		{
			for (i = 0; i<m_terrainWidth; i++)
			{
				index = (m_terrainWidth * j) + i;

				// Store the height at this point in the height map array.
				m_heightMap[index].y = (float)m_rawImage[index];
			}
		}
		m_heightMap[vertex].y = (float)m_rawImage[vertex];
	}
		
	// Read in the raw image data.
	//count = fread(rawImage, sizeof(unsigned short), imageSize, filePtr);
	//FILE * newFilePtr = fopen("newH.r16", "wb");
	/*if (!newFilePtr)
	{
		return false;
	}*/
	//fwrite(rawImage, sizeof(unsigned short), imageSize, newFilePtr);
	///*if (count != imageSize)
	//{
	//	return false;
	//}*/

	//// Close the file.
	//error = fclose(filePtr);
	

	// Copy the image data into the height map array.
	
	return true;
}
void AngelCore::AngelRendering::TerrainManager::CreateChunks()
{
	using namespace DirectX;
	m_chunks = new Chunk[NUM_CHUNKS];
	UINT vertexPerChunk = m_vertexCount / NUM_CHUNKS;
	UINT startLocation = 0;
	for (int i = 0;i < NUM_CHUNKS;i++)
	{
		DirectX::XMFLOAT3 * points = new DirectX::XMFLOAT3[vertexPerChunk];
		m_chunks[i].vertices = new VertexTypesStruct::Terrain[vertexPerChunk];
		m_chunks[i].indices = new unsigned long[vertexPerChunk];
		m_chunks[i].indicesEndLocation = vertexPerChunk+ startLocation;
		m_chunks[i].indicesStartLocation = startLocation;
		for (unsigned int j = startLocation,k=0;j < vertexPerChunk+ startLocation;j++,k++)
		{
			points[k] = m_vertices[j].Position;
			m_chunks[i].vertices[k] = m_vertices[j];
			m_chunks[i].indices[k] = m_indices[j];

		}
		m_chunks[i].bondingBox.CreateFromPoints(m_chunks[i].bondingBox, vertexPerChunk, points, sizeof(XMFLOAT3));

		m_chunks[i].collsionShape = new DebugCollisionShape();
		m_chunks[i].collsionShape->Create();
		startLocation += vertexPerChunk;
	}

}
void AngelCore::AngelRendering::TerrainManager::BuildTileNeighbors()
{
	m_tileNeighbors = new TileNeighbors[m_terrainHeight*m_terrainWidth];
	UINT triCount = m_indexCount / 3;
	for (unsigned int i = 0;i < triCount;i++)
	{
		int mul = i / ((m_terrainHeight * 2) - 2);
		int vertex = (i) / 2 + mul;
		m_tileNeighbors[vertex].tileIndex = vertex;
		int t = (m_terrainHeight*(mul + 1)) - 2;
		//If we have up neighbor
		if (vertex != t)
		{
			m_tileNeighbors[vertex].tileUpIndex = vertex + 1;
		}
		else
			m_tileNeighbors[vertex].tileUpIndex = -1;
		//If we have down neighbor
		if ((vertex) % (m_terrainHeight) != 0)
		{
			m_tileNeighbors[vertex].tileDownIndex = vertex - 1;
		}
		else
			m_tileNeighbors[vertex].tileDownIndex = -1;
		//If we right neighbor
		if (vertex - (m_terrainWidth - 1) > 0)
			m_tileNeighbors[vertex].tileRightIndex = vertex - (m_terrainWidth);
		else
			m_tileNeighbors[vertex].tileRightIndex = -1;

		//If we left neighbor
		if (mul != m_terrainWidth - 2)
			m_tileNeighbors[vertex].tileLeftIndex = vertex + (m_terrainWidth);
		else
			m_tileNeighbors[vertex].tileLeftIndex = -1;

	}
}
void AngelCore::AngelRendering::TerrainManager::UpdateTileNeighbors()
{
	//UINT triCount = m_indexCount / 3;
	//for (int i = 0;i < triCount;i++)
	//{
	//	int mul = i / ((m_terrainHeight * 2) - 2);
	//	int vertex = (i) / 2 + mul;

	//	//Initalize with zero
	//	m_heightMap[vertex].u = 0;
	//	m_heightMap[vertex].rt = 0;
	//	m_heightMap[vertex].l = 0;
	//	m_heightMap[vertex].d = 0;

	//	//Check up tile
	//	if (GetTileLayer(m_tileNeighbors[vertex].tileIndex)!= GetTileLayer(m_tileNeighbors[vertex].tileUpIndex))
	//	{
	//		//if we have up tile
	//		if(m_tileNeighbors[vertex].tileUpIndex!=-1)
	//			m_heightMap[vertex].u = 1;
	//	}

	//	//Check right tile
	//	if (GetTileLayer(m_tileNeighbors[vertex].tileIndex) != GetTileLayer(m_tileNeighbors[vertex].tileRightIndex))
	//	{
	//		//if we have right tile
	//		if (m_tileNeighbors[vertex].tileRightIndex != -1)
	//			m_heightMap[vertex].rt = 1;
	//	}

	//	//Check left tile
	//	if (GetTileLayer(m_tileNeighbors[vertex].tileIndex) != GetTileLayer(m_tileNeighbors[vertex].tileLeftIndex))
	//	{
	//		//if we have left tile
	//		if (m_tileNeighbors[vertex].tileLeftIndex != -1)
	//			m_heightMap[vertex].l = 1;
	//	}

	//	//Check down tile
	//	if (GetTileLayer(m_tileNeighbors[vertex].tileIndex) != GetTileLayer(m_tileNeighbors[vertex].tileDownIndex))
	//	{
	//		//if we have down tile
	//		if (m_tileNeighbors[vertex].tileDownIndex != -1)
	//			m_heightMap[vertex].d = 1;
	//	}
	//}
}

int AngelCore::AngelRendering::TerrainManager::GetTileLayer(int index)
{
	if (m_heightMap[index].l1 == 1)
		return 1;
	if (m_heightMap[index].l2 == 1)
		return 2;
	if (m_heightMap[index].l3 == 1)
		return 3;
	if (m_heightMap[index].l4 == 1)
		return 4;
}

void AngelCore::AngelRendering::TerrainManager::AddTerrainObject(int index)
{
	int mul = m_pickedTriangle / ((m_terrainHeight * 2) - 2);
	UINT vertex = (m_pickedTriangle) / 2 + mul;

	m_terrainObjects.at(index)->instances++;
	m_terrainObjects.at(index)->Position.push_back(DirectX::XMFLOAT3(m_mousePosInTerrainSpace.x
		,m_heightMap[vertex].y, m_mousePosInTerrainSpace.z));
	m_terrainObjects.at(index)->Rotation.push_back(DirectX::XMFLOAT3(0,0,0));
	if (index==2)
		m_terrainObjects.at(index)->Scale.push_back(DirectX::XMFLOAT3(0.1,0.1,0.1));
	else
		m_terrainObjects.at(index)->Scale.push_back(DirectX::XMFLOAT3(1, 1, 1));
}

void AngelCore::AngelRendering::TerrainManager::Update()
{
	using namespace DirectX;

	if (AngelCore::AngelSubSystem::inputManager->IsLeftMouseClicked() && m_pickedTriangle!=-1)
	{
		int mul = m_pickedTriangle / ((m_terrainHeight * 2) - 2);
		UINT vertex = (m_pickedTriangle) / 2 + mul;
		if (m_applyHeight)
		{
			LoadRawHeightMap();
			CalculateNormals();
			SetTerrainCoordinates();
		}
			

		if(m_applyColor)
			LoadColorMap();

		if (m_applyLayer)
		{
			LoadLayerMap();
			UpdateTileNeighbors();
		}

		if (m_addModel)
		{
			AddTerrainObject(m_selectedModel);
		}
			

		
		BuildTerrainModel();
		UpdateBuffers();
	}

	m_pickedTriangle = -1;
	for (int i = 0;i < NUM_CHUNKS;i++)
	{
		DirectX::BoundingOrientedBox tmp;
		tmp.Center = m_chunks[i].bondingBox.Center;
		tmp.Extents = m_chunks[i].bondingBox.Extents;
		tmp.Transform(tmp, DirectX::XMMatrixIdentity());
		m_chunks[i].collsionShape->SetBoundingBox(tmp);
		m_chunks[i].collsionShape->Update(
			AngelCore::AngelSubSystemResources::BaseCameraProperties::BCamera->GetView(),
			AngelCore::AngelSubSystemResources::BaseCameraProperties::BCamera->GetProjection());
	}


	std::vector<int> selectedChunks;
	float min = INT_MAX;
	float dist = 0.0f;
	for (int i = 0;i < NUM_CHUNKS;i++)
	{
		DirectX::BoundingOrientedBox tmp;
		tmp.Center = m_chunks[i].bondingBox.Center;
		tmp.Extents = m_chunks[i].bondingBox.Extents;
		if (AngelCore::AngelSubSystemResources::BaseCameraProperties::BCamera->TestIntersects(tmp,
			DirectX::XMMatrixIdentity(), nullptr, &m_mousePosInTerrainSpace,&dist))
		{
			if (dist < MAX_DISTANCE)
				selectedChunks.push_back(i);
		}
	}



	if (selectedChunks.size() != 0)
	{
		for (unsigned int i = 0;i < selectedChunks.size();i++)
		{
			//OutputDebugStringA("\n");
			//OutputDebugStringA("Selected Chunk :  ");
			//OutputDebugStringA(std::to_string(selectedChunks[i]).c_str());
			UINT startTri = m_chunks[selectedChunks[i]].indicesStartLocation / 3;
			UINT triCount = (m_chunks[selectedChunks[i]].indicesEndLocation - m_chunks[selectedChunks[i]].indicesStartLocation) / 3;
			for (unsigned int i = startTri
				;i <startTri + triCount
				;i++)
			{
				UINT i0 = m_indices[i * 3 + 0];
				UINT i1 = m_indices[i * 3 + 1];
				UINT i2 = m_indices[i * 3 + 2];

				DirectX::XMVECTOR v0 = DirectX::XMLoadFloat3(&m_vertices[i0].Position);
				DirectX::XMVECTOR v1 = DirectX::XMLoadFloat3(&m_vertices[i1].Position);
				DirectX::XMVECTOR v2 = DirectX::XMLoadFloat3(&m_vertices[i2].Position);
				if (AngelCore::AngelSubSystemResources::BaseCameraProperties::BCamera->TestIntersectsTriangle
					(
						DirectX::XMMatrixIdentity(), v0, v1, v2))
				{
					////OutputDebugStringA("\n");
					//OutputDebugStringA("Intersects : ");
					//OutputDebugStringA(std::to_string(i).c_str());
					m_pickedTriangle = i;
					break;
				}
				else
				{
					//OutputDebugStringA("\n");
					//OutputDebugStringA("NOT Intersects");
				}
			}
		}
	}

	HandleFreeDrawing();
}

void AngelCore::AngelRendering::TerrainManager::Render()
{
	DirectX::XMStoreFloat4x4(&m_cbWorld.Const.WVP
		, DirectX::XMMatrixTranspose(
			DirectX::XMMatrixIdentity()*
			AngelCore::AngelSubSystemResources::BaseCameraProperties::BCamera->GetView()*
			AngelCore::AngelSubSystemResources::BaseCameraProperties::BCamera->GetProjection()));

	DirectX::XMStoreFloat4x4(&m_cbWorld.Const.WorldView
		, DirectX::XMMatrixTranspose(
			DirectX::XMMatrixIdentity()));

	DirectX::XMStoreFloat4x4(&m_cbWorld.Const.WVPLast
		, DirectX::XMMatrixTranspose(
			DirectX::XMMatrixIdentity()));

	DirectX::XMStoreFloat4x4(&m_cbWorld.Const.WorldInvTranspose
		, InverseTranspose(DirectX::XMMatrixIdentity()));

	DirectX::XMStoreFloat4x4(&this->m_cbWorld.Const.lightViewMatrix, XMMatrixTranspose(
		XMLoadFloat4x4(&AngelCore::AngelRendering::LightManager::GetInstance()->GetViews()[0])));
	DirectX::XMStoreFloat4x4(&this->m_cbWorld.Const.lightProjectionMatrix,
		XMMatrixTranspose
		(XMLoadFloat4x4(&AngelCore::AngelRendering::LightManager::GetInstance()->GetProjections()[0])));

	AngelCore::AngelSubSystemResources::GraphicDeviceResources::DeviceContext->RSSetState(
		AngelCore::AngelSubSystemResources::GraphicDeviceResources::SolidRasterizer.Get());
	//AngelCore::AngelSubSystemResources::GraphicDeviceResources::DeviceContext->draind
	UINT stride = sizeof(VertexTypesStruct::Terrain);
	UINT offset = 0;
	m_cbWorld.Update();
	m_shader->SetConstantBuffer(0, 1, m_cbWorld.getBuffer());
	AngelCore::AngelRendering::LightManager::GetInstance()->Update();
	AngelCore::AngelRendering::LightManager::GetInstance()->GetBuffer().Update();
	m_shader->SetConstantBuffer(2, 1, AngelCore::AngelRendering::LightManager::GetInstance()->GetBuffer().getBuffer());
	m_shader->SetSampler(0, 1, AngelCore::AngelSubSystemResources::GraphicDeviceResources::Samplers.at(0).GetAddressOf());
	m_cbTextureData.Update();
	m_shader->SetConstantBuffer(4, 1, m_cbTextureData.getBuffer());
	m_srv[0] = m_dirtTexture->GetSRV().Get();
	m_srv[1] = m_dirt1Texture->GetSRV().Get();
	m_srv[2] = m_dirt2Texture->GetSRV().Get();
	m_srv[3] = m_StoneTexture->GetSRV().Get();
	m_shader->BindSRV(10, 4, m_srv, ShaderType::PixelShader);
	m_shader->SetSampler(3, 1, AngelCore::AngelSubSystemResources::GraphicDeviceResources::Samplers.at(4).GetAddressOf());
	m_shader->BindSRV(9, 1, AngelCore::AngelRendering::LightManager::GetInstance()->GetSRVDirectionalLight()
		, ShaderType::PixelShader);
	m_shader->Apply();
	AngelCore::AngelSubSystemResources::GraphicDeviceResources::DeviceContext->IASetIndexBuffer
		(m_indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
	AngelCore::AngelSubSystemResources::GraphicDeviceResources::DeviceContext->IASetVertexBuffers
		(0, 1, m_vertexBuffer.GetAddressOf(), &stride, &offset);
	AngelCore::AngelSubSystemResources::GraphicDeviceResources::DeviceContext->IASetPrimitiveTopology
		(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	AngelCore::AngelSubSystemResources::GraphicDeviceResources::DeviceContext->DrawIndexed(m_indexCount, 0, 0);

	m_shader->UnBindSampler(0, 1);
	m_shader->UnBindSampler(3, 1);
	m_shader->UnBindSRV(10, 4, PixelShader);
	m_shader->UnBindSRV(15, 1, PixelShader);
	m_shader->UnBindSRV(9, 1, PixelShader);

	

	AngelCore::AngelSubSystemResources::GraphicDeviceResources::DeviceContext->RSSetState(
		AngelCore::AngelSubSystemResources::GraphicDeviceResources::SolidRasterizer.Get());

	if (m_pickedTriangle != -1)
	{
		//AngelCore::AngelSubSystemResources::GraphicDeviceResources::DeviceContext->DrawIndexed(3, 3* m_pickedTriangle, 0);
	}

	for (int i = 0;i < NUM_CHUNKS;i++)
	{
		//m_chunks[i].collsionShape->Render();
	}

	RenderObjects();
	//UpdateDrawing();
}

void AngelCore::AngelRendering::TerrainManager::RenderObjects()
{
	for (auto const & member : m_terrainObjects)
	{
		if (member->instances != 0)
		{
			std::vector<DirectX::XMMATRIX> Worlds;
			for (unsigned int i = 0;i < member->instances;i++)
			{
				Worlds.push_back(DirectX::XMMatrixScaling(member->Scale.at(i).x,
					member->Scale.at(i).y, 
					member->Scale.at(i).z)
					*DirectX::XMMatrixRotationRollPitchYaw(member->Rotation.at(i).x, 
						member->Rotation.at(i).y, member->Rotation.at(i).z)*
					DirectX::XMMatrixTranslation(member->Position.at(i).x, 
						member->Position.at(i).y, member->Position.at(i).z));
			}
			member->mesh->RenderInstanced(Worlds);
		}
	}
}

bool AngelCore::AngelRendering::TerrainManager::LoadBitmapHeightMap()
{
	int error, imageSize, i, j, k, index;
	FILE* filePtr;
	unsigned long long count;
	BITMAPFILEHEADER bitmapFileHeader;
	BITMAPINFOHEADER bitmapInfoHeader;
	unsigned char* bitmapImage;
	unsigned char height;

	if(!m_heightMap)
	// Start by creating the array structure to hold the height map data.
	m_heightMap = new HeightMapType[m_terrainWidth * m_terrainHeight];
	if (!m_heightMap)
	{
		return false;
	}

	// Open the bitmap map file in binary.
	error = fopen_s(&filePtr, m_terrainFilename, "rb");
	if (error != 0)
	{
		return false;
	}

	// Read in the bitmap file header.
	count = fread(&bitmapFileHeader, sizeof(BITMAPFILEHEADER), 1, filePtr);
	if (count != 1)
	{
		return false;
	}

	// Read in the bitmap info header.
	count = fread(&bitmapInfoHeader, sizeof(BITMAPINFOHEADER), 1, filePtr);
	if (count != 1)
	{
		return false;
	}

	// Make sure the height map dimensions are the same as the terrain dimensions for easy 1 to 1 mapping.
	if ((bitmapInfoHeader.biHeight != m_terrainHeight) || (bitmapInfoHeader.biWidth != m_terrainWidth))
	{
		return false;
	}

	// Calculate the size of the bitmap image data.  
	// Since we use non-divide by 2 dimensions (eg. 257x257) we need to add an extra byte to each line.
	imageSize = m_terrainHeight * ((m_terrainWidth * 3) + 1);

	// Allocate memory for the bitmap image data.
	bitmapImage = new unsigned char[imageSize];
	if (!bitmapImage)
	{
		return false;
	}

	// Move to the beginning of the bitmap data.
	fseek(filePtr, bitmapFileHeader.bfOffBits, SEEK_SET);

	// Read in the bitmap image data.
	count = fread(bitmapImage, 1, imageSize, filePtr);
	if (count != imageSize)
	{
		return false;
	}

	// Close the file.
	error = fclose(filePtr);
	if (error != 0)
	{
		return false;
	}

	// Initialize the position in the image data buffer.
	k = 0;

	// Read the image data into the height map array.
	for (j = 0; j<m_terrainHeight; j++)
	{
		for (i = 0; i<m_terrainWidth; i++)
		{
			// Bitmaps are upside down so load bottom to top into the height map array.
			index = (m_terrainWidth * (m_terrainHeight - 1 - j)) + i;

			// Get the grey scale pixel value from the bitmap image data at this location.
			height = bitmapImage[k];

			// Store the pixel value as the height at this point in the height map array.
			m_heightMap[index].y = (float)height;

			// Increment the bitmap image data index.
			k += 3;
		}

		// Compensate for the extra byte at end of each line in non-divide by 2 bitmaps (eg. 257x257).
		k++;
	}

	// Release the bitmap image data now that the height map array has been loaded.
	delete[] bitmapImage;
	bitmapImage = 0;

	// Release the terrain filename now that is has been read in.
	//delete[] m_terrainFilename;
	//m_terrainFilename = 0;

	return true;
}

void AngelCore::AngelRendering::TerrainManager::SetTerrainCoordinates()
{
	int i, j, index;
	if(!m_heightMap)
		m_heightMap = new HeightMapType[m_terrainWidth * m_terrainHeight];
	// Loop through all the elements in the height map array and adjust their coordinates correctly.
	for (j = 0; j<m_terrainHeight; j++)
	{
		for (i = 0; i<m_terrainWidth; i++)
		{
			index = (m_terrainWidth * j) + i;

			// Set the X and Z coordinates.
			m_heightMap[index].x = (float)i;
			m_heightMap[index].z = -(float)j;

			// Move the terrain depth into the positive range.  For example from (0, -256) to (256, 0).
			m_heightMap[index].z += (float)(m_terrainHeight - 1);

			if(m_heightScale!=0)
			// Scale the height.
			m_heightMap[index].y /= m_heightScale;
		}
	}
	return;
}

bool AngelCore::AngelRendering::TerrainManager::BuildTerrainModel()
{
	int i, j, index, index1, index2, index3, index4;


	// Calculate the number of vertices in the 3D terrain model.
	m_vertexCount = (m_terrainHeight - 1) * (m_terrainWidth - 1) * 6;

	if(!m_terrainModel)
	// Create the 3D terrain model array.
	m_terrainModel = new ModelType[m_vertexCount];
	// Initialize the index into the height map array.
	index = 0;

	// Load the 3D terrain model with the height map terrain data.
	// We will be creating 2 triangles for each of the four points in a quad.
	for (j = 0; j<(m_terrainHeight - 1); j++)
	{
		for (i = 0; i<(m_terrainWidth - 1); i++)
		{
			// Get the indexes to the four points of the quad.
			index1 = (m_terrainWidth * j) + i;          // Upper left.
			index2 = (m_terrainWidth * j) + (i + 1);      // Upper right.
			index3 = (m_terrainWidth * (j + 1)) + i;      // Bottom left.
			index4 = (m_terrainWidth * (j + 1)) + (i + 1);  // Bottom right.

															
			m_terrainModel[index].x = m_heightMap[index1].x;
			m_terrainModel[index].y = m_heightMap[index1].y;
			m_terrainModel[index].z = m_heightMap[index1].z;
			m_terrainModel[index].tu = 0.0f;
			m_terrainModel[index].tv = 0.0f;
			m_terrainModel[index].nx = m_heightMap[index1].nx;
			m_terrainModel[index].ny = m_heightMap[index1].ny;
			m_terrainModel[index].nz = m_heightMap[index1].nz;
			m_terrainModel[index].r = m_heightMap[index1].r;
			m_terrainModel[index].g = m_heightMap[index1].g;
			m_terrainModel[index].b = m_heightMap[index1].b;
			m_terrainModel[index].l1 = m_heightMap[index1].l1;
			m_terrainModel[index].l2 = m_heightMap[index1].l2;
			m_terrainModel[index].l3 = m_heightMap[index1].l3;
			m_terrainModel[index].l4 = m_heightMap[index1].l4;
			m_terrainModel[index].tileNumber = i + j*m_terrainWidth;

			index++;

			// Triangle 1 - Upper right.
			m_terrainModel[index].x = m_heightMap[index2].x;
			m_terrainModel[index].y = m_heightMap[index2].y;
			m_terrainModel[index].z = m_heightMap[index2].z;
			m_terrainModel[index].tu = 1.0f;
			m_terrainModel[index].tv = 0.0f;
			m_terrainModel[index].nx = m_heightMap[index2].nx;
			m_terrainModel[index].ny = m_heightMap[index2].ny;
			m_terrainModel[index].nz = m_heightMap[index2].nz;
			m_terrainModel[index].r = m_heightMap[index2].r;
			m_terrainModel[index].g = m_heightMap[index2].g;
			m_terrainModel[index].b = m_heightMap[index2].b;
			m_terrainModel[index].l1 = m_heightMap[index1].l1;
			m_terrainModel[index].l2 = m_heightMap[index1].l2;
			m_terrainModel[index].l3 = m_heightMap[index1].l3;
			m_terrainModel[index].l4 = m_heightMap[index1].l4;
			m_terrainModel[index].tileNumber = i + j*m_terrainWidth;
			index++;

			// Triangle 1 - Bottom left.
			m_terrainModel[index].x = m_heightMap[index3].x;
			m_terrainModel[index].y = m_heightMap[index3].y;
			m_terrainModel[index].z = m_heightMap[index3].z;
			m_terrainModel[index].tu = 0.0f;
			m_terrainModel[index].tv = 1.0f;
			m_terrainModel[index].nx = m_heightMap[index3].nx;
			m_terrainModel[index].ny = m_heightMap[index3].ny;
			m_terrainModel[index].nz = m_heightMap[index3].nz;
			m_terrainModel[index].r = m_heightMap[index3].r;
			m_terrainModel[index].g = m_heightMap[index3].g;
			m_terrainModel[index].b = m_heightMap[index3].b;
			m_terrainModel[index].l1 = m_heightMap[index1].l1;
			m_terrainModel[index].l2 = m_heightMap[index1].l2;
			m_terrainModel[index].l3 = m_heightMap[index1].l3;
			m_terrainModel[index].l4 = m_heightMap[index1].l4;
			m_terrainModel[index].tileNumber = i + j*m_terrainWidth;
			index++;

			// Triangle 2 - Bottom left.
			m_terrainModel[index].x = m_heightMap[index3].x;
			m_terrainModel[index].y = m_heightMap[index3].y;
			m_terrainModel[index].z = m_heightMap[index3].z;
			m_terrainModel[index].tu = 0.0f;
			m_terrainModel[index].tv = 1.0f;
			m_terrainModel[index].nx = m_heightMap[index3].nx;
			m_terrainModel[index].ny = m_heightMap[index3].ny;
			m_terrainModel[index].nz = m_heightMap[index3].nz;
			m_terrainModel[index].r = m_heightMap[index3].r;
			m_terrainModel[index].g = m_heightMap[index3].g;
			m_terrainModel[index].b = m_heightMap[index3].b;
			m_terrainModel[index].l1 = m_heightMap[index1].l1;
			m_terrainModel[index].l2 = m_heightMap[index1].l2;
			m_terrainModel[index].l3 = m_heightMap[index1].l3;
			m_terrainModel[index].l4 = m_heightMap[index1].l4;
			m_terrainModel[index].tileNumber = i + j*m_terrainWidth;
			index++;

			// Triangle 2 - Upper right.
			m_terrainModel[index].x = m_heightMap[index2].x;
			m_terrainModel[index].y = m_heightMap[index2].y;
			m_terrainModel[index].z = m_heightMap[index2].z;
			m_terrainModel[index].tu = 1.0f;
			m_terrainModel[index].tv = 0.0f;
			m_terrainModel[index].nx = m_heightMap[index2].nx;
			m_terrainModel[index].ny = m_heightMap[index2].ny;
			m_terrainModel[index].nz = m_heightMap[index2].nz;
			m_terrainModel[index].r = m_heightMap[index2].r;
			m_terrainModel[index].g = m_heightMap[index2].g;
			m_terrainModel[index].b = m_heightMap[index2].b;
			m_terrainModel[index].l1 = m_heightMap[index1].l1;
			m_terrainModel[index].l2 = m_heightMap[index1].l2;
			m_terrainModel[index].l3 = m_heightMap[index1].l3;
			m_terrainModel[index].l4 = m_heightMap[index1].l4;
			m_terrainModel[index].tileNumber = i + j*m_terrainWidth;
			index++;

			// Triangle 2 - Bottom right.
			m_terrainModel[index].x = m_heightMap[index4].x;
			m_terrainModel[index].y = m_heightMap[index4].y;
			m_terrainModel[index].z = m_heightMap[index4].z;
			m_terrainModel[index].tu = 1.0f;
			m_terrainModel[index].tv = 1.0f;
			m_terrainModel[index].nx = m_heightMap[index4].nx;
			m_terrainModel[index].ny = m_heightMap[index4].ny;
			m_terrainModel[index].nz = m_heightMap[index4].nz;
			m_terrainModel[index].r = m_heightMap[index4].r;
			m_terrainModel[index].g = m_heightMap[index4].g;
			m_terrainModel[index].b = m_heightMap[index4].b;
			m_terrainModel[index].l1 = m_heightMap[index1].l1;
			m_terrainModel[index].l2 = m_heightMap[index1].l2;
			m_terrainModel[index].l3 = m_heightMap[index1].l3;
			m_terrainModel[index].l4 = m_heightMap[index1].l4;
			m_terrainModel[index].tileNumber = i + j*m_terrainWidth;
			index++;
		}
	}

	return true;
}

#pragma region Free Drawing
void AngelCore::AngelRendering::TerrainManager::HandleFreeDrawing()
{
	if (AngelCore::AngelSubSystem::inputManager->IsLeftMouseClicked())
	{
		if (m_points.size() >= 1)
		{
			if (m_points.at(m_points.size() - 1).x != AngelCore::AngelSubSystem::inputManager->GetMousePosition().x
				&&
				m_points.at(m_points.size() - 1).y != AngelCore::AngelSubSystem::inputManager->GetMousePosition().y)
			{
				DirectX::XMFLOAT2 n = DirectX::XMFLOAT2((m_mousePosInTerrainSpace.x*(800/(m_terrainWidth-1)))
					,600- (m_mousePosInTerrainSpace.z*(600 / (m_terrainHeight-1))));
				m_points.push_back(
					DirectX::XMFLOAT2((m_mousePosInTerrainSpace.x*(800 / (m_terrainWidth-1)))
						,600-(m_mousePosInTerrainSpace.z*(600 / (m_terrainHeight-1)))));
				/*OutputDebugStringA("\n");
				OutputDebugStringA("In 2D Space X :");
				OutputDebugStringA(std::to_string(n.x).c_str());
				OutputDebugStringA("\n");
				OutputDebugStringA("In 2D Space y :");
				OutputDebugStringA(std::to_string(n.y).c_str());*/
			}
		}
		else
		{
			
			m_points.push_back(
				DirectX::XMFLOAT2((m_mousePosInTerrainSpace.x*(800 / (m_terrainWidth-1)))
					, 600-(m_mousePosInTerrainSpace.z*(600 / (m_terrainHeight-1)))));

			
		}

	}

	if (!AngelCore::AngelSubSystem::inputManager->IsLeftMouseClicked())
	{
		if (m_points.size() >= 1)
		{
			if (m_points.at(m_points.size() - 1).x != -1)
			{
				m_points.push_back(DirectX::XMFLOAT2(-1, -1));
			}
		}
	}
}

void AngelCore::AngelRendering::TerrainManager::UpdateDrawing()
{
	m_renderTarget2D->BeginDraw();
	D2D1_COLOR_F color;
	color.a =1.0f;
	color.b = 1.0f;
	color.g = 0.0f;
	color.r = 1.0f;
	m_renderTarget2D->Clear(color);
	if (m_points.size() < 2) return;
	int j = 0;
	unsigned int i = 0;
	int start = 0;
	while (true)
	{

		for (i = start;i < m_points.size() - 1;i++)
		{
			if (m_points[i].x == -1)
			{
				start = i + 1;
				break;
			}
			if (m_points[i + 1].x != -1)
				m_renderTarget2D
				->DrawLine(D2D1::Point2F(m_points[i].x, m_points[i].y),
					D2D1::Point2F(m_points[i + 1].x, m_points[i + 1].y), m_brush.Get(), 2);
		}
		if (start >= m_points.size() - 1 || i >= m_points.size() - 1)
			break;
	}

	m_renderTarget2D->EndDraw();

}

void AngelCore::AngelRendering::TerrainManager::InitializeFreeDrawing()
{
	ID3D11Texture2D *texture;
	RECT rect = {};
	GetClientRect(AngelCore::AngelSubSystemResources::WindowProperties::GetHWND(), &rect);
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

	HR(AngelCore::AngelSubSystemResources::GraphicDeviceResources::Device->CreateTexture2D
		(&texDesc, NULL, &texture));

	IDXGISurface1 *pDxgiSurface = NULL;

	texture->QueryInterface(IID_PPV_ARGS(&pDxgiSurface));

	D2D1_RENDER_TARGET_PROPERTIES props =
		D2D1::RenderTargetProperties(
			D2D1_RENDER_TARGET_TYPE_DEFAULT,
			D2D1::PixelFormat(DXGI_FORMAT_UNKNOWN, D2D1_ALPHA_MODE_PREMULTIPLIED),
			96,
			96
			);


	HR(AngelGUISystem::AngelGUI::GetFactory()->CreateDxgiSurfaceRenderTarget(
		pDxgiSurface, props, m_renderTarget2D.GetAddressOf()));



	D3D11_SHADER_RESOURCE_VIEW_DESC srvd;
	srvd.Format = texDesc.Format;
	srvd.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvd.Texture2D.MipLevels = 1;
	srvd.Texture2D.MostDetailedMip = 0;

	HR(AngelCore::AngelSubSystemResources::GraphicDeviceResources::Device->CreateShaderResourceView
		(texture, &srvd, m_shaderResourceView.GetAddressOf()));

	m_renderTarget2D->SetTextAntialiasMode(D2D1_TEXT_ANTIALIAS_MODE_GRAYSCALE);

	Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> b;
	COM_RELEASE(m_brush);
	m_renderTarget2D
		->CreateSolidColorBrush(D2D1::ColorF(
			1.0f,
			1.0f,
			0.0f,
			0.6f),
			&b);
	m_brush = b;
	COM_RELEASE(b);
}
#pragma endregion

#pragma region Render Images
void AngelCore::AngelRendering::TerrainManager::UpdateImagesBuffer(D2D1_RECT_F rect, float width, float height)
{
	using namespace DirectX;
	if (!m_buffersInitialized)
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


		m_shaderImage = new Shader();
		Shader::LoadShader(ShaderType::VertexShader, "VertexGUI.cso", VertexTypes::PositionTexCoord, m_shaderImage);
		Shader::LoadShader(ShaderType::PixelShader, "PixelGUI.cso", VertexTypes::NOP, m_shaderImage);
	}

	

	float leftTopX = rect.left / width + rect.left / width;
	float rightBottomX = (width - rect.right) / width + (width - rect.right) / width;
	float leftTopY = rect.top / height + rect.top / height;
	float rightBottomY = (height - rect.bottom) / height+ (height - rect.bottom) / height;

	DirectX::XMStoreFloat4x4(&m_cbVertexTranslation.Const.vertexNegative1Negative1Tranlation,
		DirectX::XMMatrixTranslation(+leftTopX, +rightBottomY, 0.0f));
	DirectX::XMStoreFloat4x4(&m_cbVertexTranslation.Const.vertexNegative1Positive1Tranlation,
		DirectX::XMMatrixTranslation(+leftTopX, -leftTopY, 0.0f));
	DirectX::XMStoreFloat4x4(&m_cbVertexTranslation.Const.vertexPositive1Negative1Tranlation,
		DirectX::XMMatrixTranslation(-rightBottomX,+rightBottomY, 0.0f));
	DirectX::XMStoreFloat4x4(&m_cbVertexTranslation.Const.vertexPositive1Positive1Tranlation,
		DirectX::XMMatrixTranslation(-rightBottomX,-leftTopY,0.0f));

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

	D3D11_MAPPED_SUBRESOURCE vtx_resource,idx_resource;
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

}

void AngelCore::AngelRendering::TerrainManager::RenderImages()
{
	AngelCore::AngelSubSystemResources::GraphicDeviceResources::DeviceContext
		->OMSetRenderTargets(1, &this->m_renderTargetView,nullptr);
	/*D3D11_VIEWPORT v;
	v.Height = AngelSubSystemResources::WindowProperties::GetHeight();
	v.MaxDepth = 1.0f;
	v.MinDepth = 0.0f;
	v.TopLeftX = 0.0f;
	v.TopLeftY = 0.0f;
	v.Width = AngelSubSystemResources::WindowProperties::GetWidth();
	AngelSubSystemResources::GraphicDeviceResources::DeviceContext->RSSetViewports(1, &v);*/
	float color[4] = { 1.0f, 1.0f, 0.0f, 0.0f };
	AngelCore::AngelSubSystemResources::GraphicDeviceResources::DeviceContext
		->ClearRenderTargetView(this->m_renderTargetView, color);

	m_cbVertexTranslation.Update();
	m_shaderImage->SetConstantBuffer(0, 1, m_cbVertexTranslation.getBuffer());

	m_shaderImage->SetSampler(0, 1, 
		AngelCore::AngelSubSystemResources::GraphicDeviceResources::Samplers.at(0).GetAddressOf());
	m_shaderImage->BindSRV(0, 1, m_fullTexture->GetSRV().Get(), ShaderType::PixelShader);
	UINT stride = sizeof(VertexTypesStruct::PositionTexCoord);
	UINT offset = 0;
	AngelCore::AngelSubSystemResources::GraphicDeviceResources::DeviceContext->
		IASetIndexBuffer(m_indexBufferImage.Get()
		, DXGI_FORMAT_R16_UINT,0);

	AngelCore::AngelSubSystemResources::GraphicDeviceResources::DeviceContext->
		IASetVertexBuffers(0, 1, m_vertexBufferImage.GetAddressOf()
		, &stride, &offset);

	AngelCore::AngelSubSystemResources::GraphicDeviceResources::DeviceContext->
		IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	m_shaderImage->Apply();

	AngelCore::AngelSubSystemResources::GraphicDeviceResources::DeviceContext->
		DrawIndexed(m_indicesSizeImage, 0, 0);


}
#pragma endregion

