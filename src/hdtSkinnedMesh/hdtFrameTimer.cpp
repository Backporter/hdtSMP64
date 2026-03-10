#include "hdtFrameTimer.h"

namespace hdt
{
	FrameTimer* FrameTimer::instance()
	{
		static FrameTimer s_instance;
		return &s_instance;
	}

	void FrameTimer::reset(int nFrames)
	{
		m_nFrames = nFrames;
		m_count = nFrames / 2;
		m_sumsCPU.clear();
		m_sumsSquaredCPU.clear();
		m_sumsGPU.clear();
		m_sumsSquaredGPU.clear();
		m_nManifoldsCPU = 0;
		m_nManifolds2CPU = 0;
		m_nManifoldsGPU = 0;
		m_nManifolds2GPU = 0;
	}

	void FrameTimer::logEvent(FrameTimer::Events e)
	{
		if (!running())
		{
			return;
		}

		LARGE_INTEGER ticks;
		QueryPerformanceCounter(&ticks);
		m_timings[e] = ticks.QuadPart;

		if (e == kEnd)
		{
			QueryPerformanceFrequency(&ticks);
			float ticks_per_us = static_cast<float>(ticks.QuadPart) / 1e6f;
			float internalTime = (m_timings[kInternal] - m_timings[kStart]) / ticks_per_us;
			float collisionLaunchTime = (m_timings[kLaunched] - m_timings[kInternal]) / ticks_per_us;
			float collisionProcessTime = (m_timings[kEnd] - m_timings[kLaunched]) / ticks_per_us;
			float totalTime = (m_timings[kEnd] - m_timings[kStart]) / ticks_per_us;

			if (cudaFrame())
			{
				m_sumsGPU[kInternalUpdate] += internalTime;
				m_sumsSquaredGPU[kInternalUpdate] += internalTime * internalTime;
				m_sumsGPU[kCollisionLaunch] += collisionLaunchTime;
				m_sumsSquaredGPU[kCollisionLaunch] += collisionLaunchTime * collisionLaunchTime;
				m_sumsGPU[kCollisionProcess] += collisionProcessTime;
				m_sumsSquaredGPU[kCollisionProcess] += collisionProcessTime * collisionProcessTime;
				m_sumsGPU[kTotal] += totalTime;
				m_sumsSquaredGPU[kTotal] += totalTime * totalTime;
			}
			else
			{
				m_sumsCPU[kInternalUpdate] += internalTime;
				m_sumsSquaredCPU[kInternalUpdate] += internalTime * internalTime;
				m_sumsCPU[kCollisionLaunch] += collisionLaunchTime;
				m_sumsSquaredCPU[kCollisionLaunch] += collisionLaunchTime * collisionLaunchTime;
				m_sumsCPU[kCollisionProcess] += collisionProcessTime;
				m_sumsSquaredCPU[kCollisionProcess] += collisionProcessTime * collisionProcessTime;
				m_sumsCPU[kTotal] += totalTime;
				m_sumsSquaredCPU[kTotal] += totalTime * totalTime;
			}

			if (--m_nFrames == 0)
			{
				RE::ConsoleLog* log = RE::ConsoleLog::GetSingleton();
				
				log->Print("Timings over %d frames:", m_count);
				log->Print("  CPU:");
				float mean = m_sumsCPU[kInternalUpdate] / m_count;
				log->Print("    Internal update mean %f us, std %f us",
					mean,
					sqrt(m_sumsSquaredCPU[kInternalUpdate] / m_count - mean * mean));
				mean = m_sumsCPU[kCollisionLaunch] / m_count;
				log->Print("    Collision launch mean %f us, std %f us",
					mean,
					sqrt(m_sumsSquaredCPU[kCollisionLaunch] / m_count - mean * mean));
				mean = m_sumsCPU[kCollisionProcess] / m_count;
				log->Print("    Collision process mean %f us, std %f us",
					mean,
					sqrt(m_sumsSquaredCPU[kCollisionProcess] / m_count - mean * mean));
				mean = m_sumsCPU[kTotal] / m_count;
				log->Print("    Total mean %f us, std %f us",
					mean,
					sqrt(m_sumsSquaredCPU[kTotal] / m_count - mean * mean));
				mean = static_cast<float>(m_nManifoldsCPU / m_count);
				log->Print("    Collision manifolds %f, std %f",
					mean,
					sqrt(m_nManifolds2CPU / m_count - mean * mean));

				log->Print("  GPU:");
				mean = m_sumsGPU[kInternalUpdate] / m_count;
				log->Print("    Internal update mean %f us, std %f us",
					mean,
					sqrt(m_sumsSquaredGPU[kInternalUpdate] / m_count - mean * mean));
				mean = m_sumsGPU[kCollisionLaunch] / m_count;
				log->Print("    Collision launch mean %f us, std %f us",
					mean,
					sqrt(m_sumsSquaredGPU[kCollisionLaunch] / m_count - mean * mean));
				mean = m_sumsGPU[kCollisionProcess] / m_count;
				log->Print("    Collision process mean %f us, std %f us",
					mean,
					sqrt(m_sumsSquaredGPU[kCollisionProcess] / m_count - mean * mean));
				mean = m_sumsGPU[kTotal] / m_count;
				log->Print("    Total mean %f us, std %f us",
					mean,
					sqrt(m_sumsSquaredGPU[kTotal] / m_count - mean * mean));
				mean = static_cast<float>(m_nManifoldsGPU / m_count);
				log->Print("    Collision manifolds %f, std %f",
					mean,
					sqrt(m_nManifolds2GPU / m_count - mean * mean));
			}
		}
	}

	void FrameTimer::addManifoldCount(int nManifolds)
	{
		if (cudaFrame())
		{
			m_nManifoldsGPU += nManifolds;
			m_nManifolds2GPU += nManifolds * nManifolds;
		}
		else
		{
			m_nManifoldsCPU += nManifolds;
			m_nManifolds2CPU += nManifolds * nManifolds;
		}
	}

	bool FrameTimer::running()
	{
		return m_nFrames > 0;
	}

	bool FrameTimer::cudaFrame()
	{
		return m_nFrames > m_count;
	}
}
