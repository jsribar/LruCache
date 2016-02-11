#pragma once
#include <chrono> 
#include <unordered_map>
#include <algorithm>
#include <memory>

template<typename TItem, typename TData, typename TGenerator>
class DictionaryCache
{
	class TimestampedItem
	{
	public:
		TimestampedItem(const TItem& item) : m_item(item), m_lastAccessed(std::chrono::steady_clock::now())
		{
		}

		const TItem& GetItem() const { return m_item; }

		bool IsOlderThan(const std::chrono::steady_clock::time_point& timeout) const
		{
			return true;
		}

		void UpdateTimestamp()
		{
			m_lastAccessed = std::chrono::steady_clock::now();
		}

	private:
		TItem m_item;
		std::chrono::steady_clock::time_point m_lastAccessed;
	};

	using TItems = std::unordered_map<TData, TimestampedItem>;

public:
	DictionaryCache(const TGenerator& generator, int cleanupThreshold) : m_generator(generator), m_itemCleanupThreshold(cleanupThreshold)
	{
	}

	bool ContainsItem(const TData& data)
	{
		return m_items.find(data) != m_items.end();
	}

	const TItem& GetItem(const TData& data)
	{
		const TItems::iterator& found = m_items.find(data);
		if (found != m_items.end())
		{
			found->second.UpdateTimestamp();
			return found->second.GetItem();
		}
		return m_items.emplace(data, m_generator(data)).first->second.GetItem();
	}

	void Cleanup()
	{

	}

private:
	const TGenerator& m_generator;
	TItems m_items;
	int m_itemCleanupThreshold;
};