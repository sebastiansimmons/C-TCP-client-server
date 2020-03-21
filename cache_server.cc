/*
tcp_server.cc 
*/
#include<iostream>
#include<stdio.h>
#include<ctype.h>
#include<stdlib.h>
#include <unistd.h>
using namespace std;

int main(int argc, char **argv){
    int opt,m,p,t;
    string s;
    while ((opt = getopt(argc,argv,"m:s:p:t:d")) != EOF)
        switch(opt)
        {
            case 'm': m = 1; cout << "cache size set to "<< optarg <<endl ; break;
            case 's': s = 1; cout << "running server on "<< optarg <<endl ; break;
            case 'p': p = 1; cout << "with port "<< optarg <<endl ; break;
            case 't': t = 1; cout << "on "<< optarg << " threads" <<endl ; break;
            default: /* '?' */
                   fprintf(stderr, "Usage: %s [-m bytes] maxmem [-s] server ip\n",
                           argv[0]);
                   exit(EXIT_FAILURE);
        }

    return 0;
}