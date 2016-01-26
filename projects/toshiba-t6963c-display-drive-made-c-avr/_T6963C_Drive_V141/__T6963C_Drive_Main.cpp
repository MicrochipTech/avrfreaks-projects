/*
 /////////////////////////////////////////////////////////////////////////////////////////
 //         _____ ____  _____  ____  _____ _____  ______ _           _
 //        |_   _/ ___||  _  |/ ___||____ /  __ \ |  _  (_)         | |
 //          | |/ /___ | |_| / /___     / / /  \/ | | | |_ ___ _ __ | | __ _ _   _
 //          | || ___ \\____ | ___ \    \ \ |     | | | | / __| '_ \| |/ _` | | | |
 //          | || \_/ |.___/ / \_/ |.___/ / \__/\ | |/ /| \__ \ |_) | | (_| | |_| |
 //          \_/\_____/\____/\_____/\____/ \____/ |___/ |_|___/ .__/|_|\__,_|\__, |
 //                                                           | |             __/ |
 //                                                           |_|            |___/
 //		     ______      _             _   _  __  _____  _____
 //			 |  _  \    (_)           | | | |/  ||  _  ||  _  |
 //		     | | | |_ __ ___   _____  | | | |`| || |/' || |/' |
 //			 | | | | '__| \ \ / / _ \ | | | | | ||  /| ||  /| |
 //			 | |/ /| |  | |\ V /  __/ \ \_/ /_| |\ |_/ /\ |_/ /
 //          |___/ |_|  |_| \_/ \___|  \___/ \___/\___/  \___/
 //     
 /////////////////////////////////////////////////////////////////////////////////////////
--------------------------------- Main Program (tab 4) C++ ------------------------------
*/

/**
******************************************************************************************
*@file   
*@author Joao Dartagnan Antunes Oliveira
*@date 07/02/2016
*@brief <b>(Main)</b> File Program That Run a Demo for T6963C for users learn more about this drive
*@copyright GNU Public License.
******************************************************************************************
*/

/*
==========================================================================================
@HARDWARE:
@OBSERVAÇÃO:
==========================================================================================
______                                      _
| ___ \                                    | |
| |_/ / __ ___   __ _ _ __ __ _ _ __ ___   | |     ___   __ _
|  __/ '__/ _ \ / _` | '__/ _` | '_ ` _ \  | |    / _ \ / _` |
| |  | | | (_) | (_| | | | (_| | | | | | | | |___| (_) | (_| |
\_|  |_|  \___/ \__, |_|  \__,_|_| |_| |_| \_____/\___/ \__, |
                 __/ |                                   __/ |
                 |___/                                   |___/
==========================================================================================
DATA		HORA		DESCRIÇÃO
__________	_____		__________________________________________________________________________
2015/12/22	21:32	    Projeto do Software Iniciado.

*/

/*
+--------------------------------------+
|        AVR/GNU C++ COMPILER	       |
+--------------------------------------+

Symbols		    = -DF_CPU=16000000L -D_TIME_TIMER_NUMBER_=0
Optimization	= -Os
Miscellaneous	= -std=gnu++11
*/

/*
+--------------------------------------+
|        GLOBALS INCLUSION		       |
+--------------------------------------+
*/


/*
+--------------------------------------+
|        HARDWARE DEPENDENT INCLUDES   |
+--------------------------------------+
*/
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>

/*
+--------------------------------------+
|        C LIBRARY INCLUDES		       |
+--------------------------------------+
*/

#include <stdio.h>
#include <string.h>

/*
+--------------------------------------+
|        HARDWARE RESOURCES	           |
+--------------------------------------+
*/

/*
+--------------------------------------+
|        LIBRARY INCLUDES	           |
+--------------------------------------+
*/

#include "_Time_v125.h"
#include "_T6963C_Display_Drive_V141.h"


/*
==================================================================================
=        ___  ___      _        ______                                           =
=        |  \/  |     (_)       | ___ \                                          =
=        | .  . | __ _ _ _ __   | |_/ / __ ___   __ _ _ __ __ _ _ __ ___         =
=        | |\/| |/ _` | | '_ \  |  __/ '__/ _ \ / _` | '__/ _` | '_ ` _ \        =
=        | |  | | (_| | | | | | | |  | | | (_) | (_| | | | (_| | | | | | |       =
=        \_|  |_/\__,_|_|_| |_| \_|  |_|  \___/ \__, |_|  \__,_|_| |_| |_|       =
=                                                __/ |                           =
=                                               |___/                            =
==================================================================================
*/

/////////////////////////////////////////////////////
//  INITIALIZE CLASSES
/////////////////////////////////////////////////////

class_T6963C_Drive_V141	dp(16,20,20,_T6963C_ScanMode::DUAL);
class_Time				time;

/////////////////////////////////////////////////////
//  LOCAL VARIABLES
/////////////////////////////////////////////////////

uint8_t Demo_Count=0;

/////////////////////////////////////////////////////
//  SPRINTF BUFFER
/////////////////////////////////////////////////////

char bf[300];


/////////////////////////////////////////////////////
//  BITMAPS
/////////////////////////////////////////////////////

