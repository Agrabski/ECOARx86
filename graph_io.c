// graf_io.c : Defines the entry point for the console application.
//

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <math.h>
extern unsigned long byteswap(unsigned long num);

typedef struct
{
	int width, height;
	unsigned char* pImg;
	int cX, cY;
	int col;
} imgInfo;

typedef struct
{
	int x, y;
} Point;

typedef struct
{
	unsigned short bfType; 
	unsigned long  bfSize; 
	unsigned short bfReserved1; 
	unsigned short bfReserved2; 
	unsigned long  bfOffBits; 
	unsigned long  biSize; 
	long  biWidth; 
	long  biHeight; 
	short biPlanes; 
	short biBitCount; 
	unsigned long  biCompression; 
	unsigned long  biSizeImage; 
	long biXPelsPerMeter; 
	long biYPelsPerMeter; 
	unsigned long  biClrUsed; 
	unsigned long  biClrImportant;
	unsigned long  RGBQuad_0;
	unsigned long  RGBQuad_1;
} bmpHdr;

void* freeResources(FILE* pFile, void* pFirst, void* pSnd)
{
	if (pFile != 0)
		fclose(pFile);
	if (pFirst != 0)
		free(pFirst);
	if (pSnd !=0)
		free(pSnd);
	return 0;
}

imgInfo* readBMP(const char* fname)
{
	imgInfo* pInfo = 0;
	FILE* fbmp = 0;
	bmpHdr bmpHead;
	int lineBytes, y;
	unsigned long imageSize = 0;
	unsigned char* ptr;

	pInfo = 0;
	fbmp = fopen(fname, "rb");
	if (fbmp == 0)
		return 0;

	fread((void *) &bmpHead, sizeof(bmpHead), 1, fbmp);
	// par� sprawdze�
	if (bmpHead.bfType != 0x4D42 || bmpHead.biPlanes != 1 ||
		bmpHead.biBitCount != 1 || bmpHead.biClrUsed != 2 ||
		(pInfo = (imgInfo *) malloc(sizeof(imgInfo))) == 0)
		return (imgInfo*) freeResources(fbmp, pInfo->pImg, pInfo);

	pInfo->width = bmpHead.biWidth;
	pInfo->height = bmpHead.biHeight;
	imageSize = (((pInfo->width + 31) >> 5) << 2) * pInfo->height;

	if ((pInfo->pImg = (unsigned char*) malloc(imageSize)) == 0)
		return (imgInfo*) freeResources(fbmp, pInfo->pImg, pInfo);

	// process height (it can be negative)
	ptr = pInfo->pImg;
	lineBytes = ((pInfo->width + 31) >> 5) << 2; // line size in bytes
	if (pInfo->height > 0)
	{
		// "upside down", bottom of the image first
		ptr += lineBytes * (pInfo->height - 1);
		lineBytes = -lineBytes;
	}
	else
		pInfo->height = -pInfo->height;

	// reading image
	// moving to the proper position in the file
	if (fseek(fbmp, bmpHead.bfOffBits, SEEK_SET) != 0)
		return (imgInfo*) freeResources(fbmp, pInfo->pImg, pInfo);

	for (y=0; y<pInfo->height; ++y)
	{
		fread(ptr, 1, abs(lineBytes), fbmp);
		ptr += lineBytes;
	}
	fclose(fbmp);
	return pInfo;
}

int saveBMP(const imgInfo* pInfo, const char* fname)
{
	int lineBytes = ((pInfo->width + 31) >> 5)<<2;
	bmpHdr bmpHead = 
	{
	0x4D42,				// unsigned short bfType; 
	sizeof(bmpHdr),		// unsigned long  bfSize; 
	0, 0,				// unsigned short bfReserved1, bfReserved2; 
	sizeof(bmpHdr),		// unsigned long  bfOffBits; 
	40,					// unsigned long  biSize; 
	pInfo->width,		// long  biWidth; 
	pInfo->height,		// long  biHeight; 
	1,					// short biPlanes; 
	1,					// short biBitCount; 
	0,					// unsigned long  biCompression; 
	lineBytes * pInfo->height,	// unsigned long  biSizeImage; 
	11811,				// long biXPelsPerMeter; = 300 dpi
	11811,				// long biYPelsPerMeter; 
	2,					// unsigned long  biClrUsed; 
	0,					// unsigned long  biClrImportant;
	0x00000000,			// unsigned long  RGBQuad_0;
	0x00FFFFFF			// unsigned long  RGBQuad_1;
	};

	FILE * fbmp;
	unsigned char *ptr;
	int y;

	if ((fbmp = fopen(fname, "wb")) == 0)
		return -1;
	if (fwrite(&bmpHead, sizeof(bmpHdr), 1, fbmp) != 1)
	{
		fclose(fbmp);
		return -2;
	}

	ptr = pInfo->pImg + lineBytes * (pInfo->height - 1);
	for (y=pInfo->height; y > 0; --y, ptr -= lineBytes)
		if (fwrite(ptr, sizeof(unsigned char), lineBytes, fbmp) != lineBytes)
		{
			fclose(fbmp);
			return -3;
		}
	fclose(fbmp);
	return 0;
}

