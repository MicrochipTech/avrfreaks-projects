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
	//////////////////////////////////////////////////////////////////////////////////////////
	----------------------------- Time Common Header (tab 4) ---------------------------------
		  
*/

/**
******************************************************************************************
*@file   
*@author Joao Dartagnan Antunes Oliveira
*@date 07/02/2016
*@brief <b>(Header)</b> File for Graphics engine
*@copyright GNU Public License.
******************************************************************************************
*/
//+-------------------------------------------------------
//| Doxygen Group Definitions
//+-------------------------------------------------------

///@defgroup groupGraphic		Graphics
///@brief 					    All Functions that involve Graphics
///@defgroup groupDrawGraphic   Graphics Draw
///@brief 					    Functions to Draw in Graphic Area
///@defgroup groupVersions		Versions
///@brief 					    Functions to handdle Control Versions

#ifndef _GRAPH_BASIC_LIB_H_
#define _GRAPH_BASIC_LIB_H_

/*
 +----------------------------------------------------------------------+
 | ENABLE BELOW LINE TO USE CIRCLE ALGORITH INSTEAD ELLIPSE ALGORITHM	|
 +----------------------------------------------------------------------+
*/

//#define _GRAPH_USE_CIRCLE_ALGORITHM_

/*
 +--------------------------------------+
 |        HARDWARE DEPENDENT INCLUDES	|
 +--------------------------------------+
*/
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
/*
 +--------------------------------------+
 |        C LIBRARY INCLUDES		    |
 +--------------------------------------+
*/
#include <stdlib.h>

// 
//  +--------------------------------------+
//  |        DEPENDENT INCLUDES            |
//  +--------------------------------------+
// 

// 
//  +--------------------------------------+
//  |        TIMER STRUCTS             	   |
//  +--------------------------------------+
// 

 
// 
// 	+--------------------------------------+
// 	|       CONSTANTS DEFINITIONS          |
// 	+--------------------------------------+
// 

#define _GRAPHBASIC_VERSION_MAJOR  1
#define _GRAPHBASIC_VERSION_MINOR  15

// 
// 	+--------------------------------------+
// 	|       ENUMERATIONS                   |
// 	+--------------------------------------+
// 

//
// 	+--------------------------------------+
// 	|       TYPEDEF			               |
// 	+--------------------------------------+
//
/**
********************************************
*@class _enum_DrawMode
*@brief This <b>typedef</b> enum class defines a Draw Modes 
********************************************
*/
typedef enum class _enum_DrawMode
{
	LINE=0,
	SOLID,
	PATTERN	
} DrawMode;
/**
********************************************
*@class _enum_Pen
*@brief This <b>typedef</b> enum class defines a pens types
********************************************
*/
typedef enum class _enum_Pen
{
	Black		=0xff,
	Dot			=0x10,
	Dot_Dot		=0x11,
	Dot_Dot_2	=0x33,
	Dot_Dot_3	=0x77,
	Bar			=0x3c,
	Bar_Dot		=0x72,
	White		=0x00	
} Pen;

typedef uint8_t Pixel;



/**
******************************************************************************
*
*	@class class_GraphBasic
*	@brief This <b>Class</b> Implement a Drawing commands like Line,Box,Circle,Ellipse etc
*
******************************************************************************
*/

class class_GraphBasic
{
//         
//          +--------------------------------------+
//          |       PRIVATE VARIABLES			   |
//          +--------------------------------------+
//         
private:
		DrawMode	_DrawMode=DrawMode::LINE;
        int			_Rows=1,
					_Cols=1;
		int			_X=0,
					_Y=0;
		int			_Width=1,
					_Height=1;
//         
//          +--------------------------------------+
//          |       PRIVATE FUNCTIONS              |
//          +--------------------------------------+
//         
		//	+-------------------------------------------------------------------------------+
		///@brief  Initialize Graph engine									
		///@fn     Initialize
		///@return none\n
		///@brief 
		//	+-------------------------------------------------------------------------------+
        void	Initialize(void);
		//	+-------------------------------------------------------------------------------+
		///@brief UnInitialize graph engine									
		///@fn    UnInitialize
		///@return none\n
		///@brief 
		//	+-------------------------------------------------------------------------------+
        void	UnInitialize(void);
		//	+-------------------------------------------------------------------------------+
		///@brief Plot 4 circular symmetrical points or fill a ellipse
		///@fn    PlotP4
		///@param [in] xc,yc center coordinates
		///@param [in] x,y offsets
		///@param [in] pen Pen used
		///@param [in] pixel Pixel used
		///@return none\n
		///@brief _______________________________________________________________________________
		///@brief Used by DrawEllipse() function 
		//	+-------------------------------------------------------------------------------+
		void	PlotP4(int xc,int yc,int x,int y,Pen pen,Pixel pixel);
		//	+-------------------------------------------------------------------------------+
		///@brief Plot 8 circular symmetric points or fill circle
		///@fn    PlotP8
		///@param [in] xx,yy center coordinates
		///@param [in] x,y offsets
		///@param [in] pen Pen used
		///@param [in] pixel Pixel used
		///@return none\n
		///@brief _______________________________________________________________________________
		///@brief Used by DrawEllipse() function
		
