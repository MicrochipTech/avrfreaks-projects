/*
    //////////////////////////////////////////////////////////////////////////////////////////
              _____                 _      ______           _        _____ _
             |  __ \               | |     | ___ \         (_)      /  __ \ |
             | |  \/_ __ __ _ _ __ | |__   | |_/ / __ _ ___ _  ___  | /  \/ | __ _ ___ ___
             | | __| '__/ _` | '_ \| '_ \  | ___ \/ _` / __| |/ __| | |   | |/ _` / __/ __|
             | |_\ \ | | (_| | |_) | | | | | |_/ / (_| \__ \ | (__  | \__/\ | (_| \__ \__ \
              \____/_|  \__,_| .__/|_| |_| \____/ \__,_|___/_|\___|  \____/_|\__,_|___/___/
                             | |
                             |_|
    
    //////////////////////////////////////////////////////////////////////////////////////////
    // This Drive running on ATMEGA8,ATMEGA162,ATMEGA128 and atmega328p devices             //
    // Some function need global interrupts must enabled to work properly                   //
    // Compile this library using optimization -os or -o1                                   //
	//////////////////////////////////////////////////////////////////////////////////////////
	------------------------------- Graph Basic Functions C++ (tab 4)-------------------------		  
*/


#ifndef	F_CPU
	#error F_CPU not defined
#else
	#if (F_CPU!=16000000L)
		#error This WS2812 Drive Version work only F_CPU=16000000L
	#endif
#endif

//     +--------------------------------------+
//     |        LOCAL LIBRARY INCLUDES	      |
//     +--------------------------------------+
//

    #include "_GraphBasic.h"

// 
//     +--------------------------------------+
//     |        ASCII CHARACTER TABLE         |
//     +--------------------------------------+
// 

//comments below line to use BIG CHARACTERS instead normal



const char _ASCII_Table[] PROGMEM={
	/////////////////////////////////////
	//// NORMAL CHAR DEFINITIONS
	/////////////////////////////////////
#if defined(_GRAPH_NORMAL_CHAR_SIZE_)
	#include "_ASCII_Table_3x5.h"	
	/////////////////////////////////////
	//// BIG CHAR DEFINITIONS
	/////////////////////////////////////
#elif defined(_GRAPH_BIG_CHAR_SIZE_)
	#include "_ASCII_Table_5x8.h"
#endif
};


//
//     +--------------------------------------+
//     |        LOCAL STRUCTS                 |
//     +--------------------------------------+
//

// 
//         +-------------------------------------------------------+
//         |        CLASS GRAPH BASIC IMPLEMENTATION               |
//         +-------------------------------------------------------+
//         | @PURPOSE      : Implements basic drawing interface    |
//         | @OBSERVATIONS :									   |
//         +-------------------------------------------------------+
//
// 
//         +-----------------------------------------------+
//         |       PRIVATE FUNCTIONS IMPLEMENTATION        |
//         +-----------------------------------------------+
// 

//	+-------------------------------------------------------------------------------+
//	| FUNCTION:  Graph Basic initialization 				                        |
//	| NAME    :  Initialize								                            |
//	| INPUT   :  none								                                |
//	| OUTPUT  :  none								                                |
//	| REMARKS :  									                                |
//	+-------------------------------------------------------------------------------+
void class_GraphBasic::Initialize()
{
    unsigned char sreg;
    size_t  space;
    //Save SREG
    sreg=SREG;
    //Disable Interrupts
    cli();
    //reserve space for ledpixels;
    space=this->Rows*this->Cols*sizeof(Color);
    ColorBuffer=(Color*)malloc(space);
    //Restore Interrupts
    SREG=sreg;
}
//	+-------------------------------------------------------------------------------+
//	| FUNCTION:  Graph Basic Unitialization         		                        |
//	| NAME    :  UnInitialize						                                |
//	| INPUT   :  none							                                    |
//	| OUTPUT  :  none							                                    |
//	| REMARKS :  								                                    |
//	+-------------------------------------------------------------------------------+
void class_GraphBasic::UnInitialize()
{
    unsigned char sreg;

    //Save SREG
    sreg=SREG;
    //Disable Interrupts
    cli();
    //
    free(ColorBuffer);
    //Restore Interrupts
    SREG=sreg;
}

// 
//         +-----------------------------------------------+
//         |       CONSTRUCTORS AND DESTRUCTORS            |
//         +-----------------------------------------------+
// 

