#include "stdafx.h"
#include "CppUnitTest.h"
#include <thread>
#include <iostream>

#include "..\LruCache\LruCache.h"

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
		TEST_METHOD(LruCache_Pointers_AllObjectsInCacheAreDeletedWhenCacheGoesOutOfScope)
		{
			{
				auto my_generator = [](const int& a) -> MyStruct* { return new MyStruct(a * a); };
				int a = 5;
				LruCache<MyStruct*, int, decltype(my_generator)> cache{ my_generator, 5 };
				cache.GetItem(1);
				cache.GetItem(2);
				cache.GetItem(1);
				Assert::AreEqual(2, MyStruct::counter);
			}
			Assert::AreEqual(0, MyStruct::counter);
		}

		TEST_METHOD(LruCache_Pointers_ContainsItemMethodReturnsFalseForEmptyCache)
		{
			auto my_generator = [](const int& a) -> int* { return new int(a * a); };
			LruCache<int*, int, decltype(my_generator)> cache{ my_generator, 5 };
			Assert::IsFalse(cache.ContainsItem(9));
		}

		TEST_METHOD(LruCache_Pointers_GetItemMethodForObjectThatIsNotInTheCacheAddsItToCache)
		{
			auto my_generator = [](const int& a) -> int* { return new int(a * a); };
			LruCache<int*, int, decltype(my_generator)> cache{ my_generator, 5 };
			cache.GetItem(3);
			Assert::IsTrue(cache.ContainsItem(3));
		}

		TEST_METHOD(LruCache_Pointers_GetItemMethodReturnsItsValueIfItemWasNotInCache)
		{
			auto my_generator = [](const int& a) -> int* { return new int(a * a); };
			LruCache<int*, int, decltype(my_generator)> cache{ my_generator, 5 };
			Assert::AreEqual(*my_generator(3), *cache.GetItem(3));
		}

		TEST_METHOD(LruCache_Pointers_GetItemMethodReturnsItsValueIfItemIsAlreadyInCache)
		{
			auto my_generator = [](const int& a) -> int* { return new int(a * a); };
			LruCache<int*, int, decltype(my_generator)> cache{ my_generator, 5 };
			cache.GetItem(3);
			Assert::AreEqual(*my_generator(3), *cache.GetItem(3));
		}

		TEST_METHOD(LruCache_Pointers_GetItemMethodForThreeDifferentObjectsAddsThemToCache)
		{
			auto my_generator = [](const int& a) -> int* { return new int(a * a); };
			LruCache<int*, int, decltype(my_generator)> cache{ my_generator, 5 };
			cache.GetItem(1);
			cache.GetItem(2);
			cache.GetItem(3);
			Assert::IsTrue(cache.ContainsItem(1));
			Assert::IsTrue(cache.ContainsItem(2));
			Assert::IsTrue(cache.ContainsItem(3));
		}

		TEST_METHOD(LruCache_Pointers_GetItemMethodReturnsTheSamePointerWhenCalledSequentially)
		{
			auto my_generator = [](const int& a) -> int* { return new int(a * a); };
			LruCache<int*, int, decltype(my_generator)> cache{ my_generator, 3 };
			int* p1 = cache.GetItem(1);
			Assert::AreEqual(p1, cache.GetItem(1));
			int* p2 = cache.GetItem(2);
			Assert::AreEqual(p2, cache.GetItem(2));
			Assert::AreEqual(p1, cache.GetItem(1));
		}

		TEST_METHOD(LruCache_Pointers_ContainsItemMethodReturnsFalseForItemStrippedOutOfTheCache)
		{
			auto my_generator = [](const int& a) -> int* { return new int(a * a); };
			LruCache<int*, int, decltype(my_generator)> cache{ my_generator, 3 };
			cache.GetItem(1);
			cache.GetItem(2);
			cache.GetItem(3);
			Assert::IsTrue(cache.ContainsItem(1));
			cache.GetItem(4);
			Assert::IsFalse(cache.ContainsItem(1));
		}

		TEST_METHOD(LruCache_Pointers_ContainsItemMethodReturnsTrueForItemThatHasBeenRefetched)
		{
			auto my_generator = [](const int& a) -> int* { return new int(a * a); };
			LruCache<int*, int, decltype(my_generator)> cache{ my_generator, 3 };
			cache.GetItem(1);
			cache.GetItem(2);
			cache.GetItem(3);
			Assert::IsTrue(cache.ContainsItem(1));
			cache.GetItem(1);
			cache.GetItem(4);
			Assert::IsTrue(cache.ContainsItem(1));
			Assert::IsFalse(cache.ContainsItem(2));
		}

		TEST_METHOD(LruCache_Pointers_ResizeMethodStripsAllExcessiveItemsWhenNewSizeIsSmallerThanNumberOfItems)
		{
			auto my_generator = [](const int& a) -> int* { return new int(a * a); };
			LruCache<int*, int, decltype(my_generator)> cache{ my_generator, 3 };
			cache.GetItem(1);
			cache.GetItem(2);
			cache.GetItem(3);
			cache.Resize(1);
			Assert::IsFalse(cache.ContainsItem(1));
			Assert::IsFalse(cache.ContainsItem(2));
			Assert::IsTrue(cache.ContainsItem(3));
		}

		TEST_METHOD(LruCache_Pointers_ResizeMethodLeavesAllItemsWhenNewSizeIsLargerThanNumberOfItems)
		{
			auto my_generator = [](const int& a) -> int* { return new int(a * a); };
			LruCache<int*, int, decltype(my_generator)> cache{ my_generator, 3 };
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

		TEST_METHOD(LruCache_Pointers_CheckThatCacheWorksWithPointerToFunctions)
		{
			LruCache<int*, int, int*(int)> cache{ NewSquare, 5 };
			cache.GetItem(3);
			Assert::IsTrue(cache.ContainsItem(3));
		}
	};

	int LruPtrCacheTest::MyStruct::counter = 0; // test that instances are really removed at the end
}