//	GRAPH.C

#include <stdio.h>
#include <conio.h>
#include <graphics.h>
#include <stdlib.h>
#include <dos.h>

#define EVER ; ;
#define PAGE_SIZE 128
#define START_PAGE 25
#define FILE_OFFSET (PAGE_SIZE*START_PAGE)
#define SKIP_TEMP 4
#define SKIP_LDR 2
#define TOP_MARGIN 10
#define LINE_WIDTH THICK_WIDTH  //NORM_WIDTH
#define BOTTOM (getmaxy()-TOP_MARGIN)
#define TOP TOP_MARGIN
#define LABEL_POS BOTTOM-170


const char* month[12] = { "JAN","FEB","MAR","APR","MAY","JUN",
			  "JUL","AUG","SEP","OCT","NOV","DEC"};


static int DrawGraph(FILE* fpIn,float X_SCALE);

void Draw(const char* infile)
{
	FILE* fpIn=NULL;
	float X_SCALE;
	int graphdriver = DETECT, graphmode;
	int prompt;
	if( (fpIn = fopen(infile,"rb")) == NULL) {
		printf("Error in opening file '%s' for reading..\n",infile);
		getch();
		return;
	}

	initgraph(&graphdriver,&graphmode,"d:\\DEVELO~1\\CPP\\bgi");
	X_SCALE = 0.5;

	do {

	    prompt = DrawGraph(fpIn,X_SCALE);
	    //fflush(stdin);

	    switch (prompt) {

		case '=':
		case '+': X_SCALE += (X_SCALE < 4)? X_SCALE/3:0;  break;
		case '-':
		case '_': X_SCALE -= (X_SCALE > 0.2)? X_SCALE/3:0; break;
		default: break;

	    }
	    //fpIn = fopen(infile,"rb");


	} while(prompt != '\r');

	fclose(fpIn);
	closegraph();

}

static int DrawGraph(FILE* fpIn,float X_SCALE)
{

	int time_init_hh,time_init_mm,time_end_hh,time_end_mm;
	int key,date_init,month_init,page_count;
	int xVal,yVal,xTemp1,xTemp2,yTemp1,yTemp2;

	unsigned int w,count;
	const float TEMP_SCALE = (float)460/4000;
	const float LDR_SCALE = (float)460/1024 ;
	char date_string[20],hour[3],number[5],time_range[25];
	struct date today;

	//initialise graphics


	// Draw axes and labels
	count = 0;
	setlinestyle(SOLID_LINE,1,NORM_WIDTH);

	for( yVal=BOTTOM; yVal>=0; count++,yVal-=(getmaxy()+1-2*TOP_MARGIN)/8) {

		setcolor(DARKGRAY);
		line(0,yVal,getmaxx(),yVal); //horizontal gridlines
		setcolor(LIGHTRED);
		settextjustify(LEFT_TEXT,TOP_TEXT);
		settextstyle(SMALL_FONT,HORIZ_DIR,5);
		itoa(count*5,number,10);
		outtextxy(5,yVal-8,number); //text 5 pixels from left edge
		setcolor(LIGHTBLUE);
		settextjustify(RIGHT_TEXT,TOP_TEXT);
		itoa(1024-count*128,number,10);
		outtextxy(getmaxx(),yVal-8,number); // 8 is distance to keep number along grid line

	}

	//draw the graph
	count=0;   //<debug lesson> essential (avoid garbage value!)
	page_count = START_PAGE;
	xVal=yVal=0;
	xTemp1=xTemp2=yTemp1=yTemp2=0;
	fseek(fpIn,FILE_OFFSET,SEEK_SET);
	month_init = getc(fpIn);
	date_init = getc(fpIn);
	time_init_hh = getc(fpIn);
	time_init_mm = getc(fpIn);
	fseek(fpIn,-2,SEEK_CUR);

	for(EVER)
	{

		time_end_hh = getc(fpIn); //skip hh
		time_end_mm = getc(fpIn); //and mm

		if(time_end_mm == 0x00) {  //every hour

		    setlinestyle(SOLID_LINE,1,NORM_WIDTH);
		    setcolor(DARKGRAY);
		    line(xVal*X_SCALE,0,xVal*X_SCALE,getmaxy()); //vertical gridlines
		    sprintf(hour,"%d",time_end_hh);
		    setcolor(YELLOW);
		    settextstyle(SMALL_FONT,HORIZ_DIR,5);
		    settextjustify(CENTER_TEXT,TOP_TEXT);
		    outtextxy(xVal*X_SCALE,0,hour);

		}
		setlinestyle(SOLID_LINE,1,LINE_WIDTH);
		w = getw(fpIn);
		yVal = w*TEMP_SCALE;

		if(xVal % SKIP_TEMP == 0) {
		    setcolor(RED);
		    line(xTemp1, BOTTOM-yTemp1, xVal*X_SCALE, BOTTOM-yVal);
		    xTemp1 = xVal*X_SCALE;
		    yTemp1 = yVal;
		}

		w = getw(fpIn);
		yVal = w*LDR_SCALE;
		if(xVal % SKIP_LDR == 0) {
		    setcolor(BLUE);
		    line(xTemp2, yTemp2+TOP_MARGIN, xVal*X_SCALE, yVal+TOP_MARGIN);
		    yTemp2 = yVal;
		    xTemp2 = xVal*X_SCALE;
		}

		if(++count==21) {  //end of one page (21 readings)
			page_count++;
			fseek(fpIn,page_count*PAGE_SIZE,SEEK_SET);  // skip to next page
			count = 0;
		}
		if( (w=getc(fpIn)) == 0xFF) //an 0xFF in a page indicates end of valid readings
			break;
		else
			fseek(fpIn,-1,SEEK_CUR);
		xVal++;

	}

	getdate( (struct date*)&today);
	sprintf(date_string,"%s %02d,%d",month[month_init-1],date_init,today.da_year);
	sprintf(time_range,"%02d:%02d - %02d:%02d",
		time_init_hh,time_init_mm,time_end_hh,time_end_mm);

	setcolor(RED);
	settextstyle(SMALL_FONT,HORIZ_DIR,5);
	outtextxy(getmaxx()/2,LABEL_POS,"TEMP(C)");
	setcolor(BLUE);
	outtextxy(getmaxx()/2,LABEL_POS+15,"LDR READING");
	setcolor(CYAN);
	settextstyle(SMALL_FONT,HORIZ_DIR,7);
	settextjustify(CENTER_TEXT,TOP_TEXT);
	outtextxy(getmaxx()/2,LABEL_POS+40,"RECORDED BETWEEN");
	outtextxy(getmaxx()/2,LABEL_POS+70,time_range);
	outtextxy(getmaxx()/2,LABEL_POS+90,date_string);
	setcolor(BLACK);
	setlinestyle(SOLID_LINE,1,THICK_WIDTH);
	line(0,0,0,getmaxy());
	key = getch();
	cleardevice();
	return key;

}