//	+-------------------------------------------------------------------------------+
//	| FUNCTION:  Graph Basic initialization with rows=1,cols=1	                    |
//	| NAME    :  timeInit								                            |
//	| INPUT   :  none								                                |
//	| OUTPUT  :  none								                                |
//	| REMARKS :  									                                |
//	+-------------------------------------------------------------------------------+
class_GraphBasic::class_GraphBasic()
{
	unsigned char sreg;

    //save SREG
	sreg=SREG;
    //Disable Interrupts
	cli();
    //
    this->Rows=1;
    this->Cols=1;
    //Initialize
    Initialize();
    //Restore SREG state
	SREG=sreg;
}
//	+-------------------------------------------------------------------------------+
//	| FUNCTION:  Graph Basic initialization	    				                    |
//	| NAME    :  Graph Basic Init overload 2 				                        |
//	| INPUT   :  rows,cols with rows and cols		                                |
//	| OUTPUT  :  none								                                |
//	| REMARKS :  									                                |
//	+-------------------------------------------------------------------------------+
class_GraphBasic::class_GraphBasic(int rows,int cols)
{
    unsigned char sreg;

    //save SREG
    sreg=SREG;
    //Disable Interrupts
    cli();
    this->Cols=cols;
    this->Rows=rows;
    //Initialize
    Initialize();
    //Restore SREG state
    SREG=sreg;
}
//	+-------------------------------------------------------------------------------+
//	| FUNCTION:  Graph Basic Uninitialization                                       |
//	| NAME    :  timeInit								                            |
//	| INPUT   :  none								                                |
//	| OUTPUT  :  none								                                |
//	| REMARKS :  									                                |
//	+-------------------------------------------------------------------------------+
class_GraphBasic::~class_GraphBasic()
{
    unsigned char sreg;

    //save SREG
    sreg=SREG;
    //Disable Interrupts
    cli();
    //UnInitialize
    UnInitialize();
    //Restore SREG state
    SREG=sreg;
}

//	+-------------------------------------------------------------------------------+
//	| FUNCTION:  Set Backcolor panel                                               	|
//	| NAME    :  SetBackColor  	        					                        |
//	| INPUT   :                                 			                        |
//	| OUTPUT  :                                                                     |
//	| REMARKS :                                             		                |
//	+-------------------------------------------------------------------------------+
void    class_GraphBasic::SetBackColor(Color backColor)
{
    this->BackColor=backColor;
}
//	+-------------------------------------------------------------------------------+
//	| FUNCTION:  Set Backcolor panel                                               	|
//	| NAME    :  SetBackColor  	        					                        |
//	| INPUT   :                                 			                        |
//	| OUTPUT  :                                                                     |
//	| REMARKS :                                             		                |
//	+-------------------------------------------------------------------------------+
void    class_GraphBasic::SetBackColor(uint8_t red,uint8_t green,uint8_t blue)
{
    this->BackColor.red=red;
    this->BackColor.green=green;
    this->BackColor.blue=blue;
}
//	+-------------------------------------------------------------------------------+
//	| FUNCTION:  Set ForeColor panel                                               	|
//	| NAME    :  SetForeColor  	        					                        |
//	| INPUT   :                                 			                        |
//	| OUTPUT  :                                                                     |
//	| REMARKS :                                             		                |
//	+-------------------------------------------------------------------------------+
void    class_GraphBasic::SetForeColor(Color foreColor)
{
    this->ForeColor=foreColor;
}
//	+-------------------------------------------------------------------------------+
//	| FUNCTION:  Set Forecolor panel                                               	|
//	| NAME    :  SetForeColor  	        					                        |
//	| INPUT   :                                 			                        |
//	| OUTPUT  :                                                                     |
//	| REMARKS :                                             		                |
//	+-------------------------------------------------------------------------------+
void    class_GraphBasic::SetForeColor(uint8_t red,uint8_t green,uint8_t blue)
{
    this->ForeColor.red=red;
    this->ForeColor.green=green;
    this->ForeColor.blue=blue;
}
//	+-------------------------------------------------------------------------------+
//	| FUNCTION:  Get Backcolor panel                                               	|
//	| NAME    :  GetBackColor  	        					                        |
//	| INPUT   :                                 			                        |
//	| OUTPUT  :                                                                     |
//	| REMARKS :                                             		                |
//	+-------------------------------------------------------------------------------+
Color   class_GraphBasic::GetBackColor()
{
    return this->BackColor;
}
//	+-------------------------------------------------------------------------------+
//	| FUNCTION:  Get ForeColor panel                                               	|
//	| NAME    :  GetForeColor  	        					                        |
//	| INPUT   :                                 			                        |
//	| OUTPUT  :                                                                     |
//	| REMARKS :                                             		                |
//	+-------------------------------------------------------------------------------+
Color   class_GraphBasic::GetForeColor()
{
    return this->ForeColor;
}

