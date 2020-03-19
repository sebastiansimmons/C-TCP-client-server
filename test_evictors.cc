/*
 * Test cache_lib.cc
 */

#include "cache.hh"
#include <assert.h>
#include <string.h>
#include <iostream>
#include "fifo_evictor.hh"
#include "lru_evictor.hh"

void test_fifo_evictor() {
    Fifo_evictor* test_evictor = new Fifo_evictor();
    Cache test_cache(1024, 0.75, test_evictor);
    char charr0[] = "In this assignment, you will write a generic look-aside cache. A look-aside cache is a key-valueSTOPstorage for items that are difficult or slow to compute. Whenever a client code requires such anSTOPitem, it first queries the cache for the desired key. If the cache has it, it returns the associatedSTOPvalue. If it doesn't, the client obtains the value on its own (presumably through a slower process),STOPand then saves it to the cache for future references. If the data access patterns exhibit temporalSTOPlocality, the use of the cache will end up being beneficial and faster than always computing theSTOPvalues. Your job will be to implement the cache. In C++, write a cache_lib.cc file to complementSTOP";
    test_cache.set("key0", charr0, 712);
    char charr1[] = "cache.hh. You may not modify any of the .hh files provided. Please read through this entireSTOPassignment before you start implementing, to save yourself the trouble of redesigning the codeSTOPlater.STOPThis is a paired assignment. You are required to find a partner and work together, in particularSTOPcollaborating on the design and the debugging. Your github repo should include all the source files,STOPyour complete tests, a Makefile or build script, and a README.md with your design choices.";
    test_cache.set("key1", charr1, 499);
    uint32_t s = 712;
    assert(test_cache.get("key0", s) == nullptr);
    s = 499;
    assert(test_cache.get("key1", s) != nullptr);
    assert(strcmp(charr1, test_cache.get("key1", s)) == 0);
    assert(test_cache.space_used() == 499);
    char charr2[] = "You will implement your cache as a RAM-based storage of values, indexed by keys. All keys andSTOPvalues will reside in main memory. The cache needs to conform to the interface (API, or contract),STOPthat's provided in the attached cache.hh header file. This API abstracts away the implementation";
    test_cache.set("key2", charr2, 297);
    assert(test_cache.get("key1", s) != nullptr);
    s = 297;
    assert(test_cache.get("key2", s) != nullptr);
    assert(test_cache.space_used() == 499+297);
    delete test_evictor;
}

void test_lru_evictor_basic() {
    Lru_evictor* test_evictor = new Lru_evictor();  // A fun exercise for the reader: replace this with a Fifo_evictor and watch it fail.
    Cache test_cache(1024, 0.75, test_evictor);
    char charr0[] = "In this assignment, you will write a generic look-aside cache. A look-aside cache is a key-valueSTOPstorage for items that are difficult or slow to compute. Whenever a client code requires such anSTOPitem, it first queries the cache for the desired key. If the cache has it, it returns the associatedSTOPvalue. If it doesn't, the client obtains the value on its own (presumably through a slower process),STOPand then saves it to the cache for future references. If the data access patterns exhibit temporalSTOPlocality, the use of the cache will end up being beneficial and faster than always computing theSTOPvalues. Your job will be to implement the cache. In C++, write a cache_lib.cc file to complementSTOP";
    test_cache.set("key0", charr0, 712);
    char charr1[] = "cache.hh. You may not modify any of the .hh files provided. Please read through this entireSTOPassignment before you start implementing, to save yourself the trouble of redesigning the codeSTOPlater.STOPThis is a paired assignment. You are required to find a partner and work together, in particularSTOPcollaborating on the design and the debugging. Your github repo should include all the source files,STOPyour complete tests, a Makefile or build script, and a README.md with your design choices.";
    test_cache.set("key1", charr1, 499);
    uint32_t s = 712;
    assert(test_cache.get("key0", s) == nullptr);
    s = 499;
    assert(test_cache.get("key1", s) != nullptr);
    assert(test_cache.space_used() == 499);
    char charr2[] = "You will implement your cache as a RAM-based storage of values, indexed by keys. All keys andSTOPvalues will reside in main memory. The cache needs to conform to the interface (API, or contract),STOPthat's provided in the attached cache.hh header file. This API abstracts away the implementation";
    test_cache.set("key2", charr2, 297);
    assert(test_cache.get("key1", s) != nullptr);
    s = 297;
    assert(test_cache.get("key2", s) != nullptr);
    assert(test_cache.space_used() == 499+297);
    s = 499;
    // Now add some tests which FIFO will not pass.
    char charr3[] = "the quick brown fox jumps over the lazy dog";
    test_cache.set("key1", charr3, 44);
    assert(test_cache.space_used() == 44+297);
    test_cache.get("key1",s);
    test_cache.set("key0", charr0, 712);
    assert(test_cache.get("key2", s) == nullptr);
    assert(test_cache.get("key1", s) != nullptr);
    delete test_evictor;
}

int main() {
    test_fifo_evictor();
    test_lru_evictor_basic();
}

