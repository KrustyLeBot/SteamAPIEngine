#pragma once
#include <chrono>

class CpuTimer
{
public:
	CpuTimer();
	~CpuTimer();

	bool IsStarted();

	void Start();
	void Stop();
	void Reset();

	uint64_t  GetElapsedTimeMs();

private:
	bool m_isStarted;
	std::chrono::time_point<std::chrono::system_clock> m_startTime;
	std::chrono::time_point<std::chrono::system_clock> m_stopTime;
};