#include "../src/lru_cache.h"
#include <gtest/gtest.h>
#include <random>

namespace {
    static auto gen = std::mt19937_64(0);

    TEST(SimpleTest, NoValue) {
        lru_cache<int, int> lru(3);
        ASSERT_FALSE(lru.get(1).has_value());
    }

    TEST(SimpleTest, PutOneValue) {
        lru_cache<int, int> lru(3);
        lru.put(1, 2);
        ASSERT_EQ(2, lru.get(1));
    }

    TEST(SimpleTest, DoublePutOneValue) {
        lru_cache<int, int> lru(3);
        lru.put(1, 2);
        lru.put(1, 3);
        ASSERT_EQ(3, lru.get(1));
    }

    TEST(SimpleTest, Overflow) {
        lru_cache<int, int> lru(3);
        lru.put(1, 1);
        lru.put(2, 2);
        lru.put(3, 3);
        lru.put(4, 4);
        ASSERT_FALSE(lru.get(1).has_value());
        ASSERT_TRUE(lru.get(2).has_value());
        ASSERT_TRUE(lru.get(3).has_value());
        ASSERT_TRUE(lru.get(4).has_value());
    }

    TEST(RandomTest, Squares) {
        lru_cache<int, int> lru(100);
        auto rng_key = std::uniform_int_distribution(0, 200);
        for (int i = 0; i < 1000; ++i) {
            int k1 = rng_key(gen);
            int k2 = rng_key(gen);
            lru.put(k1, k1 * k1);
            auto ov = lru.get(k2);
            if (ov.has_value()) {
                ASSERT_EQ(ov.value(), k2 * k2);
            }
        }
    }
}