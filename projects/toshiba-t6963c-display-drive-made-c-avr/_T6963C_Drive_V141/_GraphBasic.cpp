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

/**
******************************************************************************************
*@file   
*@author Joao Dartagnan Antunes Oliveira
*@date 07/02/2016
*@brief <b>(C++)</b> File for Graphics engine
*@copyright GNU Public License.
******************************************************************************************
*/

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
//	| FUNCTION:  Generate a Milliseconds delay										|
//	| NAME    :  delayMs									     					|
//	| INPUT   :	 timeMS Delay in Milliseconds										|
//	| OUTPUT  :																		|
//	| REMARKS :																		|
//	+-------------------------------------------------------------------------------+

/*
static void delayMs(uint16_t timeMS)
{
	#ifdef _T6963C_TEST_MODE_
	return;
	#endif
	unsigned int timeCount;
	//compute 1 millisecond time
	//this function generate a error of 0.375uS by millisecond at 16Mhz
	timeCount=(F_CPU)/4000;
	//generate N milliseconds time
	for(;;)
	{
		//exit if zero(0)
		if (timeMS==0) break;
		//delay 1 millisecond
		asm volatile("movw   r24,%A0	    \t\n"	\
		"1:                    \t\n"   \
		"sbiw r24,1		    \t\n"	\
		"brne 1b               \t\n"   \
		:"+r" (timeCount)
		:
		:"r24","r25"
		);
		//pump Time
		timeMS--;
	}
	
}
*/

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
    //Save SREG
    sreg=SREG;
    //Disable Interrupts
    cli();
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
    //Restore Interrupts
    SREG=sreg;
}

// 
//         +-----------------------------------------------+
//         |       CONSTRUCTORS AND DESTRUCTORS            |
//         +-----------------------------------------------+
// 

//	+-------------------------------------------------------------------------------+
//	| FUNCTION:  Graph Basic initialization	    				                    |
//	| NAME    :  Constructor								                        |
//	| INPUT   :  rows,cols with rows and cols		                                |
//	| OUTPUT  :  none								                                |
//	| REMARKS :  									                                |
//	+-------------------------------------------------------------------------------+
class_GraphBasic::class_GraphBasic(int Rows,int Cols,int GraphWidth,int GraphHeight)
{
    unsigned char sreg;

    //save SREG
    sreg=SREG;
    //Disable Interrupts
    cli();
	_Rows=Rows;
	_Cols=Cols;
	_Width=GraphWidth;
	_Height=GraphHeight;
    //Restore SREG state
    SREG=sreg;
}
//	+-------------------------------------------------------------------------------+
//	| FUNCTION:  Graph Basic Uninitialization                                       |
//	| NAME    :  Destructor								                            |
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
//	| FUNCTION:  VIRTUAL FUNCTIONS			                                        |
//	| NAME    :           	        						                        |
//	| INPUT   :							               			                    |
//	| OUTPUT  :                                                                     |
//	| REMARKS :                                              		                |
//	+-------------------------------------------------------------------------------+

void	class_GraphBasic::SetPixel(int x,int y,Pixel pixel)	{}				 //virtual
Pixel	class_GraphBasic::GetPixel(int x,int y) { return (Pixel)0;}			 //virtual
void	class_GraphBasic::DrawHorzLine(int y,int xS,int xE,Pen pen) {}	     //virtual

//	+-------------------------------------------------------------------------------+
//	| FUNCTION:  Draw Vertical Line with pen                                        |
//	| NAME    :  VLine         	        					                        |
//	| INPUT   :  x start,yi start,yf                  			                    |
//	| OUTPUT  :                                                                     |
//	| REMARKS :                                              		                |
//	+-------------------------------------------------------------------------------+
void    class_GraphBasic::DrawVertLine(int x,int yi,int yf,Pen pen)
{
	DrawLine(x,yi,x,yf,pen);
}

