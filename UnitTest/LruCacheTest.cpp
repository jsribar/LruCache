#include "stdafx.h"
#include "CppUnitTest.h"
#include <thread>

#include "..\TimedCache\LruCache.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTest
{
	TEST_CLASS(LruCacheTest)
	{
	public:
		TEST_METHOD(LruCache_ContainsItemMethodReturnsFalseForEmptyCache)
		{
			auto my_generator = [](const int& a) -> int { return a * a; };
			LruCache<int, int, decltype(my_generator)> cache{ my_generator, 0, 0, 20000 };
			Assert::IsFalse(cache.ContainsItem(9));
		}

		TEST_METHOD(LruCache_GetItemMethodForObjectThatIsNotInTheCacheAddsItToCache)
		{
			auto my_generator = [](const int& a) -> int { return a * a; };
			LruCache<int, int, decltype(my_generator)> cache{ my_generator, 0, 0, 20000 };
			Assert::IsFalse(cache.ContainsItem(3));
			cache.GetItem(3);
			Assert::IsTrue(cache.ContainsItem(3));
		}

		TEST_METHOD(LruCache_GetItemMethodReturnsItsValueIfItemWasNotInCache)
		{
			auto my_generator = [](const int& a) -> int { return a * a; };
			LruCache<int, int, decltype(my_generator)> cache{ my_generator, 0, 0, 20000 };
			Assert::AreEqual(my_generator(3), cache.GetItem(3));
		}

		TEST_METHOD(LruCache_GetItemMethodReturnsItsValueIfItemIsAlreadyInCache)
		{
			auto my_generator = [](const int& a) -> int { return a * a; };
			LruCache<int, int, decltype(my_generator)> cache{ my_generator, 0, 0, 20000 };
			cache.GetItem(3);
			Assert::IsTrue(cache.ContainsItem(3));
			Assert::AreEqual(my_generator(3), cache.GetItem(3));
		}

		TEST_METHOD(LruCache_GetItemForThreeDifferentObjectsThatAreNotInTheCacheAddsThemToCache)
		{
			auto my_generator = [](const int& a) -> int { return a * a; };
			LruCache<int, int, decltype(my_generator)> cache{ my_generator, 0, 0, 20000 };
			cache.GetItem(1);
			cache.GetItem(2);
			cache.GetItem(3);
			Assert::IsTrue(cache.ContainsItem(1));
			Assert::IsTrue(cache.ContainsItem(2));
			Assert::IsTrue(cache.ContainsItem(3));
		}

		TEST_METHOD(LruCache_CleanupMethodRemovesItemIfItHasExpired)
		{
			auto my_generator = [](const int& a) -> int { return a * a; };
			size_t timeout = 10;
			LruCache<int, int, decltype(my_generator)> cache{ my_generator, 0, timeout, 20000 };
			cache.GetItem(1);
			std::this_thread::sleep_for(std::chrono::milliseconds(timeout + 1));
			Assert::IsTrue(cache.ContainsItem(1));
			cache.Cleanup(std::chrono::steady_clock::now() - std::chrono::milliseconds(timeout));
			Assert::IsFalse(cache.ContainsItem(1));
		}

		TEST_METHOD(LruCache_CleanupMethodDoesNotRemoveItemIfLastGetItemWasCalledWithinTimeoutPeriod)
		{
			auto my_generator = [](const int& a) -> int { return a * a; };
			size_t timeout = 10;
			LruCache<int, int, decltype(my_generator)> cache{ my_generator, 0, timeout, 20000 };
			cache.GetItem(1);
			cache.GetItem(2);
			std::this_thread::sleep_for(std::chrono::milliseconds(timeout + 1));
			cache.GetItem(1);
			cache.Cleanup(std::chrono::steady_clock::now() - std::chrono::milliseconds(timeout));
			Assert::IsTrue(cache.ContainsItem(1));
			Assert::IsFalse(cache.ContainsItem(2));
		}

		TEST_METHOD(LruCache_CleanupMethodRemovesAllItemsThatHaveExpired)
		{
			auto my_generator = [](const int& a) -> int { return a * a; };
			size_t timeout = 10;
			LruCache<int, int, decltype(my_generator)> cache{ my_generator, 0, timeout, 20000 };
			cache.GetItem(1);
			cache.GetItem(2);
			std::this_thread::sleep_for(std::chrono::milliseconds(timeout + 1));
			cache.Cleanup(std::chrono::steady_clock::now() - std::chrono::milliseconds(timeout));
			Assert::IsFalse(cache.ContainsItem(1));
			Assert::IsFalse(cache.ContainsItem(2));
		}
	};
}