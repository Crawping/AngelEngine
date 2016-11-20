#include "../Public/WorldManager.h"
#include<future>

#define DEFAULT_TIME_SCALE 1000
#define NIGHT_START_TIME 72000
#define NIGHT_END_TIME 25200

AngelCore::AngelSubSystem::WorldManager::Clock *
AngelCore::AngelSubSystem::WorldManager::Clock::m_clockInstance = nullptr;
int AngelCore::AngelSubSystem::WorldManager::Clock::m_time = 0;
int AngelCore::AngelSubSystem::WorldManager::Clock::m_timeScale = 1000;
bool AngelCore::AngelSubSystem::WorldManager::Clock::m_isStarted = false;
HWND AngelCore::AngelSubSystem::WorldManager::Clock::m_hwnd = nullptr;
float AngelCore::AngelSubSystem::WorldManager::Clock::m_elapsedTimeLastEvent = 0;
int AngelCore::AngelSubSystem::WorldManager::Clock::m_nextEventTimer = 0;
AngelCore::AngelSubSystem::CallBackFunction AngelCore::AngelSubSystem::WorldManager::Clock::m_func;
int AngelCore::AngelSubSystem::WorldManager::Clock::m_totalElapsedTime = 0.0f;
float AngelCore::AngelSubSystem::WorldManager::Clock::m_elapsedTimeLog = 0.0f;
int AngelCore::AngelSubSystem::WorldManager::Clock::m_currentClockTime = 0.0f;
clock_t AngelCore::AngelSubSystem::WorldManager::Clock::m_now = 0.0f;
clock_t AngelCore::AngelSubSystem::WorldManager::Clock::m_end = 0.0f;

bool AngelCore::AngelSubSystem::WorldManager::StartUp(...)
{
	this->m_skyMap = new AngelCore::AngelRendering::SkyMap();
	this->m_skyMap->Initialize();
	Clock::GetInstance()->SetTimeScale(0.1*CLOCKS_PER_SEC);
	Clock::GetInstance()->Start();
	m_dynamicWeather = false;
	m_dynamicClouds = false;
	Condition c;
	c.condition = WEATHER_CONDITIONS::DAY;
	c.name = L"BlueSky.dds";
	Texture2D tex;
	tex.LoadTexture(c.name, false);
	c.srv = tex.GetSRV();
	m_weatherConditions.push_back(c);
	c.condition = WEATHER_CONDITIONS::NIGHT;
	c.name = L"Night.dds";
	tex.LoadTexture(c.name, false);
	c.srv = tex.GetSRV();
	m_weatherConditions.push_back(c);
	c.condition = WEATHER_CONDITIONS::SUNRISE;
	c.name = L"BlueSky.dds";
	tex.LoadTexture(c.name, false);
	c.srv = tex.GetSRV();
	m_weatherConditions.push_back(c);
	c.condition = WEATHER_CONDITIONS::SUNSET;
	c.name = L"BlueSky.dds";
	tex.LoadTexture(c.name,false);
	c.srv = tex.GetSRV();
	m_weatherConditions.push_back(c);
	return true;
}


bool AngelCore::AngelSubSystem::WorldManager::ShutDown()
{
	return true;
}

void AngelCore::AngelSubSystem::WorldManager::Update()
{	
	Clock::GetInstance()->Update();
	this->m_skyMap->Update();

	if (m_dynamicWeather)
	{
		//Handle dynamic weather and daynight

		if (Clock::IsCurrentTimeBetween(Clock::GetTimeByHMS(0, 0, 0), Clock::GetTimeByHMS(5, 0, 0)))
		{
			m_skyMap->SetCurrentSkyMapTexture(m_weatherConditions[NIGHT].srv.Get());
			float blendAmount = (float)Clock::GetCurrentClockTime() / (float)Clock::GetTimeByHMS(5, 0, 0);
			m_skyMap->ChangeApexColor(DirectX::XMFLOAT4(0, 0, 0, 0), 1.0f);
			m_skyMap->ChangeCenterColor(DirectX::XMFLOAT4(0, 0, 0, 0), 1.0f);
		}
		if (Clock::IsCurrentTimeBetween(Clock::GetTimeByHMS(6, 0, 0), Clock::GetTimeByHMS(7, 0, 0)))
		{
			float blendAmount = ((float)Clock::GetCurrentClockTime() - (float)Clock::GetTimeByHMS(6, 0, 0))
				/ ((float)Clock::GetTimeByHMS(7, 0, 0) - (float)Clock::GetTimeByHMS(6, 0, 0));
			m_skyMap->ChangeApexColor(DirectX::XMFLOAT4(0.15f, 0.15f, 0.66f, 1.0f), blendAmount);
			m_skyMap->ChangeCenterColor(DirectX::XMFLOAT4(0.81f, 0.68f, 0.76f, 1.0f), blendAmount);
			m_skyMap->SetCurrentSkyMapTexture(m_weatherConditions[DAY].srv.Get());

		}
		if (Clock::IsCurrentTimeBetween(Clock::GetTimeByHMS(5, 0, 0), Clock::GetTimeByHMS(6, 0, 0)))
		{
			float blendAmount = ((float)Clock::GetCurrentClockTime() - (float)Clock::GetTimeByHMS(5, 0, 0))
				/ ((float)Clock::GetTimeByHMS(6, 0, 0) - (float)Clock::GetTimeByHMS(5, 0, 0));
			m_skyMap->ChangeApexColor(DirectX::XMFLOAT4(0.0f, 0.15f, 0.66f, 1.0f), blendAmount);
			m_skyMap->ChangeCenterColor(DirectX::XMFLOAT4(0.81f, 0.38f, 0.66f, 1.0f), blendAmount);
			m_skyMap->SetCurrentSkyMapTexture(m_weatherConditions[NIGHT].srv.Get());
			m_skyMap->SetNextTextureBlend(blendAmount, (m_weatherConditions[SUNRISE].srv.Get()));

		}
		if (Clock::IsCurrentTimeBetween(Clock::GetTimeByHMS(18, 0, 0), Clock::GetTimeByHMS(19, 0, 0)))
		{
			float blendAmount = ((float)Clock::GetCurrentClockTime() - (float)Clock::GetTimeByHMS(18, 0, 0))
				/ ((float)Clock::GetTimeByHMS(19, 0, 0) - (float)Clock::GetTimeByHMS(18, 0, 0));
			m_skyMap->ChangeApexColor(DirectX::XMFLOAT4(0.0f, 0.15f, 0.66f, 1.0f), blendAmount);
			m_skyMap->ChangeCenterColor(DirectX::XMFLOAT4(0.81f, 0.38f, 0.66f, 1.0f), blendAmount);
			m_skyMap->SetCurrentSkyMapTexture(m_weatherConditions[DAY].srv.Get());
			m_skyMap->SetNextTextureBlend(blendAmount, (m_weatherConditions[SUNSET].srv.Get()));

		}
		if (Clock::IsCurrentTimeBetween(Clock::GetTimeByHMS(19, 0, 0), Clock::GetTimeByHMS(20, 0, 0)))
		{
			float blendAmount = ((float)Clock::GetCurrentClockTime() - (float)Clock::GetTimeByHMS(19, 0, 0))
				/ ((float)Clock::GetTimeByHMS(20, 0, 0) - (float)Clock::GetTimeByHMS(19, 0, 0));
			m_skyMap->ChangeApexColor(DirectX::XMFLOAT4(0, 0, 0, 0), blendAmount);
			m_skyMap->ChangeCenterColor(DirectX::XMFLOAT4(0, 0, 0, 0), blendAmount);
			m_skyMap->SetCurrentSkyMapTexture(m_weatherConditions[SUNRISE].srv.Get());
			m_skyMap->SetNextTextureBlend(blendAmount, (m_weatherConditions[NIGHT].srv.Get()));
		}
	}
}

