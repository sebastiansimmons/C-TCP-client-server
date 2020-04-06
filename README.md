# cscihw4

Sebastian Simmons

## Server Design

The server keeps a cache with an LRU evictor to manage the cache. I wrote a simple curl script to test the server without a client during development. That script sent various requests

## Client Design

The client has a private function send_request(method, target) which sends the appropiate request to a target address. 

## Network Tests

When developing I ran out the server on localhost on my virtual machine which worked flawlessly. Running the server on a raspberry pi on the same router was also successful as I was able to communicate with the server from my laptop. I was not able to communicate with the server outside of the same router. I think I needed to change something on my router but did not figure it out. 