//	+-------------------------------------------------------------------------------+
//	| FUNCTION:  Set pixel                                                        	|
//	| NAME    :  SetPixel   	        					                        |
//	| INPUT   :                                 			                        |
//	| OUTPUT  :                                                                     |
//	| REMARKS :                                              		                |
//	+-------------------------------------------------------------------------------+
void    class_GraphBasic::SetPixel(int x,int y)
{
    SetPixel(x,y,this->ForeColor.red,this->ForeColor.green,this->ForeColor.blue);
}
//	+-------------------------------------------------------------------------------+
//	| FUNCTION:  Set pixel with color                                             	|
//	| NAME    :  SetPixel   	        					                        |
//	| INPUT   :  Color                               			                    |
//	| OUTPUT  :                                                                     |
//	| REMARKS :                                              		                |
//	+-------------------------------------------------------------------------------+
void    class_GraphBasic::SetPixel(int x,int y,Color pixelColor)
{
    SetPixel(x,y,pixelColor.red,pixelColor.green,pixelColor.blue);
}
//	+-------------------------------------------------------------------------------+
//	| FUNCTION:  Set pixel on r,g,b color                                        	|
//	| NAME    :  SetPixel   	        					                        |
//	| INPUT   :  Color                               			                    |
//	| OUTPUT  :                                                                     |
//	| REMARKS :                                              		                |
//	+-------------------------------------------------------------------------------+
void    class_GraphBasic::SetPixel(int x,int y,uint8_t red,uint8_t green,uint8_t blue)
{
    uint16_t offset;
    Color   *p;

    if (x>=0 && x<Cols && y>=0 && y<Rows)
    {
        p=this->ColorBuffer;
        offset=y*this->Cols+x;
 
		p+=offset;
		p->red=red;
		p->green=green;
		p->blue=blue;
    }
}
//	+-------------------------------------------------------------------------------+
//	| FUNCTION:  Get pixel and return color                                       	|
//	| NAME    :  GetPixel   	        					                        |
//	| INPUT   :  x,y coords                           			                    |
//	| OUTPUT  :  Color of x,y coords                                                |
//	| REMARKS :  return backcolor if x,y out of range else return pixel color       |
//	+-------------------------------------------------------------------------------+
Color   class_GraphBasic::GetPixel(int x,int y)
{
    uint16_t offset;
    Color   *p;

    //check if x,y in range
    if (x>=0 && x<Cols && y>=0 && y<Rows)
    {
        //compute pixel address
        p=this->ColorBuffer;
        offset=y*this->Cols+x;
        p+=offset;
        //return pixel
        return *p;
    }
    else
    {
        return BackColor;
    }
}
//	+-------------------------------------------------------------------------------+
//	| FUNCTION:  Draw Vertical Line  with r,g,b values                            	|
//	| NAME    :  VLine         	        					                        |
//	| INPUT   :  x start,yi start,yf end,r,g,b values  			                    |
//	| OUTPUT  :                                                                     |
//	| REMARKS :                                              		                |
//	+-------------------------------------------------------------------------------+
void    class_GraphBasic::VertLine(int x,int yi,int yf,uint8_t red,uint8_t green,uint8_t blue)
{
    int tmp;

    //check coords
    if (yi>yf)
    {
        tmp=yi;
        yi=yf;
        yf=tmp;
    }
    //draw
    for(int i=yi;i<=yf;i++)
    {
        SetPixel(x,i,red,green,blue);
    }
}
//	+-------------------------------------------------------------------------------+
//	| FUNCTION:  Draw Vertical Line with forecolor Color                            |
//	| NAME    :  vLine         	        					                        |
//	| INPUT   :  x start,yi start,yf                  			                    |
//	| OUTPUT  :                                                                     |
//	| REMARKS :                                              		                |
//	+-------------------------------------------------------------------------------+
void    class_GraphBasic::VertLine(int x,int yi,int yf)
{
    VertLine(x,yi,yf,this->ForeColor.red,this->ForeColor.green,this->ForeColor.blue);
}
//	+-------------------------------------------------------------------------------+
//	| FUNCTION:  Draw Vertical Line with color                                      |
//	| NAME    :  VLine         	        					                        |
//	| INPUT   :  x start,yi start,yf                  			                    |
//	| OUTPUT  :                                                                     |
//	| REMARKS :                                              		                |
//	+-------------------------------------------------------------------------------+
void    class_GraphBasic::VertLine(int x,int yi,int yf,Color pixelColor)
{
    VertLine(x,yi,yf,pixelColor.red,pixelColor.green,pixelColor.blue);
}

