#ifndef _NETDB_H
#define _NETDB_H
#include "wasm-libc.h"
struct hostent { char *h_name; char **h_aliases; int h_addrtype; int h_length; char **h_addr_list; };
struct addrinfo { int ai_flags; int ai_family; int ai_socktype; int ai_protocol; unsigned int ai_addrlen; struct sockaddr *ai_addr; char *ai_canonname; struct addrinfo *ai_next; };
#endif