		//	+-------------------------------------------------------------------------------+
		void	PlotP8(int xx,int yy,int x,int y,Pen pen,Pixel pixel);	
		//	+-------------------------------------------------------------------------------+
		///@brief Used by Circle() function
		///@fn    CircleIntern												
		///@param [in] px,py center coordinates
		///@param [in] radius
		///@param [in] pen Pen used
		///@param [in] pixel Pixel used
		///@return none\n
		///@brief _______________________________________________________________________________
		///@brief Use by DrawCircle() function
		//	+-------------------------------------------------------------------------------+
		void	CircleIntern(int px,int py,int radius,Pen pen,Pixel pixel);
		//	+-------------------------------------------------------------------------------+
		///@brief Used by DrawEllipse() function
		///@fn    DrawEllipseIntern												
		///@param [in] xc,yc center coordinates
		///@param [in] rx,ry radius of axis x and axis y respectivelly
		///@param [in] pen Pen used
		///@param [in] pixel Pixel used
		///@return none\n
		///@brief 
		//	+-------------------------------------------------------------------------------+
		void	DrawEllipseIntern(int xc,int yc, int rx, int ry,Pen pen,Pixel pixel);
		///@brief Draw Box Full used internally by DrawBox()
		///@fn    DrawBoxFull													
		///@param [in] xi,yi start coordinates of the box
		///@param [in] xf,yf end coordinates of the box
		///@param [in] pen Pen used if not provide Pen::Black is assumed
		///@return none\n
		///@brief _______________________________________________________________________________
		///@brief Draw a box with Pen border if border is true	\n
		///@brief if DrawMode::SOLID Draw a Fill Box with a Pen \n							
		///@brief if DrawMode::PATTERN Draw a Fill box with Pattern	\n
		///@brief used by DrawBox() function
		void	DrawBoxFull(int xi,int yi,int xf,int yf,Pen pen);

///@}

//          +--------------------------------------+
//          |        PUBLIC FUNCTIONS		       |
//          +--------------------------------------+
//         
public:
		//	+-------------------------------------------------------------------------------+
		///@brief Initialization with Rows,Cols,Graphwidth,GraphHeight		
		///@fn    class_GraphBasic
		///@return none\n
		///@brief													
		//	+-------------------------------------------------------------------------------+
        class_GraphBasic(int Rows,int Cols,int GraphWidth,int GraphHeight);      
		//	+-------------------------------------------------------------------------------+
		///@brief Initialization with Rows,Cols,Graphwidth,GraphHeight		
		///@fn    ~class_GraphBasic
		///@return none\n
		///@brief													
		//	+-------------------------------------------------------------------------------+
        ~class_GraphBasic();
		//	+-------------------------------------------------------------------------------+
		///@brief Draw Fast Horizontal Line Virtual Function					
		///@fn    DrawHorzLine													
		///@param [in] y Line Ordinate
		///@param [in] xS,xE Line abscissa Start xS and xE end
		///@param [in] pen Pen if not provide, Pen::Black is assumed
		///@return none\n
		///@brief _______________________________________________________________________________
		///@brief DrawHorzLine() Examples with many pen types \n\n
		///@brief <table style="width:160px">
		///@brief 	<tr>
		///@brief 		<td><img src="_Photos\Lines_Example.jpg" style="width:150px;height:100px;"></td>
		///@brief 	</tr>
		///@brief </table>
		///@brief _______________________________________________________________________________			
		//	+-------------------------------------------------------------------------------+
		virtual void	DrawHorzLine(int y,int xS,int xE,Pen pen=Pen::Black);
		//	+-------------------------------------------------------------------------------+
		///@brief Set Pixel At coordinates x,y with color Virtual Function	
		///@fn    SetPixel													
		///@param [in] x,y	coordinates of the pixel							
		///@param [in] pixel Pixel value (0 or 1) if not provide (1)=black is assumed
		///@return none\n
		///@brief 	VIRTUAL														
		//	+-------------------------------------------------------------------------------+
        virtual void	SetPixel(int x,int y,Pixel pixel=1);
		//	+-------------------------------------------------------------------------------+
		///@brief Get Pixel At coordinates x,y with color Virtual Function	
		///@fn    GetPixel													
		///@param [in]   x,y	coordinates of the pixel							
		///@return Color of the pixel											
		///@brief													
		//	+-------------------------------------------------------------------------------+
        virtual Pixel	GetPixel(int x,int y);
		//	+-------------------------------------------------------------------------------+
		///@brief Draw Fast Vertical Line at coordinates xi,yi,xf,yf			
		///@fn    DrawVertLine												
		///@param [in] x start abscissa of the line
		///@param [in] yi,yf start and end ordenates 
		///@param [in] pen Pen used if not provide Pen::Black is assumed
		///@return none\n
		///@brief 
		//	+-------------------------------------------------------------------------------+
        void DrawVertLine(int x,int yi,int yf,Pen pen=Pen::Black);
		//	+-------------------------------------------------------------------------------+
		///@brief Draw Box at coordinates xi,yi,xf,yf with pen and border	
		///@fn    DrawBox													
		///@param [in] xi,yi start coordinates of the box
		///@param [in] xf,yf end coordinates of the box
		///@param [in] pen Pen used if not provide Pen::Black is assumed
		///@param [in] border if true draw a box border if not provide is assumed true
		///@return none\n
		///@brief _______________________________________________________________________________
		///@brief Draw a box with Pen border if border is true	\n
		///@brief if DrawMode::SOLID Draw a Fill Box with a Pen \n							
		///@brief if DrawMode::PATTERN Draw a Fill box with Pattern	\n
		///@brief Below some examples:\n\n
		///@brief <table style="width:150px">
		///@brief 	<tr>
		///@brief 		<td><img src="_Photos\Box_Line.jpg" style="width:64px;height:48px;"></td>
		///@brief 		<td><img src="_Photos\Box_Solid.jpg" style="width:64px;height:48px;"></td>
		///@brief 		<td><img src="_Photos\Box_Pattern.jpg" style="width:64px;height:48px;"></td>
		///@brief 	</tr>
		///@brief </table>
		///@brief _______________________________________________________________________________		
		//	+-------------------------------------------------------------------------------+
        void DrawBox(int xi,int yi,int xf,int yf,Pen pen=Pen::Black,bool border=true);
		//	+-------------------------------------------------------------------------------+
		///@brief Draw Line at coordinates xi,yi,xf,yf with pen				
		///@fn    DrawLine													
		///@param [in] xi,yi start coordinates of the Line
		///@param [in] xf,yf end coordinates of the line
		///@param [in] pen Pen used if not provide Pen::Black is assumed
		///@return none\n
		///@brief _______________________________________________________________________________
		///@brief DrawLine() Examples with many pen types \n\n
		///@brief <table style="width:160px">
		///@brief 	<tr>
		///@brief 		<td><img src="_Photos\Lines_Example.jpg" style="width:150px;height:100px;"></td>
		///@brief 	</tr>
		///@brief </table>
		///@brief _______________________________________________________________________________			
		//	+-------------------------------------------------------------------------------+
        void DrawLine(int xi,int yi,int xf,int yf,Pen pen=Pen::Black);
		//	+-------------------------------------------------------------------------------+
		///@brief Draw Line from previous x,y coordinates to new x,y			
		///@fn    DrawLineTo													
		///@param [in] x,y new coordinates
		///@param [in] pen Pen if not provide Pen::Black is assumed
		///@return none\n
		///@see DrawLine function 
		//	+-------------------------------------------------------------------------------+
		void DrawLineTo(int x,int y,Pen pen=Pen::Black);
		//	+-------------------------------------------------------------------------------+
		///@brief  Draw Circle at coordinates x,y and Radius with Pixel and border
		///@fn     DrawCircle													
		///@param [in] x,y center coordinates
		///@param [in] radius
		///@param [in] pen Pen used if not provide Pen::Black is assumed
		///@param [in] pixel Pixel (0 or 1) if not provide (1)=black is assumed
		///@param [in] border if true draw a box border if not provide is assumed true
		///@return none\n
		///@brief _______________________________________________________________________________
		///@brief Draw a Cirle with pen border if border is true \n
		///@brief if DrawMode::SOLID Draw a Fill circle	\n
		///@brief if DrawMode::PATTERN Draw a Fill circle with that pattern	\n\n
		///@brief Below some examples:\n\n
		///@brief <table style="width:150px">
		///@brief 	<tr>
		///@brief 		<td><img src="_Photos\Circle_Line.jpg" style="width:64px;height:48px;"></td>
		///@brief 		<td><img src="_Photos\Circle_Solid.jpg" style="width:64px;height:48px;"></td>
		///@brief 		<td><img src="_Photos\Circle_Pattern.jpg" style="width:64px;height:48px;"></td>
		///@brief 	</tr>
		///@brief </table>
		///@brief _______________________________________________________________________________
		//	+-------------------------------------------------------------------------------+
        void DrawCircle(int x,int y,int radius,Pen pen=Pen::Black,Pixel pixel=1,bool border=true);
		//	+-------------------------------------------------------------------------------+
		///@brief  Draw Ellipse on graphic area
		///@fn     DrawEllipse												
		///@param [in] xc,yc center oordinates
		///@param [in] rx radius of x axis
		///@param [in] ry radius of y axis
		///@param [in] pen Pen used if not provide Pen::Black is assumed
		///@param [in] pixel Pixel (0 or 1) if not provide (1)=black is assumed		
		///@param [in] border if true draw a box border if not provide is assumed true
		///@return none\n
		///@brief _______________________________________________________________________________
		///@brief Draw a Ellipse with pen border if border is true \n
		///@brief if DrawMode::SOLID Draw a Fill circle	\n
		///@brief if DrawMode::PATTERN Draw a Fill circle with that pattern	\n\n
		///@brief Below some examples:\n\n
		///@brief <table style="width:150px">
		///@brief 	<tr>
		///@brief 		<td><img src="_Photos\Ellipse_Line.jpg" style="width:64px;height:48px;"></td>
		///@brief 		<td><img src="_Photos\Ellipse_Solid.jpg" style="width:64px;height:48px;"></td>
		///@brief 		<td><img src="_Photos\Ellipse_Pattern.jpg" style="width:64px;height:48px;"></td>
		///@brief 	</tr>
		///@brief </table>
		///@brief _______________________________________________________________________________
		//	+-------------------------------------------------------------------------------+
		void DrawEllipse(int xc,int yc, int rx, int ry,Pen pen=Pen::Black,Pixel pixel=1,bool border=true);		
		//	+-------------------------------------------------------------------------------+
		///@brief Set New Draw Mode											
		///@fn    SetDrawMode												
		///@param [in] drawMode new draw mode										
		///@return none\n
		///@brief _______________________________________________________________________________
		///@brief DrawMode maybe \n
		///@brief \n													
		///@brief DrawMode::LINE	\n
		///@brief <table style="width:150px">
		///@brief 	<tr>
		///@brief 		<td><img src="_Photos\Box_Line.jpg" style="width:64px;height:48px;"></td>
		///@brief 		<td><img src="_Photos\Circle_Line.jpg" style="width:64px;height:48px;"></td>
		///@brief 		<td><img src="_Photos\Ellipse_Line.jpg" style="width:64px;height:48px;"></td>
		///@brief 	</tr>
		///@brief </table>		
		///@brief DrawMode::SOLID	\n
		///@brief <table style="width:150px">
		///@brief 	<tr>
		///@brief 		<td><img src="_Photos\Box_Solid.jpg" style="width:64px;height:48px;"></td>
		///@brief 		<td><img src="_Photos\Circle_Solid.jpg" style="width:64px;height:48px;"></td>
		///@brief 		<td><img src="_Photos\Ellipse_Solid.jpg" style="width:64px;height:48px;"></td>
		///@brief 	</tr>
		///@brief </table>		
		///@brief DrawMode::PATTERN \n
		///@brief <table style="width:150px">
		///@brief 	<tr>
		///@brief 		<td><img src="_Photos\Box_Pattern.jpg" style="width:64px;height:48px;"></td>
		///@brief 		<td><img src="_Photos\Circle_Pattern.jpg" style="width:64px;height:48px;"></td>
		///@brief 		<td><img src="_Photos\Ellipse_Pattern.jpg" style="width:64px;height:48px;"></td>
		///@brief 	</tr>
		///@brief </table>		
		//	+-------------------------------------------------------------------------------+
		void inline SetDrawMode(DrawMode drawMode) {_DrawMode=drawMode; }
		//	+-------------------------------------------------------------------------------+
		///@brief Get Actual Draw Mode										
		///@fn    GetDrawMode
		///@return  DrawMode \n
		///@brief _______________________________________________________________________________
		///@brief DrawMode maybe \n
		///@brief 	\n													
		///@brief 	DrawMode::LINE	\n
		///@brief 	DrawMode::SOLID	\n
		///@brief 	DrawMode::PATTERN \n
		//	+-------------------------------------------------------------------------------+
		DrawMode inline GetDrawMode(void) { return _DrawMode; }
		//	+-------------------------------------------------------------------------------+
		///@brief Get Version of Major Value									
		///@fn    VersionMajor
		///@return  return the Value of Major Version 
		///@brief 
		//	+-------------------------------------------------------------------------------+
		uint8_t inline VersionMajor() { return _GRAPHBASIC_VERSION_MAJOR; }
		//	+-------------------------------------------------------------------------------+
		///@brief Get Version of Minor Value									
		///@fn    VersionMinor
		///@return  return the Value of Minor Version 
		///@brief 
		//	+-------------------------------------------------------------------------------+
		uint8_t inline VersionMinor() { return _GRAPHBASIC_VERSION_MINOR; }

};


#endif //_GRAPH_BASIC_LIB_H_