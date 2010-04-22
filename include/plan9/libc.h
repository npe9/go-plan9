/* pragma	lib	"/usr/sape/src/go/lib/$M/libposix.a" */
/* pragma	src	"/sys/src/libc" */

#include "/sys/include/libc.h"

typedef long long s64int;
typedef char s8int;
typedef short s16int;
typedef long intptr;
typedef long s32int;

typedef s8int int8;
typedef u8int uint8;
typedef s16int int16;
typedef u16int uint16;
typedef s32int int32;
typedef u32int uint32;
typedef s64int int64;
typedef u64int uint64;

#define unlink(x) remove(x)