#include "_Resources/BMP_borboletas.H"
#include "_Resources/BMP_mario_bros.H"
#include "_Resources/BMP_SONIC.H"
#include "_Resources/BMP_GATO.H"
#include "_Resources/BMP_et.H"
#include "_Resources/BMP_AVR.H"
#include "_Resources/BMP_guarda-chuva.H"
#include "_Resources/BMP_Ball.H"

///@ingroup groupBMP
///@{

/////////////////////////////////////////////////////
//  HALT PROGRAM
/////////////////////////////////////////////////////
void	inline halt()
{
	for(;;);
}
/////////////////////////////////////////////////////
//  COMPUTE RANDON NUMBER FROM START TO END
/////////////////////////////////////////////////////
int rnd(int startValue,int endValue)
{
	return rand() % (endValue-startValue+1)+startValue;
}
/////////////////////////////////////////////////////
//  ROTATE PATTERN LEFT
/////////////////////////////////////////////////////
/**
*@brief Rotate Patterns Left
*@brief Rotate Patterns Left
*@brief Rotate Patterns Left
*@brief Rotate Patterns Left
*@brief Rotate Patterns Left
*@brief Rotate Patterns Left
*@brief Rotate Patterns Left
*/
void	RotatePatternLeft(_T6963C_Pattern &pattern)
{
	uint16_t shift;
	
	for(uint8_t i=0;i<8;i++)
	{
		shift=pattern.row[i];
		shift<<=1;
		shift|=(shift>>8)&1;
		pattern.row[i]=shift;
	}
}
/////////////////////////////////////////////////////
//  ROTATE PATTERN RIGHT
/////////////////////////////////////////////////////
void	RotatePatternRight(_T6963C_Pattern &pattern)
{
	uint16_t shift;
	
	for(uint8_t i=0;i<8;i++)
	{
		shift=pattern.row[i];
		shift|=((shift&1)<<8);
		shift>>=1;
		pattern.row[i]=shift;
	}
}
/////////////////////////////////////////////////////
//  DRAW GRAPH TEXT CENTERED
/////////////////////////////////////////////////////
void DrawGTextCentered(const char *textString,bool fromFlash=true)
{
	int xi,yi,xf,yf;

	dp.GetWindow(xi,yi,xf,yf);
	if (fromFlash)
	{
		dp.LocateXY(((xf-xi+1)-strlen_P(textString)*dp.GetGraphCharWidth())/2,((yf-yi+1)-dp.GetGraphCharHeight())/2+yi);
		dp.GPutSF(textString);
	}
	else
	{
		dp.LocateXY(((xf-xi+1)-strlen(textString)*dp.GetGraphCharWidth())/2,((yf-yi+1)-dp.GetGraphCharHeight())/2+yi);
		dp.GPutS(textString);
	}
	
	
}
/////////////////////////////////////////////////////
//  DEMO CLEAR SCREEN
/////////////////////////////////////////////////////
void Demo_Clear_Screen()
{
	dp.SetWindow(0,0,dp.GraphWidth()-1,dp.GraphHeight()-1);
	for(uint8_t c=1;c<=dp.TextWidth();c++)
	{
		for(uint8_t l=1;l<dp.TextHeight();l++)
		{
			dp.LocateRC(l,c);dp.PutSF(PSTR(" "));
		}
		for(uint8_t x=(c-1)*8;x<(c-1)*8+8;x++)
		{
			dp.DrawVertLine(x,0,dp.GraphHeight(),Pen::Black);
			dp.DrawVertLine(x-1,0,dp.GraphHeight(),Pen::White);
		}
		//time.DelayMillisByInterrupt(5);
	}
}
/////////////////////////////////////////////////////
//  DRAW TITLE
/////////////////////////////////////////////////////
void DrawTitle(const char *titleString)
{
	DrawMode dm;

	//set window for writing
	dp.SetWindow(0,0,dp.GraphWidth()-1,32);
	//save DrawMode
	dm=dp.GetDrawMode();
	//Set new Draw Mode
	dp.SetDrawMode(DrawMode::LINE);
	//Clear Graph Screen and Text too
	dp.ClsGraph();
//	dp.ClsText();
	//Draw title Text
//	dp.LocateRC(1,1);
	dp.LocateXY(0,0);
	sprintf(bf,"#%02d-",Demo_Count++);
	dp.GPutS(bf);
	dp.GPutSF(titleString);
	//Draw Graph Area border
	dp.SetWindow(0,33,dp.GraphWidth()-1,dp.GraphHeight()-1);
	//Draw Box
	dp.DrawBox(0,33,dp.GraphWidth()-1,dp.GraphHeight()-1,Pen::Black);
	//Restore Draw Mode
	dp.SetDrawMode(dm);
	//Set Graph Window
	dp.SetWindow(1,34,158,126);
}
/////////////////////////////////////////////////////
//  INITIALIZATION SCREEN
/////////////////////////////////////////////////////
void Demo_Initialization(const char *titleString,uint32_t timeToExit)
{
	int xi,yi,xf,yf;

	DrawTitle(titleString);
	dp.GetWindow(xi,yi,xf,yf);

	dp.LocateXY(6,yi+8);
	if (dp.GetTextPages()>1)
	{
		sprintf(bf,"RAM........ = %d Bytes\r\n Text Pages. = %4d\r\n Graph Pages = %4d\r\n Text Addr1. = %04X Hex\r\n Text Addr2. = %04X Hex\r\n Graph Addr1 = %04X Hex\r\n Graph Addr2 = %04X Hex\r\n Display 160x128 Pixels\r\n This Display\r\n Support Pages",dp.GetRAMSize(),dp.GetTextPages(),dp.GetGraphPages(),dp.GetTextAddress(),dp.GetText2Address(),dp.GetGraphAddress(),dp.GetGraph2Address());
	}
	else
	{
		sprintf(bf,"RAM........ = %d Bytes\r\n Text Pages. = %4d\r\n Graph Pages = %4d\r\n Text Addr1. = %04X Hex\r\n Graph Addr1 = %04X Hex\r\n Display 160x128 Pixels\r\n This Display Do \r\n Not Support Pages",dp.GetRAMSize(),dp.GetTextPages(),dp.GetGraphPages(),dp.GetTextAddress(),dp.GetGraphAddress());
	}
	dp.GPutS(bf);
	time.DelayMillisByInterrupt(timeToExit);
}
/////////////////////////////////////////////////////
//  ANIMATED PATTERNS AND PENS
/////////////////////////////////////////////////////
void Demo_Animated_Patterns_and_Pens(const char *titleString,uint32_t timeToExit,uint32_t timeSpeed=100)
{
	uint32_t	tPen,tBorder,tSpeed,tExit;
	Pixel		border=1;
	bool		bBorder=false;
	Pen			penBar=Pen::Bar_Dot,penDotDot=Pen::Dot_Dot_2;

	_T6963C_Pattern patternCircle,patternBox,patternSlash,patternGrid;

	DrawTitle(titleString);


	dp.SetDisplayMode(_T6963C_DisplayModes::_CURSOR,false);

	dp.GetDrawPatternType(_T6963C_Patterns::CIRCLE,patternCircle);
	dp.GetDrawPatternType(_T6963C_Patterns::DIAGONAL_GRID,patternBox);
	dp.GetDrawPatternType(_T6963C_Patterns::SLASH,patternSlash);
	dp.GetDrawPatternType(_T6963C_Patterns::GRID,patternGrid);

	dp.SetDrawMode(DrawMode::PATTERN);

	time.SetElapsed(&tPen);
	time.SetElapsed(&tBorder);
	time.SetElapsed(&tExit);
	time.SetElapsed(&tSpeed);
	for(;;)
	{
		dp.SetDrawPattern(patternCircle);
		dp.DrawCircle(40,80,27,Pen::Black,1,border);
		RotatePatternLeft(patternCircle);
	
		dp.SetDrawPattern(patternSlash);
		dp.DrawBox(2,34,157,49,penBar);
		RotatePatternLeft(patternSlash);
	
		dp.SetDrawPattern(patternBox);
		dp.DrawCircle(120,80,27,Pen::Black,1,border);
		RotatePatternRight(patternBox);

		dp.SetDrawPattern(patternGrid);
		dp.DrawBox(2,110,157,125,penDotDot);
		RotatePatternRight(patternGrid);

		if (time.GetElapsed(&tPen)>2000)
		{
			time.SetElapsed(&tPen);
			Pen tmp;
			tmp=penBar;
			penBar=penDotDot;
			penDotDot=tmp;
		}
		if (time.GetElapsed(&tBorder)>10000)
		{
			time.SetElapsed(&tBorder);
			bBorder=!bBorder;
		}
	
		//rotate Pen
		uint16_t shift;
	
		shift=static_cast<uint8_t>(penBar);
		shift<<=1;
		shift|=(shift>>8);
		penBar=static_cast<Pen>(shift);
	
		shift=static_cast<uint8_t>(penDotDot);
		shift<<=1;
		shift|=(shift>>8);
		penDotDot=static_cast<Pen>(shift);
		
		//see if time to exit
		if (time.GetElapsed(&tExit)>timeToExit)
		{
			break;
		}
		//speed
		do{
		} while (time.GetElapsed(&tSpeed)<timeSpeed);
		time.SetElapsed(&tSpeed);
	}	
}
/////////////////////////////////////////////////////
//  SUPPORT LARGE BMP IMAGE
/////////////////////////////////////////////////////
void Demo_Support_Large_BMP_Image(const char *titleString,uint32_t timeToExit)
{
	uint32_t	tExit;
	int			x=0,y=0,iX=1,iY=1,maxY;
	
	maxY=dp.GetBMPHeight(BMP_borboletas)-dp.GraphHeight();

				  //12345678901234567890
	DrawTitle(titleString);

	time.SetElapsed(&tExit);

	for(;;)
	{
		//draw bmp
		dp.DrawBitMap(-x,-y,BMP_borboletas);
		//compute show position
		x+=iX;
		if (x>dp.GraphWidth()) iX=-iX;
		if (x<0) iX=-iX;
		y+=iY;
		if (y>maxY) iY=-iY;
		if (y<0) iY=-iY;

		//see if time to exit
		if (time.GetElapsed(&tExit)>timeToExit)
		{
			break;
		}
	}	
}
/////////////////////////////////////////////////////
//  SAMPLE LINE PENS
/////////////////////////////////////////////////////
void Demo_Sample_Line_Pens(const char *titleString,uint32_t timeToExit)
{
	uint32_t	tExit;
	int			xi,yi,xf,yf,
				h,y;

	DrawTitle(titleString);

	time.SetElapsed(&tExit);

	//get window position
	dp.GetWindow(xi,yi,xf,yf);
	
	h=(yf-yi)/8;

	y=yi+h;

	//Draw Lines
	dp.DrawLine(xi+10,y,xf-10,y,Pen::Bar);	
	y+=h;
	dp.DrawLine(xi+10,y,xf-10,y,Pen::Bar_Dot);
	y+=h;
	dp.DrawLine(xi+10,y,xf-10,y,Pen::Black);
	y+=h;
	dp.DrawLine(xi+10,y,xf-10,y,Pen::Dot);
	y+=h;
	dp.DrawLine(xi+10,y,xf-10,y,Pen::Dot_Dot);
	y+=h;
	dp.DrawLine(xi+10,y,xf-10,y,Pen::Dot_Dot_2);
	y+=h;
	dp.DrawLine(xi+10,y,xf-10,y,Pen::Dot_Dot_3);
	y+=h;
	//wait show time
	time.DelayMillisByInterrupt(timeToExit);
}

