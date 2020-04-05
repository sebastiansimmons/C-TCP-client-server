/*
 * test_cache_client.cc
 */

#include "cache.hh"
#include <string.h>
#include <iostream>
#define CATCH_CONFIG_MAIN
#include "catch.hpp"

// The prompt says to use a global variable and to reset it after each test.
// I checked with Eitan, and he confirms this is desired behavior.
Cache test_cache("localhost", "6300");

TEST_CASE( "test_get", "[placeholder]" ) {
    char charr0[] = "thequickbrownfoxjumpsoverthelazydog";
    test_cache.set("key0", charr0, strlen(charr0) + 1);
    // It sure would be nice if the interface defined the second argument
    // to get as a const. But, it was not to be.
    // Make sure we can get back "thequickbrownfoxjumpsoverthelazydog".
    uint32_t s = 36;
    const char* ret0 = test_cache.get("key0", s);

    // This goes against the "only one REQUIRE per test" axiom. We are fine with this
    // because there is more than one thing we want to test, and having REQUIRE(bla && BLA && blalala)
    // would be silly. Also, if we split this up into multiple tests, there would be
    // more code duplication.
    REQUIRE(ret0 != nullptr);
    REQUIRE(s == 36);
    REQUIRE(strcmp(charr0, ret0) == 0);
    // Make sure we return only 36 bytes even if we request 45 bytes, as per the specification.
    s = 45;
    const char* ret1 = test_cache.get("key0", s);
    REQUIRE(ret1 != nullptr);
    REQUIRE(s == 36);    // Our implementation fails this test, embarassingly.
    REQUIRE(strcmp(charr0, ret1) == 0);
    // Add another test, make sure we can get back "J Q Vandz struck my big fox whelp".
    char charr2[] = "TheGovernmentTracksYourPhoneAndLaptop";
    test_cache.set("key2", charr2, strlen(charr2) + 1);
    s = strlen(charr2) + 1;
    const char* ret2 = test_cache.get("key2", s);
    REQUIRE(ret2 != nullptr);
    REQUIRE(s == strlen(charr2) + 1);
    REQUIRE(strcmp(charr2, ret2) == 0);
    // Make sure we can still get "thequickbrownfoxjumpsoverthelazydog" if we want to.
    s = 36;
    const char* ret3 = test_cache.get("key0", s);
    REQUIRE(ret3 != nullptr);
    REQUIRE(s == 36);
    REQUIRE(strcmp(charr0, ret3) == 0);
    test_cache.reset();
}


TEST_CASE( "test_del", "[placeholder]" ) {
    // Short test
    char charr0[] = "thequickbrownfoxjumpsoverthelazydog";
    test_cache.set("key0", charr0, strlen(charr0) + 1);
    uint32_t s = 42;
    test_cache.del("key0");
    REQUIRE(test_cache.get("key0", s) == nullptr);
    test_cache.reset();
}

TEST_CASE( "test_space_used", "[placeholder]" ) {
    REQUIRE(test_cache.space_used() == 0);
    char charr0[] = "thequickbrownfoxjumpsoverthelazydog";
    test_cache.set("key0", charr0, strlen(charr0) + 1);      // Include the null terminator in all sizes.
    REQUIRE(test_cache.space_used() == strlen(charr0) + 1);
    // Make sure we do a deep copy by ending the string early
    auto key0_size = strlen(charr0) + 1;
    charr0[3] = '\0';
    REQUIRE(test_cache.space_used() == key0_size);
    charr0[3] = 'q';
    char charr1[] = "SomeMaySayImADreamerButImNot";
    auto key1_size = strlen(charr1) + 1;
    // Make sure space adds linearly
    test_cache.set("key1", charr1, strlen(charr1) + 1);
    REQUIRE(test_cache.space_used() == key0_size + key1_size);
    // Make sure the old value gets overwritten
    char charr2[] = "GivingUpIsThePastOfLeastResistance";
    test_cache.set("key0", charr2, strlen(charr2) + 1);
    key0_size = strlen(charr2) + 1;
    REQUIRE(test_cache.space_used() ==  key0_size + key1_size);
    // Make sure the old value got deleted
    test_cache.del("key1");
    REQUIRE(test_cache.space_used() == key0_size);
    test_cache.reset();
}

TEST_CASE( "test_reset", "[placeholder]" ) {
    // Give our test_cache some keys and values
    char charr0[] = "thequickbrownfoxjumpsoverthelazydog";
    test_cache.set("key0", charr0, strlen(charr0) + 1);
    char charr1[] = "SomeMaySayImADreamerButImNot";
    test_cache.set("key1", charr1, strlen(charr1) + 1);
    char charr2[] = "GivingUpIsThePastOfLeastResistance";
    test_cache.set("key2", charr2, strlen(charr2) + 1);
    // Reset it
    test_cache.reset();
    // Make sure it got deleted. (This assumes test_space_used is working properly).
    REQUIRE(test_cache.space_used() == 0);
    // I don't recall what the desired behavior is for reset if there is nothing in the cache. Let's say it just deletes nothing.
    test_cache.reset();
    REQUIRE(test_cache.space_used() == 0);
    test_cache.reset();
}


