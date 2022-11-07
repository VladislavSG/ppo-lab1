#pragma once
#include <cstddef>
#include <optional>
#include <list>
#include <unordered_map>
#include <cassert>

template<typename T>
concept Hashable = requires
{
     std::hash<T>();
};

template<Hashable K, typename V>
class lru_cache {
    struct Vit;
    using list = std::list<K>;
    using map = std::unordered_map<K, Vit>;

    struct Vit {
        V value;
        typename list::const_iterator it;
    };

    const std::size_t CAPACITY;
    list uses;
    map cache;
public:
    lru_cache() : CAPACITY(10) {}
    explicit lru_cache(std::size_t capacity) : CAPACITY(capacity) {
        assert(capacity > 0);
    }

    void put(const K& key, V value) {
        auto map_it = cache.find(key);
        if (map_it == cache.end()) {
            if (size() == CAPACITY) {
                cache.erase(uses.front());
                uses.erase(uses.begin());
            }
            uses.emplace_back(key);
            cache.emplace_hint(map_it, key, Vit{std::move(value), --uses.end()});
        } else {
            const auto list_it = map_it->second.it;
            assert(*list_it == key);
            map_it->second.value = std::move(value);
            if (list_it != --uses.end()) {
                uses.erase(map_it->second.it);
                uses.emplace_back(key);
                map_it->second.it = --uses.end();
            }
        }
    }

    std::optional<V> get(const K& key) {
        auto map_it = cache.find(key);
        if (map_it == cache.end()) {
            return std::nullopt;
        } else {
            const auto list_it = map_it->second.it;
            assert(*list_it == key);
            return map_it->second.value;
        }
    }

    [[nodiscard]] std::size_t size() const {
        assert(uses.size() == cache.size());
        assert(uses.size() <= CAPACITY);
        return uses.size();
    }
};
