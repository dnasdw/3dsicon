#include "icn2ico.h"
#define STB_IMAGE_RESIZE_IMPLEMENTATION
#define STBIR_DEFAULT_FILTER_UPSAMPLE     STBIR_FILTER_BOX
#define STBIR_DEFAULT_FILTER_DOWNSAMPLE   STBIR_FILTER_BOX
#include <stb_image_resize.h>

static const n32 s_nDecodeTransByte[64] =
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

n32 decode(u8* a_pSrc, n32 a_nSrcWidth, n32 a_nSrcHeight, u8* a_pDest, n32 a_nDestWidth, n32 a_nDestHeight)
{
	u8* pTemp = new u8[a_nSrcWidth * a_nSrcHeight * 2];
	for (u32 i = 0; i < a_nSrcWidth * a_nSrcHeight / SDW_ARRAY_COUNT(s_nDecodeTransByte); i++)
	{
		for (u32 j = 0; j < SDW_ARRAY_COUNT(s_nDecodeTransByte); j++)
		{
			for (n32 k = 0; k < 2; k++)
			{
				pTemp[(i * SDW_ARRAY_COUNT(s_nDecodeTransByte) + j) * 2 + k] = a_pSrc[(i * SDW_ARRAY_COUNT(s_nDecodeTransByte) + s_nDecodeTransByte[j]) * 2 + k];
			}
		}
	}
	u8* pRGB565 = new u8[a_nSrcWidth * a_nSrcHeight * 2];
	n32 nInnerWidth = 8;
	n32 nInnerHeight = 8;
	n32 nOuterWidth = a_nSrcWidth / nInnerWidth;
	n32 nOuterHeight = a_nSrcHeight / nInnerHeight;
	for (n32 i = 0; i < a_nSrcHeight; i++)
	{
		for (n32 j = 0; j < a_nSrcWidth; j++)
		{
			for (n32 k = 0; k < 2; k++)
			{
				pRGB565[(i * nInnerWidth * nOuterWidth + j) * 2 + k] = pTemp[(((i / nInnerHeight) * nOuterWidth + j / nInnerWidth) * (nInnerWidth * nInnerHeight) + i % nInnerHeight * nInnerWidth + j % nInnerWidth) * 2 + k];
			}
		}
	}
	delete[] pTemp;
	if (a_nDestWidth == a_nSrcWidth && a_nDestHeight == a_nSrcHeight)
	{
		for (n32 i = 0; i < a_nSrcHeight; i++)
		{
			for (n32 j = 0; j < a_nSrcWidth; j++)
			{
				u16 uRGB565 = *reinterpret_cast<u16*>(pRGB565 + ((a_nSrcHeight - 1 - i) * a_nSrcWidth + j) * 2);
				a_pDest[(i * a_nSrcWidth + j) * 4] = (uRGB565 << 3 & 0xF8) | (uRGB565 >> 2 & 7);
				a_pDest[(i * a_nSrcWidth + j) * 4 + 1] = (uRGB565 >> 3 & 0xFC) | (uRGB565 >> 9 & 3);
				a_pDest[(i * a_nSrcWidth + j) * 4 + 2] = (uRGB565 >> 8 & 0xF8) | (uRGB565 >> 13 & 7);
				a_pDest[(i * a_nSrcWidth + j) * 4 + 3] = 0xFF;
			}
		}
	}
	else
	{
		u8* pTemp0 = new u8[a_nSrcWidth * a_nSrcHeight * 4];
		for (n32 i = 0; i < a_nSrcHeight; i++)
		{
			for (n32 j = 0; j < a_nSrcWidth; j++)
			{
				u16 uRGB565 = *reinterpret_cast<u16*>(pRGB565 + (i * a_nSrcWidth + j) * 2);
				pTemp0[(i * a_nSrcWidth + j) * 4] = (uRGB565 << 3 & 0xF8) | (uRGB565 >> 2 & 7);
				pTemp0[(i * a_nSrcWidth + j) * 4 + 1] = (uRGB565 >> 3 & 0xFC) | (uRGB565 >> 9 & 3);
				pTemp0[(i * a_nSrcWidth + j) * 4 + 2] = (uRGB565 >> 8 & 0xF8) | (uRGB565 >> 13 & 7);
				pTemp0[(i * a_nSrcWidth + j) * 4 + 3] = 0xFF;
			}
		}
		pTemp = new u8[a_nDestWidth * a_nDestHeight * 4];
		stbir_resize_uint8(pTemp0, a_nSrcWidth, a_nSrcHeight, 0, pTemp, a_nDestWidth, a_nDestHeight, 0, 4);
		delete[] pTemp0;
		for (n32 i = 0; i < a_nDestHeight; i++)
		{
			for (n32 j = 0; j < a_nDestWidth; j++)
			{
				a_pDest[(i * a_nDestWidth + j) * 4] = pTemp[((a_nDestHeight - 1 - i) * a_nDestWidth + j) * 4];
				a_pDest[(i * a_nDestWidth + j) * 4 + 1] = pTemp[((a_nDestHeight - 1 - i) * a_nDestWidth + j) * 4 + 1];
				a_pDest[(i * a_nDestWidth + j) * 4 + 2] = pTemp[((a_nDestHeight - 1 - i) * a_nDestWidth + j) * 4 + 2];
				a_pDest[(i * a_nDestWidth + j) * 4 + 3] = 0xFF;
			}
		}
		delete[] pTemp;
	}
	delete[] pRGB565;
	memset(a_pDest + a_nDestWidth * a_nDestHeight * 4, 0, ((a_nDestWidth + 7) / 8 + 3) / 4 * 4 * a_nDestHeight);
	return 0;
}

