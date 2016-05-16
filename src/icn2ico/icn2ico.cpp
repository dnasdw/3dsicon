#include "icn2ico.h"
#include <PVRTextureUtilities.h>

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

n32 decode(u8* pSrc, n32 a_nSrcWidth, n32 a_nSrcHeight, u8* pDest, n32 a_nDestWidth, n32 a_nDestHeight)
{
	u8* pTemp = new u8[a_nSrcWidth * a_nSrcHeight * 2];
	for (u32 i = 0; i < a_nSrcWidth * a_nSrcHeight / DNA_ARRAY_COUNT(s_nDecodeTransByte); i++)
	{
		for (u32 j = 0; j < DNA_ARRAY_COUNT(s_nDecodeTransByte); j++)
		{
			for (n32 k = 0; k < 2; k++)
			{
				pTemp[(i * DNA_ARRAY_COUNT(s_nDecodeTransByte) + j) * 2 + k] = pSrc[(i * DNA_ARRAY_COUNT(s_nDecodeTransByte) + s_nDecodeTransByte[j]) * 2 + k];
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
				pDest[(i * a_nSrcWidth + j) * 4] = (uRGB565 << 3 & 0xF8) | (uRGB565 >> 2 & 7);
				pDest[(i * a_nSrcWidth + j) * 4 + 1] = (uRGB565 >> 3 & 0xFC) | (uRGB565 >> 9 & 3);
				pDest[(i * a_nSrcWidth + j) * 4 + 2] = (uRGB565 >> 8 & 0xF8) | (uRGB565 >> 13 & 7);
				pDest[(i * a_nSrcWidth + j) * 4 + 3] = 0;
			}
		}
	}
	else
	{
		pTemp = new u8[a_nSrcWidth * a_nSrcHeight * 4];
		for (n32 i = 0; i < a_nSrcHeight; i++)
		{
			for (n32 j = 0; j < a_nSrcWidth; j++)
			{
				u16 uRGB565 = *reinterpret_cast<u16*>(pRGB565 + (i * a_nSrcWidth + j) * 2);
				pTemp[(i * a_nSrcWidth + j) * 4] = (uRGB565 << 3 & 0xF8) | (uRGB565 >> 2 & 7);
				pTemp[(i * a_nSrcWidth + j) * 4 + 1] = (uRGB565 >> 3 & 0xFC) | (uRGB565 >> 9 & 3);
				pTemp[(i * a_nSrcWidth + j) * 4 + 2] = (uRGB565 >> 8 & 0xF8) | (uRGB565 >> 13 & 7);
				pTemp[(i * a_nSrcWidth + j) * 4 + 3] = 0xFF;
			}
		}
		PVRTextureHeaderV3 pvrTextureHeaderV3;
		pvrTextureHeaderV3.u64PixelFormat = pvrtexture::PixelType('r', 'g', 'b', 'a', 8, 8, 8, 8).PixelTypeID;
		pvrTextureHeaderV3.u32Height = a_nSrcHeight;
		pvrTextureHeaderV3.u32Width = a_nSrcWidth;
		MetaDataBlock metaDataBlock;
		metaDataBlock.DevFOURCC = PVRTEX3_IDENT;
		metaDataBlock.u32Key = ePVRTMetaDataTextureOrientation;
		metaDataBlock.u32DataSize = 3;
		metaDataBlock.Data = new PVRTuint8[metaDataBlock.u32DataSize];
		metaDataBlock.Data[0] = ePVRTOrientRight;
		metaDataBlock.Data[1] = ePVRTOrientUp;
		metaDataBlock.Data[2] = ePVRTOrientIn;
		pvrtexture::CPVRTextureHeader pvrTextureHeader(pvrTextureHeaderV3, 1, &metaDataBlock);
		pvrtexture::CPVRTexture* pPVRTexture = new pvrtexture::CPVRTexture(pvrTextureHeader, pTemp);
		delete[] pTemp;
		pvrtexture::Resize(*pPVRTexture, a_nDestWidth, a_nDestHeight, 1, pvrtexture::eResizeNearest);
		pTemp = reinterpret_cast<u8*>(pPVRTexture->getDataPtr());
		for (n32 i = 0; i < a_nDestHeight; i++)
		{
			for (n32 j = 0; j < a_nDestWidth; j++)
			{
				pDest[(i * a_nDestWidth + j) * 4] = pTemp[((a_nDestHeight - 1 - i) * a_nDestWidth + j) * 4];
				pDest[(i * a_nDestWidth + j) * 4 + 1] = pTemp[((a_nDestHeight - 1 - i) * a_nDestWidth + j) * 4 + 1];
				pDest[(i * a_nDestWidth + j) * 4 + 2] = pTemp[((a_nDestHeight - 1 - i) * a_nDestWidth + j) * 4 + 2];
				pDest[(i * a_nDestWidth + j) * 4 + 3] = 0;
			}
		}
		delete pPVRTexture;
	}
	delete[] pRGB565;
	memset(pDest + a_nDestWidth * a_nDestHeight * 4, 0, ((a_nDestWidth + 7) / 8 + 3) / 4 * 4 * a_nDestHeight);
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
	n32 nIcnSize = static_cast<n32>(ftell(fp));
	fseek(fp, 0, SEEK_SET);
	u8* pIcn = new u8[nIcnSize];
	fread(pIcn, 1, nIcnSize, fp);
	fclose(fp);
	u8* pCtpk[2] = { pIcn + 0x24C0, pIcn + 0x2040 };
	const n32 nCount = 3;
	n32 nSize[nCount] = { 256, 48, 24 };
	fp = fopen(argv[2], "wb");
	SIcoHeader icoHeader;
	icoHeader.Reserved = 0;
	icoHeader.Type = 1;
	icoHeader.Count = nCount;
	fwrite(&icoHeader, sizeof(icoHeader), 1, fp);
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
