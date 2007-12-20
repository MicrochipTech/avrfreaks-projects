///////
//
//desc: Simple onverts 4bpp bmp file to a header file.
//note: BMP file must have appropriate color palete (pallette.bmp)
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

  if(argc != 2)
  {
 	printf("Usage: tile2h.exe  [input file name]");
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


  if(bmpHeader.bitsPerPixel != 4 && bmpHeader.bitsPerPixel != 24)
  {
	printf("Only 4bpp bitmaps are supported!");
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


  unsigned int ap = (bmpHeader.width * bmpHeader.bitsPerPixel) / 8;
  unsigned char *ptr = buf+(bmpHeader.height-1)*ap;


  if(bmpHeader.bitsPerPixel == 4)
   for(i=0;i<bmpHeader.height;i++)
   {
      unsigned char* ptr2 = ptr;
      for(x=0;x<ap;x++) fprintf(f2, "0x%02X, ", *ptr2++);
      fprintf(f2, "\\\n");
      ptr -= ap;
   }




 else
  for(i=0;i<bmpHeader.height;i++)
  {
     unsigned char* ptr2 = ptr;
     unsigned char pix=0;

     for(x=0;x<bmpHeader.width;x++)
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
     fprintf(f2, "\\\n");
     ptr -= ap;
  }


  fprintf(f2, "}\n");

  free(buf);
  fclose(f2);

  printf("Done.");
  exit(0);
}