//	+-------------------------------------------------------------------------------+
//	| FUNCTION:  Draw Horizontal Line with r,g,b, values                          	|
//	| NAME    :  HorzLine      	        					                        |
//	| INPUT   :  x start,yi start,yf end,r,g,b values  			                    |
//	| OUTPUT  :                                                                     |
//	| REMARKS :                                              		                |
//	+-------------------------------------------------------------------------------+
void    class_GraphBasic::HorzLine(int xi,int xf,int y,uint8_t red,uint8_t green,uint8_t blue)
{
    int tmp;

    //check coords
    if (xi>xf)
    {
        tmp=xi;
        xi=xf;
        xf=tmp;
    }
    //draw
    for(int i=xi;i<=xf;i++)
    {
        SetPixel(i,y,red,green,blue);
    }
}
//	+-------------------------------------------------------------------------------+
//	| FUNCTION:  Draw Horizontal Line with forecolor Color                          |
//	| NAME    :  HorzLine         	            			                        |
//	| INPUT   :  x start,yi start,yf                  			                    |
//	| OUTPUT  :                                                                     |
//	| REMARKS :                                              		                |
//	+-------------------------------------------------------------------------------+
void    class_GraphBasic::HorzLine(int xi,int xf,int y)
{
    HorzLine(xi,xf,y,this->ForeColor.red,this->ForeColor.green,this->ForeColor.blue);
}
//	+-------------------------------------------------------------------------------+
//	| FUNCTION:  Draw Horizontal Line with color                                    |
//	| NAME    :  HorzLine      	        					                        |
//	| INPUT   :  x start,yi start,yf                  			                    |
//	| OUTPUT  :                                                                     |
//	| REMARKS :                                              		                |
//	+-------------------------------------------------------------------------------+
void    class_GraphBasic::HorzLine(int xi,int xf,int y,Color pixelColor)
{
    HorzLine(xi,xf,y,pixelColor.red,pixelColor.green,pixelColor.blue);
}

//	+-------------------------------------------------------------------------------+
//	| FUNCTION:  Draw Box with r,g,b values                          	            |
//	| NAME    :  Box         	        					                        |
//	| INPUT   :  xi start,xf end,yi start,yf end,r,g,b values                       |
//	| OUTPUT  :                                                                     |
//	| REMARKS :                                              		                |
//	+-------------------------------------------------------------------------------+
void    class_GraphBasic::Box(int xi,int yi,int xf,int yf,uint8_t red,uint8_t green,uint8_t blue)
{
    HorzLine(xi,xf,yi,red,green,blue);
    HorzLine(xi,xf,yf,red,green,blue);
    VertLine(xi,yi,yf,red,green,blue);
    VertLine(xf,yi,yf,red,green,blue);
}
//	+-------------------------------------------------------------------------------+
//	| FUNCTION:  Draw Box Line with forecolor color                                 |
//	| NAME    :  Box         	        					                        |
//	| INPUT   :  xi start,yi start,xf end,yf end       			                    |
//	| OUTPUT  :                                                                     |
//	| REMARKS :                                              		                |
//	+-------------------------------------------------------------------------------+
void    class_GraphBasic::Box(int xi,int yi,int xf,int yf)
{
    Box(xi,yi,xf,yf,this->ForeColor.red,this->ForeColor.green,this->ForeColor.blue);
}
//	+-------------------------------------------------------------------------------+
//	| FUNCTION:  Draw Box Line with Color                                           |
//	| NAME    :  Box         	        					                        |
//	| INPUT   :  x start,yi start,yf                  			                    |
//	| OUTPUT  :                                                                     |
//	| REMARKS :                                              		                |
//	+-------------------------------------------------------------------------------+
void    class_GraphBasic::Box(int xi,int yi,int xf,int yf,Color pixelColor)
{
    Box(xi,yi,xf,yf,pixelColor.red,pixelColor.green,pixelColor.blue);
}
//	+-------------------------------------------------------------------------------+
//	| FUNCTION:  Draw Box Full with r,g,b values                      	            |
//	| NAME    :  Box         	        					                        |
//	| INPUT   :  xi start,xf end,yi start,yf end,r,g,b values                       |
//	| OUTPUT  :                                                                     |
//	| REMARKS :                                              		                |
//	+-------------------------------------------------------------------------------+
void    class_GraphBasic::BoxFull(int xi,int yi,int xf,int yf,uint8_t red,uint8_t green,uint8_t blue)
{
	//swap coords if need
	if (xi>xf)
	{
		int tmp=xi;
		xi=xf;
		xf=tmp;
	}
	//draw box full
	for(int x=xi;x<=xf;x++)
	{
		VertLine(x,yi,yf,red,green,blue);
	}
}
//	+-------------------------------------------------------------------------------+
//	| FUNCTION:  Draw Box Full with forecolor	                      	            |
//	| NAME    :  Box         	        					                        |
//	| INPUT   :  xi start,xf end,yi start,yf end,r,g,b values                       |
//	| OUTPUT  :                                                                     |
//	| REMARKS :                                              		                |
//	+-------------------------------------------------------------------------------+
void    class_GraphBasic::BoxFull(int xi,int yi,int xf,int yf)
{
	BoxFull(xi,yi,xf,yf,this->ForeColor.red,this->ForeColor.green,this->ForeColor.blue);	
}
//	+-------------------------------------------------------------------------------+
//	| FUNCTION:  Draw Box Full with Color		                      	            |
//	| NAME    :  Box         	        					                        |
//	| INPUT   :  xi start,xf end,yi start,yf end,r,g,b values                       |
//	| OUTPUT  :                                                                     |
//	| REMARKS :                                              		                |
//	+-------------------------------------------------------------------------------+
void    class_GraphBasic::BoxFull(int xi,int yi,int xf,int yf,Color pixelColor)
{
	BoxFull(xi,yi,xf,yf,pixelColor.red,pixelColor.green,pixelColor.blue);
}
//	+-------------------------------------------------------------------------------+
//	| FUNCTION:  Draw Line with r,g,b values                                        |
//	| NAME    :  HLine         	        					                        |
//	| INPUT   :  xi start,yi start,xf end,yf end      			                    |
//	| OUTPUT  :                                                                     |
//	| REMARKS : using Bresenham algorithm                       		            |
//	+-------------------------------------------------------------------------------+

