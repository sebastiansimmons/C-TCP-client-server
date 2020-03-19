/*
 * Test cache_lib.cc
 */

#include "cache.hh"
#include <assert.h>
#include <string.h>
#include <iostream>
#include "fifo_evictor.hh"
#include "lru_evictor.hh"

void test_set_and_get() {
    Cache test_cache(1024);
    char charr0[] = "the quick brown fox jumps over the lazy dog";
    test_cache.set("key0", charr0, 44);
    // It sure would be nice if the interface defined the second argument
    // to get as a const. But, it was not to be.
    uint32_t s = 44;
    const char* ret = test_cache.get("key0", s);  
    assert(ret != nullptr);
    assert(strcmp(charr0, ret) == 0);
    char charr1[] = "J Q Vandz struck my big fox whelp";
    test_cache.set("key1", charr1, 34);
    s = 34;
    const char* ret2 = test_cache.get("key1", s);
    assert(ret2 != nullptr);
    assert(strcmp(ret, ret2) != 0);
}

void test_del() {
    // Short test
    Cache test_cache(1024);
    char charr0[] = "the quick brown fox jumps over the lazy dog";
    test_cache.set("key0", charr0, 44);
    // The del function has return type bool. It isn't specified what it
    // should return. Let's say it returns true if the key was in the cache
    // and false otherwise.
    assert(test_cache.del("key0"));
    uint32_t s = 42;
    assert(test_cache.get("key0", s) == nullptr);
    assert(test_cache.del("key0") == false);
}

void test_space_used() {
    Cache test_cache(1024);
    assert(test_cache.space_used() == 0);
    char charr0[] = "the quick brown fox jumps over the lazy dog";
    test_cache.set("key0", charr0, 44);      // Include the null terminator in all sizes.
    assert(test_cache.space_used() == 44);
    // Make sure we do a deep copy by ending the string early
    charr0[3] = '\0';
    assert(test_cache.space_used() == 44);
    char charr1[] = "J Q Vandz struck my big fox whelp";
    // Make sure space adds linearly
    test_cache.set("key1", charr1, 34);
    assert(test_cache.space_used() == 44+34);
    // Make sure the old value gets overwritten
    char charr2[] = "Sphinx of black quartz, hear my vow!";
    test_cache.set("key0", charr2, 38);
    assert(test_cache.space_used() == 38+34);
    // Make sure the old value got deleted
    test_cache.del("key1");
    assert(test_cache.space_used() == 38);
}

void test_reset() {
    Cache test_cache(1024);
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
    assert(test_cache.space_used() == 0);
    // I don't recall what the desired behavior is for reset if there is nothing in the cache. Let's say it just deletes nothing.
    test_cache.reset();
    assert(test_cache.space_used() == 0);
}

int main() {
    // For all of these, we assume the previous tests are working properly.
    // This is a good assumption because otherwise assert would exit the program.
    // We decided against an explicit creation/deletion test. We were not sure what
    // we would put in them, and thought valgrind would be better at detecting
    // errors than we would ever be.
    test_set_and_get();
    test_del();
    test_space_used();
    test_reset();
}