//	+-------------------------------------------------------------------------------+
//	| FUNCTION:  Draw Box  with Pen													|
//	| NAME    :  Box         	        					                        |
//	| INPUT   :  x start,yi start,yf                  			                    |
//	| OUTPUT  :                                                                     |
//	| REMARKS :                                              		                |
//	+-------------------------------------------------------------------------------+
void    class_GraphBasic::DrawBox(int xi,int yi,int xf,int yf,Pen pen,bool border)
{
	DrawMode dm;
	//update draw pen cord
	this->_X=xf;
	this->_Y=yf;	
	dm=this->_DrawMode;
	switch (this->_DrawMode)
	{
		case DrawMode::LINE:
			DrawHorzLine(yi,xi,xf,pen);
			DrawHorzLine(yf,xi,xf,pen);
			DrawLine(xi,yi,xi,yf,pen);
			DrawLine(xf,yi,xf,yf,pen);
		break;	
		case DrawMode::PATTERN:
			if (xi>xf)
			{
				int tmp;
				tmp=xi;
				xi=xf;
				xf=tmp;
			}
			if ((xf-xi)>2)
				DrawBoxFull(xi+1,yi+1,xf-1,yf-1,pen);
			if (border)
			{
				this->_DrawMode=DrawMode::LINE;
				DrawHorzLine(yi,xi,xf,pen);
				DrawHorzLine(yf,xi,xf,pen);
				DrawLine(xi,yi,xi,yf,pen);
				DrawLine(xf,yi,xf,yf,pen);				
			}
		break;
		case DrawMode::SOLID:
			DrawBoxFull(xi,yi,xf,yf,pen);
		break;
	}
	this->_DrawMode=dm;
}

//	+-------------------------------------------------------------------------------+
//	| FUNCTION:  Draw Box Full with Pen		                      					|
//	| NAME    :  Box         	        					                        |
//	| INPUT   :  xi start,xf end,yi start,yf end,pen		                        |
//	| OUTPUT  :                                                                     |
//	| REMARKS :                                              		                |
//	+-------------------------------------------------------------------------------+
void    class_GraphBasic::DrawBoxFull(int xi,int yi,int xf,int yf,Pen pen)
{
	int w;
	//swap coords if need
	if (yi>yf)
	{
		w=yi;
		yi=yf;
		yf=w;
	}
	//check if in range
	if (yi<0 && yf<0) return;
	if (yi>(this->_Height-1) && yf>(this->_Height-1)) return;
	//draw full box
	for(int i=yi;i<=yf;i++)
	{
		DrawHorzLine(i,xi,xf,pen);
	}
}

