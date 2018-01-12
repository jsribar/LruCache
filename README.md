# LruCache
Implementation of Least Recently Used (LRU) cache.

## Basics
When creation of objects that are used repetitively is expensive, it makes sense to save them into a cache and access those cached objects by some key. When cache is filled-up, least recently used items are removed from the cache (https://en.wikipedia.org/wiki/Cache_replacement_policies#Least_Recently_Used_(LRU)). In order to identify which item has been accessed least recently, a timestamp is stored with each item. Timestamp must be updated on each access to object to ensure that recently used objects are kept longer in the cache.

This project was created to solve repetitive use of a number of CFont objects (see TestFontCache project), but can be easily applied to any type of objects.
Also, types of containers used internally can be easily switched and there are even template class specializations for typical containers.
Solution contains also a benchmark project that writes access times for different configurations, providing a quick access time comparisons.
