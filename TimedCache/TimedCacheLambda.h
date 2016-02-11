#pragma once
#include <chrono> 
#include <unordered_map>
#include <algorithm>
#include <memory>

template<typename TItem, typename TData, typename TGenerator>
class TimedCacheLambda
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
	TimedCacheLambda(const TGenerator& gen) : generator(gen)
	{
	}

	bool ContainsItem(const TData& data)
	{
		return items.find(data) != items.end();
	}

	const TItem& GetItem(const TData& data)
	{
		const TItems::iterator& found = items.find(data);
		if (found != items.end())
		{
			found->second.UpdateTimestamp();
			return found->second.GetItem();
		}
		return items.emplace(data, generator(data)).first->second.GetItem();
	}

private:
	const TGenerator& generator;
	TItems items;
};