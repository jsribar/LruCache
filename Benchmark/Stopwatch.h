#pragma once

#include <chrono>

class Stopwatch
{
public:
	Stopwatch();
	~Stopwatch();

	void Restart();
	std::chrono::duration<double> Stop();

	std::chrono::high_resolution_clock::time_point m_startPoint;
};
