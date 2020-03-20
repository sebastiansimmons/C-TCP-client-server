/*
 * Test cache_lib.cc
 */

#include "cache.hh"
#include <string.h>
#include <iostream>
#define CATCH_CONFIG_MAIN
#include "catch.hpp"

// The prompt says to use a global variable and to reset it after each test.
// I checked with Eitan, and he confirms this is desired behavior.
Cache test_cache(1024);

TEST_CASE( "test_get", "[placeholder]" ) {
    char charr0[] = "the quick brown fox jumps over the lazy dog";
    test_cache.set("key0", charr0, 44);
    // It sure would be nice if the interface defined the second argument
    // to get as a const. But, it was not to be.
    // Make sure we can get back "the quick brown fox jumps over the lazy dog".
    uint32_t s = 44;
    const char* ret0 = test_cache.get("key0", s);
    // This goes against the "only one REQUIRE per test" axiom. We are fine with this
    // because there is more than one thing we want to test, and having REQUIRE(bla && BLA && blalala)
    // would be silly. Also, if we split this up into multiple tests, there would be
    // more code duplication.
    REQUIRE(ret0 != nullptr);
    REQUIRE(s == 44);
    REQUIRE(strcmp(charr0, ret0) == 0);
    // Make sure we return only 44 bytes even if we request 45 bytes, as per the specification.
    s = 45;
    const char* ret1 = test_cache.get("key0", s);
    REQUIRE(ret1 != nullptr);
    REQUIRE(s == 44);    // Our implementation fails this test, embarassingly.
    REQUIRE(strcmp(charr0, ret1) == 0);
    // Add another test, make sure we can get back "J Q Vandz struck my big fox whelp".
    char charr2[] = "J Q Vandz struck my big fox whelp";
    test_cache.set("key2", charr2, 34);
    s = 34;
    const char* ret2 = test_cache.get("key2", s);
    REQUIRE(ret2 != nullptr);
    REQUIRE(s == 34);
    REQUIRE(strcmp(charr2, ret2) == 0);
    // Make sure we can still get "the quick brown fox jumps over the lazy dog" if we want to.
    s = 44;
    const char* ret3 = test_cache.get("key0", s);
    REQUIRE(ret3 != nullptr);
    REQUIRE(s == 44);
    REQUIRE(strcmp(charr0, ret3) == 0);
    test_cache.reset();
}


TEST_CASE( "test_del", "[placeholder]" ) {
    // Short test
    char charr0[] = "the quick brown fox jumps over the lazy dog";
    test_cache.set("key0", charr0, 44);
    uint32_t s = 42;
    test_cache.del("key0");
    REQUIRE(test_cache.get("key0", s) == nullptr);
    test_cache.reset();
}

TEST_CASE( "test_space_used", "[placeholder]" ) {
    REQUIRE(test_cache.space_used() == 0);
    char charr0[] = "the quick brown fox jumps over the lazy dog";
    test_cache.set("key0", charr0, 44);      // Include the null terminator in all sizes.
    REQUIRE(test_cache.space_used() == 44);
    // Make sure we do a deep copy by ending the string early
    charr0[3] = '\0';
    REQUIRE(test_cache.space_used() == 44);
    char charr1[] = "J Q Vandz struck my big fox whelp";
    // Make sure space adds linearly
    test_cache.set("key1", charr1, 34);
    REQUIRE(test_cache.space_used() == 44+34);
    // Make sure the old value gets overwritten
    char charr2[] = "Sphinx of black quartz, hear my vow!";
    test_cache.set("key0", charr2, 38);
    REQUIRE(test_cache.space_used() == 38+34);
    // Make sure the old value got deleted
    test_cache.del("key1");
    REQUIRE(test_cache.space_used() == 38);
    test_cache.reset();
}

TEST_CASE( "test_reset", "[placeholder]" ) {
    // Give our test_cache some keys and values
    char charr0[] = "the quick brown fox jumps over the lazy dog";
    test_cache.set("key0", charr0, 44);
    char charr1[] = "J Q Vandz struck my big fox whelp";
    test_cache.set("key1", charr1, 34);
    char charr2[] = "Sphinx of black quartz, hear my vow!";
    test_cache.set("key2", charr2, 38);
    // Reset it
    test_cache.reset();
    // Make sure it got deleted. (This assumes test_space_used is working properly).
    REQUIRE(test_cache.space_used() == 0);
    // I don't recall what the desired behavior is for reset if there is nothing in the cache. Let's say it just deletes nothing.
    test_cache.reset();
    REQUIRE(test_cache.space_used() == 0);
    test_cache.reset();
}


