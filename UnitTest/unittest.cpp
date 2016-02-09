#include "stdafx.h"
#include "CppUnitTest.h"

#include "..\TimedCache\TimedCache.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTest
{		
	double squared(int a)
	{
		return double(a * a);
	}

	TEST_CLASS(UnitTest1)
	{
	public:

		TEST_METHOD(FetchingAnObjectThatIsNotInTheCacheAddsItToCache)
		{
			auto my_generator = [](int a) -> int { return a * a; };
			TimedCache<int, int> tc;
			Assert::AreEqual(9, tc.GetItem(3, squared));
		}

	};
}