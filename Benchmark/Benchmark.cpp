// Benchmark.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <random>
#include <iostream>
#include <fstream>

#include "..\LruCache\LruCache.h"
#include "..\LruCache\LruCacheWithDeque.h"
#include "StopWatch.h"

class RandomIntGenerator
{
public:
	RandomIntGenerator(int lowerLimit, int upperLimit)
		: m_eng{ m_rd() }
		, m_distr{ lowerLimit, upperLimit }
	{
	}

	int operator() ()
	{
		return m_distr(m_eng);
	}

private:
	std::random_device m_rd;
	std::mt19937 m_eng;
	std::uniform_int_distribution<> m_distr;
};

double TestForIntegerKeyUnorderedMap(size_t cacheSize, size_t uniqueRequests, size_t totalRequests)
{
	RandomIntGenerator rg(1, uniqueRequests - 1);

	auto generator = [](const int& a) -> int { return a * a; };
	LruCache<int, int, decltype(generator)> cache{ generator, cacheSize };

	Stopwatch sw;
	for (size_t i = 0; i < totalRequests; ++i)
	{
		cache.GetItem(rg());
	}
	return sw.Stop().count();
}

double TestForIntegerKeyMap(size_t cacheSize, size_t uniqueRequests, size_t totalRequests)
{
	RandomIntGenerator rg(1, uniqueRequests - 1);

	auto generator = [](const int& a) -> int { return a * a; };
	LruCacheMap<int, int, decltype(generator)> cache{ generator, cacheSize };

	Stopwatch sw;
	for (size_t i = 0; i < totalRequests; ++i)
	{
		cache.GetItem(rg());
	}
	return sw.Stop().count();
}

double TestForIntegerKeyUnorderedMapList(size_t cacheSize, size_t uniqueRequests, size_t totalRequests)
{
	RandomIntGenerator rg(1, uniqueRequests - 1);

	auto generator = [](const int& a) -> int { return a * a; };
	LruCacheDeque<int, int, decltype(generator)> cache{ generator, cacheSize };

	Stopwatch sw;
	for (size_t i = 0; i < totalRequests; ++i)
	{
		cache.GetItem(rg());
	}
	return sw.Stop().count();
}

int main()
{
	std::vector<double> times(12, 100.0);

	for (int i = 0; i < 100; ++i)
	{
		int j = 0;
		double time = TestForIntegerKeyUnorderedMap(100, 100, 10000);
		if (times[j] > time)
			times[j] = time;
		++j;
		time = TestForIntegerKeyMap(100, 100, 10000);
		if (times[j] > time)
			times[j] = time;
		++j;
		time = TestForIntegerKeyUnorderedMapList(100, 100, 10000);
		if (times[j] > time)
			times[j] = time;
		++j;

		time = TestForIntegerKeyUnorderedMap(100, 200, 10000);
		if (times[j] > time)
			times[j] = time;
		++j;
		time = TestForIntegerKeyMap(100, 200, 10000);
		if (times[j] > time)
			times[j] = time;
		++j;
		time = TestForIntegerKeyUnorderedMapList(100, 200, 10000);
		if (times[j] > time)
			times[j] = time;
		++j;

		time = TestForIntegerKeyUnorderedMap(100, 400, 10000);
		if (times[j] > time)
			times[j] = time;
		++j;
		time = TestForIntegerKeyMap(100, 400, 10000);
		if (times[j] > time)
			times[j] = time;
		++j;
		time = TestForIntegerKeyUnorderedMapList(100, 400, 10000);
		if (times[j] > time)
			times[j] = time;
		++j;

		time = TestForIntegerKeyUnorderedMap(100, 1000, 10000);
		if (times[j] > time)
			times[j] = time;
		++j;
		time = TestForIntegerKeyMap(100, 1000, 10000);
		if (times[j] > time)
			times[j] = time;
		++j;
		time = TestForIntegerKeyUnorderedMapList(100, 1000, 10000);
		if (times[j] > time)
			times[j] = time;

	}

	std::ofstream ofs("benchmark.txt");
	for (int i = 0; i < 12; ++i)
	{
		ofs << times[i] << std::endl;
	}

	return 0;
}

