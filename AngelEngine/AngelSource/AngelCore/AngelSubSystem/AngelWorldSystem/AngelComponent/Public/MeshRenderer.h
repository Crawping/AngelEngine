#pragma once
#include"IComponent.h"
#include<vector>
#include<fstream>
#include<istream>
#include<d3d11_1.h>
#include<wrl\client.h>
#include"../../../AngelRendering/Public/Shader.h"
#include"../Public/Model.h"
#include"../../../AngelRendering/Public/CBuffer.h"
#include"btBulletCollisionCommon.h"
#include <Assimp/scene.h>           // Output data structure
#include <Assimp/postprocess.h>     // Post processing fla

struct Material
{
	DirectX::XMFLOAT4   Ambient;
	DirectX::XMFLOAT4   Diffuse;
	DirectX::XMFLOAT4   Specular;
	float               SpecularPower;
	DirectX::XMFLOAT4   Emissive;
	DirectX::XMFLOAT4X4 UVTransform;
};


struct MaterialRecordCMO
{
	const Material*					pMaterial;
	std::wstring                    name;
	std::wstring                    pixelShader;
	std::wstring					texturename[8];
	std::wstring					txtName;
	ID3D11ShaderResourceView *		textureView[8];
	CBuffer<ShaderMaterial>			m_material;
	CBuffer<cbPerObjectBuffer>		m_wvpBuffer;
};

struct SubMesh
{
	UINT MaterialIndex;
	UINT IndexBufferIndex;
	UINT VertexBufferIndex;
	UINT StartIndex;
	UINT PrimCount;
};




namespace AngelCore
{
	namespace AngelWorldSystem
	{
		class MeshRenderer : public IComponent
		{
			friend class Actor;
			friend class RigidBody;
		public:
			virtual ~MeshRenderer();

		public:
			virtual void Initialize(Actor* _owner, ...) override;
			virtual void Update(const AngelSubSystem::TimeManager &timer) override;
			virtual void Render() override;
			void RenderDepth();
			void RenderInstanced(std::vector<DirectX::XMMATRIX> World);

		public:
			//Setter Material
			void SetDiffuseColor(DirectX::XMFLOAT4 color);
			void SetDiffuseColor(float &r,float &g,float &b,float &a);
			void SetAmbientColor(DirectX::XMFLOAT4 color);
			void SetAmbientColor(float &r, float &g, float &b, float &a);
			void SetMaterialType(int type);
			void SetAnimationClipIndex(int index) { m_animationIndex = index; }
			void SetAnimationClipIndex(int &index) { m_animationIndex = index; }
			//Enables end time by default,if you want to disable end time and use default end time use disableanimationClipendtime function.
			void SetAnimationClipEndTime(int &index, float &time);
			void SetAnimationClipTimeScale(int &index,float &time);
			void DisableAnimationClipEndTime(int &index);

			AngelCore::AngelWorldSystem::ModelMesh * GetSubMeshByName(const wchar_t * name);
		public:
#pragma region Setter
			void SetPath(const char * _path)			{ this->m_fileName = _path; }
#pragma endregion

#pragma region Getter
			const char * GetPath()						{ return this->m_fileName; }
#pragma endregion

		private:

			std::vector<ModelMesh*>	GetAllMeshes();
		private:

			const char *                 m_fileName;
			AngelCore::AngelWorldSystem::Model*              model;
			AngelRendering::Shader *     m_shader;
			AngelRendering::Shader *	 m_shaderDepth;
			CBuffer<cbPerObjectBuffer>	 m_cbPerObjectBuffer;
			//store mesh vertices for later use
			DirectX::XMFLOAT3 *			 m_points;
			
			//Scripting for animation(TODO: script this by angel script)
			UINT m_animationIndex;

			//Store last instance size
			UINT						m_lastInstanceSize;

			//For .obj models.
			std::vector<int> iMeshStartIndices;
			std::vector<int> iMeshSizes;
			std::vector<int> iMaterialIndices;
			int iNumMaterials;
		};
	}
}
