#pragma once

namespace hdt
{
	class FrameTimer
	{
	public:

		static FrameTimer* instance();

		enum Events
		{
			kStart,
			kInternal,
			kLaunched,
			kEnd
		};

		enum Measurements
		{
			kInternalUpdate,
			kCollisionLaunch,
			kCollisionProcess,
			kTotal
		};

		void reset(int nFrames);
		void addManifoldCount(int nManifolds);
		void logEvent(Events e);

		bool running();
		bool cudaFrame();

	private:

		int m_nFrames;
		std::map<Events, int64_t> m_timings;

		int m_count;
		std::map<Measurements, float> m_sumsCPU;
		std::map<Measurements, float> m_sumsSquaredCPU;
		std::map<Measurements, float> m_sumsGPU;
		std::map<Measurements, float> m_sumsSquaredGPU;

		int m_nManifoldsCPU;
		int m_nManifolds2CPU;
		int m_nManifoldsGPU;
		int m_nManifolds2GPU;
	};
}
