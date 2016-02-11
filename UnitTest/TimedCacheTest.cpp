#include "stdafx.h"
#include "CppUnitTest.h"

#include "..\TimedCache\TimedCache.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTest
{		
	int squared(const int& a)
	{
		return a * a;
	}

	struct add3 {
		int operator() (const int& v) { return v * v; }
	};

	TEST_CLASS(TimedCacheTest)
	{
	public:

		TEST_METHOD(TimedCache_FetchingAnObjectThatIsNotInTheCacheAddsItToCache)
		{
			auto my_generator = [](const int& a) -> int { return a * a; };
			TimedCache<int, int, add3> tc;
			Assert::AreEqual(9, tc.GetItem(3));
		}

	};
}