/////////////////////////////////////////////////////
//  LINE PERFORMANCE
/////////////////////////////////////////////////////
void Demo_Line_Performance(const char *titleString,uint32_t timeToExit)
{
	uint32_t	tExit,tMeasure,tMeasureResult,tMeasureTotal,tMeasureCount;
	int			xi,yi,xf,yf,
				x,y;
	uint8_t		pColor=0;

	DrawTitle(titleString);

	time.SetElapsed(&tExit);

	//get window position
	dp.GetWindow(xi,yi,xf,yf);

	//clear timers
	time.SetElapsed(&tExit);
	
	//main loop
	tMeasureTotal=0;
	tMeasureCount=0;
	for(;;)
	{
		//Compute random points range from xi..xf,yi..yf
		x=rnd(xi,xf);
		y=rnd(yi,yf);
		//set time line measure
		time.SetElapsed(&tMeasure);
		//draw line
		dp.DrawLineTo(x,y,(pColor&1)?Pen::Black:Pen::White);
		//compute time
		tMeasureResult=time.GetElapsed(&tMeasure);
		tMeasureTotal+=tMeasureResult;
		tMeasureCount++;
		//new pen
		pColor++;
		//check if time to exit
		if (time.GetElapsed(&tExit)>timeToExit) break;
	}
	//print results
	dp.ClsGraph();
	dp.LocateXY(8,yi+8);
			  //12345678901234567890
	sprintf(bf,"%lu Lines/Sec",1000/(tMeasureTotal/tMeasureCount));
	//dp.GPutS(bf);
	DrawGTextCentered(bf,false);
	time.DelayMillisByInterrupt(timeToExit/5);
}
/////////////////////////////////////////////////////
//  BOX PATTERNS
/////////////////////////////////////////////////////
void Demo_Box_Patterns(const char *titleString,uint32_t timeToExit)
{
	uint32_t	tExit;
	int			xi,yi,xf,yf,
				w,h;
	_T6963C_Pattern	sPat;
	uint8_t sPats;
	DrawMode dm;

	DrawTitle(titleString);

	time.SetElapsed(&tExit);

	//get window position
	dp.GetWindow(xi,yi,xf,yf);

	//compute Width and Height for boxes
	w=(xf-xi)/3;
	h=(yf-yi)/3;
	//Save Pattern
	dp.GetDrawPattern(sPat);
	//save Drawmode
	dm=dp.GetDrawMode();
	dp.SetDrawMode(DrawMode::PATTERN);
	sPats=static_cast<uint8_t>(_T6963C_Patterns::EMPYT);
	//draw Boxes
	for(int y=0;y<3;y++)
	{
		for(int x=0;x<3;x++)
		{
			dp.SetDrawPatternType(static_cast<_T6963C_Patterns>(sPats));
			dp.DrawBox(x*w+xi+2,y*h+yi+2,x*w+xi+w-2,y*h+yi+h-2,Pen::Black);
			sPats++;
		}
	}
	//restore pattern
	dp.SetDrawPattern(sPat);
	//restore Draw Mode
	dp.SetDrawMode(dm);
	//wait show time
	time.DelayMillisByInterrupt(timeToExit);
}
/////////////////////////////////////////////////////
//  CIRCLE PATTERNS
/////////////////////////////////////////////////////
void Demo_Circle_Patterns(const char *titleString,uint32_t timeToExit,bool ellipse=false)
{
	uint32_t	tExit;
	int			xi,yi,xf,yf,
				w,h;
	_T6963C_Pattern	sPat;
	uint8_t sPats;
	DrawMode dm;

	DrawTitle(titleString);

	time.SetElapsed(&tExit);

	//get window position
	dp.GetWindow(xi,yi,xf,yf);

	//compute Width and Height for boxes
	w=(xf-xi)/3;
	h=(yf-yi)/3;
	//Save Pattern
	dp.GetDrawPattern(sPat);
	//save Drawmode
	dm=dp.GetDrawMode();
	dp.SetDrawMode(DrawMode::PATTERN);
	sPats=static_cast<uint8_t>(_T6963C_Patterns::EMPYT);
	//draw Boxes
	for(int y=0;y<3;y++)
	{
		for(int x=0;x<3;x++)
		{
			dp.SetDrawPatternType(static_cast<_T6963C_Patterns>(sPats));
			if (ellipse)
			{
				dp.DrawEllipse(x*w+w/2+xi+2,y*h+h/2+yi+2,rnd(1,w/2),rnd(1,h/2),Pen::Black);
			}
			else
			{
				dp.DrawCircle(x*w+w/2+xi+2,y*h+h/2+yi+2,((w>h)?h:w)/2-1,Pen::Black);
			}
			
			sPats++;
		}
	}
	//restore pattern
	dp.SetDrawPattern(sPat);
	//restore Draw Mode
	dp.SetDrawMode(dm);
	//wait show time
	time.DelayMillisByInterrupt(timeToExit);
}
/////////////////////////////////////////////////////
//  DRAW BITMAPS
/////////////////////////////////////////////////////
void Demo_Draw_BitMaps(const char *titleString,uint32_t timeToExit)
{
	uint32_t	tExit;
	int			xi,yi,xf,yf,
				w,h;

	DrawTitle(titleString);

	time.SetElapsed(&tExit);

	//get window position
	dp.GetWindow(xi,yi,xf,yf);

	//compute Width and Height for boxes
	w=(xf-xi)/3;
	h=(yf-yi)/2;

	//draw bmps
	dp.DrawBitMap(xi,yi,BMP_mario_bros);
	dp.DrawBitMap(xi+w,yi,BMP_Sonic);
	dp.DrawBitMap(xi+2*w,yi,BMP_GATO);
	dp.DrawBitMap(xi,yi+h,BMP_et);
	dp.DrawBitMap(xi+w,yi+h,BMP_AVR);
	dp.DrawBitMap(xi+2*w,yi+h,BMP_guarda_chuva);
	//wait show time
	time.DelayMillisByInterrupt(timeToExit);
}