static int sgn(int a)
{
    if (a==0) return 0;
    if (a>0) return 1;
    return -1;
}
void    class_GraphBasic::Line(int xi,int yi,int xf,int yf,uint8_t red,uint8_t green,uint8_t blue)
{
    int a,b,t,ct,d,j,k;
    int xn,xd;
    int yn,yd;
    int x,y;
    //prepare for drawing
    a=abs(xf-xi);
    b=abs(yf-yi);
    if (a>=b)
    {
        xn=sgn(xf-xi);
        yn=0;
    }
    else
    {
        xn=0;
        yn=sgn(yf-yi);
        t=a;
        a=b;
        b=t;
    }
    //check directions
    if (xi>xf) xd=-1; else xd=1;
    if (yi>yf) yd=-1; else yd=1;
    ct=a+1;
    d=(b << 1)-a;
    k=(b-a) << 1;
    j=b << 1;
    x=xi;
    y=yi;
    //draw line
    while (ct>0) {
        SetPixel(x,y,red,green,blue);
        ct--;
		//update decision flags
        if (d>0) {
            x+=xd;
            y+=yd;
            d+=k;
        }
        else {
            x+=xn;
            y+=yn;
            d+=j;
        }
    }
}
//	+-------------------------------------------------------------------------------+
//	| FUNCTION:  Draw Line with forecolor color                                     |
//	| NAME    :  HLine         	        					                        |
//	| INPUT   :  xi start,yi start,xf end,yf end      			                    |
//	| OUTPUT  :                                                                     |
//	| REMARKS :                                              		                |
//	+-------------------------------------------------------------------------------+
void    class_GraphBasic::Line(int xi,int yi,int xf,int yf)
{
    Line(xi,yi,xf,yf,this->ForeColor.red,this->ForeColor.green,this->ForeColor.blue);
}
//	+-------------------------------------------------------------------------------+
//	| FUNCTION:  Draw Line with Color                                               |
//	| NAME    :  HLine         	        					                        |
//	| INPUT   :  xi start,yi start,xf end,yf end      			                    |
//	| OUTPUT  :                                                                     |
//	| REMARKS :                                              		                |
//	+-------------------------------------------------------------------------------+
void    class_GraphBasic::Line(int xi,int yi,int xf,int yf,Color pixelColor)
{
    Line(xi,yi,xf,yf,pixelColor.red,pixelColor.green,pixelColor.blue);
}
//	+-------------------------------------------------------------------------------+
//	| FUNCTION:  Clear Graph Basic                                              	|
//	| NAME    :  Cls           	        					                        |
//	| INPUT   :                                        			                    |
//	| OUTPUT  :                                                                     |
//	| REMARKS :                                              		                |
//	+-------------------------------------------------------------------------------+
void    class_GraphBasic::Cls()
{
    Color       *p;
    uint16_t    bufEnd;
    
    p=this->ColorBuffer;
    //show all values
    bufEnd=this->Rows*this->Cols;
    for(uint16_t i=0;i<bufEnd;i++)
    {
        p->red=this->BackColor.red;
        p->green=this->BackColor.green;
        p->blue=this->BackColor.blue;
        p++;
    }
}
//	+-------------------------------------------------------------------------------+
//	| FUNCTION: Plotting 8 symmetric points                                        	|
//	| NAME    :  Cls           	        					                        |
//	| INPUT   :                                        			                    |
//	| OUTPUT  :                                                                     |
//	| REMARKS :                                              		                |
//	+-------------------------------------------------------------------------------+

