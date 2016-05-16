#ifndef ICN2ICO_ICN2ICO_H_
#define ICN2ICO_ICN2ICO_H_

#include "../utility.h"

#include MSC_PUSH_PACKED
struct SIcoHeader
{
	u16 Reserved;
	u16 Type;
	u16 Count;
} GNUC_PACKED;

struct SIcoInfo
{
	u8 Width;
	u8 Height;
	u8 Color;
	u8 Reserved3[3];
	u8 BitCount;
	u8 Reserved7;
	u32 Size;
	u32 Offset;
} GNUC_PACKED;
#include MSC_POP_PACKED

#endif	// ICN2ICO_ICN2ICO_H_