/////////////////////////////////////////////////////
//  BOX PERFORMANCE
/////////////////////////////////////////////////////
void Demo_Box_Performance(const char *titleString,uint32_t timeToExit,bool border=true)
{
	uint32_t	tExit,tMeasure,tMeasureResult,tMeasureTotal,tMeasureCount;
	int			xi,yi,xf,yf,
				x0,y0,x1,y1;
	_T6963C_Patterns pat;
	DrawMode dm;

	DrawTitle(titleString);

	time.SetElapsed(&tExit);

	//get window position
	dp.GetWindow(xi,yi,xf,yf);

	//save Draw Mode
	dm=dp.GetDrawMode();
	//save pattern type
	//Set Draw Pattern
	dp.SetDrawMode(DrawMode::PATTERN);
	//clear timers
	time.SetElapsed(&tExit);
	
	//main loop
	tMeasureTotal=0;
	tMeasureCount=0;
	for(;;)
	{
		//Compute random points range from xi..xf,yi..yf
		x0=rnd(xi,xf);
		y0=rnd(yi,yf);
		x1=rnd(xi,xf);
		y1=rnd(yi,yf);
		pat=static_cast<_T6963C_Patterns>(rnd(0,9));
		dp.SetDrawPatternType(pat);
		//set time line measure
		time.SetElapsed(&tMeasure);
		//draw box
		dp.DrawBox(x0,y0,x1,y1,Pen::Black,border);
		//compute time
		tMeasureResult=time.GetElapsed(&tMeasure);
		tMeasureTotal+=tMeasureResult;
		tMeasureCount++;
		//check if time to exit
		if (time.GetElapsed(&tExit)>timeToExit) break;
	}
	//print results
	dp.ClsGraph();
	dp.LocateXY(8,yi+8);
	//12345678901234567890
	sprintf(bf,"%lu Box/Sec",1000/(tMeasureTotal/tMeasureCount));
	//dp.GPutS(bf);
	DrawGTextCentered(bf,false);
	//set draw mode
	dp.SetDrawMode(dm);
	//little time to show results
	time.DelayMillisByInterrupt(timeToExit/5);
}

