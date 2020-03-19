/*
 * Declare interface for FIFO evictor
 */

#pragma once

#include "evictor.hh"
#include <queue>

class Fifo_evictor : public Evictor {
  public:
    Fifo_evictor();
    void touch_key(const key_type& key);
    const key_type evict();
 private:
    std::queue<key_type> my_queue_;
};

