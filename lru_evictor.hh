/*
 * Declare interface for LRU evictor
 */

#pragma once

#include "evictor.hh"
#include <map>

// Define our own node struct, which we will use in the implementation.
struct node {
    node* previous;
    node* next;
    key_type payload;
};

// Got this from https://stackoverflow.com/questions/19079534/how-to-correctly-extend-a-class-in-c-and-writing-its-header-file.
// https://docs.microsoft.com/en-us/cpp/cpp/public-cpp?view=vs-2019 says this means "the public and protected members of the base class are public and protected members, respectively, of the derived class", which sounds fine.
class Lru_evictor : public Evictor {
  public:
    Lru_evictor();
    ~Lru_evictor();
    void touch_key(const key_type& key);
    const key_type evict();
  private:
    node* head_;
    node* tail_;
    std::map<key_type, node*> pointer_lookup_;
};