/////////////////////////////////////////////////////
//  CIRCLE PERFORMANCE
/////////////////////////////////////////////////////
void Demo_Circle_Performance(const char *titleString,uint32_t timeToExit,bool border=true,bool ellipse=false)
{
	uint32_t	tExit,tMeasure,tMeasureResult,tMeasureTotal,tMeasureCount;
	int			xi,yi,xf,yf,
				x0,y0,r0,r1,r2;
	_T6963C_Patterns pat;
	DrawMode dm;

	DrawTitle(titleString);

	time.SetElapsed(&tExit);

	//get window position
	dp.GetWindow(xi,yi,xf,yf);

	//max radious
	r0=(yf-yi)/2;
	//save Draw Mode
	dm=dp.GetDrawMode();
	//save pattern type
	//Set Draw Pattern
	dp.SetDrawMode(DrawMode::PATTERN);
	//clear timers
	time.SetElapsed(&tExit);
	
	//main loop
	tMeasureTotal=0;
	tMeasureCount=0;
	for(;;)
	{
		//Compute random points range from xi..xf,yi..yf
		x0=rnd(xi,xf);
		y0=rnd(yi,yf);
		r1=rnd(1,r0);
		r2=rnd(1,r0);
		pat=static_cast<_T6963C_Patterns>(rnd(0,9));
		dp.SetDrawPatternType(pat);
		//set time line measure
		time.SetElapsed(&tMeasure);
		//draw box
		if (ellipse)
		{
			dp.DrawEllipse(x0,y0,r1,r2,Pen::Black,1,border);
		}
		else
		{
			dp.DrawCircle(x0,y0,r1,Pen::Black,1,border);
		}
		
		//compute time
		tMeasureResult=time.GetElapsed(&tMeasure);
		tMeasureTotal+=tMeasureResult;
		tMeasureCount++;
		//check if time to exit
		if (time.GetElapsed(&tExit)>timeToExit) break;
	}
	//print results
	dp.ClsGraph();
	dp.LocateXY(8,yi+8);
	//12345678901234567890
	if (ellipse)
	{
		sprintf(bf,"%lu Ellipses/Sec",1000/(tMeasureTotal/tMeasureCount));
	}
	else
	{
		sprintf(bf,"%lu Circles/Sec",1000/(tMeasureTotal/tMeasureCount));
	}
	DrawGTextCentered(bf,false);
	//dp.GPutS(bf);
	//set draw mode
	dp.SetDrawMode(dm);
	//little time to show results
	time.DelayMillisByInterrupt(timeToExit/5);
}
/////////////////////////////////////////////////////
//  PIXEL PERFORMANCE
/////////////////////////////////////////////////////
void Demo_Pixel_Performance(const char *titleString,uint32_t timeToExit)
{
	uint32_t	tExit,PixelCount;
	int			xi,yi,xf,yf,
				x,y;

	DrawTitle(titleString);

	//get window position
	dp.GetWindow(xi,yi,xf,yf);

	//clear timers
	time.SetElapsed(&tExit);
	
	PixelCount=0;
	//main loop
	for(;;)
	{
		//Compute random points range from xi..xf,yi..yf
		x=rnd(xi,xf);
		y=rnd(yi,yf);
		//draw box
		dp.SetPixel(x,y);
		PixelCount++;
		//check if time to exit
		if (time.GetElapsed(&tExit)>timeToExit) break;
	}
	//print results
	dp.ClsGraph();
	dp.LocateXY(8,yi+8);
	//12345678901234567890
	sprintf(bf,"%lu Pixels/Sec",PixelCount/10);
	DrawGTextCentered(bf,false);
	//dp.GPutS(bf);
	//little time to show results
	time.DelayMillisByInterrupt(timeToExit/5);
}

