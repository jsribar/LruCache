#include "stdafx.h"
#include "CppUnitTest.h"
#include <thread>
#include <iostream>

#include "..\TimedCache\LruCache1.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTest
{
	int* NewSquare(int a)
	{
		return new int(a * a);
	}

	TEST_CLASS(LruPtrCacheTest)
	{
		struct MyStruct
		{
			static int counter;

			MyStruct(int a) { ++counter; }
			~MyStruct() { --counter; }
		};

	public:
		TEST_METHOD(LruPtrCache_VerifyAllObjectsAreDeletedWhenCacheGoesOutOfScope)
		{
			{
				auto my_generator = [](const int& a) -> MyStruct* { return new MyStruct(a * a); };
				int a = 5;
				LruCache1<MyStruct*, int, decltype(my_generator)> cache{ my_generator, 0, 60000, 20000 };
				cache.GetItem(1);
				cache.GetItem(2);
				cache.GetItem(1);
				Assert::AreEqual(2, MyStruct::counter);
			}
			Assert::AreEqual(0, MyStruct::counter);
		}

		TEST_METHOD(LruPtrCache_ContainsItemMethodReturnsFalseForEmptyCache)
		{
			auto my_generator = [](const int& a) -> int* { return new int(a * a); };
			LruCache1<int*, int, decltype(my_generator)> cache{ my_generator, 0, 60000, 20000 };
			Assert::IsFalse(cache.ContainsItem(9));
		}

		TEST_METHOD(LruPtrCache_GetItemMethodForObjectThatIsNotInTheCacheAddsItToCache)
		{
			auto my_generator = [](const int& a) -> int* { return new int(a * a); };
			LruCache1<int*, int, decltype(my_generator)> cache{ my_generator, 0, 60000, 20000 };
			cache.GetItem(3);
			Assert::IsTrue(cache.ContainsItem(3));
		}

		TEST_METHOD(LruPtrCache_GetItemMethodReturnsItsValueIfItemWasNotInCache)
		{
			auto my_generator = [](const int& a) -> int* { return new int(a * a); };
			LruCache1<int*, int, decltype(my_generator)> cache{ my_generator, 0, 60000, 20000 };
			Assert::AreEqual(*my_generator(3), *cache.GetItem(3));
		}

		TEST_METHOD(LruPtrCache_GetItemMethodReturnsItsValueIfItemIsAlreadyInCache)
		{
			auto my_generator = [](const int& a) -> int* { return new int(a * a); };
			LruCache1<int*, int, decltype(my_generator)> cache{ my_generator, 0, 60000, 20000 };
			cache.GetItem(3);
			Assert::AreEqual(*my_generator(3), *cache.GetItem(3));
		}

		TEST_METHOD(LruPtrCache_GetItemForThreeDifferentObjectsThatAreNotInTheCacheAddsThemToCache)
		{
			auto my_generator = [](const int& a) -> int* { return new int(a * a); };
			LruCache1<int*, int, decltype(my_generator)> cache{ my_generator, 0, 60000, 20000 };
			cache.GetItem(1);
			cache.GetItem(2);
			cache.GetItem(3);
			Assert::IsTrue(cache.ContainsItem(1));
			Assert::IsTrue(cache.ContainsItem(2));
			Assert::IsTrue(cache.ContainsItem(3));
		}

		TEST_METHOD(LruCache_CleanupMethodRemovesItemIfItHasExpired)
		{
			auto my_generator = [](const int& a) -> int* { return new int(a * a); };
			size_t timeout = 10;
			LruCache1<int*, int, decltype(my_generator)> cache{ my_generator, 0, timeout, 20000 };
			cache.GetItem(1);
			std::this_thread::sleep_for(std::chrono::milliseconds(timeout + 1));
			Assert::IsTrue(cache.ContainsItem(1));
			cache.Cleanup(std::chrono::steady_clock::now() - std::chrono::milliseconds(timeout));
			Assert::IsFalse(cache.ContainsItem(1));
		}

		TEST_METHOD(LruPtrCache_CleanupMethodDoesNotRemoveItemIfLastGetItemWasCalledWithinTimeoutPeriod)
		{
			auto my_generator = [](const int& a) -> int* { return new int(a * a); };
			size_t timeout = 10;
			LruCache1<int*, int, decltype(my_generator)> cache{ my_generator, 0, timeout, 20000 };
			cache.GetItem(1);
			cache.GetItem(2);
			std::this_thread::sleep_for(std::chrono::milliseconds(timeout + 1));
			cache.GetItem(1);
			cache.Cleanup(std::chrono::steady_clock::now() - std::chrono::milliseconds(timeout));
			Assert::IsTrue(cache.ContainsItem(1));
			Assert::IsFalse(cache.ContainsItem(2));
		}

		TEST_METHOD(LruPtrCache_CleanupMethodRemovesAllItemsThatHaveExpired)
		{
			auto my_generator = [](const int& a) -> int* { return new int(a * a); };
			size_t timeout = 10;
			LruCache1<int*, int, decltype(my_generator)> cache{ my_generator, 0, timeout, 20000 };
			cache.GetItem(1);
			cache.GetItem(2);
			std::this_thread::sleep_for(std::chrono::milliseconds(timeout + 1));
			cache.Cleanup(std::chrono::steady_clock::now() - std::chrono::milliseconds(timeout));
			Assert::IsFalse(cache.ContainsItem(1));
			Assert::IsFalse(cache.ContainsItem(2));
		}

		TEST_METHOD(LruPtrCache_DefiningTDurationTemplateParameterInSecondsChangesTimeoutUnits)
		{
			auto my_generator = [](const int& a) -> int* { return new int(a * a); };
			size_t timeout = 2;
			LruCache1<int*, int, decltype(my_generator), std::chrono::seconds> cache{ my_generator, 0, timeout, 20000 };
			cache.GetItem(1);
			std::this_thread::sleep_for(std::chrono::milliseconds(timeout + 1));
			cache.Cleanup(std::chrono::steady_clock::now() - std::chrono::seconds(timeout));
			Assert::IsTrue(cache.ContainsItem(1));

			std::this_thread::sleep_for(std::chrono::seconds(timeout));
			cache.GetItem(2);
			cache.Cleanup(std::chrono::steady_clock::now() - std::chrono::seconds(timeout));
			Assert::IsFalse(cache.ContainsItem(1));
			Assert::IsTrue(cache.ContainsItem(2));
		}

		TEST_METHOD(LruPtrCache_CheckThatCacheWorksWithPointerToFunctions)
		{
			LruCache1<int*, int, int*(int)> cache{ NewSquare, 0, 0, 0 };
			cache.GetItem(3);
			Assert::IsTrue(cache.ContainsItem(3));
		}
	};

	int LruPtrCacheTest::MyStruct::counter = 0; // test instances are really removed at the end
}