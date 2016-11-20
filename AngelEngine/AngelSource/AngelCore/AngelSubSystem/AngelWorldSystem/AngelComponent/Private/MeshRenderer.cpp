#include"../Public/MeshRenderer.h"
#include<sstream>
#include"../../../AngelCore/AngelSubSystem/AngelLog/Public/AngelLog.h"
#include"../../../Public/SubSystemsResource.h"
#include"../Public/Transform.h"
#include"../Public/Component.h"
#include"../../Public/Actor.h"
#include"../../../AngelRendering/Public/Texture2D.h"
#include<DirectXCollision.h>
#include"../../../AngelRendering/Public/PlatformHelpers.h"
#include"../Public/Transform.h"
#include"../../../Public/InputManager.h"//For testing.
#include<btBulletDynamicsCommon.h>
#include<Assimp\Importer.hpp>

void AngelCore::AngelWorldSystem::MeshRenderer::Initialize(Actor* _owner, ...)
{

	
	va_list args;
	va_start(args, _owner);
	const char * name = va_arg(args, const char *);
	bool enableSkinning = va_arg(args, bool);
	int matType = va_arg(args, int);
	bool enableInstancing = va_arg(args, bool);
	this->m_fileName = name;
	this->SetOwner(_owner);
	this->m_shader = new AngelRendering::Shader();
	this->m_cbPerObjectBuffer.Load();

	//Always set to zero.
	m_animationIndex = 0;

	m_lastInstanceSize = 0;

	std::string dataS = name;

	std::wstring path = AngelCore::AngelSubSystem::FileSystemManager::GetContentDirectory();
	std::string pathS(path.begin(), path.end());
	//AngelCore::AngelSubSystem::AngelLog::WriteLogMessage(pathS.c_str());
	dataS = pathS + "\\AngelData\\Model\\" + dataS;
	this->m_fileName = dataS.c_str();
	const size_t size = strlen(this->m_fileName) + 1;

	

	wchar_t * m_wText = new wchar_t[size];
	mbstowcs(m_wText, m_fileName, size);

	WCHAR ext[_MAX_EXT];
	_wsplitpath_s(m_wText, nullptr, 0, nullptr, 0, nullptr, 0, ext, _MAX_EXT);

	if (_wcsicmp(ext, L".obj") == 0)
	{
		Assimp::Importer importer;
		const aiScene* scene = importer.ReadFile(dataS,
			aiProcess_CalcTangentSpace |
			aiProcess_Triangulate |
			aiProcess_JoinIdenticalVertices |
			aiProcess_SortByPType);
		if (!scene)
		{
			MessageBoxA(0, "Couldn't load model ", "Error Importing Asset", MB_ICONERROR);
		}
		model = new AngelCore::AngelWorldSystem::Model();
		const int iVertexTotalSize = sizeof(aiVector3D) * 2 + sizeof(aiVector2D);
		int iTotalVertices = 0;
		const aiVector3D Zero3D(0.0f, 0.0f, 0.0f);
		for (int curMesh = 0;curMesh < scene->mNumMeshes;curMesh++)
		{
			const aiMesh* paiMesh = scene->mMeshes[curMesh];
			auto mesh = std::make_shared<AngelCore::AngelWorldSystem::ModelMesh>();
			mesh->ccw = true;
			mesh->pmalpha = true;
			mesh->armatureBuffer.Load();
			mesh->index = curMesh;

			Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer;
			Microsoft::WRL::ComPtr<ID3D11Buffer> indexBuffer;

			std::vector<VertexTypesStruct::VertexPositionNormalTangentTextureSkinned> vertices;
			std::vector<USHORT> indices;

			for (unsigned int i = 0; i < paiMesh->mNumVertices; i++) {
				const aiVector3D* pPos = &(paiMesh->mVertices[i]);
				const aiVector3D* pNormal = &(paiMesh->mNormals[i]);
				const aiVector3D* pTexCoord = paiMesh->HasTextureCoords(0) ?
					&(paiMesh->mTextureCoords[0][i]) : &Zero3D;

				VertexTypesStruct::VertexPositionNormalTangentTextureSkinned
					v;
				v.Position = DirectX::XMFLOAT3(pPos->x, pPos->y, pPos->z);
				v.Normal= DirectX::XMFLOAT3(pNormal->x, pNormal->y, pNormal->z);
				v.TexCoord = DirectX::XMFLOAT2(pTexCoord->x, pTexCoord->y);

				vertices.push_back(v);
			}

			//Create vertex buffer.
			D3D11_BUFFER_DESC bufferDesc;
			bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
			bufferDesc.ByteWidth = vertices.size()*sizeof(VertexTypesStruct::VertexPositionNormalTangentTextureSkinned);
			bufferDesc.CPUAccessFlags = 0;
			bufferDesc.MiscFlags = 0;
			bufferDesc.StructureByteStride = 0;
			bufferDesc.Usage = D3D11_USAGE_DEFAULT;

			D3D11_SUBRESOURCE_DATA data;
			data.pSysMem = vertices.data();
			data.SysMemPitch = data.SysMemSlicePitch = 0;

			HR(AngelCore::AngelSubSystemResources::GraphicDeviceResources::Device->
				CreateBuffer(&bufferDesc, &data, vertexBuffer.GetAddressOf()));

			for (unsigned int i = 0; i < paiMesh->mNumFaces; i++) {
				const aiFace& Face = paiMesh->mFaces[i];
				assert(Face.mNumIndices == 3);
				indices.push_back(Face.mIndices[0]);
				indices.push_back(Face.mIndices[1]);
				indices.push_back(Face.mIndices[2]);
			}

			bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
			bufferDesc.ByteWidth = indices.size()*sizeof(USHORT);
			bufferDesc.CPUAccessFlags = 0;
			bufferDesc.MiscFlags = 0;
			bufferDesc.StructureByteStride = 0;
			bufferDesc.Usage = D3D11_USAGE_DEFAULT;

			data.pSysMem = indices.data();
			data.SysMemPitch = data.SysMemSlicePitch = 0;

			HR(AngelCore::AngelSubSystemResources::GraphicDeviceResources::Device->
				CreateBuffer(&bufferDesc, &data, indexBuffer.GetAddressOf()));
			std::vector<MaterialRecordCMO> materials;
			
			{
				MaterialRecordCMO m;
				for (int i = 0;i < 8;i++)
				{
					m.textureView[i] = nullptr;
				}
				const aiMaterial* pMaterial = scene->mMaterials[curMesh+1];
				aiString texPath;
				if (pMaterial->GetTextureCount(aiTextureType_DIFFUSE) > 0) {
					aiString Path;
					if (pMaterial->GetTexture(aiTextureType_DIFFUSE, 0,
						&Path, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS) {
						std::string pathS = Path.C_Str();
						//AngelCore::AngelSubSystem::AngelLog::WriteLogMessage(pathS.c_str());
						std::wstring pathW(pathS.begin(), pathS.end());

						Texture2D *tmp = new Texture2D();
						tmp->LoadTexture(pathW.c_str(),true);

						if (tmp->GetSRV().Get())
							m.textureView[0] = tmp->GetSRV().Get();
						//delete tmp;
					}

					if (pMaterial->GetTexture(aiTextureType_NORMALS, 0,
						&Path, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS) {
						//AngelCore::AngelSubSystem::AngelLog::WriteLogMessage(pathS.c_str());
						std::string pathS = Path.C_Str();
						//AngelCore::AngelSubSystem::AngelLog::WriteLogMessage(pathS.c_str());
						std::wstring pathW(pathS.begin(), pathS.end());

						Texture2D *tmp = new Texture2D();
						tmp->LoadTexture(pathW.c_str(),true);

						if (tmp->GetSRV().Get())
							m.textureView[1] = tmp->GetSRV().Get();
						//delete tmp;
					}

					if (pMaterial->GetTexture(aiTextureType_SPECULAR, 0,
						&Path, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS) {
						//AngelCore::AngelSubSystem::AngelLog::WriteLogMessage(pathS.c_str());
						std::string pathS = Path.C_Str();
						//AngelCore::AngelSubSystem::AngelLog::WriteLogMessage(pathS.c_str());
						std::wstring pathW(pathS.begin(), pathS.end());

						Texture2D *tmp = new Texture2D();
						tmp->LoadTexture(pathW.c_str(),true);

						if (tmp->GetSRV().Get())
							//if we have normal map.
							if(m.textureView[1])
								m.textureView[2] = tmp->GetSRV().Get();
						delete tmp;
					}
				}
				m.m_material.Load();
				aiColor4D diffuse;
				if (AI_SUCCESS == aiGetMaterialColor(pMaterial, AI_MATKEY_COLOR_DIFFUSE, &diffuse)) 
				{

					m.m_material.Const.DiffuseColor.x = diffuse.r;
					m.m_material.Const.DiffuseColor.y = diffuse.g;
					m.m_material.Const.DiffuseColor.z = diffuse.b;
					m.m_material.Const.DiffuseColor.w = diffuse.a;
				}

				aiColor4D specular;
				if (AI_SUCCESS == aiGetMaterialColor(pMaterial, AI_MATKEY_COLOR_SPECULAR, &specular))
				{

					m.m_material.Const.SpecularColor.x = specular.r;
					m.m_material.Const.SpecularColor.y = specular.g;
					m.m_material.Const.SpecularColor.z = specular.b;

				}

				aiColor4D ambient;
				if (AI_SUCCESS == aiGetMaterialColor(pMaterial, AI_MATKEY_COLOR_AMBIENT, &ambient))
				{

					m.m_material.Const.AmbientColor.x = ambient.r;
					m.m_material.Const.AmbientColor.y = ambient.g;
					m.m_material.Const.AmbientColor.z = ambient.b;
					m.m_material.Const.AmbientColor.w = ambient.a;

				}
				m.m_material.Const.MaterialType = matType;
				m.m_wvpBuffer.Load();
				materials.push_back(m);
			}

			auto part = new AngelCore::AngelWorldSystem::ModelMeshPart();
			part->collisionShape = new AngelRendering::DebugCollisionShape();
			part->collisionShape->Create();
			part->m_material = materials[0].m_material;
			part->m_wvpBuffer = materials[0].m_wvpBuffer;

			for (int i = 0;i < 8;i++)
				part->textureView[i] = materials[0].textureView[i];
			part->indexBuffer = indexBuffer;
			part->vertexBuffer = vertexBuffer;
			part->enableInstancing = false;
			part->enableSkinning = false;
			part->vertexStride = sizeof(VertexTypesStruct::PositionNormalTexture);
			part->indexCount = indices.size();
			part->shader = new AngelRendering::Shader();
			AngelRendering::Shader::LoadShader(ShaderType::VertexShader,
				"VertexForwardRenderer.cso", VertexTypes::PositionNormalTangentTextureSkinned, part->shader);
			AngelRendering::Shader::LoadShader(ShaderType::PixelShader,
				"PixelForwardRenderer.cso", VertexTypes::NOP, part->shader);
			mesh->enableSkinning = false;
			mesh->enableInstancing = false;
			mesh->meshParts.emplace_back(part);
			model->meshes.emplace_back(mesh);

		}
	}
	else
	{

		size_t dataSize = 0;
		std::unique_ptr<uint8_t[]> meshData;
		int state = 0;
		HR(AngelCore::AngelSubSystem::FileSystemManager::ReadBinaryFile(m_wText, meshData, &dataSize, nullptr));

		auto nMesh = reinterpret_cast<const UINT*>(meshData.get());
		size_t usedSize = sizeof(UINT);
		if (dataSize < usedSize)
			throw std::exception("End of file");

		if (!*nMesh)
			throw std::exception("No meshes found");

		model = new AngelCore::AngelWorldSystem::Model();

		for (UINT meshIndex = 0; meshIndex < *nMesh; ++meshIndex)
		{
			// Mesh name
			auto nName = reinterpret_cast<const UINT*>(meshData.get() + usedSize);
			usedSize += sizeof(UINT);
			if (dataSize < usedSize)
				throw std::exception("End of file");



			auto meshName = reinterpret_cast<const wchar_t*>(meshData.get() + usedSize);

			usedSize += sizeof(wchar_t)*(*nName);
			if (dataSize < usedSize)
				throw std::exception("End of file");

			auto mesh = std::make_shared<AngelCore::AngelWorldSystem::ModelMesh>();
			mesh->name.assign(meshName, *nName);
			mesh->ccw = true;
			mesh->pmalpha = true;
			mesh->armatureBuffer.Load();
			mesh->index = meshIndex;

			// Materials
			auto nMats = reinterpret_cast<const UINT*>(meshData.get() + usedSize);
			usedSize += sizeof(UINT);
			if (dataSize < usedSize)
				throw std::exception("End of file");

			std::vector<MaterialRecordCMO> materials;
			materials.reserve(*nMats);
			for (UINT j = 0; j < *nMats; ++j)
			{
				MaterialRecordCMO m;


				// Material name
				nName = reinterpret_cast<const UINT*>(meshData.get() + usedSize);
				usedSize += sizeof(UINT);
				if (dataSize < usedSize)
					throw std::exception("End of file");

				auto matName = reinterpret_cast<const wchar_t*>(meshData.get() + usedSize);

				usedSize += sizeof(wchar_t)*(*nName);
				if (dataSize < usedSize)
					throw std::exception("End of file");

				m.name.assign(matName, *nName);

				// Material settings
				auto matSetting = reinterpret_cast<const Material*>(meshData.get() + usedSize);
				usedSize += sizeof(Material);
				if (dataSize < usedSize)
					throw std::exception("End of file");

				m.pMaterial = matSetting;
				m.m_material.Load();
				m.m_material.Const.AmbientColor = matSetting->Ambient;
				m.m_material.Const.DiffuseColor = matSetting->Diffuse;
				m.m_material.Const.SpecularColor = DirectX::XMFLOAT3(matSetting->Specular.x
					, matSetting->Specular.y, matSetting->Specular.z);
				m.m_material.Const.AlphaThreshold = 0.02f;
				m.m_material.Const.GlobalAmbient = DirectX::XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f);
				m.m_material.Const.MaterialType = matType;
				m.m_material.Const.SpecularPower = matSetting->SpecularPower;
				m.m_material.Const.SpecularScale = 1.0f;

				m.m_wvpBuffer.Load();

				/*m.m_light.Load();
				m.m_light.Const.LightColor = DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
				m.m_light.Const.Direction = DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f);*/
				for (int i = 0;i < 8;i++)
				{
					m.textureView[i] = nullptr;
				}


				// Pixel shader name
				nName = reinterpret_cast<const UINT*>(meshData.get() + usedSize);
				usedSize += sizeof(UINT);
				if (dataSize < usedSize)
					throw std::exception("End of file");

				auto psName = reinterpret_cast<const wchar_t*>(meshData.get() + usedSize);

				usedSize += sizeof(wchar_t)*(*nName);
				if (dataSize < usedSize)
					throw std::exception("End of file");

				m.pixelShader.assign(psName, *nName);

				for (UINT t = 0; t < 8; ++t)
				{
					nName = reinterpret_cast<const UINT*>(meshData.get() + usedSize);
					usedSize += sizeof(UINT);
					if (dataSize < usedSize)
						throw std::exception("End of file");

					auto txtName = reinterpret_cast<const wchar_t*>(meshData.get() + usedSize);

					usedSize += sizeof(wchar_t)*(*nName);
					if (dataSize < usedSize)
						throw std::exception("End of file");

					m.texturename[t].assign(txtName, *nName);
					Texture2D * tmp = new Texture2D();
					tmp->LoadTexture(txtName,true);
					if (tmp->GetSRV().Get())
						m.textureView[t] = tmp->GetSRV().Get();
				}

				materials.emplace_back(m);
			}

			assert(materials.size() == *nMats);

			// Skeletal data?
			auto bSkeleton = reinterpret_cast<const BYTE*>(meshData.get() + usedSize);
			usedSize += sizeof(BYTE);
			if (dataSize < usedSize)
				throw std::exception("End of file");

			// Submeshes
			auto nSubmesh = reinterpret_cast<const UINT*>(meshData.get() + usedSize);
			usedSize += sizeof(UINT);
			if (dataSize < usedSize)
				throw std::exception("End of file");

			if (!*nSubmesh)
				throw std::exception("No submeshes found\n");

			auto subMesh = reinterpret_cast<const SubMesh*>(meshData.get() + usedSize);
			usedSize += sizeof(SubMesh) * (*nSubmesh);
			if (dataSize < usedSize)
				throw std::exception("End of file");

			// Index buffers
			auto nIBs = reinterpret_cast<const UINT*>(meshData.get() + usedSize);
			usedSize += sizeof(UINT);
			if (dataSize < usedSize)
				throw std::exception("End of file");

			if (!*nIBs)
				throw std::exception("No index buffers found\n");

			struct IBData
			{
				size_t          nIndices;
				const USHORT*   ptr;
			};

			std::vector<IBData> ibData;
			ibData.reserve(*nIBs);

			std::vector<Microsoft::WRL::ComPtr<ID3D11Buffer>> ibs;
			ibs.resize(*nIBs);

			for (UINT j = 0; j < *nIBs; ++j)
			{
				auto nIndexes = reinterpret_cast<const UINT*>(meshData.get() + usedSize);
				usedSize += sizeof(UINT);
				if (dataSize < usedSize)
					throw std::exception("End of file");

				if (!*nIndexes)
					throw std::exception("Empty index buffer found\n");

				size_t ibBytes = sizeof(USHORT) * (*(nIndexes));

				mesh->indices = reinterpret_cast<const USHORT*>(meshData.get() + usedSize);
				usedSize += ibBytes;
				if (dataSize < usedSize)
					throw std::exception("End of file");

				mesh->indicesSize = *nIndexes;
				std::vector<int> vec(mesh->indices, mesh->indices + mesh->indicesSize);
				mesh->indciesInt = vec;
				IBData ib;
				ib.nIndices = *nIndexes;
				ib.ptr = mesh->indices;
				ibData.emplace_back(ib);

				D3D11_BUFFER_DESC desc = { 0 };
				desc.Usage = D3D11_USAGE_DEFAULT;
				desc.ByteWidth = static_cast<UINT>(ibBytes);
				desc.BindFlags = D3D11_BIND_INDEX_BUFFER;

				D3D11_SUBRESOURCE_DATA initData = { 0 };
				initData.pSysMem = mesh->indices;
				AngelCore::AngelSubSystemResources::GraphicDeviceResources::Device->CreateBuffer(&desc, &initData, &ibs[j]);
			}

			assert(ibData.size() == *nIBs);
			assert(ibs.size() == *nIBs);

			// Vertex buffers
			auto nVBs = reinterpret_cast<const UINT*>(meshData.get() + usedSize);
			usedSize += sizeof(UINT);
			if (dataSize < usedSize)
				throw std::exception("End of file");

			if (!*nVBs)
				throw std::exception("No vertex buffers found\n");

			struct VBData
			{
				size_t                                          nVerts;
				const VertexTypesStruct::VertexPositionNormalTangentColorTexture*  ptr;
				const SkinningVertex*             skinPtr;
				VertexTypesStruct::VertexPositionNormalTangentColorTexture* combinedPtr;
			};

			std::vector<VBData> vbData;
			vbData.reserve(*nVBs);
			for (UINT j = 0; j < *nVBs; ++j)
			{
				auto nVerts = reinterpret_cast<const UINT*>(meshData.get() + usedSize);
				usedSize += sizeof(UINT);
				if (dataSize < usedSize)
					throw std::exception("End of file");

				if (!*nVerts)
					throw std::exception("Empty vertex buffer found\n");

				size_t vbBytes = sizeof(VertexTypesStruct::VertexPositionNormalTangentColorTexture) * (*(nVerts));

				auto verts = reinterpret_cast<const VertexTypesStruct::VertexPositionNormalTangentColorTexture*>(meshData.get() + usedSize);
				usedSize += vbBytes;
				if (dataSize < usedSize)
					throw std::exception("End of file");

				VBData vb;
				vb.nVerts = *nVerts;
				vb.ptr = verts;
				vb.skinPtr = nullptr;
				vbData.emplace_back(vb);
			}

			assert(vbData.size() == *nVBs);

			// Skinning vertex buffers
			auto nSkinVBs = reinterpret_cast<const UINT*>(meshData.get() + usedSize);
			usedSize += sizeof(UINT);
			if (dataSize < usedSize)
				throw std::exception("End of file");

			if (*nSkinVBs)
			{
				if (*nSkinVBs != *nVBs)
					throw std::exception("Number of VBs not equal to number of skin VBs");

				for (UINT j = 0; j < *nSkinVBs; ++j)
				{
					auto nVerts = reinterpret_cast<const UINT*>(meshData.get() + usedSize);
					usedSize += sizeof(UINT);
					if (dataSize < usedSize)
						throw std::exception("End of file");

					if (!*nVerts)
						throw std::exception("Empty skinning vertex buffer found\n");

					if (vbData[j].nVerts != *nVerts)
						throw std::exception("Mismatched number of verts for skin VBs");

					size_t vbBytes = sizeof(SkinningVertex) * (*(nVerts));

					auto verts = reinterpret_cast<const SkinningVertex*>(meshData.get() + usedSize);
					usedSize += vbBytes;
					if (dataSize < usedSize)
						throw std::exception("End of file");

					vbData[j].skinPtr = verts;
				}
			}

			// Extents
			auto extents = reinterpret_cast<const MeshExtents*>(meshData.get() + usedSize);
			usedSize += sizeof(MeshExtents);
			if (dataSize < usedSize)
				throw std::exception("End of file");

			mesh->boundingSphere.Center.x = extents->CenterX;
			mesh->boundingSphere.Center.y = extents->CenterY;
			mesh->boundingSphere.Center.z = extents->CenterZ;
			mesh->boundingSphere.Radius = extents->Radius;

			DirectX::BoundingBox b;
			DirectX::XMVECTOR min = DirectX::XMVectorSet(extents->MinX, extents->MinY, extents->MinZ, 0.f);
			DirectX::XMVECTOR max = DirectX::XMVectorSet(extents->MaxX, extents->MaxY, extents->MaxZ, 0.f);
			b.CreateFromPoints(b, min, max);
			mesh->boundingBox.Center = b.Center;
			mesh->boundingBox.Extents = b.Extents;
			//	DirectX::BoundingOrientedBox::CreateFromPoints(mesh->boundingBox,vbData[j;
				//mesh->m_collisionShape = new AngelRendering::DebugCollisionShape();
				//mesh->m_collisionShape->Create();
			mesh->enableSkinning = enableSkinning;
			mesh->enableInstancing = enableInstancing;
			// Animation data
			if (*bSkeleton)
			{
				// Bones
				auto nBones = reinterpret_cast<const UINT*>(meshData.get() + usedSize);
				usedSize += sizeof(UINT);
				if (dataSize < usedSize)
					throw std::exception("End of file");

				if (!*nBones)
					throw std::exception("Animation bone data is missing\n");

				for (UINT j = 0; j < *nBones; ++j)
				{
					// Bone name
					nName = reinterpret_cast<const UINT*>(meshData.get() + usedSize);
					usedSize += sizeof(UINT);
					if (dataSize < usedSize)
						throw std::exception("End of file");

					auto boneName = reinterpret_cast<const wchar_t*>(meshData.get() + usedSize);

					usedSize += sizeof(wchar_t)*(*nName);
					if (dataSize < usedSize)
						throw std::exception("End of file");

					// TODO - What to do with bone name?
					boneName;

					// Bone settings
					auto bones = reinterpret_cast<Bone*>(meshData.get() + usedSize);
					usedSize += sizeof(Bone);
					if (dataSize < usedSize)
						throw std::exception("End of file");
					Bone *tmp = new Bone();
					tmp->BindPos = bones->BindPos;
					tmp->InvBindPos = bones->InvBindPos;
					tmp->LocalTransform = bones->LocalTransform;
					tmp->ParentIndex = bones->ParentIndex;
					// TODO - What to do with bone data?
					mesh->bones.push_back(tmp);
					mesh->boneNames.push_back(boneName);
				}

				// Animation Clips
				auto nClips = reinterpret_cast<const UINT*>(meshData.get() + usedSize);
				usedSize += sizeof(UINT);
				if (dataSize < usedSize)
					throw std::exception("End of file");
				//*nClips == 0 ? mesh->enableSkinning = false : mesh->enableSkinning = true;
				//mesh->enableSkinning = (nClips) != 0;

				for (UINT j = 0; j < *nClips; ++j)
				{
					// Clip name
					nName = reinterpret_cast<const UINT*>(meshData.get() + usedSize);
					usedSize += sizeof(UINT);
					if (dataSize < usedSize)
						throw std::exception("End of file");

					auto clipName = reinterpret_cast<const wchar_t*>(meshData.get() + usedSize);

					usedSize += sizeof(wchar_t)*(*nName);
					if (dataSize < usedSize)
						throw std::exception("End of file");

					// TODO - What to do with clip name?
					clipName;

					auto clip = reinterpret_cast<const Clip*>(meshData.get() + usedSize);
					usedSize += sizeof(Clip);
					if (dataSize < usedSize)
						throw std::exception("End of file");
					AnimationClip * animClip = new AnimationClip();
					animClip->name = clipName;
					animClip->start = clip->StartTime;
					animClip->end = clip->EndTime;
					animClip->Keys = clip->keys;

					if (!clip->keys)
						throw std::exception("Keyframes missing in clip");

					auto keys = reinterpret_cast<const Keyframe*>(meshData.get() + usedSize);
					for (unsigned int i = 0;i < clip->keys;i++)
					{
						Keyframe * tmpKey = new Keyframe();
						tmpKey->BoneIndex = keys[i].BoneIndex;
						tmpKey->Time = keys[i].Time;
						tmpKey->Transform = keys[i].Transform;
						animClip->keyFrames.push_back(tmpKey);
						animClip->elapsedTime = 0.0f;
						animClip->timeScale = 1.0f;
					}
					usedSize += sizeof(Keyframe) * clip->keys;
					if (dataSize < usedSize)
						throw std::exception("End of file");

					// TODO - What to do with keys and clip->StartTime, clip->EndTime?
					keys;
					mesh->animationClips.push_back(animClip);
				}
			}

			// Build vertex buffers
			std::vector<Microsoft::WRL::ComPtr<ID3D11Buffer>> vbs;
			vbs.resize(*nVBs);

			const size_t stride = sizeof(VertexTypesStruct::VertexPositionNormalTangentTextureSkinned);

			for (UINT j = 0; j < *nVBs; ++j)
			{
				size_t nVerts = vbData[j].nVerts;

				size_t bytes = stride * nVerts;

				D3D11_BUFFER_DESC desc = { 0 };
				desc.Usage = D3D11_USAGE_DEFAULT;
				desc.ByteWidth = static_cast<UINT>(bytes);
				desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

				//if (!mesh->enableSkinning)
				//{
				// Can use CMO vertex data directly
				//	D3D11_SUBRESOURCE_DATA initData = { 0 };
				//initData.pSysMem = vbData[j].ptr;


				//}
				//else
				{
					std::unique_ptr<uint8_t> temp(new uint8_t[bytes + (sizeof(UINT) * nVerts)]);

					auto visited = reinterpret_cast<UINT*>(temp.get() + bytes);
					memset(visited, 0xff, sizeof(UINT) * nVerts);

					assert(vbData[j].ptr != 0);
					VertexTypesStruct::VertexPositionNormalTangentTextureSkinned*
						combinedPtr = new VertexTypesStruct::VertexPositionNormalTangentTextureSkinned[nVerts];
					//if (mesh->enableSkinning)
					{
						// Combine CMO multi-stream data into a single stream

						for (size_t v = 0; v < nVerts; ++v)
						{

							if (vbData[j].skinPtr)
							{
								auto skinptr = vbData[j].skinPtr[v];
								combinedPtr[v].boneIndex[0] = skinptr.boneIndex[0];
								combinedPtr[v].boneIndex[1] = skinptr.boneIndex[1];
								combinedPtr[v].boneIndex[2] = skinptr.boneIndex[2];
								combinedPtr[v].boneIndex[3] = skinptr.boneIndex[3];

								combinedPtr[v].boneWeight[0] = skinptr.boneWeight[0];
								combinedPtr[v].boneWeight[1] = skinptr.boneWeight[1];
								combinedPtr[v].boneWeight[2] = skinptr.boneWeight[2];
								combinedPtr[v].boneWeight[3] = skinptr.boneWeight[3];
							}

							auto sptr = vbData[j].ptr[v];
							combinedPtr[v].Position = sptr.Position;
							combinedPtr[v].Normal = sptr.Normal;
							combinedPtr[v].Tangent = sptr.Tangent;
							combinedPtr[v].TexCoord = sptr.TexCoord;

						}
					}

					if (true)
					{
						// Need to fix up VB tex coords for UV transform which is not supported by basic effects
						for (UINT k = 0; k < *nSubmesh; ++k)
						{
							auto& sm = subMesh[k];

							if (sm.VertexBufferIndex != j)
								continue;

							if ((sm.IndexBufferIndex >= *nIBs)
								|| (sm.MaterialIndex >= *nMats))
								throw std::exception("Invalid submesh found\n");

							DirectX::XMMATRIX uvTransform = XMLoadFloat4x4
								(&materials[sm.MaterialIndex].pMaterial->UVTransform);

							auto ib = ibData[sm.IndexBufferIndex].ptr;

							size_t count = ibData[sm.IndexBufferIndex].nIndices;

							for (size_t q = 0; q < count; ++q)
							{
								size_t v = ib[q];

								if (v >= nVerts)
									throw std::exception("Invalid index found\n");

								auto verts =
									reinterpret_cast<VertexTypesStruct::VertexPositionNormalTangentTextureSkinned*>
									(combinedPtr + (v * stride));
								if (visited[v] == UINT(-1))
								{
									visited[v] = sm.MaterialIndex;

									DirectX::XMVECTOR t = DirectX::XMLoadFloat2(&combinedPtr[v].TexCoord);

									t = DirectX::XMVectorSelect(DirectX::g_XMIdentityR3, t, DirectX::g_XMSelect1110);

									t = XMVector4Transform(t, uvTransform);

									DirectX::XMStoreFloat2(&combinedPtr[v].TexCoord, t);
								}
								else if (visited[v] != sm.MaterialIndex)
								{

								}
							}
						}
					}

					// Create vertex buffer from temporary buffer
					D3D11_SUBRESOURCE_DATA initData = { 0 };
					initData.pSysMem = combinedPtr;
					AngelCore::AngelSubSystemResources::GraphicDeviceResources::Device->CreateBuffer
						(&desc, &initData, &vbs[j]);
					delete[]combinedPtr;
				}
			}
			assert(vbs.size() == *nVBs);
			// Build mesh parts
			for (UINT j = 0; j < *nSubmesh; ++j)
			{
				auto& sm = subMesh[j];

				if ((sm.IndexBufferIndex >= *nIBs)
					|| (sm.VertexBufferIndex >= *nVBs)
					|| (sm.MaterialIndex >= *nMats))
					throw std::exception("Invalid submesh found\n");

				auto& mat = materials[sm.MaterialIndex];


				auto part = new AngelCore::AngelWorldSystem::ModelMeshPart();

				if (mat.pMaterial->Diffuse.w < 1)
					part->isAlpha = true;

				//if (lstrcmpW(mesh->name.c_str(), L"window_roof")==0)
				{
					part->collisionShape = new AngelRendering::DebugCollisionShape();
					mesh->points = new DirectX::XMFLOAT3[vbData[j].nVerts];
					mesh->pointsSize = vbData[j].nVerts;
					for (unsigned int k = 0;k < vbData[j].nVerts;k++)
					{
						mesh->points[k] = vbData[j].ptr[k].Position;
					}
					DirectX::BoundingOrientedBox bb;
					bb.CreateFromPoints(bb, mesh->pointsSize, mesh->points, sizeof(DirectX::XMFLOAT3));
					part->boundingBox = bb;
					part->collisionShape->Create();
					
				}

				
				CollisionModel cm;
				cm.collisionShape = part->boundingBox;
				std::string nameS(mesh->name.begin(), mesh->name.end());
				cm.name = nameS;
				AngelCore::AngelSubSystemResources::BaseCameraProperties::BCamera->AddCollisionObject(cm);

				part->indexCount = sm.PrimCount * 3;
				part->startIndex = sm.StartIndex;
				part->vertexStride = static_cast<UINT>(stride);
				//part->inputLayout = mat.il;
				part->indexBuffer = ibs[sm.IndexBufferIndex];
				part->vertexBuffer = vbs[sm.VertexBufferIndex];

				part->shader = new AngelRendering::Shader();
				AngelRendering::Shader::LoadShader(ShaderType::VertexShader,
					"VertexForwardRenderer.cso", VertexTypes::PositionNormalTangentTextureSkinned, part->shader);
				AngelRendering::Shader::LoadShader(ShaderType::PixelShader,
					"PixelForwardRenderer.cso", VertexTypes::NOP, part->shader);
				part->m_material = mat.m_material;
				part->m_wvpBuffer = mat.m_wvpBuffer;
				for (int i = 0;i < 8;i++)
					part->textureView[i] = mat.textureView[i];
				//part->effect = mat.effect;
				//part->vbDecl = enableSkinning ? g_vbdeclSkinning : g_vbdecl;
				part->enableSkinning = mesh->enableSkinning;
				mesh->meshParts.emplace_back(part);

			}

			model->meshes.emplace_back(mesh);
		}
	}
}
void AngelCore::AngelWorldSystem::MeshRenderer::Render()
{
	//World = this->GetOwner()->GetComponent<AngelCore::AngelWorldSystem::Transform>()->GetWorldMatrix();
	if (!GetOwner()->GetHidden())
	{
		model->Draw(AngelCore::AngelSubSystemResources::GraphicDeviceResources::DeviceContext.Get(),
			AngelCore::AngelSubSystemResources::BaseCameraProperties::BCamera->GetView(),
			AngelCore::AngelSubSystemResources::BaseCameraProperties::BCamera->GetProjection(), this->m_shader, false
			, GetOwner());
	}
}

void AngelCore::AngelWorldSystem::MeshRenderer::RenderDepth()
{
	model->Draw(AngelCore::AngelSubSystemResources::GraphicDeviceResources::DeviceContext.Get(),
		AngelCore::AngelSubSystemResources::BaseCameraProperties::BCamera->GetView(),
		AngelCore::AngelSubSystemResources::BaseCameraProperties::BCamera->GetProjection(), this->m_shader, true
		,GetOwner());
}
void AngelCore::AngelWorldSystem::MeshRenderer::RenderInstanced(std::vector<DirectX::XMMATRIX> World)
{
	if (World.size() == 0) return;
	if (m_lastInstanceSize != World.size())
	{
		VertexTypesStruct::Instancing * perInstanceData = new VertexTypesStruct::Instancing[World.size()];
		for (unsigned int i = 0;i < World.size();i++)
		{
			DirectX::XMStoreFloat4x4(&perInstanceData[i].world, World.at(i));
			//perInstanceData[i].scale = scale.at(i);
			//perInstanceData[i].rotation = rotation.at(i);
		}
		m_lastInstanceSize = World.size();
		D3D11_BUFFER_DESC desc;
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		desc.ByteWidth = World.size()*sizeof(VertexTypesStruct::Instancing);
		desc.CPUAccessFlags = 0;
		desc.MiscFlags = 0;
		desc.StructureByteStride = 0;
		desc.Usage = D3D11_USAGE_DEFAULT;
		Microsoft::WRL::ComPtr<ID3D11Buffer> tmpBuffer;

		D3D11_SUBRESOURCE_DATA data;
		data.SysMemPitch = data.SysMemSlicePitch = 0;
		data.pSysMem = perInstanceData;
		HR(AngelCore::AngelSubSystemResources::GraphicDeviceResources::Device->
			CreateBuffer(&desc, &data, tmpBuffer.GetAddressOf()));

		model->SetInstanceBuffer(tmpBuffer);

	}

	model->DrawInstanced(AngelCore::AngelSubSystemResources::GraphicDeviceResources::DeviceContext.Get(),
		AngelCore::AngelSubSystemResources::BaseCameraProperties::BCamera->GetView(),
		AngelCore::AngelSubSystemResources::BaseCameraProperties::BCamera->GetProjection(), this->m_shader, false
		, World.size());
}

void AngelCore::AngelWorldSystem::MeshRenderer::Update(const AngelSubSystem::TimeManager &timer)
{
	using namespace DirectX;
	for (auto it = model->meshes.cbegin(); it != model->meshes.cend(); ++it)
	{
		auto mesh = it->get();
		if (mesh->enableSkinning)
		{
			for (unsigned int i = 0;i < mesh->bones.size();i++)
			{
				mesh->armatureBuffer.Const.Bones[i] = mesh->bones[i]->LocalTransform;
			}

			//Animate
			Keyframe * lastKeyForBones =
				new Keyframe[mesh->bones.size()];

			bool *lerpedBones = new bool[mesh->bones.size()];
			for (unsigned int i = 0;i < mesh->bones.size();i++)
			{
				lerpedBones[i] = true;
			}
			for (unsigned int i = 0;i < mesh->animationClips[m_animationIndex]->Keys;i++)
			{
				Keyframe * frame =
					mesh->animationClips[m_animationIndex]->keyFrames[i];
				if (mesh->animationClips[m_animationIndex]->elapsedTime >= frame->Time)
				{
					lastKeyForBones
						[frame->BoneIndex]
					= *frame;

					mesh->armatureBuffer.Const.Bones
						[frame->BoneIndex] =
						frame->Transform;
				}
				else
				{
					if (!lerpedBones
						[frame->BoneIndex])
					{
						Keyframe * prevFrame;
						if (&lastKeyForBones
							[frame->BoneIndex] != nullptr)
						{
							prevFrame =
								&lastKeyForBones[frame->BoneIndex];
						}
						else
							continue;

						lerpedBones[frame->BoneIndex] = true;
						float lenght = frame->Time
							- prevFrame->Time;
						float timeDiff = mesh->animationClips[m_animationIndex]->elapsedTime - prevFrame->Time;
						float amount = timeDiff / lenght;
						DirectX::XMVECTOR t1, t2, s1, s2, r1, r2;
						DirectX::XMMatrixDecompose(
							&s1, &r1, &t1,
							DirectX::XMLoadFloat4x4
							(&prevFrame->Transform));

						DirectX::XMMatrixDecompose(
							&s2, &r2, &t2,
							DirectX::XMLoadFloat4x4
							(&frame->Transform));

						DirectX::XMVECTOR lerpedScale =
							DirectX::XMVectorLerp(s1, s2, amount);

						DirectX::XMVECTOR lerpedtranslation =
							DirectX::XMVectorLerp(t1, t2, amount);

						DirectX::XMVECTOR lerpedrotation =
							DirectX::XMQuaternionSlerp(r1, r2, amount);

						DirectX::XMStoreFloat4x4
							(&mesh->armatureBuffer.Const.Bones
								[frame->BoneIndex],
								DirectX::XMMatrixScaling(
									lerpedScale.m128_f32[0],
									lerpedScale.m128_f32[1],
									lerpedScale.m128_f32[2])
								*
								DirectX::XMMatrixRotationQuaternion(
									lerpedrotation)
								*
								DirectX::XMMatrixTranslation(
									lerpedtranslation.m128_f32[0]
									, lerpedtranslation.m128_f32[1],
									lerpedtranslation.m128_f32[2]));
					}
				}
			}

			for (unsigned int i = 0;i < mesh->bones.size();i++)
			{
				if (mesh->bones[i]->ParentIndex>-1)
				{
					DirectX::XMFLOAT4X4 parentTransform
						= mesh->armatureBuffer.Const.Bones[mesh->bones[i]->ParentIndex];
					DirectX::XMStoreFloat4x4(
						&mesh->armatureBuffer.Const.Bones[i],
						DirectX::XMMatrixMultiply(
							DirectX::XMLoadFloat4x4(&mesh->armatureBuffer.Const.Bones[i])
							, DirectX::XMLoadFloat4x4(&parentTransform)));
				}
			}
			for (unsigned int i = 0;i < mesh->bones.size();i++)
			{
				DirectX::XMStoreFloat4x4(&mesh->armatureBuffer.Const.Bones[i],
					DirectX::XMMatrixTranspose(
						DirectX::XMMatrixMultiply(
							DirectX::XMLoadFloat4x4(&mesh->bones[i]->InvBindPos)
							, DirectX::XMLoadFloat4x4(&mesh->armatureBuffer.Const.Bones[i]))));
			}

			if (!mesh->animationClips[m_animationIndex]->UseCodeEnding)
			{
				if (mesh->animationClips[m_animationIndex]->elapsedTime > mesh->animationClips[m_animationIndex]->end + (float)timer.GetElapsedSeconds())
				{
					mesh->animationClips[m_animationIndex]->elapsedTime = 0.0f;
				}
			}
			else
			{
				//Use our ending time instead of animation clip default time.
				if (mesh->animationClips[m_animationIndex]->elapsedTime > mesh->animationClips[m_animationIndex]->CodeEndingTime + (float)timer.GetElapsedSeconds())
				{
					mesh->animationClips[m_animationIndex]->elapsedTime = 0.0f;
				}
			}
			
			mesh->animationClips[m_animationIndex]->elapsedTime += 
				(float)timer.GetElapsedSeconds()*mesh->animationClips[m_animationIndex]->timeScale;
		}
	}
}

AngelCore::AngelWorldSystem::MeshRenderer::~MeshRenderer()
{
	delete this->m_shader;
	delete this->model;
	delete[] m_points;
}

void AngelCore::AngelWorldSystem::MeshRenderer::SetDiffuseColor(DirectX::XMFLOAT4 color)
{
	for (auto & meshes : model->meshes)
	{
		for (auto & subMeshes : meshes->meshParts)
		{
			subMeshes->m_material.Const.DiffuseColor = color;
		}
	}
}

void AngelCore::AngelWorldSystem::MeshRenderer::SetDiffuseColor(float &r,float &g,float &b,float &a)
{
	for (auto & meshes : model->meshes)
	{
		for (auto & subMeshes : meshes->meshParts)
		{
			subMeshes->m_material.Const.DiffuseColor = DirectX::XMFLOAT4(r, g, b, a);
		}
	}
}

void AngelCore::AngelWorldSystem::MeshRenderer::SetAmbientColor(DirectX::XMFLOAT4 color)
{
	for (auto & meshes : model->meshes)
	{
		for (auto & subMeshes : meshes->meshParts)
		{
			subMeshes->m_material.Const.AmbientColor = color;
		}
	}
}

void AngelCore::AngelWorldSystem::MeshRenderer::SetAmbientColor(float &r, float &g, float &b, float &a)
{
	for (auto & meshes : model->meshes)
	{
		for (auto & subMeshes : meshes->meshParts)
		{
			subMeshes->m_material.Const.AmbientColor = DirectX::XMFLOAT4(r, g, b, a);
		}
	}
}

void AngelCore::AngelWorldSystem::MeshRenderer::SetMaterialType(int type)
{
	for (auto & meshes : model->meshes)
	{
		for (auto & subMeshes : meshes->meshParts)
		{
			subMeshes->m_material.Const.MaterialType = type;
		}
	}
}

AngelCore::AngelWorldSystem::ModelMesh* AngelCore::AngelWorldSystem::MeshRenderer::GetSubMeshByName(const wchar_t * name)
{
	for (auto & subMesh : model->meshes)
	{
		if (lstrcmpW(subMesh->name.c_str(), name) == 0)
		{
			return subMesh.get();
		}
	}
	return nullptr;
}

std::vector<AngelCore::AngelWorldSystem::ModelMesh *> AngelCore::AngelWorldSystem::MeshRenderer::GetAllMeshes()
{
	std::vector<AngelCore::AngelWorldSystem::ModelMesh*> ret;
	for (auto  subMesh : model->meshes)
	{
		ret.push_back(subMesh._Get());
	}
	return ret;
}

void AngelCore::AngelWorldSystem::MeshRenderer::SetAnimationClipEndTime(int &index, float &time)
{
	for (auto & meshes : model->meshes)
	{
		meshes->animationClips[index]->UseCodeEnding = true;
		meshes->animationClips[index]->CodeEndingTime = time;

	}
}

void AngelCore::AngelWorldSystem::MeshRenderer::SetAnimationClipTimeScale(int &index, float &time)
{
	for (auto & meshes : model->meshes)
	{
		meshes->animationClips[index]->timeScale = time;
	}
}

void AngelCore::AngelWorldSystem::MeshRenderer::DisableAnimationClipEndTime(int &index)
{
	for (auto & meshes : model->meshes)
	{
		meshes->animationClips[index]->UseCodeEnding = false;
	}
}