/////////////////////////////////////////////////////
//  PIXEL PERFORMANCE
/////////////////////////////////////////////////////
void Demo_Presentation(const char *titleString,uint32_t timeToExit)
{
	int			xi,yi,xf,yf;

	DrawTitle(titleString);

	//get window position
	dp.GetWindow(xi,yi,xf,yf);

	dp.LocateXY(6,yi+1*8);
	dp.GPutSF(PSTR("Toshiba T6963C\r\n Display Drive and \r\n Atmel AVR ATMega128\r\n Version "));
	sprintf(bf,"%d.%02d",dp.VersionMajor(),dp.VersionMinor());
	dp.GPutS(bf);
	dp.GPutSF(PSTR("\r\n Made with Atmel Studio 7\r\n By Joao D´Artagnan A. O.\r\n Brasilia Brazil\r\n January, 20, 2016"));
	//delay to exit
	time.DelayMillisByInterrupt(timeToExit);
}

/////////////////////////////////////////////////////
//  Texts
/////////////////////////////////////////////////////
void Demo_Texts(const char *titleString,uint32_t timeToExit)
{
	int			xi,yi,xf,yf;

	DrawTitle(titleString);

	//get window position
	dp.GetWindow(xi,yi,xf,yf);

	dp.SetDrawMode(DrawMode::LINE);
	dp.DrawHorzLine((yf+yi)/2,xi,xf,Pen::Black);

	dp.LocateRC(6,2);
	dp.PutSF(PSTR("This a text\r\n hardware generate\r\n\r\n ABCDEFGHIJKL..."));
	dp.LocateXY(6,yi+6*8+4);
	dp.GPutSF(PSTR("This a text\r\n graph generate\r\n\r\n ABCDEFGHIJKL..."));
	//delay to exit
	time.DelayMillisByInterrupt(timeToExit);
}

/////////////////////////////////////////////////////
//  Texts
/////////////////////////////////////////////////////
void Demo_Text_Graph_Types(const char *titleString,uint32_t timeToExit)
{
	int			xi,yi,xf,yf;

	DrawTitle(titleString);

	//get window position
	dp.GetWindow(xi,yi,xf,yf);

	dp.SetDrawMode(DrawMode::PATTERN);
	dp.SetDrawPatternType(_T6963C_Patterns::DIAGONAL_GRID);
	dp.DrawBox(xi,yi,xf,yf);
	dp.SetDrawMode(DrawMode::LINE);
	dp.LocateXY(6,yi+3*8);
	dp.GPutSF(PSTR("This a Text LINE\r\n\r\n "));
	dp.SetDrawMode(DrawMode::SOLID);
	dp.GPutSF(PSTR("This a Text SOLID Normal\r\n\r\n "));
	dp.GPutSF(PSTR("This a Text SOLID Inverted\r\n\r\n"),0);
	dp.SetDrawMode(DrawMode::LINE);
	//delay to exit
	time.DelayMillisByInterrupt(timeToExit);
}

