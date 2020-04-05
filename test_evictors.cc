/*
 * Test cache_lib.cc
 */
#define CATCH_CONFIG_MAIN
#include "cache.hh"
#include <string.h>
#include <iostream>
#include "lru_evictor.hh"
#include "catch.hpp"

Lru_evictor* test_evictor_lru = new Lru_evictor();  // A fun exercise for the reader: replace this with a Fifo_evictor and watch it fail.
Cache test_cache_lru(1024, 0.75, test_evictor_lru);

TEST_CASE("test_lru_evictor_basic") {
    char charr0[] = "In this assignment, you will write a generic look-aside cache. A look-aside cache is a key-valueSTOPstorage for items that are difficult or slow to compute. Whenever a client code requires such anSTOPitem, it first queries the cache for the desired key. If the cache has it, it returns the associatedSTOPvalue. If it doesn't, the client obtains the value on its own (presumably through a slower process),STOPand then saves it to the cache for future references. If the data access patterns exhibit temporalSTOPlocality, the use of the cache will end up being beneficial and faster than always computing theSTOPvalues. Your job will be to implement the cache. In C++, write a cache_lib.cc file to complementSTOP";
    test_cache_lru.set("key0", charr0, 712);
    char charr1[] = "cache.hh. You may not modify any of the .hh files provided. Please read through this entireSTOPassignment before you start implementing, to save yourself the trouble of redesigning the codeSTOPlater.STOPThis is a paired assignment. You are required to find a partner and work together, in particularSTOPcollaborating on the design and the debugging. Your github repo should include all the source files,STOPyour complete tests, a Makefile or build script, and a README.md with your design choices.";
    test_cache_lru.set("key1", charr1, 499);
    uint32_t s = 712;
    REQUIRE(test_cache_lru.get("key0", s) == nullptr);
    s = 499;
    REQUIRE(test_cache_lru.get("key1", s) != nullptr);
    REQUIRE(test_cache_lru.space_used() == 499);
    char charr2[] = "You will implement your cache as a RAM-based storage of values, indexed by keys. All keys andSTOPvalues will reside in main memory. The cache needs to conform to the interface (API, or contract),STOPthat's provided in the attached cache.hh header file. This API abstracts away the implementation";
    test_cache_lru.set("key2", charr2, 297);
    REQUIRE(test_cache_lru.get("key1", s) != nullptr);
    s = 297;
    REQUIRE(test_cache_lru.get("key2", s) != nullptr);
    REQUIRE(test_cache_lru.space_used() == 499+297);
    s = 499;
    // Now add some tests which FIFO will not pass.
    char charr3[] = "the quick brown fox jumps over the lazy dog";
    test_cache_lru.set("key1", charr3, 44);
    REQUIRE(test_cache_lru.space_used() == 44+297);
    test_cache_lru.get("key1",s);
    test_cache_lru.set("key0", charr0, 712);
    REQUIRE(test_cache_lru.get("key2", s) == nullptr);
    REQUIRE(test_cache_lru.get("key1", s) != nullptr);
    test_cache_lru.reset();
    delete test_evictor_lru;
}