void class_GraphBasic::PlotP8(int xx,int yy,int x,int y,uint8_t red,uint8_t green,uint8_t blue)
{
   SetPixel(xx+x,yy+y,red,green,blue);
   SetPixel(xx+x,yy-y,red,green,blue);
   SetPixel(xx-x,yy+y,red,green,blue);
   SetPixel(xx-x,yy-y,red,green,blue);
   SetPixel(xx+y,yy+x,red,green,blue);
   SetPixel(xx+y,yy-x,red,green,blue);
   SetPixel(xx-y,yy+x,red,green,blue);
   SetPixel(xx-y,yy-x,red,green,blue);
}
//	+-------------------------------------------------------------------------------+
//	| FUNCTION:  Draw a Circle with r,g,b values on matrix                          |
//	| NAME    :  Circle       	        					                        |
//	| INPUT   :  px,py coords at radius using r,g,b values for color                |
//	| OUTPUT  :                                                                     |
//	| REMARKS : using Bresenham algorithm                       	                |
//	+-------------------------------------------------------------------------------+
void class_GraphBasic::Circle(int px,int py,int radius,uint8_t red,uint8_t green,uint8_t blue)
{
    int x,y,d;

    x=0;
    y=radius;
    d=3-2*radius;
    while (x<y) {
       PlotP8(px,py,x,y,red,green,blue);
       if (d<0) {
          d+=4*x+6; }
       else {
          d+=4*(x-y)+10;
          y--;
       }
       x++;
    }
    if (x==y) PlotP8(px,py,x,y,red,green,blue);
}