/////////////////////////////////////////////////////
//  Texts Graph Scale
/////////////////////////////////////////////////////
void Demo_Graph_Text_Scale(const char *titleString,uint32_t timeToExit)
{
	int			xi,yi,xf,yf;
	uint8_t		pixel;

	DrawTitle(titleString);

	//get window position
	dp.GetWindow(xi,yi,xf,yf);
	dp.SetDrawMode(DrawMode::SOLID);
	for(int x=6;x<=(80+6);x+=80)
	{
		//select pixel
		if (x==6) pixel=1; else pixel=0;
		//Draw 3 Text Scaled
		dp.SetGraphTextScale(1);
		dp.LocateXY(x,yi+dp.GetGraphCharHeight());
		dp.GPutSF(PSTR("ABC"),pixel);
	
		dp.SetGraphTextScale(2);
		dp.LocateXY(x,yi+dp.GetGraphCharHeight()*2);
		dp.GPutSF(PSTR("ABC"),pixel);

		dp.SetGraphTextScale(3);
		dp.LocateXY(x,yi+dp.GetGraphCharHeight()*4);
		dp.GPutSF(PSTR("ABC"),pixel);

		dp.SetGraphTextScale(4);
		dp.LocateXY(x,yi+dp.GetGraphCharHeight()*7);
		dp.GPutSF(PSTR("ABC"),pixel);
	}

	dp.SetGraphTextScale(1);
	dp.SetDrawMode(DrawMode::LINE);
	//delay to exit
	time.DelayMillisByInterrupt(timeToExit);
}

/////////////////////////////////////////////////////
//  Texts
/////////////////////////////////////////////////////
void Demo_Draw_Ball_Pages(const char *titleString,uint32_t timeToExit)
{
	uint32_t	tExit;
	int			xi,yi,xf,yf,
				x,y,iX,iY,x2,y2,iX2,iY2;
	uint8_t		aP,vP;

	dp.SetGraphActivePage(1);
	DrawTitle(titleString);
	dp.SetGraphActivePage(2);
	DrawTitle(titleString);
	dp.SetGraphActivePage(1);

	//get window position
	dp.GetWindow(xi,yi,xf,yf);

	xf-=dp.GetBMPWidth(BMP_Ball);
	yf-=dp.GetBMPHeight(BMP_Ball);

	x=rnd(xi,xf);
	y=rnd(yi,yf);
	iX=rnd(2,5);
	iY=rnd(2,5);

	x2=rnd(xi,xf);
	y2=rnd(yi,yf);
	iX2=rnd(2,5);
	iY2=rnd(2,5);

	vP=1;
	aP=2;
	time.SetElapsed(&tExit);
	for(;;)
	{
		dp.SetGraphVisiblePage(vP);
		dp.SetGraphActivePage(aP);
		//draw bitmap
		dp.ClsGraph();
		dp.DrawBitMap(x,y,BMP_Ball);
		dp.DrawBitMap(x2,y2,BMP_Ball);
		//swap pages
		uint8_t tmp;
		tmp=aP;
		aP=vP;
		vP=tmp;
		//update coords x,y
		x+=iX;
		if (x>xf)
		{
			x=xf;
			iX=-iX;
			if (iX<0) iX=-rnd(2,5); else iX=rnd(2,5);
		}
		else
		{
			if (x<xi)
			{
				x=xi;
				iX=-iX;
				if (iX<0) iX=-rnd(2,5); else iX=rnd(2,5);
			}
		}
		y+=iY;
		if (y>yf)
		{
			y=yf;
			iY=-iY;
			if (iY<0) iY=-rnd(2,5); else iY=rnd(2,5);
		}
		else
		{
			if (y<yi)
			{
				y=yi;
				iY=-iY;
				if (iY<0) iY=-rnd(2,5); else iY=rnd(2,5);
			}
		}

		//update coords x2,y2
		x2+=iX2;
		if (x2>xf)
		{
			x2=xf;
			iX2=-iX2;
			if (iX2<0) iX2=-rnd(2,5); else iX2=rnd(2,5);
		}
		else
		{
			if (x2<xi)
			{
				x2=xi;
				iX2=-iX2;
				if (iX2<0) iX2=-rnd(2,5); else iX2=rnd(2,5);
			}
		}
		y2+=iY2;
		if (y2>yf)
		{
			y2=yf;
			iY2=-iY2;
			if (iY2<0) iY2=-rnd(2,5); else iY2=rnd(2,5);
		}
		else
		{
			if (y2<yi)
			{
				y2=yi;
				iY2=-iY2;
				if (iY2<0) iY2=-rnd(2,5); else iY2=rnd(2,5);
			}
		}
		//time to exit?
		if (time.GetElapsed(&tExit)>timeToExit) break;
	}
	//defalt active pages
	dp.SetGraphActivePage(1);
	dp.SetGraphVisiblePage(1);
}

