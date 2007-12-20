///////
//
//desc: Converts tile set (24bpp bmp file) generated with TILESTUDIO to a header file.
//
//
///////


#include <stdio.h>

typedef unsigned char  BYTE;    //1byte
typedef unsigned int   DWORD;   //4bytes
typedef unsigned short WORD;    //2bytes

typedef struct tagBmpHeader
{
	BYTE  magic[2];       	 /* = "BM", it is matipulated separately to make the size of this structure the multiple of 4, */
       	DWORD fileSize;	         /* = offset + imageSize */
	DWORD reserved;     	 /* = 0 */
	DWORD offset;       	 /* offset from start of file = 54 + size of palette */
	DWORD structSize;        /* 40 */
	DWORD width;
        DWORD height;
	WORD  planes;        	 /* 1 */
	WORD  bitsPerPixel;   	 /* bits of each pixel, 256color it is 8, 24bit color it is 24 */
	DWORD compression;       /* 0 */
	DWORD imageSize; 	 /* fileSize - offset */
	DWORD xPixelsPerMeter; 	 /* resolution in mspaint */
	DWORD yPixelsPerMeter;
	DWORD usedColors;    	 /* if use all, it is 0 */
	DWORD colorsImp;	 /*  */
} __attribute__((packed));



int main( int argc, char *argv[] )
{
  FILE *f,*f2;
  unsigned int x, y, i;

  if(argc < 2 || argc > 3)
  {
 	printf("Usage: tile2h.exe  [input file name]  [number of blocks to convert]");
 	return 1;
  }

  f = fopen(argv[1], "rb");
  if(!f)
  {
 	printf("Cannot open %s!",argv[1]);
 	return 1;
  }

  struct tagBmpHeader bmpHeader;
  fread(&bmpHeader, sizeof(struct tagBmpHeader), 1, f);
  if(strncmp(bmpHeader.magic, "BM", 2))
  {
	printf("Not an BMP file!");
	exit(1);
  }

  if(bmpHeader.structSize != 40)
  {
	printf("BMP header corrupted!");
	exit(1);
  }


  fseek(f, 0, SEEK_END);
  unsigned long size = ftell(f);
  if(size != bmpHeader.fileSize)
  {
	printf("Incorrect BMP file size!");
	exit(1);
  }


  if(bmpHeader.imageSize != size - bmpHeader.offset)
  {
	printf("Incorrect BMP image size!");
	exit(1);
  }


  if(bmpHeader.bitsPerPixel != 24)
  {
	printf("Only 24bpp bitmaps are supported!");
	exit(1);
  }

  unsigned char *buf = (unsigned char*) malloc(bmpHeader.imageSize);
  if(!buf)
  {
	printf("Not enough memory to read BMP data!");
	exit(1);
  }
  fseek(f, bmpHeader.offset, SEEK_SET);
  fread(buf, 1, bmpHeader.imageSize, f);
  fclose(f);


  *((unsigned char*)strchr(argv[1], '.')) = 0;
  char str[128];
  strcpy(str, argv[1]);
  strcat(str,".h");
  f2 = fopen(str, "w");
  if(!f2)
  {
	printf("Cannot create %s!", str);
 	return 1;
  }
  fprintf(f2, "#define %s { \\\n", strupr(argv[1]));
  



  unsigned int maxblocks = 0xff;
  if(argc == 3) maxblocks = atoi(argv[2]);

  unsigned int ap = (bmpHeader.width * bmpHeader.bitsPerPixel) / 8;
  unsigned int cp = bmpHeader.bitsPerPixel;

  unsigned char *ptr = buf + (bmpHeader.height-1) * ap;
  unsigned char *ptr0, *ptr1, *ptr2;

  unsigned char cols = bmpHeader.width / 8;
  unsigned char rows = bmpHeader.height / 8;

  unsigned int n=0;

  do
  {
   ptr0 = ptr;
   for(i=0;i<cols;i++)
   {
    fprintf(f2," /* block : x%02X */ \\\n", n);
    ptr1 = ptr0;
    for(y=0;y<8;y++)
    {
     ptr2 = ptr1;
     unsigned char pix = 0;
     for(x=0;x<8;x++)        //pro 4bpp, x by slo jen do 4, jinak vse stejne
     {
       unsigned char r=*ptr2++;
       if(r >= 192) pix |= 0x09;
       else if(r >= 128) pix |= 0x01;
       else if(r >= 64) pix |= 0x08;

       r=*ptr2++;
       if(r >= 192) pix |= 0x0A;
       else if(r >= 128) pix |= 0x02;
       else if(r >= 64) pix |= 0x08;

       r=*ptr2++;
       if(r >= 192) pix |= 0x0C;
       else if(r >= 128) pix |= 0x04;
       else if(r >=  64) pix |= 0x08;

       if(x&1)
       {
	fprintf(f2, "0x%02X, ", pix);
	pix=0;
       }
       else pix <<= 4;

     }
     fprintf(f2, " \\\n");
     ptr1 -= ap;
    }
    ptr0 += cp;

    if(++n >= maxblocks) goto quit;
   }
   ptr -= ap*8;
  }
  while(--rows);


quit:
  fprintf(f2, "}\n");

  free(buf);
  fclose(f2);

  printf("%d blocks done.", n);

  exit(0);
}


