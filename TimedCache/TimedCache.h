#pragma once
#include <windows.h>
#include <unordered_map>
#include <memory>
#include <algorithm>

template<typename TItem, typename TData, TItem(*Generator)(const TData&)>
class TimedCache
{
	class TimestampedItem
	{
	public:
		TimestampedItem(const TData& data) : m_item(Generator(data))
		{
			GetSystemTimeAsFileTime(&m_time);
		}

		const TItem& GetItem() const { return m_item; }

		bool IsOlderThan(FILETIME timeout) const
		{
			return true;
		}

		void UpdateTimestamp()
		{
			GetSystemTimeAsFileTime(&m_time);
		}

	private:
		TItem m_item;
		FILETIME m_time;
	};

	using TItems = std::unordered_map<TData, TimestampedItem>;

public:
	TItem GetItem(const TData& data)
	{
		const TItems::iterator& found = items.find(data);
		// item is found
		if (found != items.end())
		{
			found->second.UpdateTimestamp();
			return found->second.GetItem();
		}
		return items.emplace(data, data).first->second.GetItem();
	}

private:

	TItems items;
};