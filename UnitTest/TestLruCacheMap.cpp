#include "stdafx.h"
#include "CppUnitTest.h"
#include <thread>

#include "..\LruCache\LruCache.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTest
{
	TEST_CLASS(LruCacheMapTest)
	{
	public:
		TEST_METHOD(LruCacheMap_ContainsItemMethodReturnsFalseForEmptyCache)
		{
			auto my_generator = [](const int& a) -> int { return a * a; };
			LruCacheMap<int, int, decltype(my_generator)> cache{ my_generator, 10 };
			Assert::IsFalse(cache.ContainsItem(9));
		}

		TEST_METHOD(LruCacheMap_GetItemMethodAddsObjectThatIsNotInTheCache)
		{
			auto my_generator = [](const int& a) -> int { return a * a; };
			LruCacheMap<int, int, decltype(my_generator)> cache{ my_generator, 10 };
			Assert::IsFalse(cache.ContainsItem(3));
			cache.GetItem(3);
			Assert::IsTrue(cache.ContainsItem(3));
		}

		TEST_METHOD(LruCacheMap_GetItemMethodReturnsItemValueIfItemWasNotInCache)
		{
			auto my_generator = [](const int& a) -> int { return a * a; };
			LruCacheMap<int, int, decltype(my_generator)> cache{ my_generator, 10 };
			Assert::AreEqual(my_generator(3), cache.GetItem(3));
		}

		TEST_METHOD(LruCacheMap_GetItemMethodReturnsItemValueIfItemIsAlreadyInCache)
		{
			auto my_generator = [](const int& a) -> int { return a * a; };
			LruCacheMap<int, int, decltype(my_generator)> cache{ my_generator, 10 };
			cache.GetItem(3);
			Assert::IsTrue(cache.ContainsItem(3));
			Assert::AreEqual(my_generator(3), cache.GetItem(3));
		}

		TEST_METHOD(LruCacheMap_GetItemMethodForThreeDifferentObjectsAddsThemToCache)
		{
			auto my_generator = [](const int& a) -> int { return a * a; };
			LruCacheMap<int, int, decltype(my_generator)> cache{ my_generator, 10 };
			cache.GetItem(1);
			cache.GetItem(2);
			cache.GetItem(3);
			Assert::IsTrue(cache.ContainsItem(1));
			Assert::IsTrue(cache.ContainsItem(2));
			Assert::IsTrue(cache.ContainsItem(3));
		}

		TEST_METHOD(LruCacheMap_ContainsItemMethodReturnsFalseForItemStrippedOutOfTheCache)
		{
			auto my_generator = [](const int& a) -> int { return a * a; };
			LruCacheMap<int, int, decltype(my_generator)> cache{ my_generator, 3 };
			cache.GetItem(1);
			cache.GetItem(2);
			cache.GetItem(3);
			Assert::IsTrue(cache.ContainsItem(1));
			cache.GetItem(4);
			Assert::IsFalse(cache.ContainsItem(1));
		}

		TEST_METHOD(LruCacheMap_Pointers_ContainsItemMethodReturnsFalseForOldestItemsStrippedOutOfTheCacheSequentially)
		{
			auto my_generator = [](const int& a) -> int { return a * a; };
			LruCacheMap<int, int, decltype(my_generator)> cache{ my_generator, 3 };
			cache.GetItem(1);
			cache.GetItem(2);
			cache.GetItem(3);

			cache.GetItem(4);
			Assert::IsFalse(cache.ContainsItem(1));
			Assert::IsTrue(cache.ContainsItem(2));

			cache.GetItem(5);
			Assert::IsFalse(cache.ContainsItem(1));
			Assert::IsFalse(cache.ContainsItem(2));
			Assert::IsTrue(cache.ContainsItem(3));
			Assert::IsTrue(cache.ContainsItem(4));
			Assert::IsTrue(cache.ContainsItem(5));
		}

		TEST_METHOD(LruCacheMap_ContainsItemMethodReturnsTrueForItemThatHasBeenRefetched)
		{
			auto my_generator = [](const int& a) -> int { return a * a; };
			LruCacheMap<int, int, decltype(my_generator)> cache{ my_generator, 3 };
			cache.GetItem(1);
			cache.GetItem(2);
			cache.GetItem(3);
			Assert::IsTrue(cache.ContainsItem(1));
			cache.GetItem(1);
			cache.GetItem(4);
			Assert::IsTrue(cache.ContainsItem(1));
			Assert::IsFalse(cache.ContainsItem(2));
		}

		TEST_METHOD(LruCacheMap_ResizeMethodStripsAllExcessiveItemsWhenNewSizeIsSmallerThanNumberOfItems)
		{
			auto my_generator = [](const int& a) -> int { return a * a; };
			LruCacheMap<int, int, decltype(my_generator)> cache{ my_generator, 3 };
			cache.GetItem(1);
			cache.GetItem(2);
			cache.GetItem(3);
			cache.Resize(1);
			Assert::IsFalse(cache.ContainsItem(1));
			Assert::IsFalse(cache.ContainsItem(2));
			Assert::IsTrue(cache.ContainsItem(3));
		}

		TEST_METHOD(LruCacheMap_ResizeMethodLeavesAllItemsWhenNewSizeIsLargerThanNumberOfItems)
		{
			auto my_generator = [](const int& a) -> int { return a * a; };
			LruCacheMap<int, int, decltype(my_generator)> cache{ my_generator, 3 };
			cache.GetItem(1);
			cache.GetItem(2);
			cache.GetItem(3);
			cache.Resize(5);
			Assert::IsTrue(cache.ContainsItem(1));
			Assert::IsTrue(cache.ContainsItem(2));
			Assert::IsTrue(cache.ContainsItem(3));

			cache.GetItem(4);
			Assert::IsTrue(cache.ContainsItem(4));
			cache.GetItem(5);
			Assert::IsTrue(cache.ContainsItem(5));
		}

	};
}