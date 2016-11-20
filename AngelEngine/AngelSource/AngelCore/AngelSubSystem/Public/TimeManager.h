#pragma once

#include"SubSystemBase.h"

namespace AngelCore
{
	namespace AngelSubSystem
	{
		class TimeManager : public ISubSystem
		{
		public:
			bool StartUp(...) override;

			bool ShutDown() override;

		public:

			static const I64 TicksPerSecond = 10000000;

			template<typename TUpdate>
			void Tick(const TUpdate &update)
			{
				if (m_paused)
					return;
				LARGE_INTEGER  curTime;
				if (!QueryPerformanceCounter(&curTime))
					return;
				I64 timeDelta = curTime.QuadPart - this->m_qpcLastTime.QuadPart;

				this->m_qpcLastTime = curTime;

				this->m_qpcSecondCounter += timeDelta;

				//Clamp large time deltas
				if (timeDelta > this->m_maxDeltaTime)
				{
					timeDelta = this->m_maxDeltaTime;
				}

				timeDelta *= TicksPerSecond;
				timeDelta /= this->m_qpcFrequency.QuadPart;

				I64 lastFrameCount = static_cast<I64>(this->m_frameCount);

				if (this->m_isFixedTimeStep)
				{
					if (abs(static_cast<INT64>(timeDelta - m_targetElapsedTicks)) < TicksPerSecond / 4000)
					{
						timeDelta = m_targetElapsedTicks;
					}
					this->m_leftOverTicks += timeDelta;

					while (this->m_leftOverTicks >= m_targetElapsedTicks)
					{
						m_elapsedTicks = m_targetElapsedTicks;
						m_totalTicks += m_targetElapsedTicks;
						m_leftOverTicks -= m_targetElapsedTicks;
						m_frameCount++;

						update();
					}
				}
				else
				{
					m_elapsedTicks = timeDelta;
					m_totalTicks += timeDelta;
					m_leftOverTicks = 0;
					m_frameCount++;

					update();
				}

				if (lastFrameCount != this->m_frameCount)
				{
					this->m_framesThisSecond++;
				}

				if (m_qpcSecondCounter >= static_cast<I64>(m_qpcFrequency.QuadPart))
				{
					m_framesPerSecond = m_framesThisSecond;
					m_framesThisSecond = 0;
					m_qpcSecondCounter %= m_qpcFrequency.QuadPart;
				}
			}

		public:
			// Get elapsed time since the previous Update call.
			I64 GetElapsedTicks() const												{ return m_elapsedTicks; }
			double GetElapsedSeconds() const										{ return TicksToSeconds(m_elapsedTicks); }

			// Get total time since the start of the program.
			I64 GetTotalTicks() const												{ return m_totalTicks; }
			double GetTotalSeconds() const											{ return TicksToSeconds(m_totalTicks); }

			// Get total number of updates since start of the program.
			double GetFrameCount() const											{ return m_frameCount; }

			// Get the current framerate.
			double GetFramesPerSecond() const										{ return m_framesPerSecond; }

			// Set whether to use fixed or variable timestep mode.
			void SetFixedTimeStep(bool isFixedTimestep)								{ m_isFixedTimeStep = isFixedTimestep; }

			// Set how often to call Update when in fixed timestep mode.
			void SetTargetElapsedTicks(I64 targetElapsed)							{ m_targetElapsedTicks = targetElapsed; }
			void SetTargetElapsedSeconds(double targetElapsed)						{ m_targetElapsedTicks = SecondsToTicks(targetElapsed); }

			static double TicksToSeconds(I64 ticks)									{ return static_cast<double>(ticks) / TicksPerSecond; }
			static I64 SecondsToTicks(double seconds)								{ return static_cast<I64>(seconds * TicksPerSecond); }

			void SetPause(bool & b) { if (b == true) m_paused = true;else m_paused = false; }
			void SetPause(bool b)	{ this->m_paused = b; }
			bool GetPause() const	{ return m_paused; }


		private:
			LARGE_INTEGER 								m_qpcFrequency;
			LARGE_INTEGER 								m_qpcLastTime;
			I64											m_maxDeltaTime;
			I64											m_elapsedTicks;
			I64											m_totalTicks;
			I64											m_leftOverTicks;
			double										m_frameCount;
			double										m_framesPerSecond;
			double										m_framesThisSecond;
			I64											m_qpcSecondCounter;
			bool										m_isFixedTimeStep;
			I64											m_targetElapsedTicks;
			bool										m_paused;
			
		};	
	}
}