void AngelCore::AngelSubSystem::WorldManager::Render()
{
	this->m_skyMap->Render();
}

AngelCore::AngelSubSystem::WorldManager::Clock::Clock()
{

}

AngelCore::AngelSubSystem::WorldManager::Clock::~Clock()
{
	if (m_clockInstance)
		delete m_clockInstance;
}

AngelCore::AngelSubSystem::WorldManager::Clock * AngelCore::AngelSubSystem::WorldManager::Clock::GetInstance()
{
	if (!m_clockInstance)
	{
		m_clockInstance = new AngelCore::AngelSubSystem::WorldManager::Clock();
		return m_clockInstance;
	}
	return m_clockInstance;
}


void AngelCore::AngelSubSystem::WorldManager::Clock::Start()
{
	m_totalElapsedTime = Clock::GetTimeByHMS(0, 0, 0);
	m_currentClockTime = Clock::GetTimeByHMS(0, 0, 0);
	m_isStarted = true;
	m_now = clock();
	m_end = m_now + m_timeScale;
	
}
void AngelCore::AngelSubSystem::WorldManager::Clock::Update()
{
	m_now = clock();
	if (m_now > m_end)
	{
		m_totalElapsedTime += CLOCKS_PER_SEC /m_timeScale;
		m_currentClockTime += CLOCKS_PER_SEC / m_timeScale;
		m_end = m_now + m_timeScale;
		//For 24H clock.
		m_currentClockTime %= 86400;
#ifdef _DEBUG
		PrintTime(m_currentClockTime);
#endif
	}
}

void AngelCore::AngelSubSystem::WorldManager::Clock::SetTimeScale(float _amount)
{
	m_timeScale = _amount;
}
void AngelCore::AngelSubSystem::WorldManager::SetTimeScale(float &amount)
{
	Clock::SetTimeScale(amount*CLOCKS_PER_SEC);
}

void AngelCore::AngelSubSystem::WorldManager::Clock::PrintTime(int time)
{
	if (!m_isStarted) return;
	std::string ret;
	int tTime = time;
	//HOUR
	if (tTime / 3600 > 0)
	{

		ret += std::to_string(tTime / 3600);
		tTime = tTime % 3600;
	}
	else
	{
		ret += "00";
	}
	ret += ":";

	//MINUTE
	if (tTime / 60 > 0)
	{

		ret += std::to_string(tTime / 60);
		tTime = tTime % 60;
	}
	else
	{
		ret += "00";
	}
	ret += ":";

	//SECOND
	ret += std::to_string(tTime);


	std::string dataS = ret;
	std::wstring data(dataS.begin(), dataS.end());
	OutputDebugStringA("\n");
	OutputDebugStringW(data.c_str());
}

int AngelCore::AngelSubSystem::WorldManager::Clock::GetTimeByHMS(int H, int M, int S)
{
	int ret = H * 3600;
	ret += M * 60;
	ret += S;
	return ret;
}

bool AngelCore::AngelSubSystem::WorldManager::Clock::IsCurrentTimeBetween(int timeStart, int timeEnd)
{
	if (m_currentClockTime<timeEnd && m_currentClockTime>timeStart)
		return true;
	return false;
}

void AngelCore::AngelSubSystem::WorldManager::LoadStaticSkyMap(std::string &name)
{
	std::wstring nameW(name.begin(), name.end());
	Texture2D txt;
	txt.LoadTexture(nameW.c_str(), false);
	this->m_skyMap->SetCurrentSkyMapTexture(txt.GetSRV().Get());
	//automaticly set to false if we load static skymap.
	SetDynamicWeather(false);
	SetDynamicClouds(false);
}