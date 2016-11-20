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
#include<d3d11_1.h>

#define __SHADER




namespace AngelGUISystem
{
	namespace GUI3D
	{
		namespace GUI3DRendering 
		{
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

			class Shader
			{
				struct handle_closer { void operator()(HANDLE h) { if (h) CloseHandle(h); } };
				typedef public std::unique_ptr<void, handle_closer> ScopedHandle;
				static HANDLE safe_handle(HANDLE h) { return (h == INVALID_HANDLE_VALUE) ? 0 : h; }
			public:
				Shader();
				 void CreateVertexShader(const void* bytes, SIZE_T Length);
				 void CreatePixelShader(const void* bytes, SIZE_T Length);

				void SetVertexShader(_In_ ID3D11VertexShader* VS);
				void SetPixelShader(_In_ ID3D11PixelShader* PS);
				void BindSRV(UINT startSlot, UINT numBuffer, _In_ ID3D11ShaderResourceView * SRV[2], ShaderType ST);
				void BindSRV(UINT startSlot, UINT numBuffer, _In_ ID3D11ShaderResourceView * SRV, ShaderType ST);
				void UnBindSRV(UINT startSlot, UINT numBuffer, ShaderType ST);
				void UnBindSampler(UINT startSlot, UINT numBuffer);
				void Apply();

				void SetConstantBuffer(UINT startSlot, UINT numBuffer, _In_ ID3D11Buffer* CBuffer);
				void SetInputLayout(ID3D11InputLayout* value);

				void SetSampler(UINT startSlot, UINT numView, ID3D11SamplerState ** sampler);
				void Disable();
				void Release();

				HRESULT ReadBinaryFile(_In_z_ std::wstring fileName, _Inout_ std::unique_ptr<uint8_t[]>& data,
					_Out_ size_t* dataSize, _Out_ int* fileState)
				{
					// Open the file.
#if (_WIN32_WINNT >= 0x0602 /*_WIN32_WINNT_WIN8*/)
					ScopedHandle hFile(safe_handle(CreateFile2(fileName.c_str(), GENERIC_READ, FILE_SHARE_READ, OPEN_EXISTING, nullptr)));
#else
					ScopedHandle hFile(safe_handle(CreateFileW(fileName, GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr)));
#endif

					if (!hFile)	return HRESULT_FROM_WIN32(GetLastError());

					// Get the file size.
					LARGE_INTEGER fileSize = { 0 };

#if (_WIN32_WINNT >= _WIN32_WINNT_VISTA)
					FILE_STANDARD_INFO fileInfo;

					if (!GetFileInformationByHandleEx(hFile.get(), FileStandardInfo, &fileInfo, sizeof(fileInfo)))
					{
						return HRESULT_FROM_WIN32(GetLastError());
					}

					fileSize = fileInfo.EndOfFile;
#else
					GetFileSizeEx(hFile.get(), &fileSize);
#endif

					// File is too big for 32-bit allocation, so reject read.
					if (fileSize.HighPart > 0)
						return E_FAIL;

					// Create enough space for the file data.
					data.reset(new uint8_t[fileSize.LowPart]);

					if (!data)
					{
						return E_OUTOFMEMORY;
					}
					// Read the data in.
					DWORD bytesRead = 0;

					if (!ReadFile(hFile.get(), data.get(), fileSize.LowPart, &bytesRead, nullptr))
					{
						return HRESULT_FROM_WIN32(GetLastError());
					}

					if (bytesRead < fileSize.LowPart)
					{
						return E_FAIL;
					}

					*dataSize = bytesRead;

					return S_OK;
				}

				void Load();
			private:

				Microsoft::WRL::ComPtr<ID3D11InputLayout>																inputLayout;
				Microsoft::WRL::ComPtr<ID3D11VertexShader>																vShader;
				Microsoft::WRL::ComPtr<ID3D11PixelShader>																pShader;

			private:
				void CreateInputLayout(const D3D11_INPUT_ELEMENT_DESC*, const UINT, const void*, const SIZE_T);
				void CreateInputLayout(const void*, const SIZE_T);
			};
		}
	}
}