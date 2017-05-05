#ifndef ICN2ICO_ICN2ICO_H_
#define ICN2ICO_ICN2ICO_H_

#include <sdw.h>

#include SDW_MSC_PUSH_PACKED
struct SIcoHeader
{
	u16 Reserved;
	u16 Type;
	u16 Count;
} SDW_GNUC_PACKED;

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
} SDW_GNUC_PACKED;
#include SDW_MSC_POP_PACKED

#endif	// ICN2ICO_ICN2ICO_H_
