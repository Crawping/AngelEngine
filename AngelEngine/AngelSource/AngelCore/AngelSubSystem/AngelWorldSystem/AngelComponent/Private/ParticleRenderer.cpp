

#include "../Public/ParticleRenderer.h"
#include"../../../Public/SubSystemsResource.h"
#include"../../Public/Actor.h"
#include"../Public/Transform.h"
#include"../../../AngelRendering/Public/PostProcess.h"
#include<AntTweakBar.h>



AngelCore::AngelWorldSystem::ParticleRenderer::ParticleRenderer()
{


}

void AngelCore::AngelWorldSystem::ParticleRenderer::Initialize(AngelCore::AngelWorldSystem::Actor* _owner, ...)
{
	
	//TwBar *bar = TwNewBar("TweakBar");
	//TwDefine(" GLOBAL help='This example shows how to integrate AntTweakBar into a DirectX11 application.' "); // Message added to the help bar.
	//int barSize[2] = { 224, 320 };
	//TwSetParam(bar, NULL, "size", TW_PARAM_INT32, 2, barSize);

	// Add variables to the tweak bar
	
	//TwAddVarCB(bar, "???", TW_TYPE_INT32, SetSpongeLevelCB, GetSpongeLevelCB, NULL, 
		//"min=0 max=3 group=Sponge keyincr=l keydecr=L");

	using namespace DirectX;
	this->m_owner = _owner;
	this->m_shader = new AngelCore::AngelRendering::Shader();
	AngelRendering::Shader::LoadShader(ShaderType::VertexShader,
		"VertexParticle.cso", VertexTypes::Particle, this->m_shader);
	AngelRendering::Shader::LoadShader(ShaderType::GeometryShader,
		"GeometryParticle.cso", VertexTypes::NOP, this->m_shader);
	AngelRendering::Shader::LoadShader(ShaderType::PixelShader,
		"PixelParticle.cso", VertexTypes::NOP, this->m_shader);
	m_active = true;

	this->m_stride = sizeof(VertexTypesStruct::PositionTexCoord);
	this->m_offset = 0;

	D3D11_BUFFER_DESC vbdesc;
	vbdesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbdesc.ByteWidth = MAX_PARTICLES*sizeof(VertexTypesStruct::VertexParticel);
	vbdesc.CPUAccessFlags = 0;
	vbdesc.MiscFlags = 0;
	vbdesc.Usage = D3D11_USAGE_DEFAULT;
	HR(AngelCore::AngelSubSystemResources::GraphicDeviceResources::Device
		->CreateBuffer(&vbdesc, NULL, &m_vertexBuffer));

	vbdesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	vbdesc.ByteWidth = MAX_PARTICLES*sizeof(DWORD);
	HR(AngelCore::AngelSubSystemResources::GraphicDeviceResources::Device->
		CreateBuffer(&vbdesc, NULL, &m_indexBuffer));

	m_cpuParticles = new VertexTypesStruct::VertexParticel[MAX_PARTICLES];

	for (UINT i = 0; i<MAX_PARTICLES; i++)
	{
		m_cpuParticles[i].Life = -1;	//kill all particles
	}

	m_cpuPartcilesIndices = new DWORD[MAX_PARTICLES];
	m_partriclesDepthArray = new float[MAX_PARTICLES];

	this->m_cbPerObjectBuffer.Load();
	//m_gradiantTexture = new Texture2D();
	//m_gradiantTexture->LoadTexture(L"colorgradient.dds",false);
	//m_particleTexture = new Texture2D();
	//m_particleTexture->LoadTexture(L"raindrop.dds",false);

	//We should load a texture.
	m_particleTexture = nullptr;

	//TwStructMember pointMembers[] = {
	//	{ "X", TW_TYPE_FLOAT, offsetof(Vector3, Vector3::m_x), " Min=-100 Max=100 Step=0.01 " },
	//	{ "Y", TW_TYPE_FLOAT, offsetof(Vector3, Vector3::m_y), " Min=-100 Max=100 Step=0.01 " },
	//{ "Z", TW_TYPE_FLOAT, offsetof(Vector3, Vector3::m_z), "	 Min=-100 Max=100 Step=0.01 " } };
	//TwType pointType = TwDefineStruct("POINT", pointMembers, 3, sizeof(Vector3), NULL, NULL);

	 m_fFadeDistance = 1.0f;
	 m_fParticleLifeSpan = 1.0f;
	 m_fEmitRate = 0.15f;

	 m_ParticleVel.x = -8.0f;
	 m_ParticleVel.y = -8.0f;
	 m_ParticleVel.z = -8.0f;
	 m_fParticleMaxSize = 0.25f;
	 m_fParticleMinSize = 0.0f;
	 m_LifeTime = 100.0f;

	 m_randomInXDirection = true;
	 m_randomInYDirection = true;
	 m_randomInZDirection = true;

	/* TwAddVarRW(bar, "MaxSize", TW_TYPE_FLOAT, &m_fParticleMaxSize, " Min=-100 Max=100 Step=0.01 ");
	 TwAddVarRW(bar, "Life Span", TW_TYPE_INT32, &m_fParticleLifeSpan, "");
	 TwAddVarRW(bar, "EmitRate", TW_TYPE_FLOAT, &m_fEmitRate, " Min=-100 Max=100 Step=0.01 ");
	 TwAddVarRW(bar, "LifeTime", TW_TYPE_FLOAT, &m_LifeTime, " Min=-100 Max=100 Step=0.01 ");
	 TwAddVarRW(bar, "Velocity", TW_TYPE_DIR3F, &m_ParticleVel, "");
	 
	 TwAddVarRW(bar, "MinSize", TW_TYPE_FLOAT, &m_fParticleMinSize, "");
	 TwAddVarRW(bar, "Position", pointType,
		 this->m_owner->GetComponent<AngelCore::AngelWorldSystem::Transform>()->GetPosition(), "");*/
}

