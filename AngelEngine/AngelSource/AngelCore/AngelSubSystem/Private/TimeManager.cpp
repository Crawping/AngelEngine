#include"../Public/TimeManager.h"

bool AngelCore::AngelSubSystem::TimeManager::StartUp(...)
{
	if (!QueryPerformanceFrequency(&this->m_qpcFrequency))
	{
		return false;
	}

	if (!QueryPerformanceFrequency(&this->m_qpcLastTime))
	{
		return false;
	}

	this->m_maxDeltaTime = this->m_qpcFrequency.QuadPart / 10;
	this->m_elapsedTicks = 0;
	this->m_frameCount = 0;
	this->m_framesPerSecond = 0;
	this->m_framesThisSecond = 0;
	this->m_isFixedTimeStep = false;
	this->m_leftOverTicks = 0;
	this->m_qpcSecondCounter = 0;
	this->m_targetElapsedTicks = 0;
	this->m_totalTicks = 0;
	this->m_paused = false;

	return true;
}

bool AngelCore::AngelSubSystem::TimeManager::ShutDown()
{
	return true;
}