#ifndef _SYS_WAIT_H
#define _SYS_WAIT_H
#include "wasm-libc.h"
#ifndef WEXITSTATUS
#define WIFEXITED(s) (((s) & 0x7f) == 0)
#define WEXITSTATUS(s) (((s) >> 8) & 0xff)
#endif
#endif