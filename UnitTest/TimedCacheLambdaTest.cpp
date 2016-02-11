#include "stdafx.h"
#include "CppUnitTest.h"

#include "..\TimedCache\TimedCacheLambda.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTest
{
	TEST_CLASS(TimedCacheLambdaTest)
	{
	public:

		TEST_METHOD(TimedCache_FetchingAnObjectThatIsNotInTheCacheAddsItToCache)
		{
			auto my_generator = [](const int& a) -> int { return a * a; };
			TimedCacheLambda<int, int, decltype(my_generator)> tc(my_generator);
			Assert::AreEqual(9, tc.GetItem(3));
		}

	};
}