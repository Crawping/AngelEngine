#pragma once
#include"IComponent.h"
#include"MeshRenderer.h"
#ifndef _SCRIPT_MANAGER
#include"../../../Public/ScriptManager.h"
#endif
#define __LightRenderer
namespace AngelCore
{
	class AngelSubSystem::ScriptManager;

	namespace AngelWorldSystem
	{
		class LightRenderer : public IComponent
		{
			friend class AngelSubSystem::ScriptManager;
			friend class Actor;
		public:

			LightRenderer();
			LightRenderer(const LightRenderer &that);
			LightRenderer(LightRenderer &&that);
			virtual ~LightRenderer();

			LightRenderer &operator =(const LightRenderer &that);
			LightRenderer &operator =(LightRenderer &&that);

		public:
			DirectX::XMFLOAT3 GetPosition() const { return m_PositionWS; }
			int GetCastShadow() const { return m_castShadow; }
			DirectX::XMFLOAT4 GetDirection() const { return m_DirectionWS; }
			DirectX::XMFLOAT4 GetColor() const { return m_Color; }
			float GetSpotLightAngle() const { return m_SpotlightAngle; }
			float GetRange() const { return m_Range; }
			float GetIntensity() const { return m_Intensity; }
			bool GetEnable() const { return m_Enabled; }
			UINT GetType() const { return m_Type; }
			DirectX::XMFLOAT3 GetCameraPosition() const {return m_CameraPosition;}
			UINT GetIndex() const { return m_index; }

			void SetCastShadow(int c) { this->m_castShadow = c; }
			void SetCastShadow(int &c) { this->m_castShadow = c; }
			void SetColor(float r, float g, float b, float a) { this->m_Color = DirectX::XMFLOAT4(r, g, b, a); }
			void SetColor(float &r, float &g, float &b, float &a) { this->m_Color = DirectX::XMFLOAT4(r, g, b, a); }
			void SetColor(DirectX::XMFLOAT4 color) { this->m_Color = color; }
			void SetType(int type) { this->m_Type = type; }
			void SetType(int &type) { this->m_Type = type; }
			void SetEnable(bool b) { this->m_Enabled = b; }
			void SetEnable(bool &b) { this->m_Enabled = b; }
			void SetRange(float range) { this->m_Range = range; }
			void SetRange(float &range) { this->m_Range = range; }
			void SetIntensity(float intensity) { this->m_Intensity = intensity; }
			void SetIntensity(float &intensity) { this->m_Intensity = intensity; }
			void SetSpotLightAngle(float a) { this->m_SpotlightAngle = a; }
			void SetSpotLightAngle(float &a) { this->m_SpotlightAngle = a; }

		public:

			virtual void Initialize(Actor* _owner, ...) override;

			virtual void Update(const AngelSubSystem::TimeManager &timer) override;

			virtual void Render() override;

		private:
			DirectX::XMFLOAT3   m_PositionWS;
			int					m_castShadow;
			DirectX::XMFLOAT4   m_DirectionWS;
			DirectX::XMFLOAT4   m_Color;
			float    m_SpotlightAngle;
			float    m_Range;
			float    m_Intensity;
			bool			m_Enabled;
			unsigned int		m_Type;
			DirectX::XMFLOAT3  m_CameraPosition;
			//index in array of lights.
			UINT				m_index;

		};
	}
}