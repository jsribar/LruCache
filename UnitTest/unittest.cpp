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

	TEST_CLASS(UnitTest1)
	{
	public:

		TEST_METHOD(FetchingAnObjectThatIsNotInTheCacheAddsItToCache)
		{
			auto my_generator = [](const int& a) -> int { return a * a; };
			TimedCache<int, int, squared> tc;
			Assert::AreEqual(9, tc.GetItem(3));
		}

	};
}