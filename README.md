# LruCache
Implementation of Least Recently Used (LRU) cache.

## Basics
If creation of some objects that are used often is expensive, sometimes it makes sense to save them into a cache and access those cached values by some key. When cache is filled-up, least-receantly used items are removed from the cashe (https://en.wikipedia.org/wiki/Cache_replacement_policies#Least_Recently_Used_(LRU)). In order to identify which item has been accessed least recently, a timestamp of last access must be stored with item.

This project was created to solve repetitive use of a number of CFont objects (see TestFontCache project), but can be easily applied to any type of objects.