//	+-------------------------------------------------------------------------------+
//	| FUNCTION:  Draw a Circle with forecolor                                       |
//	| NAME    :  Circle       	        					                        |
//	| INPUT   :  px,py coords at radius using forecolor                             |
//	| OUTPUT  :                                                                     |
//	| REMARKS :                                              		                |
//	+-------------------------------------------------------------------------------+
void class_GraphBasic::Circle(int px,int py,int radius)
{
    Circle(px,py,radius,this->ForeColor.red,this->ForeColor.green,this->ForeColor.blue);
}
//	+-------------------------------------------------------------------------------+
//	| FUNCTION:  Draw a Circle with pixelcolor                                      |
//	| NAME    :  Circle       	        					                        |
//	| INPUT   :  px,py coords at radius using pixelcolor                            |
//	| OUTPUT  :                                                                     |
//	| REMARKS :                                              		                |
//	+-------------------------------------------------------------------------------+
void class_GraphBasic::Circle(int px,int py,int radius,Color pixelColor)
{
    Circle(px,py,radius,pixelColor.red,pixelColor.green,pixelColor.blue);
}
//	+-------------------------------------------------------------------------------+
//	| FUNCTION:  Rotate Pixel Left/right					                    	|
//	| NAME    :  RotateX	   	        					                        |
//	| INPUT   :                     						                        |
//	| OUTPUT  :                                                                     |
//	| REMARKS : if xi<xf rotate Left else rotate right        		                |
//	+-------------------------------------------------------------------------------+
void class_GraphBasic::RotateX(int xi,int xf,int yi,int yf,bool shiftOnly)
{
    //29/10/2015 this a low speed version ,more later i implement fast version 
    Color colorTmp;
    //return if y coords outof range
    //check if in range
	//X
    if (xi<0) xi=0;
    if (xf<0) xf=0;
    if (xi>MaxX()) xi=MaxX();
    if (xf>MaxX()) xf=MaxX();
	//Y
    if (yi<0) yi=0;
    if (yf<0) yf=0;
    if (yi>MaxY()) yi=MaxY();
    if (yf>MaxY()) yf=MaxY();

	//check if yi<yf
	if (yi>yf)
	{
		int tmp=yi;
		yi=yf;
		yf=tmp;
	}
	for(int y=yi;y<=yf;y++)
	{
		//check if rotate left or right
		if (xi>xf)
		{
			if ((xi-xf)<1) return;
			//rotate right
			//+----------------------+
			//|  +-+-+-+-+-+-+-+-+-+ |
			//+->| | | | | | | | | |-+   <--Y
			//   +-+-+-+-+-+-+-+-+-+
			//    xf             xi
			colorTmp=GetPixel(xi,y);
			for(int i=xi;i>xf;i--)
				SetPixel(i,y,GetPixel(i-1,y));
			if (!shiftOnly) SetPixel(xf,y,colorTmp);
		}
		else
		{
			if ((xf-xi)<1) return;
			//rotate left
			//+----------------------+
			//| +-+-+-+-+-+-+-+-+-+  |
			//+-| | | | | | | | | |<-   <--Y
			//  +-+-+-+-+-+-+-+-+-+
			//   xi             xf
			colorTmp=GetPixel(xi,y);
			for(int i=xi;i<xf;i++)
				SetPixel(i,y,GetPixel(i+1,y));
			if (!shiftOnly) SetPixel(xf,y,colorTmp);
		}
	}
}
//	+-------------------------------------------------------------------------------+
//	| FUNCTION:  Rotate Pixel up/down							                 	|
//	| NAME    :  RotateY	   	        					                        |
//	| INPUT   :                     						                        |
//	| OUTPUT  :                                                                     |
//	| REMARKS : if yi<yf rotate up else roate down           		                |
//	+-------------------------------------------------------------------------------+
void class_GraphBasic::RotateY(int xi,int xf,int yi,int yf,bool shiftOnly)
{
	//29/10/2015 this a low speed version ,more later i implement fast version
	Color colorTmp;
	//return if y coords out of range
	//check if in range
	//X
	if (xi<0) xi=0;
	if (xf<0) xf=0;
	if (xi>MaxX()) xi=MaxX();
	if (xf>MaxX()) xf=MaxX();
	//Y
	if (yi<0) yi=0;
	if (yf<0) yf=0;
	if (yi>MaxY()) yi=MaxY();
	if (yf>MaxY()) yf=MaxY();

	//check if xi<xf
	if (xi>xf)
	{
		int tmp=xi;
		xi=xf;
		xf=tmp;
	}
	for(int x=xi;x<=xf;x++)
	{
		//check if rotate up or down
		if (yi>yf)
		{
			if ((yi-yf)<1) return;
			//rotate up
			//		+----+
			//		|    |
			//		|    |
			//    +---+  |
			//	  |   |  | yi
			//    +---+  |
			//	  |   |  |
			//    +---+  |
			//	  |   |  |
			//    +---+  |
			//	  |   |  |
			//    +---+  |
			//	  |   |  |
			//    +---+  |
			//	  |   |  | yf
			//	  +---+  |
			//		^    |
			//	    +----+
			//      x
			colorTmp=GetPixel(x,yi);
			for(int i=yi;i>yf;i--)
			SetPixel(x,i,GetPixel(x,i-1));
			if (!shiftOnly) SetPixel(x,yf,colorTmp);
		}
		else
		{
			if ((yf-yi)<1) return;
			//rotate down
			//		+----+
			//		|    |
			//		V    |
			//    +---+  |
			//	  |   |  | yi
			//    +---+  |
			//	  |   |  |
			//    +---+  |
			//	  |   |  |
			//    +---+  |
			//	  |   |  |
			//    +---+  |
			//	  |   |  |
			//    +---+  |
			//	  |   |  | yf
			//	  +---+  |
			//		|    |
			//	    +----+
			//      x
			colorTmp=GetPixel(x,yi);
			for(int i=yi;i<yf;i++)
			SetPixel(x,i,GetPixel(x,i+1));
			if (!shiftOnly) SetPixel(x,yf,colorTmp);
		}
	}
}
//	+-------------------------------------------------------------------------------+
//	| FUNCTION:  Reset WS2812 LED Stream                                           	|
//	| NAME    :  SetTime       	        					                        |
//	| INPUT   :                     						                        |
//	| OUTPUT  :                                                                     |
//	| REMARKS :  virtual function                           		                |
//	+-------------------------------------------------------------------------------+
void    class_GraphBasic::Reset()
{
       
}
//	+-------------------------------------------------------------------------------+
//	| FUNCTION:  Write values to WS2812 LED Stream                               	|
//	| NAME    :  write       	        					                        |
//	| INPUT   :  grbPtr -->point to grb stream  			                        |
//	| OUTPUT  :                                                                     |
//	| REMARKS :  virtual function                              		                |
//	+-------------------------------------------------------------------------------+
	void    class_GraphBasic::WriteRGBtoDevice(uint8_t rValue,uint8_t gValue,uint8_t bValue)
	{
   
	}