/////////////////////////////////////////////////////
//  Copy BitMap using SetPixel and GetPixel
/////////////////////////////////////////////////////
void Demo_Copy_BitMap(const char *titleString,uint32_t timeToExit)
{
	int			xi,yi,xf,yf;

	DrawTitle(titleString);

	//get window position
	dp.GetWindow(xi,yi,xf,yf);

	
	dp.DrawBitMap(xi,yi,BMP_mario_bros);

	for(int y=0;y<dp.GetBMPHeight(BMP_mario_bros);y++)
	{
		for(int x=0;x<dp.GetBMPWidth(BMP_mario_bros);x++)
		{
			dp.SetPixel(x+80,y+yi,dp.GetPixel(x+xi,y+yi));
			dp.SetPixel(x+80,y+dp.GetBMPHeight(BMP_mario_bros) +yi,dp.GetPixel(x+xi,y+yi));
			dp.SetPixel(x,y+dp.GetBMPHeight(BMP_mario_bros)+yi,dp.GetPixel(x+xi,y+yi));
		}
	}

	//delay to exit
	time.DelayMillisByInterrupt(timeToExit);
}

//////////////////////////////////////////////////////
//
// M A I N   P R O G R A M
//
//////////////////////////////////////////////////////



int main()
{
	
	//Check if interface Prensent and Ok, case not flash LED 150ms
	if (dp.Present()==false)
	{
		dp.BlinkLEDError(150);
	}

	//Set Cursor OFF
	dp.SetDisplayMode(_T6963C_DisplayModes::_CURSOR,false);
	//
	//DEMO MAIN LOOOP
	//

	for(;;)
	{
						                 //         1         2         3         4         5         6         7         8
						                 //12345678901234567890123456789012345678901234567890123456789012345678901234567890
		Demo_Count=1;


		Demo_Presentation(				PSTR("Drive Presentation\n\r    Version and Author"),10000);
		Demo_Clear_Screen();

		Demo_Initialization(            PSTR("Display\r\n    Initialization\r\n    and\n\r    Features"),10000);
		Demo_Clear_Screen();

		Demo_Texts(						PSTR("Hardware and Graph\r\n    characters generator"),10000);
		Demo_Clear_Screen();
		Demo_Text_Graph_Types(			PSTR("Graph text may be\n\r    LINE,SOLID,INVERTED"),10000);
		Demo_Clear_Screen();
		Demo_Graph_Text_Scale(			PSTR("Graph Text\r\n    with Scales"),10000);
		Demo_Clear_Screen();


		Demo_Sample_Line_Pens(	        PSTR("Draw Lines with\r\n    Pens Patterns"),10000);
		Demo_Clear_Screen();
		Demo_Box_Patterns(				PSTR("Draw Boxes\r\n    with Patterns"),5000);
		Demo_Clear_Screen();
		Demo_Circle_Patterns(			PSTR("Draw Circles\r\n    with Patterns"),5000);
		Demo_Clear_Screen();
		Demo_Circle_Patterns(			PSTR("Draw Ellipses\r\n    with Patterns"),5000,true);
		Demo_Clear_Screen();

		Demo_Pixel_Performance(			PSTR("Draw Pixels\r\n    Performance"),10000);
		Demo_Clear_Screen();
		Demo_Line_Performance(			PSTR("Draw Line\n\r    Performance"),10000);
		Demo_Clear_Screen();
		Demo_Box_Performance(			PSTR("Draw Boxes\n\r    Performance"),10000);
		Demo_Clear_Screen();
		Demo_Circle_Performance(		PSTR("Draw Circle\r\n    Performance"),10000);
		Demo_Clear_Screen();
		Demo_Circle_Performance(		PSTR("Draw Ellipses\r\n    Performance"),10000,true,true);
		Demo_Clear_Screen();

// 		Demo_Box_Performance(			PSTR("Draw Boxes\n\r    Performance\r\n    with no border"),10000,false);
// 		Demo_Clear_Screen();
// 		Demo_Circle_Performance(		PSTR("Draw Circle\r\n    Performance\r\n    with no border"),10000,false);
// 		Demo_Clear_Screen();
// 		Demo_Circle_Performance(		PSTR("Draw Ellipses\r\n    Performance\r\n    with no border"),10000,false,true);
// 		Demo_Clear_Screen();

 		Demo_Animated_Patterns_and_Pens(PSTR("Draw Circles  & Boxes\r\n    with  moving Patterns\r\n    and Pens Full speed"),10000,1);
		Demo_Clear_Screen();
 		Demo_Animated_Patterns_and_Pens(PSTR("Draw Circles  & Boxes\r\n    with  moving Patterns\r\n    and Pens Low speed"),10000,300);
		Demo_Clear_Screen();

		Demo_Draw_BitMaps(				PSTR("Draw BitMaps"),10000);
		Demo_Clear_Screen();
		Demo_Copy_BitMap(				PSTR("Copy BitMap\r\n    using\r\n    SetPixel/GetPixel"),10000);
		Demo_Clear_Screen();
		Demo_Support_Large_BMP_Image(   PSTR("Support Large\r\n    BitMap to Draw\r\n    Size < 32Kbytes"),10000);
		Demo_Clear_Screen();
		Demo_Draw_Ball_Pages(			PSTR("Ball Animation using\r\n    Graph Pages\r\n    and BitMap"),20000);
		Demo_Clear_Screen();

	}

}

///@}