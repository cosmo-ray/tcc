#ifndef _ARPA_INET_H
#define _ARPA_INET_H
#include "wasm-libc.h"
unsigned int htonl(unsigned int);
unsigned short htons(unsigned short);
void *inet_addr(const char *);
#endif