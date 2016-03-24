#pragma once
#include <unordered_map>
#include <deque>
#include <algorithm>
#include <memory>
#include <cassert>

template<typename TItem, typename TKey, typename TGenerator, typename TItemInternal = TItem, typename TGetItem = const TItem&>
class LruCacheBase
{
protected:
	using TItems = std::unordered_map<TKey, TItemInternal>;
	using TIterator = typename TItems::iterator;
	using TKeyIterators = std::deque<TIterator>;

	//template<typename TItem, typename TIterator>
	//struct ItemGetter
	//{
	//	const TItem& operator() (const TIterator& iter)
	//	{
	//		return iter->second;
	//	}
	//};

	//template<typename TItem, typename TIterator>
	//struct ItemGetter<TItem*, TIterator>
	//{
	//	TItem* operator() (const TIterator& iter)
	//	{
	//		return iter->second.get();
	//	}
	//};

protected:
	// generator - functor that creates item pointer for the key value provided
	// capacity - maximum number of items in the cache
	LruCacheBase(TGenerator& generator, size_t capacity)
		: m_generator(generator)
		, m_capacity(capacity)
	{
	}

public:
	bool ContainsItem(const TKey& key)
	{
		return m_items.find(key) != m_items.end();
	}

	TGetItem GetItem(const TKey& key)
	{
		const TIterator& found = m_items.find(key);
		if (found != m_items.end())
		{
			RepositionToQueueEnd(found);
			return found->second;
		}
		Strip();
		return AddItem(key);
	}

protected:
	TGenerator& m_generator;
	size_t m_capacity;
	TItems m_items;
	TKeyIterators m_keyIterators;
	//ItemGetter<TItem, TIterator> m_itemGetter;

	TGetItem AddItem(const TKey& key)
	{
		assert(m_items.find(key) == m_items.end());
		auto iter = m_items.emplace(key, m_generator(key)).first;
		m_keyIterators.push_back(iter);
		assert(m_items.size() == m_keyIterators.size());
		return iter->second;
	}

	void Strip()
	{
		if (m_items.size() < m_capacity)
			return;
		auto iter = m_keyIterators.front();
		m_items.erase(iter);
		m_keyIterators.pop_front();
		assert(m_items.size() == m_keyIterators.size());
	}

	void RepositionToQueueEnd(const TIterator& iter)
	{
		auto found = std::find(m_keyIterators.begin(), m_keyIterators.end(), iter);
		std::rotate(found, found + 1, m_keyIterators.end());
	}
};

template<typename TItem, typename TKey, typename TGenerator>
class LruCache : public LruCacheBase<TItem, TKey, TGenerator>
{
public:
	LruCache(TGenerator& generator, size_t capacity)
		: LruCacheBase(generator, capacity)
	{}

};

template<typename TItem, typename TKey, typename TGenerator>
class LruCache<TItem*, TKey, TGenerator> : public LruCacheBase<TItem*, TKey, TGenerator, std::unique_ptr<TItem>, TItem*>
{
public:
	LruCache(TGenerator& generator, size_t capacity)
		: LruCacheBase(generator, capacity)
	{}
};
