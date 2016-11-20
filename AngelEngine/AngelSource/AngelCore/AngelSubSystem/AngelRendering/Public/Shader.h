// ***********************************************************************
// FileName         : Shader.h
// Author           : Peyman Tahghighi
//
// ***********************************************************************
//     Copyright (c) Peyman Tahghighi . All rights reserved.
//
// <summary> Creating and using all types of shaders  </summary>
// ***********************************************************************
#pragma once

#include <windows.h>
#include <string>
#include <memory>
#include <fstream>
#include <fileapi.h>
#include <io.h>
#include <vector>
#include<wrl\client.h>

#ifndef __VERTEX_TYPES
#define __VERTEX_TYPES
#include"VertexTypes.h"
#endif

#ifndef __TEXTURE2D
#define __TEXTURE2D
#include"Texture2D.h"
#endif

#include"../../../AngelSubSystem/AngelLog/Public/AngelLog.h"
#include"../../../AngelSubSystem/Public/FileSystemManager.h"
#include"../../../AngelSubSystem/Public/SubSystemsResource.h"

#define __SHADER


enum ShaderType : byte
{
	VertexShader,
	PixelShader,
	GeometryShaderWithSO,
	GeometryShader,
	DomainShader,
	HullShader,
	ComputeShader
};

namespace AngelCore
{
	namespace AngelRendering 
	{
		class Shader
		{
		public:
			Shader();
			virtual void CreateVertexShader(const void* bytes, SIZE_T Length, VertexTypes VDeclaration);
			virtual void CreatePixelShader(const void* bytes, SIZE_T Length);
			virtual void CreateComputeShader(const void * bytes, SIZE_T length);
			virtual void CreateGeometryShaderWithSO(const void * bytes, SIZE_T length);
			virtual void CreateGeometryShader(const void * bytes, SIZE_T length);
			void SetTexture2D(UINT StartSLot, UINT NumViews, ID3D11ShaderResourceView ** SRV);

			virtual void SetVertexShader(_In_ ID3D11VertexShader* VS);
			virtual void SetPixelShader(_In_ ID3D11PixelShader* PS);
			virtual void SetComputeShader(_In_ ID3D11ComputeShader *CS);
			virtual void BindSRV(UINT startSlot, UINT numBuffer, _In_ ID3D11ShaderResourceView * SRV[2], ShaderType ST);
			virtual void BindSRV(UINT startSlot, UINT numBuffer, _In_ ID3D11ShaderResourceView * SRV, ShaderType ST);
			virtual void BindUAV(UINT startSlot, UINT nulbuffer, ID3D11UnorderedAccessView *UAV);
			virtual void UnBindSRV(UINT startSlot, UINT numBuffer, ShaderType ST);
			virtual void UnBindUAV(UINT startSlot, UINT numBuffer);
			virtual void UnBindSampler(UINT startSlot, UINT numBuffer);

			virtual void SetConstantBuffer(UINT startSlot, UINT numBuffer, _In_ ID3D11Buffer* CBuffer);
			virtual void SetInputLayout(ID3D11InputLayout* value);

			virtual void SetTexture2D(UINT StartSLot, UINT NumViews, Texture2D* texture2D);
			void SetSampler(UINT startSlot, UINT numView, ID3D11SamplerState ** sampler);
			void Dispatch(UINT X, UINT Y, UINT Z);
			virtual void Apply();
			virtual void ApplySO(ID3D11Buffer * buffer);
			virtual void Disable();
			virtual void Release();

			static void LoadShader(ShaderType ST, const char * name, VertexTypes VT , Shader* shader)
			{
				if(!shader)
					shader = new Shader();

				std::unique_ptr<uint8_t[]> data;
				size_t dataSize;
				int fileState;
				std::string dataS = name;
				std::wstring path = AngelCore::AngelSubSystem::FileSystemManager::GetContentDirectory();
				std::string pathS(path.begin(), path.end());
				//AngelCore::AngelSubSystem::AngelLog::WriteLogMessage(pathS.c_str());
				dataS = pathS + "\\AngelData\\Shader\\" + dataS;
				std::wstring pathWS(dataS.begin(), dataS.end());
				HR(AngelCore::AngelSubSystem::FileSystemManager::ReadBinaryFile(pathWS, data, &dataSize, &fileState));
				switch (ST)
				{
				case ShaderType::VertexShader:
					shader->CreateVertexShader(data.get(), dataSize, VT);
					break;
				case ShaderType::PixelShader:
					shader->CreatePixelShader(data.get(), dataSize);
					break;
				case ShaderType::ComputeShader:
					shader->CreateComputeShader(data.get(), dataSize);
					break;
				case ShaderType::GeometryShader:
					shader->CreateGeometryShader(data.get(), dataSize);
					break;
				case ShaderType::GeometryShaderWithSO:
					shader->CreateGeometryShaderWithSO(data.get(), dataSize);
					break;
				default:
					break;
				}
			}

		private:

			Microsoft::WRL::ComPtr<ID3D11InputLayout>																inputLayout;
			Microsoft::WRL::ComPtr<ID3D11VertexShader>																vShader;
			Microsoft::WRL::ComPtr<ID3D11PixelShader>																pShader;
			Microsoft::WRL::ComPtr<ID3D11ComputeShader>																cShader;
			Microsoft::WRL::ComPtr<ID3D11GeometryShader>															gShaderSO;
			Microsoft::WRL::ComPtr<ID3D11GeometryShader>															gShader;

		private:
			HRESULT CreateInputLayout(const D3D11_INPUT_ELEMENT_DESC*, const UINT, const void*, const SIZE_T);
			HRESULT CreateInputLayout(VertexTypes VT, const void*, const SIZE_T);


		private:

		};
	}
}