bool  g_bAnimateParticles = true;

void AngelCore::AngelWorldSystem::ParticleRenderer::Update(
	const AngelCore::AngelSubSystem::TimeManager &timer)
{
	if (!m_active) return;
	DirectX::XMStoreFloat4x4(&this->m_cbPerObjectBuffer.Const.WVP
		,
		DirectX::XMMatrixTranspose
		(this->GetOwner()->GetComponent<AngelCore::AngelWorldSystem::Transform>()
			->GetWorldMatrix()*AngelCore::AngelSubSystemResources::BaseCameraProperties::BCamera->GetView()*
			AngelCore::AngelSubSystemResources::BaseCameraProperties::BCamera->GetProjection()));

	DirectX::XMStoreFloat4x4(&this->m_cbPerObjectBuffer.Const.World,
		DirectX::XMMatrixTranspose
		(this->GetOwner()->GetComponent<AngelCore::AngelWorldSystem::Transform>()
			->GetWorldMatrix()));

	DirectX::XMStoreFloat4x4(&this->m_cbPerObjectBuffer.Const.InvProjection,
		DirectX::XMMatrixTranspose(DirectX::XMMatrixInverse(
			&(DirectX::XMMatrixDeterminant(
				AngelCore::AngelSubSystemResources::BaseCameraProperties::BCamera->GetProjection()))
			, AngelCore::AngelSubSystemResources::BaseCameraProperties::BCamera->GetProjection())));

	DirectX::XMStoreFloat4x4(&this->m_cbPerObjectBuffer.Const.InvView,
		DirectX::XMMatrixTranspose(DirectX::XMMatrixInverse(
			&(DirectX::XMMatrixDeterminant(
				AngelCore::AngelSubSystemResources::BaseCameraProperties::BCamera->GetView()))
			, AngelCore::AngelSubSystemResources::BaseCameraProperties::BCamera->GetView())));

	//Emitt particles
	static double fLastEmitTime = 0;
	static UINT iLastParticleEmitted = 0;

	float fEmitRate = m_fEmitRate;
	float fParticleMaxSize = m_fParticleMaxSize;
	float fParticleMinSize = m_fParticleMinSize;
	UINT NumParticlesToEmit = (UINT)((timer.GetTotalSeconds() - fLastEmitTime) / fEmitRate);
	if (NumParticlesToEmit > 0)
	{
		for (UINT i = 0; i<NumParticlesToEmit; i++)
		{
			m_cpuParticles[iLastParticleEmitted].Position.x =
				this->m_owner->GetComponent<AngelCore::AngelWorldSystem::Transform>()->GetPosition()->GetX();
			m_cpuParticles[iLastParticleEmitted].Position.y = 
				this->m_owner->GetComponent<AngelCore::AngelWorldSystem::Transform>()->GetPosition()->GetY();
			m_cpuParticles[iLastParticleEmitted].Position.z = 
				this->m_owner->GetComponent<AngelCore::AngelWorldSystem::Transform>()->GetPosition()->GetZ();
			if(m_randomInXDirection)
				m_cpuParticles[iLastParticleEmitted].Velocity.x = -0.3f*(float)((rand() % 20000) - 10000) / 1000.0f;
			else
				m_cpuParticles[iLastParticleEmitted].Velocity.x = -0.3f;

			if (m_randomInYDirection)
				m_cpuParticles[iLastParticleEmitted].Velocity.y = -0.3f*(float)((rand() % 20000) - 10000) / 1000.0f;
			else
				m_cpuParticles[iLastParticleEmitted].Velocity.y = -0.3f;

			if (m_randomInZDirection)
				m_cpuParticles[iLastParticleEmitted].Velocity.z = -0.3f*(float)((rand() % 20000) - 10000) / 1000.0f;
			else
				m_cpuParticles[iLastParticleEmitted].Velocity.z = -0.3f;
			DirectX::XMStoreFloat3(&m_cpuParticles[iLastParticleEmitted].Velocity
				, DirectX::XMVector3Normalize(
					DirectX::XMVectorSet(m_cpuParticles[iLastParticleEmitted].Velocity.x
						, m_cpuParticles[iLastParticleEmitted].Velocity.y,
						m_cpuParticles[iLastParticleEmitted].Velocity.z, 1.0f)));
			m_cpuParticles[iLastParticleEmitted].Velocity.x *= m_ParticleVel.x;
			m_cpuParticles[iLastParticleEmitted].Velocity.y *= m_ParticleVel.y;
			m_cpuParticles[iLastParticleEmitted].Velocity.z *= m_ParticleVel.z;

			m_cpuParticles[iLastParticleEmitted].Life = 0.0f;
			m_cpuParticles[iLastParticleEmitted].Size = 0.0f;
			iLastParticleEmitted = (iLastParticleEmitted + 1) % MAX_PARTICLES;
		}

		fLastEmitTime = timer.GetTotalSeconds();
	}

	DirectX::XMFLOAT3 vel;
	float lifeSq = 0;
	for (UINT i = 0; i < MAX_PARTICLES; i++)
	{
		if (m_cpuParticles[i].Life > -1)
		{
			// squared velocity falloff
			lifeSq = m_cpuParticles[i].Life*m_cpuParticles[i].Life;

			// Slow down by 50% as we age
			vel.x = m_cpuParticles[i].Velocity.x ;
			vel.y = m_cpuParticles[i].Velocity.y ;
			vel.z = m_cpuParticles[i].Velocity.z ;
			//vel.y += 0.5f;	//(add some to the up direction, becuase of buoyancy)

			m_cpuParticles[i].Position.x += vel.x*timer.GetElapsedSeconds();
			m_cpuParticles[i].Position.y += vel.y*timer.GetElapsedSeconds();
			m_cpuParticles[i].Position.z += vel.z*timer.GetElapsedSeconds();
			m_cpuParticles[i].Life += timer.GetElapsedSeconds() / m_fParticleLifeSpan;
			m_cpuParticles[i].Size = fParticleMinSize +
				(fParticleMaxSize - fParticleMinSize);

			if (m_cpuParticles[i].Life > m_LifeTime)
				m_cpuParticles[i].Life = -1;
		}
	}

	for (UINT i = 0; i<MAX_PARTICLES; i++)
	{
		m_cpuPartcilesIndices[i] = i;
	}

	AngelCore::AngelSubSystemResources::GraphicDeviceResources::DeviceContext
		->UpdateSubresource(m_vertexBuffer.Get(), NULL, NULL, m_cpuParticles, 0, 0);
	AngelCore::AngelSubSystemResources::GraphicDeviceResources::DeviceContext
		->UpdateSubresource(m_indexBuffer.Get(), NULL, NULL, m_cpuPartcilesIndices, 0, 0);
}
void AngelCore::AngelWorldSystem::ParticleRenderer::Render()
{
	if (!m_active) return;
	float factor[4] = { 1.0f,1.0f,1.0f,1.0f };
	AngelCore::AngelSubSystemResources::GraphicDeviceResources::DeviceContext->OMSetBlendState(
		AngelCore::AngelSubSystemResources::GraphicDeviceResources::TransparentBlending.Get(),factor,
		0xfffffffff);
	AngelCore::AngelSubSystemResources::GraphicDeviceResources::DeviceContext->
		OMSetDepthStencilState(AngelCore::AngelSubSystemResources::GraphicDeviceResources::DisableDepth.Get(), 0);
	m_cbPerObjectBuffer.Update();
	this->m_shader->SetConstantBuffer(0, 1, m_cbPerObjectBuffer.getBuffer());

	//this->m_shader->BindSRV(0, 1, m_gradiantTexture->GetSRV().Get(), ShaderType::PixelShader);
	//this->m_shader->BindSRV(0, 1, m_gradiantTexture->GetSRV().Get(), ShaderType::GeometryShader);

	if (m_particleTexture)
	{
		this->m_shader->BindSRV(1, 1, m_particleTexture->GetSRV().Get(), ShaderType::PixelShader);
		this->m_shader->BindSRV(1, 1, m_particleTexture->GetSRV().Get(), ShaderType::GeometryShader);
	}
	this->m_shader->
		BindSRV(2, 1,
			AngelRendering::PostProcess::GetInstance()->GetDepthStencilViewSRV().Get()
			, ShaderType::PixelShader);
	this->m_shader->
		BindSRV(2, 1,
			AngelRendering::PostProcess::GetInstance()->GetDepthStencilViewSRV().Get(),
			ShaderType::GeometryShader);

	this->m_shader->SetSampler(0, 1,
		AngelCore::AngelSubSystemResources::GraphicDeviceResources::Samplers.at(0).GetAddressOf());
	this->m_shader->SetSampler(1, 1,
		AngelCore::AngelSubSystemResources::GraphicDeviceResources::Samplers.at(3).GetAddressOf());

	UINT stride = sizeof(VertexTypesStruct::VertexParticel);
	UINT offset = 0;
	this->m_shader->Apply();
	AngelCore::AngelSubSystemResources::GraphicDeviceResources::DeviceContext
		->IASetVertexBuffers(0, 1, m_vertexBuffer.GetAddressOf(), &stride, &offset);

	AngelCore::AngelSubSystemResources::GraphicDeviceResources::DeviceContext
		->IASetIndexBuffer(m_indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);

	AngelCore::AngelSubSystemResources::GraphicDeviceResources::DeviceContext->IASetPrimitiveTopology(
		D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

	AngelCore::AngelSubSystemResources::GraphicDeviceResources::DeviceContext->DrawIndexed(MAX_PARTICLES, 0, 0);


	this->m_shader->UnBindSRV(0, 1, ShaderType::PixelShader);
	this->m_shader->UnBindSRV(1, 1, ShaderType::PixelShader);
	this->m_shader->UnBindSRV(2, 1, ShaderType::PixelShader);
	this->m_shader->UnBindSRV(0, 1, ShaderType::GeometryShader);
	this->m_shader->UnBindSRV(1, 1, ShaderType::GeometryShader);
	this->m_shader->UnBindSRV(2, 1, ShaderType::GeometryShader);
	this->m_shader->UnBindSampler(3, 1);
	this->m_shader->UnBindSampler(0, 1);
	this->m_shader->Disable();


	AngelCore::AngelSubSystemResources::GraphicDeviceResources::DeviceContext->
		OMSetDepthStencilState(nullptr, 0);
}

void AngelCore::AngelWorldSystem::ParticleRenderer::LoadParticleTexture(std::string &name)
{
	std::wstring nameW(name.begin(), name.end());
	
	if (m_particleTexture)
	{
		delete m_particleTexture;
		m_particleTexture = nullptr;
	}
	m_particleTexture = new Texture2D();
	m_particleTexture->LoadTexture(nameW.c_str(), false);
}