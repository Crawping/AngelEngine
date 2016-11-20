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
#include"../../../AngelSubSystem/Public/SubSystemsResource.h"
#include<d3d11_1.h>
#include<DirectXMath.h>
#include<wrl\client.h>

#define __CBUFFER

template<typename T>
class CBuffer
{
public:
	T Const;
	void Load()
	{
		D3D11_BUFFER_DESC constBuffer;
		constBuffer.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		constBuffer.ByteWidth = sizeof(T);
		constBuffer.CPUAccessFlags = 0;
		constBuffer.MiscFlags = 0;
		constBuffer.Usage = D3D11_USAGE_DEFAULT;
		constBuffer.StructureByteStride = 0;
		HRESULT hr = AngelCore::AngelSubSystemResources::GraphicDeviceResources::Device->CreateBuffer(&constBuffer, nullptr, &buffer);
		
	}
	void Update()
	{
		AngelCore::AngelSubSystemResources::GraphicDeviceResources::DeviceContext->UpdateSubresource(this->buffer, 0, nullptr, &this->Const, 0, 0);
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
};