int UMain(int argc, UChar* argv[])
{
	if (argc != 3)
	{
		return 1;
	}
	FILE* fp = UFopen(argv[1], USTR("rb"), false);
	if (fp == nullptr)
	{
		return 1;
	}
	fseek(fp, 0, SEEK_END);
	u32 uIcnSize = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	u8* pIcn = new u8[uIcnSize];
	fread(pIcn, 1, uIcnSize, fp);
	fclose(fp);
	u8* pCtpk[2] = { pIcn + 0x24C0, pIcn + 0x2040 };
	const n32 nCount = 3;
	n32 nSize[nCount] = { 256, 48, 24 };
	fp = UFopen(argv[2], USTR("wb"), false);
	if (fp == nullptr)
	{
		delete[] pIcn;
		return 1;
	}
	SIcoHeader icoHeader;
	icoHeader.Reserved = 0;
	icoHeader.Type = 1;
	icoHeader.Count = nCount;
	fwrite(&icoHeader, sizeof(SIcoHeader), 1, fp);
	SIcoInfo icoInfo[nCount];
	memset(icoInfo, 0, sizeof(icoInfo));
	for (n32 i = 0; i < nCount; i++)
	{
		icoInfo[i].Width = nSize[i] & 0xFF;
		icoInfo[i].Height = nSize[i] & 0xFF;
		icoInfo[i].Reserved3[1] = 1;
		icoInfo[i].BitCount = 32;
		icoInfo[i].Size = sizeof(BITMAPINFOHEADER) + nSize[i] * nSize[i] * icoInfo[i].BitCount / 8 + ((nSize[i] + 7) / 8 + 3) / 4 * 4 * nSize[i];
		if (i == 0)
		{
			icoInfo[i].Offset = sizeof(icoHeader) + sizeof(icoInfo);
		}
		else
		{
			icoInfo[i].Offset = icoInfo[i - 1].Offset + icoInfo[i - 1].Size;
		}
	}
	fwrite(icoInfo, sizeof(icoInfo), 1, fp);
	BITMAPINFOHEADER bitmapInfoHeader[nCount];
	for (n32 i = 0; i < nCount; i++)
	{
		bitmapInfoHeader[i].biSize = sizeof(bitmapInfoHeader[i]);
		bitmapInfoHeader[i].biWidth = nSize[i];
		bitmapInfoHeader[i].biHeight = nSize[i] * 2;
		bitmapInfoHeader[i].biPlanes = 1;
		bitmapInfoHeader[i].biBitCount = icoInfo[i].BitCount;
		bitmapInfoHeader[i].biCompression = BI_RGB;
		bitmapInfoHeader[i].biSizeImage = icoInfo[i].Size - sizeof(BITMAPINFOHEADER);
		bitmapInfoHeader[i].biXPelsPerMeter = 0;
		bitmapInfoHeader[i].biYPelsPerMeter = 0;
		bitmapInfoHeader[i].biClrUsed = 0;
		bitmapInfoHeader[i].biClrImportant = 0;
		u8* pBitmap = new u8[bitmapInfoHeader[i].biSizeImage];
		fwrite(bitmapInfoHeader + i, sizeof(bitmapInfoHeader[i]), 1, fp);
		decode(pCtpk[i <= 1 ? 0 : 1], nSize[i <= 1 ? 1 : 2], nSize[i <= 1 ? 1 : 2], pBitmap, nSize[i], nSize[i]);
		fwrite(pBitmap, 1, bitmapInfoHeader[i].biSizeImage, fp);
		delete[] pBitmap;
	}
	fclose(fp);
	delete[] pIcn;
	return 0;
}
