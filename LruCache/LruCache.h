#pragma once
#include <unordered_map>
#include <map>
#include <deque>
#include <algorithm>
#include <memory>
#include <cassert>

template<typename TItem, typename TKey, typename TGenerator, typename TGetItem = const TItem&, template <typename TKey, typename... Args> class Items = std::unordered_map>
class LruCacheBase
{
	// item stored in the cache
	template<typename TItem>
	class Item
	{
	public:
		Item(const TItem& item) : m_item(item) {}
		const TItem& GetItem() const { return m_item; }
	private:
		TItem m_item;
	};

	// specialization for pointer items
	template<typename TItem>
	class Item<TItem*>
	{
	public:
		Item(TItem* item) : m_item(item) {}
		TItem* GetItem() const { return m_item.get(); }
	private:
		std::unique_ptr<TItem> m_item;
	};

	using TItems = Items<TKey, Item<TItem>>;
	using TIterator = typename TItems::iterator;
	using TKeyIterators = std::deque<TIterator>;

protected:
	// generator - functor that creates item for the key value provided
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
			return found->second.GetItem();
		}
		if (m_items.size() >= m_capacity)
			StripLeastRecentlyUsed();
		return AppendItem(key);
	}

	void Resize(size_t newSize)
	{
		while (m_items.size() > newSize)
			StripLeastRecentlyUsed();
		m_capacity = newSize;
	}

private:
	TGenerator& m_generator;
	size_t m_capacity;
	TItems m_items;
	TKeyIterators m_keyIterators;

	TGetItem AppendItem(TKey key)
	{
		assert(m_items.find(key) == m_items.end());
		const auto& iter = m_items.emplace(key, m_generator(key)).first;
		m_keyIterators.push_back(iter);
		assert(m_items.size() == m_keyIterators.size());
		return iter->second.GetItem();
	}

	void StripLeastRecentlyUsed()
	{
		auto iter = m_keyIterators.front();
		m_items.erase(iter);
		m_keyIterators.pop_front();
		assert(m_items.size() == m_keyIterators.size());
	}

	void RepositionToQueueEnd(const TIterator& iter)
	{
		auto found = std::find(m_keyIterators.begin(), m_keyIterators.end(), iter);
		assert(found != m_keyIterators.end());
		std::rotate(found, found + 1, m_keyIterators.end());
		assert(m_keyIterators.back() == iter);
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
class LruCache<TItem*, TKey, TGenerator> : public LruCacheBase<TItem*, TKey, TGenerator, TItem*>
{
public:
	LruCache(TGenerator& generator, size_t capacity)
		: LruCacheBase(generator, capacity)
	{}
};


template<typename TItem, typename TKey, typename TGenerator>
class LruCacheMap : public LruCacheBase<TItem, TKey, TGenerator, const TItem&, std::map>
{
public:
	LruCacheMap(TGenerator& generator, size_t capacity)
		: LruCacheBase(generator, capacity)
	{}
};

template<typename TItem, typename TKey, typename TGenerator>
class LruCacheMap<TItem*, TKey, TGenerator> : public LruCacheBase<TItem*, TKey, TGenerator, TItem*, std::map>
{
public:
	LruCacheMap(TGenerator& generator, size_t capacity)
		: LruCacheBase(generator, capacity)
	{}
};

