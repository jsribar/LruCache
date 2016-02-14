#include "stdafx.h"
#include "CppUnitTest.h"
#include <thread>
#include <iostream>

#include "..\TimedCache\DictionaryPtrCacheUnorderedMap.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace UnitTest
{
	extern int* NewSquare(int a);

	TEST_CLASS(DictionaryPtrCacheUnorderedMapTest)
	{
		struct MyStruct
		{
			static int counter;

			MyStruct(int a) { ++counter; }
			~MyStruct() { --counter; }
		};

	public:
		TEST_METHOD(DictionaryPtrCacheUnorderedMap_VerifyAllInstancesAreDeletedWhenCacheGoesOutOfScope)
		{
			{
				auto my_generator = [](const int& a) -> MyStruct* { return new MyStruct(a * a); };
				DictionaryPtrCacheUnorderedMap<MyStruct, int, decltype(my_generator)> dc(my_generator, 0);
				dc.GetItem(1);
				dc.GetItem(2);
				dc.GetItem(1);
				Assert::AreEqual(2, MyStruct::counter);
			}
			Assert::AreEqual(0, MyStruct::counter);
		}

		TEST_METHOD(DictionaryPtrCacheUnorderedMap_ContainsMethodReturnsFalseForItemNotInCache)
		{
			auto my_generator = [](const int& a) -> int* { return new int(a * a); };
			DictionaryPtrCacheUnorderedMap<int, int, decltype(my_generator)> dc(my_generator, 0);
			Assert::IsFalse(dc.ContainsItem(9));
		}

		TEST_METHOD(DictionaryPtrCacheUnorderedMap_FetchingAnObjectThatIsNotInTheCacheAddsItToCache)
		{
			auto my_generator = [](const int& a) -> int* { return new int(a * a); };
			DictionaryPtrCacheUnorderedMap<int, int, decltype(my_generator)> dc(my_generator, 0);
			dc.GetItem(3);
			Assert::IsTrue(dc.ContainsItem(3));
		}

		TEST_METHOD(DictionaryPtrCacheUnorderedMap_FetchingAnObjectThatIsInTheCacheReturnsItsValue)
		{
			auto my_generator = [](const int& a) -> int* { return new int(a * a); };
			DictionaryPtrCacheUnorderedMap<int, int, decltype(my_generator)> dc(my_generator, 0);
			dc.GetItem(3);
			Assert::AreEqual(*my_generator(3), *dc.GetItem(3));
		}

		TEST_METHOD(DictionaryPtrCacheUnorderedMap_FetchingThreeDifferentObjectsThatAreNotInTheCacheAddsThemToCache)
		{
			auto my_generator = [](const int& a) -> int* { return new int(a * a); };
			DictionaryPtrCacheUnorderedMap<int, int, decltype(my_generator)> dc(my_generator, 0);
			dc.GetItem(1);
			dc.GetItem(2);
			dc.GetItem(3);
			Assert::IsTrue(dc.ContainsItem(1));
			Assert::IsTrue(dc.ContainsItem(2));
			Assert::IsTrue(dc.ContainsItem(3));
		}

		TEST_METHOD(DictionaryPtrCacheUnorderedMap_CleanupMethodRemovesItemsOlderThanTimeoutPeriod)
		{
			auto my_generator = [](const int& a) -> int* { return new int(a * a); };
			int timeout = 500;
			DictionaryPtrCacheUnorderedMap<int, int, decltype(my_generator)> dc(my_generator, 0, timeout);
			dc.GetItem(1);
			std::this_thread::sleep_for(std::chrono::milliseconds(timeout + 1));
			dc.GetItem(2);
			dc.Cleanup();
			Assert::IsFalse(dc.ContainsItem(1));
			Assert::IsTrue(dc.ContainsItem(2));
		}

		TEST_METHOD(DictionaryPtrCacheUnorderedMap_CleanupMethodDoesNotRemoveItemIfLastGetItemWasCalledWithinTimeoutPeriod)
		{
			auto my_generator = [](const int& a) -> int* { return new int(a * a); };
			int timeout = 500;
			DictionaryPtrCacheUnorderedMap<int, int, decltype(my_generator)> dc(my_generator, 0, timeout);
			dc.GetItem(1);
			dc.GetItem(2);
			std::this_thread::sleep_for(std::chrono::milliseconds(timeout + 1));
			dc.GetItem(1);
			dc.Cleanup();
			Assert::IsTrue(dc.ContainsItem(1));
			Assert::IsFalse(dc.ContainsItem(2));
		}

		TEST_METHOD(DictionaryPtrCacheUnorderedMap_CleanupMethodRemoveaAllItemsIfTheyAreExpired)
		{
			auto my_generator = [](const int& a) -> int* { return new int(a * a); };
			int timeout = 500;
			DictionaryPtrCacheUnorderedMap<int, int, decltype(my_generator)> dc(my_generator, 0, timeout);
			dc.GetItem(1);
			dc.GetItem(2);
			std::this_thread::sleep_for(std::chrono::milliseconds(timeout + 1));
			dc.Cleanup();
			Assert::IsFalse(dc.ContainsItem(1));
			Assert::IsFalse(dc.ContainsItem(2));
		}

		TEST_METHOD(DictionaryPtrCacheUnorderedMap_DefiningTDurationTemplateParameterInSecondsChangesTimeoutUnits)
		{
			auto my_generator = [](const int& a) -> int* { return new int(a * a); };
			int timeout = 2;
			DictionaryPtrCacheUnorderedMap<int, int, decltype(my_generator), std::chrono::seconds> dc(my_generator, 0, timeout);
			dc.GetItem(1);
			std::this_thread::sleep_for(std::chrono::milliseconds(timeout + 1));
			dc.Cleanup();
			Assert::IsTrue(dc.ContainsItem(1));

			std::this_thread::sleep_for(std::chrono::seconds(timeout));
			dc.GetItem(2);
			dc.Cleanup();
			Assert::IsFalse(dc.ContainsItem(1));
			Assert::IsTrue(dc.ContainsItem(2));
		}

		TEST_METHOD(DictionaryPtrCacheUnorderedMap_CheckThatCacheWorksWithPointerToFunctions)
		{
			DictionaryPtrCacheUnorderedMap<int, int, int*(int)> dc(NewSquare, 0, 0);
			dc.GetItem(3);
			Assert::IsTrue(dc.ContainsItem(3));
		}
	};

	int DictionaryPtrCacheUnorderedMapTest::MyStruct::counter = 0;
}