/*
tcp_server.cc 
*/
#include <iostream>
#include <string>
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char **argv){
    int opt,m,p,t;
    std::string s;
    while ((opt = getopt(argc,argv,"m:s:p:t:d")) != EOF)
        switch(opt)
        {
            case 'm': m = 1; std::cout << "cache size set to "<< optarg << std::endl ; break;
            case 's': s = 1; std::cout << "running server on "<< optarg << std::endl ; break;
            case 'p': p = 1; std::cout << "with port "<< optarg << std::endl ; break;
            case 't': t = 1; std::cout << "on "<< optarg << " threads" << std::endl ; break;
            default: /* '?' */
                   fprintf(stderr, "Usage: %s [-m <bytes>] maxmem [-s <ip>] server ip [-p <int>] port number [-t <int>] number of threads\n",
                           argv[0]);
                   exit(EXIT_FAILURE);
        }

    return 0;
}