/****************************************************************************************/
imgInfo* InitScreen (int w, int h)
{
	imgInfo *pImg;
	if ( (pImg = (imgInfo *) malloc(sizeof(imgInfo))) == 0)
		return 0;
	pImg->height = h;
	pImg->width = w;
	pImg->pImg = (unsigned char*) malloc((((w + 31) >> 5) << 2) * h);
	if (pImg->pImg == 0)
	{
		free(pImg);
		return 0;
	}
	memset(pImg->pImg, 0xFF, (((w + 31) >> 5) << 2) * h);
	pImg->cX = 0;
	pImg->cY = 0;
	pImg->col = 0;
	return pImg;
}

void FreeScreen(imgInfo* pInfo)
{
	if (pInfo && pInfo->pImg)
		free(pInfo->pImg);
	if (pInfo)
		free(pInfo);
}

imgInfo* SetColor(imgInfo* pImg, int col)
{
	pImg->col = col != 0;
	return pImg;
}

imgInfo* MoveTo(imgInfo* pImg, int x, int y)
{
	if (x >= 0 && x < pImg->width)
		pImg->cX = x;
	if (y >= 0 && y < pImg->height)
		pImg->cY = y;
	return pImg;
}

void InvPixel(imgInfo* pImg, int x, int y)
{
	unsigned char *pPix = pImg->pImg + (((pImg->width + 31) >> 5) << 2) * y + (x >> 3);
	unsigned char mask = 0x80 >> (x & 0x07);

	if (x < 0 || x >= pImg->width || y < 0 || y >= pImg->height)
		return;

	*pPix ^= mask;
}

void InvRect(imgInfo* pImg, Point* pt, int pSize)
{
	int rx = pSize >> 16;
	int ry = pSize & 0xFFFF;
	int i, j;

	for (i=0; i<ry; ++i)
		for (j=0; j<rx; ++j)
			InvPixel(pImg, pt->x + j, pt->y + i);
}

int GetPixel(imgInfo* pImg, int x, int y)
{
	unsigned char *pPix = pImg->pImg + (((pImg->width + 31) >> 5) << 2) * y + (x >> 3);
	unsigned char mask = 0x80 >> (x & 0x07);

	if (x < 0 || x >= pImg->width || y < 0 || y >= pImg->height)
		return 0;

	return (*pPix & mask)!=0; 
}

unsigned long getWord(imgInfo* pImg, unsigned x,unsigned y)
{
	unsigned char* tmp = pImg->pImg + (((pImg->width + 31) >> 5) << 2) * y + x;
	unsigned long t = *((unsigned long*)tmp);
	unsigned long f = byteswap(t);
	//printf("%08x\t%08x\t(%02i,%02i)\t%02i\n",f,t,x,y, (((pImg->width + 31) >> 5) << 2) * y + x);
	return f;
}

int checkPattern(unsigned long buffer, unsigned long mask, int pattern, unsigned n)
{
	unsigned long patternBuffer = ((((unsigned long)pattern))<<n);
	unsigned long shiftedBuffer = (buffer&(mask<<n));
	//printf("%08x\t%08x\t%08x\t%08x\n",buffer,patternBuffer,shiftedBuffer,mask);

	if(  (shiftedBuffer^patternBuffer) ==0  )
	{
		return 1;
	}
	else
	{
		return 0;
	}


}

void findPatterns(imgInfo * pImg, int*patterns,int patternsCount, Point*result, long unsigned int *resultCount)
{
	int patternLength = 7;
	unsigned currentX = 0;
	unsigned currentY = 0;
	unsigned long patternMask = 0;
	for(unsigned i =0; i<patternLength; i++)
		patternMask=(patternMask<<1)|1;
	int widith =  ( ((pImg->width + 7)/8 + 3)/4 ) * 4;
	int actualWidith = (pImg->width + 7)/8;
	int padding =8* widith - pImg->width; 
	printf("\n%x\n",*(pImg->pImg));
	getWord(pImg,0,0);
	getWord(pImg,1,0);
	getWord(pImg,2,0);
	getWord(pImg,3,0);
	getWord(pImg,4,0);
	getWord(pImg,5,0);
		printf("\n%x\n",0);

	do
	{
		int potentialPatternInWordPositions[32] = {-1};
		unsigned potentialIndex = 0;
		unsigned long buffer =getWord(pImg,currentX,currentY);
		for(unsigned n = 0; n<=(sizeof(long))*8-patternLength;n++)
		{
			if(checkPattern(buffer,patternMask,patterns[0],n))
			{
				//printf("(%03i,%03i)\n",currentX*8+n,currentY);
				//printf("%02i\t%08x\t%08x\t%08x\t%02i\t(%03i;%03i)\t%02i\n",*resultCount,buffer,patternBuffer,shiftedBuffer^patternBuffer,potentialIndex,currentX*8+n,currentY,n);
				potentialPatternInWordPositions[potentialIndex] = n;
				potentialIndex++;
			}
			else
				potentialPatternInWordPositions[potentialIndex] =-1;
		}
		for(unsigned i = 1; i<patternsCount; i++)
		{
			unsigned long buffer = getWord(pImg,currentX,(currentY+i));
			for(unsigned n =0;n<potentialIndex;n++)
			{
				if(potentialPatternInWordPositions[n]!=-1)
				{
					printf("%08x\t%02i\tits hit or miss\n",buffer,i);

					if(!checkPattern(buffer,patternMask,patterns[i],potentialPatternInWordPositions[n]))
						{
							potentialPatternInWordPositions[n]=-1;
							printf("miss\n");
						}
					else
					printf("i guess they newer miss huh?\n");

					
				}
			}

		}
		for(unsigned n =0;n<potentialIndex;n++)
		{
			if(potentialPatternInWordPositions[n]!=-1)
			{
				result[*resultCount].x= currentX*8+patternLength-potentialPatternInWordPositions[n]+padding;
				result[*resultCount].y=currentY;
				(*resultCount)++;
			}
		}
		currentX+=3;

		if(currentX>=(widith)-2)
			{
				currentX=0;
				currentY+=1;
			}


	}while(currentY<pImg->height);
}

