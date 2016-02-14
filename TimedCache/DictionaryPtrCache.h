#pragma once
#include <chrono> 
#include <map>
#include <vector>
#include <algorithm>
#include <memory>

template<typename TItem, typename TData, typename TGenerator>
class DictionaryPtrCache
{
	using steady_clock = std::chrono::steady_clock;
	using time_point = std::chrono::steady_clock::time_point;

	class TimestampedItem
	{
	public:
		TimestampedItem(TItem* item) : m_item(item), m_lastAccessed(steady_clock::now())
		{
		}

		TItem* GetItem() const { return m_item; }

		const time_point& LastAccessed() const { return m_lastAccessed; }

		void UpdateTimestamp()
		{
			m_lastAccessed = steady_clock::now();
		}

	private:
		TItem* m_item;
		time_point m_lastAccessed;
	};

	using TItems = std::map<TData, std::unique_ptr<TimestampedItem>>;

public:
	DictionaryPtrCache(const TGenerator& generator, int cleanupThreshold = 200, size_t maxLifetimeMs = 60000) : m_generator(generator), m_itemCleanupThreshold(cleanupThreshold), m_maxLifetime(maxLifetimeMs)
	{
	}

	bool ContainsItem(const TData& data)
	{
		return m_items.find(data) != m_items.end();
	}

	TItem* GetItem(const TData& data)
	{
		const TItems::iterator& found = m_items.find(data);
		if (found != m_items.end())
		{
			found->second->UpdateTimestamp();
			return found->second->GetItem();
		}
		return m_items.insert(std::pair<TData, std::unique_ptr<TimestampedItem>>(data, std::make_unique<TimestampedItem>(m_generator(data)))).first->second->GetItem();
	}

	void Cleanup()
	{
		if (m_items.size() < m_itemCleanupThreshold)
			return;
		// find boundary timestamp for the m_maxLifetime
		time_point timeStampLimit = steady_clock::now() - m_maxLifetime;
		// fetch all keys for which lifetime has expired
		for (auto it = m_items.begin(); it != m_items.end(); )
		{
			if (it->second->LastAccessed() < timeStampLimit)
				it = m_items.erase(it);
			else
				++it;
		}
	}

private:
	const TGenerator& m_generator;
	TItems m_items;
	size_t m_itemCleanupThreshold;
	std::chrono::milliseconds m_maxLifetime;
};