//	+-------------------------------------------------------------------------------+
//	| FUNCTION:  Show r,g,b matrix on screen                                    	|
//	| NAME    :  Show       	        					                        |
//	| INPUT   :                                 			                        |
//	| OUTPUT  :                                                                     |
//	| REMARKS :  Interrupts are disabled and restore        		                |
//	+-------------------------------------------------------------------------------+
void    class_GraphBasic::Show()
{
    uint8_t     sreg;
    Color       *ColorPtr;
    uint16_t    bufEnd;

    //save sreg state
    sreg=SREG;
    //disable interrupts
    cli();
    //show all values in buffer
    //reset panel
    Reset();
    //set to first value
    ColorPtr=this->ColorBuffer;
    bufEnd=this->Rows*this->Cols;
    //show all values
    for(uint16_t i=0;i<bufEnd;i++)
    {
        WriteRGBtoDevice(ColorPtr->red,ColorPtr->green,ColorPtr->blue);
        ColorPtr++;
    }
    //restore sreg state
    SREG=sreg;
}
//	+-------------------------------------------------------------------------------+
//	| FUNCTION:  Locate character cursor at row,col                               	|
//	| NAME    :  Locate       	        					                        |
//	| INPUT   :                                 			                        |
//	| OUTPUT  :                                                                     |
//	| REMARKS :                                                		                |
//	+-------------------------------------------------------------------------------+
void  class_GraphBasic::Locate(int x,int y)
{
    this->X=x;
    this->Y=y;
}
//	+-------------------------------------------------------------------------------+
//	| FUNCTION:  Put character on graph matrix at coordiantes of last row,col     	|
//	| NAME    :  Locate       	        					                        |
//	| INPUT   :                                 			                        |
//	| OUTPUT  :                                                                     |
//	| REMARKS :  forecolor define a character color            		                |
//	+-------------------------------------------------------------------------------+
void  class_GraphBasic::PutC(char c,bool useBackColor)
{
    
#if defined(_GRAPH_NORMAL_CHAR_SIZE_)
	
	uint8_t b,mask;
	uint16_t  index;

    //check if in range
    if (c>=' ' && c<='~')
    {
        //translate ascii to binary
        c-=' ';
        index=((c>>1)*5);
        //check if c is odd or even

        //draw character
        for(uint8_t j=0;j<5;j++)
        {
			//select start bit according ascii(char) is odd or even
            if ((c&1)==0) mask=0x80; else mask=0x04;
            b=pgm_read_byte(&_ASCII_Table[index++]);
            for(uint8_t i=0;i<3;i++)
            {
                if ((b & mask)!=0)
                {
                    SetPixel(this->X+i,this->Y+j);
                }
                else
                {
                    if (useBackColor) SetPixel(this->X+i,this->Y+j,this->BackColor.red,this->BackColor.green,this->BackColor.blue);
                }
                mask>>=1;
            } 
        }
        this->X+=GetTextWidth();
    }
#elif defined(_GRAPH_BIG_CHAR_SIZE_)
	uint8_t b,mask;
	uint16_t  index;

	//check if in range
	if (c>=' ' && c<='~')
	{
		//translate ascii to binary
		c-=' ';
		index=(c*8);
		//draw chars
		for(uint8_t j=0;j<8;j++)
		{
			mask=0x80;
			b=pgm_read_byte(&_ASCII_Table[index++]);
			for(uint8_t i=0;i<5;i++)
			{
				if ((b & mask)!=0)
				{
					SetPixel(this->X+i,this->Y+j);
				}
				else
				{
					if (useBackColor) SetPixel(this->X+i,this->Y+j,this->BackColor.red,this->BackColor.green,this->BackColor.blue);
				}
				mask>>=1;
			}
		}
		this->X+=GetTextWidth();
	}
#endif

}
//	+-------------------------------------------------------------------------------+
//	| FUNCTION:  Put character string on graph matrix at coordinates x,y	    	|
//	| NAME    :  Locate       	        					                        |
//	| INPUT   :                                 			                        |
//	| OUTPUT  :                                                                     |
//	| REMARKS :  forecolor define a character color  , use Locate function before   |
//	|			 to set coordinates.												|
//	+-------------------------------------------------------------------------------+
void   class_GraphBasic::PutS(const char *c,bool useBackColor)
{
    while (*c!=0)
    {
        PutC(*c,useBackColor);
        c++;
    }
}
//	+-------------------------------------------------------------------------------+
//	| FUNCTION:  Put character string from Flash on graph matrix at coordinates x,y |
//	| NAME    :  Locate       	        					                        |
//	| INPUT   :                                 			                        |
//	| OUTPUT  :                                                                     |
//	| REMARKS :  forecolor define a character color, use Locate function before     |
//	|			 to set coordinates													|
//	+-------------------------------------------------------------------------------+
void   class_GraphBasic::PutSF(const char *c,bool useBackColor)
{
	static char ch;
	
	for(;;)
	{
		ch=pgm_read_byte(c);
		if (ch==0) break;
		PutC(ch,useBackColor);
		c++;
	};
}
