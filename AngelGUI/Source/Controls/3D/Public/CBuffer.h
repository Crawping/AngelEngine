// ***********************************************************************
// FileName         : CBuffer.h
// Author           : Peyman Tahghighi
//
// ***********************************************************************
//     Copyright (c) Peyman Tahghighi . All rights reserved.
//
// <summary> Constant buffer template class  </summary>
// ***********************************************************************

#pragma once
#include<d3d11_1.h>
#include<DirectXMath.h>
#include<wrl\client.h>
#include"../../../Core/GlobalTools.h"

#define __CBUFFER

namespace AngelGUISystem
{
	namespace GUI3D
	{
		namespace GUI3DRendering
		{
			template<typename T>
			class CBuffer
			{
			public:
				T Const;
				void Load(ID3D11Device1 *device,ID3D11DeviceContext1* context)
				{
					m_deviceContext = context;

					D3D11_BUFFER_DESC constBuffer;
					constBuffer.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
					constBuffer.ByteWidth = sizeof(T);
					constBuffer.CPUAccessFlags = 0;
					constBuffer.MiscFlags = 0;
					constBuffer.Usage = D3D11_USAGE_DEFAULT;
					constBuffer.StructureByteStride = 0;
					HR(device->CreateBuffer(&constBuffer, nullptr, &buffer));

				}
				void Update()
				{
					m_deviceContext->UpdateSubresource(this->buffer, 0, nullptr, &this->Const, 0, 0);
				}
				ID3D11Buffer * getBuffer()
				{
					return this->buffer;
				}

				void Release()
				{
					buffer->Release();
					buffer = nullptr;
				}
			private:
				ID3D11Buffer								*buffer;
				//cache pointer to device context.
				Microsoft::WRL::ComPtr<ID3D11DeviceContext1> m_deviceContext;
			};
		}
	}
}