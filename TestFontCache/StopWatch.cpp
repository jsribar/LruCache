#include "stdafx.h"
#include "Stopwatch.h"

Stopwatch::Stopwatch()
{
	m_startPoint = std::chrono::high_resolution_clock::now();
}

Stopwatch::~Stopwatch()
{
}

void Stopwatch::Restart()
{
	m_startPoint = std::chrono::high_resolution_clock::now();
}

std::chrono::duration<double> Stopwatch::Stop()
{
	auto endPoint = std::chrono::high_resolution_clock::now();
	return std::chrono::duration_cast<std::chrono::duration<double>>(endPoint - m_startPoint);
}
