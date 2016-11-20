#pragma once

#include"SubSystemBase.h"
#include"../AngelRendering/Public/SkyMap.h"
#include<time.h>
namespace AngelCore
{
	namespace AngelSubSystem
	{
		using CallBackFunction = void(*)(float,float,float,ID3D11ShaderResourceView *);

		class WorldManager : public ISubSystem
		{
			enum WEATHER_CONDITIONS
			{
				DAY,
				NIGHT,
				SUNRISE,
				SUNSET
			};
			//refers to time of day or weather condition
			struct Condition
			{
				const wchar_t * name;
				WEATHER_CONDITIONS condition;
				Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> srv;
			};
			class Clock
			{
			public:
				enum CLOCK_TIME_TYPE { HOUR, MINUTE, SECONDS };
				enum TIME_SCALE_TYPE { FAST = 0, SLOW, NORMAL };
			public:
				static Clock * GetInstance();
				static void Start();
				static void Update();
				static void  PrintTime(int time);
				static void SetTimeScale(float _amount);
				static int GetCurrentClockTime() { return m_currentClockTime; }
				static int GetTotalElapsedTime() { return m_totalElapsedTime; }
				static int GetTimeScale() { return m_timeScale; }
				static int GetTimeByHMS(int H, int M, int S);
				static bool IsCurrentTimeBetween(int timeStart, int timeEnd);
			private:
				Clock();
				~Clock();
				static Clock *			m_clockInstance;
				static int				m_time;
				static int				m_timeScale;
				static bool				m_isStarted;
				static HWND				m_hwnd;
				static float			m_elapsedTimeLastEvent;
				static int				m_totalElapsedTime;
				static int				m_nextEventTimer;
				static float			m_elapsedTimeLog;
				static CallBackFunction	m_func;
				static int				m_currentClockTime;
				static bool				m_isFinished;
				static clock_t			m_now;
				static clock_t			m_end;

			private:
				static void FireEvent();
			};
		public:
			bool StartUp(...) override;
			bool ShutDown() override;

			void Update();
			void Render();


		public:
			void SetSkyMap(AngelRendering::SkyMap * skymap) { this->m_skyMap = skymap; }
			void SetDynamicWeather(bool &b) { if (b == true) m_dynamicWeather = true;else m_dynamicWeather = false; }
			void SetDynamicWeather(bool b) { if (b == true) m_dynamicWeather = true;else m_dynamicWeather = false; }
			void SetDynamicClouds(bool &b) { if (b == true) m_dynamicClouds = true;else m_dynamicClouds = false; m_skyMap->SetDynamicClouds(m_dynamicClouds); }
			void SetDynamicClouds(bool b) { if (b == true) m_dynamicClouds = true;else m_dynamicClouds = false; m_skyMap->SetDynamicClouds(m_dynamicClouds); }
			void LoadStaticSkyMap(std::string &in);

			bool GetDynamicWeather() const	{ return m_dynamicWeather; }
			int GetCurrentTime() const	{ return Clock::GetCurrentClockTime(); }
			void SetTimeScale(float &scale);

			float GetTimeScale() const { return Clock::GetTimeScale(); }

		private:
			AngelRendering::SkyMap *	m_skyMap;
			bool						m_dynamicWeather;
			bool						m_dynamicClouds;
			//store all possible weather conditions.
			std::vector<Condition>		m_weatherConditions;
		};
	}
}