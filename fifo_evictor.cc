/*
 * Implementation for FIFO_evictor class
 */

#include "fifo_evictor.hh"


Fifo_evictor::Fifo_evictor() {
    my_queue_ = std::queue<key_type>();
}
void Fifo_evictor::touch_key(const key_type& key) {
    // From part 6 of the prompt:
    //""" It's OK to have multiple copies of the same key in the FIFO queue"""
    my_queue_.push(key);
}
const key_type Fifo_evictor::evict() {
    if (my_queue.empty()) {
        return "";
    }
    const key_type retval = my_queue_.front();
    my_queue_.pop();
    return retval;
}




