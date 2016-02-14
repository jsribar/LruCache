#include "stdafx.h"
#include "CppUnitTest.h"
#include <thread>
#include <iostream>

#include "..\TimedCache\DictionaryPtrCache.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTest
{
	int* NewSquare(const int& a)
	{
		return new int(a * a);
	}

	TEST_CLASS(DictionaryPtrCacheTest)
	{
		struct MyStruct
		{
			static int counter;
			MyStruct(int a)
			{
				++counter;
			}
			~MyStruct()
			{
				--counter;
			}
		};

	public:
		TEST_METHOD(DictionaryPtrCache_VerifyAllInstancesAreDeletedWhenCacheGoesOutOfScope)
		{
			{
				auto my_generator = [](const int& a) -> MyStruct* { return new MyStruct(a * a); };
				DictionaryPtrCache<MyStruct, int, decltype(my_generator)> dc(my_generator, 0, 0);
				dc.GetItem(1);
				dc.GetItem(2);
				dc.GetItem(1);
				Assert::AreEqual(2, MyStruct::counter);
			}
			Assert::AreEqual(0, MyStruct::counter);
		}

		TEST_METHOD(DictionaryPtrCache_ContainsMethodReturnsFalseForItemNotInCache)
		{
			auto my_generator = [](const int& a) -> int* { return new int(a * a); };
			DictionaryPtrCache<int, int, decltype(my_generator)> dc(my_generator, 0, 0);
			Assert::IsFalse(dc.ContainsItem(9));
		}

		TEST_METHOD(DictionaryPtrCache_FetchingAnObjectThatIsNotInTheCacheAddsItToCache)
		{
			auto my_generator = [](const int& a) -> int* { return new int (a * a); };
			DictionaryPtrCache<int, int, decltype(my_generator)> dc(my_generator, 0, 0);
			dc.GetItem(3);
			Assert::IsTrue(dc.ContainsItem(3));
		}

		TEST_METHOD(DictionaryPtrCache_FetchingAnObjectThatIsInTheCacheReturnsItsValue)
		{
			auto my_generator = [](const int& a) -> int* { return new int(a * a); };
			DictionaryPtrCache<int, int, decltype(my_generator)> dc(my_generator, 0, 0);
			dc.GetItem(3);
			Assert::AreEqual(*my_generator(3), *dc.GetItem(3));
		}

		TEST_METHOD(DictionaryPtrCache_FetchingThreeDifferentObjectsThatAreNotInTheCacheAddsThemToCache)
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

		TEST_METHOD(DictionaryPtrCache_CleanupMethodRemovesItemsOlderThanTimeoutPeriod)
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

		TEST_METHOD(DictionaryPtrCache_CleanupMethodDoesNotRemoveItemIfLastGetItemWasCalledWithinTimeoutPeriod)
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

		TEST_METHOD(DictionaryPtrCache_CleanupMethodRemoveaAllItemsIfTheyAreExpired)
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

		TEST_METHOD(DictionaryPtrCache_TestThatCacheWorksWithPointerToFunctions)
		{
			DictionaryPtrCache<int, int, int*(const int&)> dc(*NewSquare, 0, 0);
			dc.GetItem(3);
			Assert::IsTrue(dc.ContainsItem(3));
		}
	};

	int DictionaryPtrCacheTest::MyStruct::counter = 0;
}