Point* FindPattern(imgInfo* pImg, int pSize, int* ptrn, Point* pDst, int* fCnt)
{
	int i, j, k, l;
	int mask;
	int rx = pSize >> 16;
	int ry = pSize & 0xFFFF;

	*fCnt = 0;
	for (i=0; i < pImg->height - ry; ++i)
		for (j=0; j < pImg->width - rx; ++j)
		{
			// for a rectangle with upper lefr corner in (i,j)
			// check if there is pattern in image
			for (k=0; k < ry; ++k)
			{
				mask = 1 << (rx - 1);
				for (l=0; l < rx; ++l, mask >>= 1)
					if (GetPixel(pImg, j+l, i+k) != ((ptrn[k] & mask) != 0))
						break;
				if (l < rx) // pattern not found
					break;
			}
			if (k >= ry) //pattern found
			{
				pDst[*fCnt].x = j;
				pDst[*fCnt].y = i;
				++(*fCnt);
			}
		}
	return pDst;
}

/****************************************************************************************/

int main(int argc, char* argv[])
{
	imgInfo* pInfo;
	int pattern[] = { 0x40, 0x3d, 0x3d, 0x3d, 0x41, 0x7d, 0x7d, 0x43 }; 
					// { 0x3f, 0x42, 0x42, 0x42, 0x3e, 0x02, 0x02, 0x3c }; inverted color version
					// it is small g!
	int invPattern[]={0x43,0x73,0x7d,0x41,0x3d,0x3d,0x3d,0x40};
	int pCnt, pSize, i, mask;
	Point *pts;
	if (sizeof(bmpHdr) != 62)
	{
		printf("Change compilation options so as bmpHdr struct size is 62 bytes.\n");
		return 1;
	}
	char f1[12] = "src_001.bmp";
	char f2[17]="small - Copy.bmp";
	pInfo = readBMP(f1);

	printf("width=%i\n height=%i\npImg=%i\ncX=%i\ncY=%i\ncol=%i\n",
			pInfo->width,pInfo->height,pInfo->cX,pInfo->cY,pInfo->col);
	// let's display the pattern 
	for (i=0; i < 8; ++i)
	{
		for (mask=0x40; mask != 0; mask >>= 1)
			printf("%c", (pattern[i] & mask) ? ' ' : '*');
		printf("\n");
	}

	pts = (Point *) malloc(sizeof(Point)*512);

	pSize = (7 << 16) | 8;

	//FindPattern(pInfo, pSize, pattern, pts, &pCnt); 
	// because it is hard to count based on coordinates alone 
	// I invert rectangles found
/*
	printf("Pattern occurences found: %d\n", pCnt);
	for (i=0; i<pCnt; ++i)
	{
		printf("(%d, %d)\n", pts[i].x, pts[i].y);
		InvRect(pInfo, & pts[i], pSize);
	}*/
	saveBMP(pInfo, "result.bmp");

	Point tmp [1000];
	unsigned long resultCount =00;
	for(unsigned char i=0;i<99;i++)
		printf("%02x ",*((pInfo->pImg)+i));
	findPatterns(pInfo,pattern,8,&tmp,&resultCount);

	printf("Pattern occurences found by your algorithm you ding dong: %d\n", resultCount);
	for (i=0; i<resultCount; ++i)
	{
		printf("(%d, %d)\n", tmp[i].x, tmp[i].y);
		InvRect(pInfo, & tmp[i], pSize);
	}

	saveBMP(pInfo, "result_tested.bmp");

	FreeScreen(pInfo);
	free(pts);
	return 0;
}

