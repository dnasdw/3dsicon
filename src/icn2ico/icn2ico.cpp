#include <Windows.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

typedef int8_t n8;
typedef int16_t n16;
typedef int32_t n32;
typedef int64_t n64;
typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

#include <pshpack1.h>
struct IcoHeader
{
	u16 Reserved;
	u16 Type;
	u16 Count;
};

struct IcoInfo
{
	u8 Width;
	u8 Height;
	u8 Color;
	u8 Reserved3[3];
	u8 BitCount;
	u8 Reserved7;
	u32 Size;
	u32 Offset;
};
#include <poppack.h>

static const int decodeTransByte[64] =
{
	 0,  1,  4,  5, 16, 17, 20, 21,
	 2,  3,  6,  7, 18, 19, 22, 23,
	 8,  9, 12, 13, 24, 25, 28, 29,
	10, 11, 14, 15, 26, 27, 30, 31,
	32, 33, 36, 37, 48, 49, 52, 53,
	34, 35, 38, 39, 50, 51, 54, 55,
	40, 41, 44, 45, 56, 57, 60, 61,
	42, 43, 46, 47, 58, 59, 62, 63
};

int decode(u8* buffer, int width, int height, u8* pixel)
{
	u8* temp = new u8[width * height * 2];
	for (int i = 0; i < width * height / 64; i++)
	{
		for (int j = 0; j < 64; j++)
		{
			for (int k = 0; k < 2; k++)
			{
				temp[(i * 64 + j) * 2 + k] = buffer[(i * 64 + decodeTransByte[j]) * 2 + k];
			}
		}
	}
	u8* rgba = new u8[width * height * 2];
	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
			for (int k = 0; k < 2; k++)
			{
				rgba[(i * width + j) * 2 + k] = temp[(((i / 8) * (width / 8) + j / 8) * 64 + i % 8 * 8 + j % 8) * 2 + k];
			}
		}
	}
	delete[] temp;
	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
			u16 rgb565 = *reinterpret_cast<u16*>(rgba + ((height - 1 - i) * width + j) * 2);
			pixel[(i * width + j) * 4] = (rgb565 << 3 & 0xF8) | (rgb565 >> 2 & 7);
			pixel[(i * width + j) * 4 + 1] = (rgb565 >> 3 & 0xFC) | (rgb565 >> 9 & 3);
			pixel[(i * width + j) * 4 + 2] = (rgb565 >> 8 & 0xF8) | (rgb565 >> 13 & 7);
			pixel[(i * width + j) * 4 + 3] = 0;
		}
	}
	memset(pixel + width * height * 4, 0, ((width + 7) / 8 + 3) / 4 * 4 * height);
	delete[] rgba;
	return 0;
}

int main(int argc, char* argv[])
{
	if (argc != 3)
	{
		return 1;
	}
	FILE* fp = fopen(argv[1], "rb");
	if (fp == nullptr)
	{
		return 1;
	}
	fseek(fp, 0, SEEK_END);
	int icnSize = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	u8* icn = new u8[icnSize];
	fread(icn, 1, icnSize, fp);
	fclose(fp);
	u8* icon[2] = { icn + 0x24C0, icn + 0x2040 };
	fp = fopen(argv[2], "wb");
	IcoHeader icoHeader;
	icoHeader.Reserved = 0;
	icoHeader.Type = 1;
	icoHeader.Count = 2;
	fwrite(&icoHeader, sizeof(icoHeader), 1, fp);
	IcoInfo icoInfo[2];
	memset(icoInfo, 0, sizeof(icoInfo));
	icoInfo[0].Width = 48;
	icoInfo[0].Height = 48;
	icoInfo[0].Reserved3[1] = 1;
	icoInfo[0].BitCount = 32;
	icoInfo[0].Size = sizeof(BITMAPINFOHEADER) + icoInfo[0].Width * icoInfo[0].Height * icoInfo[0].BitCount / 8 + ((icoInfo[0].Width + 7) / 8 + 3) / 4 * 4 * icoInfo[0].Height;
	icoInfo[0].Offset = sizeof(icoHeader) + sizeof(icoInfo);
	icoInfo[1].Width = 24;
	icoInfo[1].Height = 24;
	icoInfo[1].Reserved3[1] = 1;
	icoInfo[1].BitCount = 32;
	icoInfo[1].Size = sizeof(BITMAPINFOHEADER) + icoInfo[1].Width * icoInfo[1].Height * icoInfo[1].BitCount / 8 + ((icoInfo[1].Width + 7) / 8 + 3) / 4 * 4 * icoInfo[1].Height;
	icoInfo[1].Offset = icoInfo[0].Offset + icoInfo[0].Size;
	fwrite(icoInfo, sizeof(icoInfo), 1, fp);
	BITMAPINFOHEADER bitmapInfoHeader[2];
	for (int i = 0; i < 2; i++)
	{
		bitmapInfoHeader[i].biSize = sizeof(bitmapInfoHeader[i]);
		bitmapInfoHeader[i].biWidth = icoInfo[i].Width;
		bitmapInfoHeader[i].biHeight = icoInfo[i].Height * 2;
		bitmapInfoHeader[i].biPlanes = 1;
		bitmapInfoHeader[i].biBitCount = icoInfo[i].BitCount;
		bitmapInfoHeader[i].biCompression = BI_RGB;
		bitmapInfoHeader[i].biSizeImage = icoInfo[i].Size - sizeof(BITMAPINFOHEADER);
		bitmapInfoHeader[i].biXPelsPerMeter = 0;
		bitmapInfoHeader[i].biYPelsPerMeter = 0;
		bitmapInfoHeader[i].biClrUsed = 0;
		bitmapInfoHeader[i].biClrImportant = 0;
		u8* pixel = new u8[bitmapInfoHeader[i].biSizeImage];
		fwrite(bitmapInfoHeader + i, sizeof(bitmapInfoHeader[i]), 1, fp);
		decode(icon[i], icoInfo[i].Width, icoInfo[i].Height, pixel);
		fwrite(pixel, 1, bitmapInfoHeader[i].biSizeImage, fp);
		delete[] pixel;
	}
	fclose(fp);
	delete[] icn;
	return 0;
}
