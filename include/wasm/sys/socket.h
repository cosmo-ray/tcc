#ifndef _SYS_SOCKET_H
#define _SYS_SOCKET_H
#include "wasm-libc.h"
typedef int socklen_t;
struct sockaddr { unsigned short sa_family; char sa_data[14]; };
#endif