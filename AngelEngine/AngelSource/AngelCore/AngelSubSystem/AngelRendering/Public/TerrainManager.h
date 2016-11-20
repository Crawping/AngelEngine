#pragma once
#include<DirectXMath.h>
#include<vector>
#include"CBuffer.h"
#include"VertexTypes.h"
#include"../../AngelWorldSystem/AngelComponent/Public/MeshRenderer.h"

class Texture2D;
namespace AngelCore
{
	namespace AngelRendering
	{
		class Shader;
		class DebugCollisionShape;
		

		class TerrainManager
		{
			struct PerFrame
			{
				DirectX::XMFLOAT4X4 WVP;
				DirectX::XMFLOAT4X4 WVPLast;
				DirectX::XMFLOAT4X4 WorldView;
				DirectX::XMFLOAT4X4 WorldInvTranspose;
				DirectX::XMFLOAT4X4 lightViewMatrix;
				DirectX::XMFLOAT4X4 lightProjectionMatrix;
			};

			//represent a texture that maps on whole terrain
			struct TextureData
			{
				float width;
				float height;
				int numTileWidth;
				int numTileHeight;
			};

			struct VertexTranslation
			{
				DirectX::XMFLOAT4X4 vertexPositive1Positive1Tranlation;
				DirectX::XMFLOAT4X4 vertexNegative1Negative1Tranlation;
				DirectX::XMFLOAT4X4 vertexNegative1Positive1Tranlation;
				DirectX::XMFLOAT4X4 vertexPositive1Negative1Tranlation;
			};

			struct HeightMapType
			{
				float x, y, z;
				float nx, ny, nz;
				float r, g, b;
				float l1, l2, l3, l4;
				int tileNumber;
			};

			struct ModelType
			{
				float x, y, z;
				float tu, tv;
				float nx, ny, nz;
				float r, g, b;
				float l1, l2, l3, l4;
				int tileNumber;
			};

			struct VectorType
			{
				float x, y, z;
			};

			struct Chunk
			{		
				DirectX::BoundingBox					bondingBox;
				DebugCollisionShape *					collsionShape;
				unsigned long *							indices;
				VertexTypesStruct::Terrain*				vertices;
				UINT									indicesEndLocation;
				UINT									indicesStartLocation;
			};

			struct TileNeighbors
			{
				int tileIndex;
				int tileLeftIndex;
				int tileRightIndex;
				int tileUpIndex;
				int tileDownIndex;
			};

			struct TerrainObjects
			{
				AngelWorldSystem::MeshRenderer *mesh;
				UINT instances;
				std::vector<DirectX::XMFLOAT3> Position;
				std::vector<DirectX::XMFLOAT3> Rotation;
				std::vector<DirectX::XMFLOAT3> Scale;
			};

		public:
			void Initialize();
			void Update();
			void Render();
			static TerrainManager * GetInstance();
			static void CreateInstance();

			void SetEnable(bool b)			{ this->m_enable = b; }
			bool GetEnable() const			{ return m_enable; }

			void SetInitialized(bool b)		{ this->m_initialized = b; }
			bool GetInitialized() const		{ return m_initialized; }

		private:
			TerrainManager();
			TerrainManager(const TerrainManager & l) { };

			bool LoadBitmapHeightMap();
			void ShutdownHeightMap();
			bool LoadRawHeightMap();
			void SetTerrainCoordinates();
			bool BuildTerrainModel();
			void ShutdownTerrainModel();
			bool LoadHeightMap(char*);
			void UpdateBuffers();
			void CreateChunks();
			bool CalculateNormals();
			bool LoadColorMap();
			bool LoadLayerMap();
			void BuildTileNeighbors();
			void UpdateTileNeighbors();
			int GetTileLayer(int index);
			void RenderObjects();
			void AddTerrainObject(int index);
			

			//Free Drawing
			void HandleFreeDrawing();
			void UpdateDrawing();
			void InitializeFreeDrawing();
		public:
			//Draw images
			//Should be called before any render in applicaiton.
			void RenderImages();
			void UpdateImagesBuffer(D2D1_RECT_F rect,float width,float height);

			Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> getSRV() { return m_srvRtv; }

		private:

			static TerrainManager *					m_instance;
			Microsoft::WRL::ComPtr<ID3D11Buffer>	m_vertexBuffer;
			Microsoft::WRL::ComPtr<ID3D11Buffer>	m_indexBuffer;
			int m_vertexCount,						m_indexCount;
			AngelRendering::Shader *				m_shader;
			CBuffer<PerFrame>						m_cbWorld;
			CBuffer<TextureData>					m_cbTextureData;
			CBuffer<VertexTranslation>				m_cbVertexTranslation;
			int m_terrainHeight,					m_terrainWidth;
			float									m_heightScale;
			char*									m_terrainFilename;
			HeightMapType*							m_heightMap;
			float* m_testMap;
			DirectX::XMFLOAT3 *						m_colorMap;
			DirectX::XMFLOAT4 *						m_layerMap;
			ModelType*								m_terrainModel;
			DirectX::BoundingBox					m_boundingBox;
			AngelRendering::DebugCollisionShape *	m_collisonShape;
			unsigned long *							m_indices;
			VertexTypesStruct::Terrain*				m_vertices;
			int										m_pickedTriangle;
			Chunk *									m_chunks;
			unsigned short*							m_rawImage;
			Texture2D *								m_dirtTexture;
			Texture2D *								m_StoneTexture;
			Texture2D *								m_dirt1Texture;
			Texture2D *								m_dirt2Texture;
			Texture2D *								m_fullTexture;
			float									m_heightThreshold;
			DirectX::XMFLOAT4						m_currentColor;
			bool									m_applyColor;
			bool									m_applyHeight;
			bool									m_applyLayer;
			bool									m_addModel;
			int										m_selectedLayer;
			int										m_selectedModel;
			ID3D11ShaderResourceView *				m_srv[4];
			TileNeighbors *							m_tileNeighbors;
			DirectX::XMFLOAT3						m_mousePosInTerrainSpace;
			bool									m_enable;
			bool									m_initialized;

			//Terrain models
			std::vector<TerrainObjects*>			m_terrainObjects;

			//Free Drawing
			std::vector<DirectX::XMFLOAT2>						m_points;
			bool												m_useBitmap;
			int													m_lPointCount;
			ID2D1GeometrySink *									m_pSink = NULL;
			ID2D1PathGeometry *									m_path;
			bool												m_update;
			Microsoft::WRL::ComPtr<ID2D1RenderTarget>			m_renderTarget2D;
			Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>	m_shaderResourceView;
			Microsoft::WRL::ComPtr<ID2D1Brush>					m_brush;

			//Drawing images
			UINT									m_indicesSizeImage;
			UINT									m_strideImage;
			UINT									m_offsetImage;
			AngelRendering::Shader*					m_shaderImage;
			Microsoft::WRL::ComPtr<ID3D11Buffer>	m_vertexBufferImage;
			Microsoft::WRL::ComPtr<ID3D11Buffer>	m_indexBufferImage;
			bool									m_buffersInitialized;
			ID3D11Texture2D				*			m_renderTargetViewTexture;
			ID3D11RenderTargetView		*			m_renderTargetView;
			ID3D11ShaderResourceView	*			m_srvRtv;

		};
	}
}