//	+-------------------------------------------------------------------------------+
//	| FUNCTION:  Draw Line with r,g,b values                                        |
//	| NAME    :  DrawLine      	        					                        |
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
void    class_GraphBasic::DrawLine(int xi,int yi,int xf,int yf,Pen pen)
{
    int a,b,t,ct,d,j,k;
    int xn,xd;
    int yn,yd;
    int x,y;
	
	uint16_t shift;
	
	//update draw pen cord
	this->_X=xf;
	this->_Y=yf;
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
	shift=static_cast<uint16_t>(pen);
    while (ct>0) {
		{
			shift<<=1;
			shift|=(shift>>8);
		}
        //SetPixel(x,y,pen);
		SetPixel(x,y,shift);
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
//	| FUNCTION:  Draw Line from pen to x,y		                                    |
//	| NAME    :  DrawLineTo    	        					                        |
//	| INPUT   :  x,y	Coordinates				      			                    |
//	|			 pen  Pen Pen													|
//	| OUTPUT  :                                                                     |
//	| REMARKS : using Bresenham algorithm                       		            |
//	+-------------------------------------------------------------------------------+
void    class_GraphBasic::DrawLineTo(int x,int y,Pen pen)
{
	DrawLine(this->_X,this->_Y,x,y,pen);
}


//	+-------------------------------------------------------------------------------+
//	| FUNCTION:  Draw a Circle (intenal called by circle main function)             |
//	| NAME    :  Circle       	        					                        |
//	| INPUT   :  px,py coords at radius with pen					                |
//	| OUTPUT  :                                                                     |
//	| REMARKS : using Bresenham algorithm                       	                |
//	+-------------------------------------------------------------------------------+
void class_GraphBasic::CircleIntern(int px,int py,int radius,Pen pen,Pixel pixel)
{
#ifdef _GRAPH_USE_CIRCLE_ALGORITHM_
    int x,y,d;

    x=0;
    y=radius;
    d=3-2*radius;
    while (x<y) {
       PlotP8(px,py,x,y,pen,pixel);
       if (d<0) {
          d+=4*x+6; }
       else {
          d+=4*(x-y)+10;
          y--;
       }
       x++;
    }
    if (x==y) PlotP8(px,py,x,y,pen,pixel);
#endif
}
//	+-------------------------------------------------------------------------------+
//	| FUNCTION:  Draw a Circle according drawMode at coordinates x,y with radius and|
//	|			 pen																|
//	| NAME    :  DrawCircle    	        					                        |
//	| INPUT   :  px,py coordinates at radius with pen and pixel		                |
//	| OUTPUT  :                                                                     |
//	| REMARKS : using Bresenham algorithm                       	                |
//	|			pixel defines color of circle border, Pen used in DrawMode::Solid	|
//	+-------------------------------------------------------------------------------+
void class_GraphBasic::DrawCircle(int px,int py,int radius,Pen pen,Pixel pixel,bool border)
{
#ifdef _GRAPH_USE_CIRCLE_ALGORITHM_
	DrawMode dm;
	
	dm=this->GetDrawMode();
	
	switch (this->_DrawMode)
	{
		case DrawMode::LINE:
		case DrawMode::SOLID:
			CircleIntern(px,py,radius,pen,pixel);			
		break;
		case DrawMode::PATTERN:
			CircleIntern(px,py,radius,Pen::Black,pixel);
			if (border)
			{
				this->_DrawMode=DrawMode::LINE;
				CircleIntern(px,py,radius,Pen::Black,pixel);				
			}	
		break;
	}
	
	this->_DrawMode=dm;
#else
	DrawEllipse(px,py,radius,radius,pen,pixel,border);
#endif
}
//	+-------------------------------------------------------------------------------+
//	| FUNCTION:  Draw a Ellipse Internally according drawMode at coordinates x,y	|
//	|			 radius x,radius Y													|
//	| NAME    :  DrawEllipse   	        					                        |
//	| INPUT   :  xc,yc Center coordinates of ellipse				                |
//	|			 rx,ry respectively Radius x,Radius y								|
//	| OUTPUT  :                                                                     |
//	| REMARKS : using Bresenham algorithm                       	                |
//	|			pixel defines color of circle border, Pen used in DrawMode::Solid	|
//	+-------------------------------------------------------------------------------+
void class_GraphBasic::DrawEllipseIntern(int xc,int yc, int rx, int ry,Pen pen,Pixel pixel)
{
	long int rx2=rx*rx,
			 ry2=ry*ry,
			 tworx2=2*rx2,
			 twory2=2*ry2,
			 p,x=0,y=ry,px=twory2*x,py=tworx2*y;
	
	PlotP4(xc,yc,x,y,pen,pixel);
	//p=(ry2-(rx2*ry)+(0.25*rx2));
	p=(ry2-(rx2*ry)+(rx2>>2));
	while(px<py)
	{
		x++;
		px+=twory2;
		if(p<0)
			p+=ry2+px;
		else
		{
			y--;
			py-=tworx2;
			p+=ry2+px-py;
		}
		PlotP4(xc,yc,x,y,pen,pixel);
	}
	//p=(ry2*(x+0.5)*(x+0.5)+(rx2*(y-1)*(y-1))-(rx2*ry2));
	p=(ry2*(x)*(x)+(rx2*(y-1)*(y-1))-(rx2*ry2));
	while(y>0)
	{
		y--;
		py-=tworx2;
			if(p>0)
		p+=rx2-py;
		else
		{
			x++;
			px+=twory2;
			p+=rx2-py+px;
		}
		PlotP4(xc,yc,x,y,pen,pixel);
	}
}
//	+-------------------------------------------------------------------------------+
//	| FUNCTION:  Draw a Ellipse according drawMode at coordinates x,y				|
//	|			 radius x,radius Y													|
//	| NAME    :  DrawEllipse   	        					                        |
//	| INPUT   :  xc,yc Center coordinates of ellipse				                |
//	|			 rx,ry respectively Radius x,Radius y								|
//	| OUTPUT  :                                                                     |
//	| REMARKS : using Bresenham algorithm                       	                |
//	|			pixel defines color of circle border, Pen used in DrawMode::Solid	|
//	+-------------------------------------------------------------------------------+
void class_GraphBasic::DrawEllipse(int xc,int yc, int rx, int ry,Pen pen,Pixel pixel,bool border)
{
	DrawMode dm;
	
	dm=this->GetDrawMode();
	
	switch (this->_DrawMode)
	{
		case DrawMode::LINE:
		case DrawMode::SOLID:
			DrawEllipseIntern(xc,yc,rx,ry,pen,pixel);
		break;
		case DrawMode::PATTERN:
			DrawEllipseIntern(xc,yc,rx,ry,Pen::Black,pixel);
			if (border)
			{
				this->_DrawMode=DrawMode::LINE;
				DrawEllipseIntern(xc,yc,rx,ry,Pen::Black,pixel);
			}
		break;
	}
	
	this->_DrawMode=dm;
}

//	+-------------------------------------------------------------------------------+
//	| FUNCTION: Plotting 4 symmetric points                                        	|
//	| NAME    : PlotP4         	        					                        |
//	| INPUT   : Pen,pixel                              			                    |
//	| OUTPUT  :                                                                     |
//	| REMARKS :                                              		                |
//	+-------------------------------------------------------------------------------+

void class_GraphBasic::PlotP4(int xc,int yc,int x,int y,Pen pen,Pixel pixel)
{
	switch (GetDrawMode())
	{
		case DrawMode::LINE:
			SetPixel(xc+x,yc+y,pixel);
			SetPixel(xc-x,yc+y,pixel);
			SetPixel(xc+x,yc-y,pixel);
			SetPixel(xc-x,yc-y,pixel);
		break;
		case DrawMode::SOLID:
		case DrawMode::PATTERN:
			DrawHorzLine(yc+y,xc-x+1,xc+x-1,pen);
			DrawHorzLine(yc-y,xc-x+1,xc+x-1,pen);
		break;
	}
}

//	+-------------------------------------------------------------------------------+
//	| FUNCTION: Plotting 8 symmetric points                                        	|
//	| NAME    : PlotP8        	        					                        |
//	| INPUT   : pen,pixel                              			                    |
//	| OUTPUT  :                                                                     |
//	| REMARKS :                                              		                |
//	+-------------------------------------------------------------------------------+

void class_GraphBasic::PlotP8(int xx,int yy,int x,int y,Pen pen,Pixel pixel)
{
#ifdef _GRAPH_USE_CIRCLE_ALGORITHM_
	switch (_DrawMode)
	{
		case DrawMode::LINE:
			SetPixel(xx-x,yy+y,pixel);
			SetPixel(xx+x,yy+y,pixel);
			SetPixel(xx-x,yy-y,pixel);
			SetPixel(xx+x,yy-y,pixel);
			SetPixel(xx+y,yy+x,pixel);
			SetPixel(xx+y,yy-x,pixel);
			SetPixel(xx-y,yy+x,pixel);
			SetPixel(xx-y,yy-x,pixel);
		break;
		case DrawMode::SOLID:
		case DrawMode::PATTERN:
			DrawHorzLine(yy+y,xx-x+1,xx+x-1,pen);
			DrawHorzLine(yy-y,xx-x+1,xx+x-1,pen);
			DrawHorzLine(yy+x,xx-y+1,xx+y-1,pen);
			DrawHorzLine(yy-x,xx-y+1,xx+y-1,pen);
		break;
	}
#endif
}