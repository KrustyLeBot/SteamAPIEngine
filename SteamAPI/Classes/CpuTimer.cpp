#include "CpuTimer.h"

CpuTimer::CpuTimer()
	: m_isStarted(false)
{
	Reset();
}

CpuTimer::~CpuTimer()
{
}

bool CpuTimer::IsStarted()
{
	return m_isStarted;
}

void CpuTimer::Start()
{
	m_startTime = std::chrono::system_clock::now();
	m_isStarted = true;
}

void CpuTimer::Stop()
{
	m_stopTime = std::chrono::system_clock::now();
	m_isStarted = false;
}

void CpuTimer::Reset()
{
	Stop();
	m_startTime = m_stopTime;
}

uint64_t CpuTimer::GetElapsedTimeMs()
{
	if (m_isStarted)
	{
		return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count() - std::chrono::duration_cast<std::chrono::milliseconds>(m_startTime.time_since_epoch()).count();
	}
	else
	{
		return std::chrono::duration_cast<std::chrono::milliseconds>(m_stopTime.time_since_epoch()).count() - std::chrono::duration_cast<std::chrono::milliseconds>(m_startTime.time_since_epoch()).count();
	}
}
