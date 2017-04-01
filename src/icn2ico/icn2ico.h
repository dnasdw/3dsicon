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

#if SDW_PLATFORM != SDW_PLATFORM_WINDOWS
struct BITMAPINFOHEADER
{
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
} SDW_GNUC_PACKED;
#endif
#include SDW_MSC_POP_PACKED

#ifndef BI_RGB
#define BI_RGB 0L
#endif

#endif	// ICN2ICO_ICN2ICO_H_
