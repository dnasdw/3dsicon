#ifndef UTILITY_H_
#define UTILITY_H_

#define COMPILER_MSC  1
#define COMPILER_GNUC 2

#if defined(_MSC_VER)
#define _3DSICON_COMPILER COMPILER_MSC
#define _3DSICON_COMPILER_VERSION _MSC_VER
#else
#define _3DSICON_COMPILER COMPILER_GNUC
#define _3DSICON_COMPILER_VERSION (__GNUC__ * 10000 + __GNUC_MINOR__ * 100 + __GNUC_PATCHLEVEL__)
#endif

#if _3DSICON_COMPILER == COMPILER_MSC
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#endif
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

using namespace std;

typedef int8_t n8;
typedef int16_t n16;
typedef int32_t n32;
typedef int64_t n64;
typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

#if _3DSICON_COMPILER == COMPILER_MSC
#define MSC_PUSH_PACKED <pshpack1.h>
#define MSC_POP_PACKED <poppack.h>
#define GNUC_PACKED
#else
#define MSC_PUSH_PACKED <stdlib.h>
#define MSC_POP_PACKED <stdlib.h>
#define GNUC_PACKED __attribute__((packed))
struct BITMAPINFOHEADER{
	u32 biSize;
	n32 biWidth;
	n32 biHeight;
	u16 biPlanes;
	u16 biBitCount;
	u32 biCompression;
	u32 biSizeImage;
	n32 biXPelsPerMeter;
	n32 biYPelsPerMeter;
	u32 biClrUsed;
	u32 biClrImportant;
} GNUC_PACKED;
#endif

#define DNA_ARRAY_COUNT(a) (sizeof(a) / sizeof(a[0]))

#endif	// UTILITY_H_
