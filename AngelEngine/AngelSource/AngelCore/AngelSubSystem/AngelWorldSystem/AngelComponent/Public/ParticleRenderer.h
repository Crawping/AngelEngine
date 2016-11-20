// ***********************************************************************
// FileName         : SpriteRenderer.h
// Author           : Peyman Tahghighi
//
// ***********************************************************************
//     Copyright (c) Peyman Tahghighi . All rights reserved.
//
// <summary> Component for rendering Particles  </summary>
// ***********************************************************************
#pragma once
#define __PARTICLE_RENDERER



#define MAX_PARTICLES 5000
#include"../../AngelComponent/Public/IComponent.h"
#include"../../../Public/TimeManager.h"
#include"../../../AngelRendering/Public/Shader.h"

#include<wrl\client.h>

#ifndef __CBUFFER
#define __CBUFFER
#include"../../../AngelRendering/Public/CBuffer.h"
#endif

#include<string>

namespace AngelCore
{
	namespace AngelWorldSystem
	{
		class ParticleRenderer : public AngelCore::AngelWorldSystem::IComponent
		{
			struct cbPerObject
			{
				DirectX::XMFLOAT4X4 WVP;
				DirectX::XMFLOAT4X4 World;
				DirectX::XMFLOAT4X4 InvView;
				DirectX::XMFLOAT4X4 InvProjection;
			};

		public:
			ParticleRenderer();
		public:

			virtual void Initialize(AngelCore::AngelWorldSystem::Actor* _owner, ...) override;

			virtual void Update(const AngelCore::AngelSubSystem::TimeManager &timer) override;

			virtual void Render() override;

		public:

			void SetLifeTime(float &time)			{ this->m_LifeTime = time; }
			void SetLifeTime(float time)			{ this->m_LifeTime = time; }

			void SetVelocity(Vector3 &vec)			{ this->m_ParticleVel = vec.GetXMFLOAT3(); }
			void SetVelocity(DirectX::XMFLOAT3 vec) { this->m_ParticleVel = vec; }

			void SetEmitRate(float &rate)			{ this->m_fEmitRate = rate; }
			void SetEmitRate(float rate)			{ this->m_fEmitRate = rate; }

			void SetParticleMinSize(float & size)	{ this->m_fParticleMinSize = size; }
			void SetParticleMinSize(float  size)	{ this->m_fParticleMinSize = size; }

			void SetParticleMaxSize(float & size)	{ this->m_fParticleMaxSize = size; }
			void SetParticleMaxSize(float  size)	{ this->m_fParticleMaxSize = size; }

			void SetRandomInXDirection(bool &b)		{ if (b == true) m_randomInXDirection = true; else m_randomInXDirection = false; }
			void SetRandomInXDirection(bool b)		{ if (b == true) m_randomInXDirection = true; else m_randomInXDirection = false; }

			void SetRandomInYDirection(bool &b)		{ if (b == true) m_randomInYDirection = true; else m_randomInYDirection = false; }
			void SetRandomInYDirection(bool b)		{ if (b == true) m_randomInYDirection = true; else m_randomInYDirection = false; }

			void SetRandomInZDirection(bool &b)		{ if (b == true) m_randomInZDirection = true; else m_randomInZDirection = false; }
			void SetRandomInZDirection(bool b)		{ if (b == true) m_randomInZDirection = true; else m_randomInZDirection = false; }

			float GetLifeTime() const				{ return m_LifeTime; }
			Vector3 GetVelocity() const				{ return m_ParticleVel; }
			float GetEmitRate() const				{ return m_fEmitRate; }
			float GetParticleMaxSize() const		{ return m_fParticleMaxSize; }
			float GetParticleMinSize() const		{ return m_fParticleMinSize; }

			void LoadParticleTexture(std::string &name);

		private:
			struct cbPerObjectBuffer
			{
				DirectX::XMFLOAT4X4 WVP;
			};


			Microsoft::WRL::ComPtr<ID3D11Buffer>m_vertexBuffer;
			Microsoft::WRL::ComPtr<ID3D11Buffer>m_indexBuffer;
			UINT								m_stride;
			UINT								m_offset;
			AngelCore::AngelRendering::Shader *	m_shader;
			CBuffer<cbPerObject>				m_cbPerObjectBuffer;
			VertexTypesStruct::VertexParticel*	m_cpuParticles;
			DWORD*								m_cpuPartcilesIndices;
			float*								m_partriclesDepthArray;
			Texture2D	*						m_gradiantTexture;
			Texture2D	*						m_particleTexture;
			float								m_fFadeDistance;
			float								m_fParticleLifeSpan;
			float								m_fEmitRate;

			DirectX::XMFLOAT3					m_ParticleVel;
			float								m_fParticleMaxSize;
			float								m_fParticleMinSize;
			float								m_LifeTime;
			bool								m_randomInYDirection;
			bool								m_randomInXDirection;
			bool								m_randomInZDirection;
		};
	}
}