#include "stdafx.h"
#include "CppUnitTest.h"
#include <thread>

#include "..\TimedCache\DictionaryPtrCache.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTest
{
	TEST_CLASS(DictionaryPtrCacheTest)
	{
	public:
		TEST_METHOD(DictionaryCache_ContainsMethodReturnsFalseForItemNotInCache)
		{
			auto my_generator = [](const int& a) -> int { return a * a; };
			DictionaryPtrCache<int, int, decltype(my_generator)> dc(my_generator, 0, 0);
			Assert::IsFalse(dc.ContainsItem(9));
		}

		TEST_METHOD(DictionaryCache_FetchingAnObjectThatIsNotInTheCacheAddsItToCache)
		{
			auto my_generator = [](const int& a) -> int* { return new int (a * a); };
			DictionaryPtrCache<int, int, decltype(my_generator)> dc(my_generator, 0, 0);
			dc.GetItem(3);
			Assert::IsTrue(dc.ContainsItem(3));
		}

		TEST_METHOD(DictionaryCache_FetchingAnObjectThatIsInTheCacheReturnsItsValue)
		{
			auto my_generator = [](const int& a) -> int* { return new int(a * a); };
			DictionaryPtrCache<int, int, decltype(my_generator)> dc(my_generator, 0, 0);
			dc.GetItem(3);
			Assert::AreEqual(*my_generator(3), *dc.GetItem(3));
		}

		TEST_METHOD(DictionaryCache_FetchingThreeDifferentObjectsThatAreNotInTheCacheAddsThemToCache)
		{
			auto my_generator = [](const int& a) -> int* { return new int(a * a); };
			DictionaryPtrCache<int, int, decltype(my_generator)> dc(my_generator, 0, 0);
			dc.GetItem(1);
			dc.GetItem(2);
			dc.GetItem(3);
			Assert::IsTrue(dc.ContainsItem(1));
			Assert::IsTrue(dc.ContainsItem(2));
			Assert::IsTrue(dc.ContainsItem(3));
		}

		TEST_METHOD(DictionaryCache_CleanupMethodRemovesItemsOlderThanTimeoutPeriod)
		{
			auto my_generator = [](const int& a) -> int* { return new int(a * a); };
			int timeout = 500;
			DictionaryPtrCache<int, int, decltype(my_generator)> dc(my_generator, 0, timeout);
			dc.GetItem(1);
			std::this_thread::sleep_for(std::chrono::milliseconds(timeout + 1));
			dc.GetItem(2);
			dc.Cleanup();
			Assert::IsFalse(dc.ContainsItem(1));
			Assert::IsTrue(dc.ContainsItem(2));
		}

		TEST_METHOD(DictionaryCache_CleanupMethodDoesNotRemoveItemIfLastGetItemWasCalledWithinTimeoutPeriod)
		{
			auto my_generator = [](const int& a) -> int* { return new int(a * a); };
			int timeout = 500;
			DictionaryPtrCache<int, int, decltype(my_generator)> dc(my_generator, 0, timeout);
			dc.GetItem(1);
			dc.GetItem(2);
			std::this_thread::sleep_for(std::chrono::milliseconds(timeout + 1));
			dc.GetItem(1);
			dc.Cleanup();
			Assert::IsTrue(dc.ContainsItem(1));
			Assert::IsFalse(dc.ContainsItem(2));
		}

		TEST_METHOD(DictionaryCache_CleanupMethodRemoveaAllItemsIfTheyAreExpired)
		{
			auto my_generator = [](const int& a) -> int* { return new int(a * a); };
			int timeout = 500;
			DictionaryPtrCache<int, int, decltype(my_generator)> dc(my_generator, 0, timeout);
			dc.GetItem(1);
			dc.GetItem(2);
			std::this_thread::sleep_for(std::chrono::milliseconds(timeout + 1));
			dc.Cleanup();
			Assert::IsFalse(dc.ContainsItem(1));
			Assert::IsFalse(dc.ContainsItem(2));
		}
	};
}