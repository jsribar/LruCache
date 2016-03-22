#pragma once
#include <chrono> 
#include <unordered_map>
#include <vector>
#include <algorithm>
#include <memory>

using steady_clock = std::chrono::steady_clock;
using time_point = std::chrono::steady_clock::time_point;

template<typename TItem, typename TKey, typename TGenerator, typename TDuration = std::chrono::milliseconds>
class LruCacheBase
{
	template<typename TItem>
	class TimestampedItem
	{
	public:
		TimestampedItem(const TItem& item) : m_item(item), m_lastAccessed(steady_clock::now())
		{
		}

		const TItem& GetItem() const { return m_item; }

		const time_point& LastAccessed() const { return m_lastAccessed; }

		void UpdateTimestamp()
		{
			m_lastAccessed = steady_clock::now();
		}

	private:
		TItem m_item;
		time_point m_lastAccessed;
	};

	template<typename TItem>
	class TimestampedItem<TItem*>
	{
	public:
		TimestampedItem(TItem* item) : m_item(item), m_lastAccessed(steady_clock::now())
		{
		}

		TItem* GetItem() const { return m_item.get(); }

		const time_point& LastAccessed() const { return m_lastAccessed; }

		void UpdateTimestamp()
		{
			m_lastAccessed = steady_clock::now();
		}

	private:
		std::unique_ptr<TItem> m_item;
		time_point m_lastAccessed;
	};

	using TItems = std::unordered_map<TKey, TimestampedItem<TItem>>;

public:
	// generator - functor that creates item pointer for the key value provided
	// cleanupThreshold - number of items in cache above which cache is cleaned-up
	// maxLifetime - maximum lifetime (in ms) of item in the cache, measured from time item was accessed last
	// cleanUpPeriod - minimum period (in ms) between two periodic clean-ups
	LruCacheBase(const TGenerator& generator, int cleanupThreshold, size_t maxLifetime, size_t cleanUpPeriod)
		: m_generator(generator)
		, m_itemsCleanupThreshold(cleanupThreshold)
		, m_maxLifetime(maxLifetime)
		, m_cleanUpPeriod(cleanUpPeriod)
		, m_nextCleanupTime(steady_clock::now() + m_cleanUpPeriod)
	{
	}

	bool ContainsItem(const TKey& key)
	{
		return m_items.find(key) != m_items.end();
	}

	void Cleanup(time_point lifetimeThreshold)
	{
		for (auto it = m_items.begin(); it != m_items.end(); )
		{
			if (it->second.LastAccessed() < lifetimeThreshold)
				it = m_items.erase(it);
			else
				++it;
		}
	}

protected:
	const TGenerator& m_generator;
	TItems m_items;
	size_t m_itemsCleanupThreshold;
	TDuration m_maxLifetime;
	TDuration m_cleanUpPeriod;
	time_point m_nextCleanupTime;


	void RemoveExpiredItems()
	{
		if (m_items.size() < m_itemsCleanupThreshold)
			return;
		if (steady_clock::now() > m_nextCleanupTime)
		{
			// erase all items for which lifetime has expired
			Cleanup(steady_clock::now() - m_maxLifetime);
			m_nextCleanupTime = steady_clock::now() + m_cleanUpPeriod;
		}
	}
};

template<typename TItem, typename TKey, typename TGenerator, typename TDuration = std::chrono::milliseconds>
class LruCache1 : public LruCacheBase<TItem, TKey, TGenerator, TDuration>
{
public:
	LruCache1(const TGenerator& generator, int cleanupThreshold, size_t maxLifetime, size_t cleanUpPeriod)
		: LruCacheBase(generator, cleanupThreshold, maxLifetime, cleanUpPeriod)
	{}

	const TItem& GetItem(const TKey& key)
	{
		const TItems::iterator& found = m_items.find(key);
		if (found != m_items.end())
		{
			found->second.UpdateTimestamp();
			if (m_items.size() > 1)
				RemoveExpiredItems();
			return found->second.GetItem();
		}
		RemoveExpiredItems();
		return m_items.emplace(key, m_generator(key)).first->second.GetItem();
	}
};

template<typename TItem, typename TKey, typename TGenerator, typename TDuration>
class LruCache1<TItem*, TKey, TGenerator, TDuration> : public LruCacheBase<TItem*, TKey, TGenerator, TDuration>
{
public:
	LruCache1(const TGenerator& generator, int cleanupThreshold, size_t maxLifetime, size_t cleanUpPeriod)
		: LruCacheBase(generator, cleanupThreshold, maxLifetime, cleanUpPeriod)
	{}

	TItem* GetItem(const TKey& key)
	{
		const TItems::iterator& found = m_items.find(key);
		if (found != m_items.end())
		{
			found->second.UpdateTimestamp();
			if (m_items.size() > 1)
				RemoveExpiredItems();
			return found->second.GetItem();
		}
		RemoveExpiredItems();
		return m_items.emplace(key, m_generator(key)).first->second.GetItem();
	}
};
