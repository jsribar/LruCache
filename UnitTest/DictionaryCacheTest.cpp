#include "stdafx.h"
#include "CppUnitTest.h"

#include "..\TimedCache\DictionaryCache.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTest
{
	TEST_CLASS(DictionaryCacheTest)
	{
	public:
		TEST_METHOD(DictionaryCache_ContainsMethodReturnsFalseForItemNotInCache)
		{
			auto my_generator = [](const int& a) -> int { return a * a; };
			DictionaryCache<int, int, decltype(my_generator)> dc(my_generator, 20);
			Assert::IsFalse(dc.ContainsItem(9));
		}

		TEST_METHOD(DictionaryCache_FetchingAnObjectThatIsNotInTheCacheAddsItToCache)
		{
			auto my_generator = [](const int& a) -> int { return a * a; };
			DictionaryCache<int, int, decltype(my_generator)> dc(my_generator, 20);
			dc.GetItem(3);
			Assert::IsTrue(dc.ContainsItem(3));
		}

		TEST_METHOD(DictionaryCache_FetchingAnObjectThatIsInTheCacheReturnsItsValue)
		{
			auto my_generator = [](const int& a) -> int { return a * a; };
			DictionaryCache<int, int, decltype(my_generator)> dc(my_generator, 20);
			dc.GetItem(3);
			Assert::AreEqual(my_generator(3), dc.GetItem(3));
		}

		TEST_METHOD(DictionaryCache_FetchingThreeDifferentObjectsThatAreNotInTheCacheAddsThemToCache)
		{
			auto my_generator = [](const int& a) -> int { return a * a; };
			DictionaryCache<int, int, decltype(my_generator)> dc(my_generator, 20);
			dc.GetItem(1);
			dc.GetItem(2);
			dc.GetItem(3);
			Assert::IsTrue(dc.ContainsItem(1));
			Assert::IsTrue(dc.ContainsItem(2));
			Assert::IsTrue(dc.ContainsItem(3));
		}

		TEST_METHOD(DictionaryCache_CleanupMethodRemovesOldestItemIfCalledAfterItemsNumberExceedesThresholdByOne)
		{
			auto my_generator = [](const int& a) -> int { return a * a; };
			DictionaryCache<int, int, decltype(my_generator)> dc(my_generator, 5);
			dc.GetItem(1);
			dc.GetItem(2);
			dc.GetItem(3);
			dc.GetItem(4);
			dc.GetItem(5);
			dc.GetItem(6);
			dc.Cleanup();
			Assert::IsFalse(dc.ContainsItem(1));
			Assert::IsTrue(dc.ContainsItem(2));
		}


	};
}