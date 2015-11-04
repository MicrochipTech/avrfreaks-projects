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

#ifndef _GRAPH_BASIC_LIB_H_
#define _GRAPH_BASIC_LIB_H_

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
    #include "_Color.h"
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

//Enable define line bellow according size of char required (NORNAL) or (BIG) 
//only one line bellow is enabled at a time

//#define _GRAPH_NORMAL_CHAR_SIZE_

#define _GRAPH_BIG_CHAR_SIZE_

// 
// 	+--------------------------------------+
// 	|       ENUMERATIONS                   |
// 	+--------------------------------------+
// 

// 
//         +-------------------------------------------------------+
//         |        CLASS GRAPH BASIC  COMMON IMPLEMENTATION       |
//         +-------------------------------------------------------+
//         | @PURPOSE      : Implements basic graph drawing shapes |
//		   |			     and Texts							   |
//         | @OBSERVATIONS :									   |
//         +-------------------------------------------------------+
// 



class class_GraphBasic
{
//         
//          +--------------------------------------+
//          |       PRIVATE VARIABLES			   |
//          +--------------------------------------+
//         
private:
        Color   *ColorBuffer=nullptr;
        Color   BackColor=Color(0,0,0);
        Color   ForeColor=Color(255,255,255);
        int     Rows=1;
        int     Cols=1;
        int     X=0;
        int     Y=0;
//         
//          +--------------------------------------+
//          |       PRIVATE FUNCTIONS              |
//          +--------------------------------------+
//         
		/************************************************************************/
		/* Initialize WS2812 Drive                                              */
		/************************************************************************/
        void        Initialize(void);
		/************************************************************************/
		/* Un Initialize WS2812 Drive                                           */
		/************************************************************************/
        void        UnInitialize(void);
        /************************************************************************/
        /* Reset ww2812 LED Stream                                              */
        /************************************************************************/
        virtual void Reset(void);
        /************************************************************************/
        /* Write a r,g,b values to one LED Stream                               */
        /************************************************************************/
        virtual void WriteRGBtoDevice(uint8_t rValue,uint8_t gValue,uint8_t bValue); 
        /************************************************************************/
        /* plot 8 symmetric points                                              */
        /************************************************************************/        
        void PlotP8(int xx,int yy,int x,int y,uint8_t red,uint8_t green,uint8_t blue);
//          +--------------------------------------+
//          |        PUBLIC FUNCTIONS		       |
//          +--------------------------------------+
//         
public:
        /************************************************************************/
        /* Initialize graph basic with 1 row and 1 column                       */
        /************************************************************************/
       	class_GraphBasic();
        /************************************************************************/
        /* UnInitialize graph basic                                             */
        /************************************************************************/
        ~class_GraphBasic();
        /************************************************************************/
        /* Initialize graph basic with rows and cols size                       */
        /************************************************************************/
        class_GraphBasic(int rows,int cols);
        /************************************************************************/
        /* Get graph matrix Rows                                                */
        /************************************************************************/
        uint8_t GetRows(void) { return this->Rows;}
        /************************************************************************/
        /* get graph matrix Cols                                                */
        /************************************************************************/
        uint8_t GetCols(void) { return this->Cols;}
		/************************************************************************/
		/* Return minimal value of x axis			                            */
		/************************************************************************/
        int     inline MinX(void) { return 0;}
		/************************************************************************/
		/* Return maximum value of x axis                                       */
		/************************************************************************/
        int     inline MaxX(void) { return this->Cols-1;}
		/************************************************************************/
		/* Return minimal value of y axis			                            */
		/************************************************************************/
        int     inline MinY(void) {return 0;}
		/************************************************************************/
		/* Return maximum value of y axis                                       */
		/************************************************************************/
        int     inline MaxY(void) { return this->Rows-1;}
        /************************************************************************/
        /* Set Back color of graph matrix                                       */
        /************************************************************************/
        void    SetBackColor(Color color);
        /************************************************************************/
        /* Set BackColor of graph matrix with r,g,b values                      */
        /************************************************************************/
        void    SetBackColor(uint8_t red,uint8_t green,uint8_t blue);
        /************************************************************************/
        /* Get Back color                                                       */
        /************************************************************************/
        Color   GetBackColor(void);
        /************************************************************************/
        /* Set Fore color of graph matrix used to draw shapes                   */
        /************************************************************************/
        void    SetForeColor(Color color);
        /************************************************************************/
        /* Set Fore color of graph matrix with r,g,b values                     */
        /************************************************************************/
        void    SetForeColor(uint8_t red,uint8_t green,uint8_t blue);
        /************************************************************************/
        /* Get fore color                                                       */
        /************************************************************************/
        Color   GetForeColor(void);
        /************************************************************************/
        /* Set pixel at coordinates x,y using forecolor on graph matrix         */
        /************************************************************************/
        void    SetPixel(int x,int y);
        /************************************************************************/
        /* Set pixel at coordinates x,y using pixelcolor on graph matrix        */
        /************************************************************************/
        void    SetPixel(int x,int y,Color pixelColor);
        /************************************************************************/
        /* Set pixel at coordinates x,y using r,g,b values on graph matrix      */
        /************************************************************************/
        void    SetPixel(int x,int y,uint8_t red,uint8_t green,uint8_t blue);
        /************************************************************************/
        /* Get pixel at coordinates x,y from graph matrix if x,y out of range   */
        /* backcolor is returned                                                */
        /************************************************************************/
        Color   GetPixel(int x,int y);
        /************************************************************************/
        /* Draw a Vertical Line using r,g,b values on graph matrix              */
        /************************************************************************/
        void    VertLine(int x,int yi,int yf,uint8_t red,uint8_t green,uint8_t blue);
        /************************************************************************/
        /* Draw a Vertical Line using forecolor on graph matrix                 */
        /************************************************************************/
        void    VertLine(int x,int yi,int yf);
        /************************************************************************/
        /* Draw a Vertical Line using pixelcolor on graph matrix                */
        /************************************************************************/
        void    VertLine(int x,int yi,int yf,Color pixelColor);
        /************************************************************************/
        /* Draw a Horizontal Line using r,g,b values on graph matrix            */
        /************************************************************************/
        void    HorzLine(int xi,int xf,int y,uint8_t red,uint8_t green,uint8_t blue);
        /************************************************************************/
        /* Draw a Horizontal Line using forecolor on graph matrix               */
        /************************************************************************/
        void    HorzLine(int xi,int xf,int y);
        /************************************************************************/
        /* Draw a Horizontal Line using pixelcolor on graph matrix              */
        /************************************************************************/
        void    HorzLine(int xi,int xf,int y,Color pixelColor);
        /************************************************************************/
        /* Draw a Box using r,g,b values on graph matrix                        */
        /************************************************************************/
        void    Box(int xi,int yi,int xf,int yf,uint8_t red,uint8_t green,uint8_t blue);
        /************************************************************************/
        /* Draw a Box using forecolor on graph matrix                           */
        /************************************************************************/
        void    Box(int xi,int yi,int xf,int yf);
        /************************************************************************/
        /* Draw a Box using pixelcolor on graph matrix                          */
        /************************************************************************/
        void    Box(int xi,int yi,int xf,int yf,Color pixelColor);
		/************************************************************************/
		/* Draw a Box Full using r,g,b values on graph matrix                   */
		/************************************************************************/
		void    BoxFull(int xi,int yi,int xf,int yf,uint8_t red,uint8_t green,uint8_t blue);
		/************************************************************************/
		/* Draw a Box Full using forecolor on graph matrix                      */
		/************************************************************************/
		void    BoxFull(int xi,int yi,int xf,int yf);
		/************************************************************************/
		/* Draw a BoxFull using pixelcolor on graph matrix                      */
		/************************************************************************/
		void    BoxFull(int xi,int yi,int xf,int yf,Color pixelColor);		
        /************************************************************************/
        /* Draw a Line using r,g,b values on graph matrix                       */
        /************************************************************************/
        void    Line(int xi,int yi,int xf,int yf,uint8_t red,uint8_t green,uint8_t blue);
        /************************************************************************/
        /* Draw a Line using forecolor on graph matrix                          */
        /************************************************************************/
        void    Line(int xi,int yi,int xf,int yf);
        /************************************************************************/
        /* Draw a Line using pixelcolor on graph matrix                         */
        /************************************************************************/
        void    Line(int xi,int yi,int xf,int yf,Color pixelColor);
        /************************************************************************/
        /* Clear graph matrix using backcolor                                   */
        /************************************************************************/
        void    Cls(void);
        /************************************************************************/
        /* Draw a Circle using r,g,b values on graph matrix                     */
        /************************************************************************/
        void    Circle(int x,int y,int radius,uint8_t red,uint8_t green,uint8_t blue);
        /************************************************************************/
        /* Draw a Circle using forecolor on graph matrix                        */
        /************************************************************************/
        void    Circle(int x,int y,int radius);
        /************************************************************************/
        /* Draw a Circle using pixelcolor on graph matrix                       */
        /************************************************************************/
        void    Circle(int x,int y,int radius,Color pixelColor);
        /************************************************************************/
        /* Rotate Left or Right depend if xi<xf rotate left else right on       */
        /* graph matrix Horizontal Line                                         */
        /************************************************************************/
        void    RotateX(int xi,int xf,int yi,int yf,bool shiftOnly=false);        
		/************************************************************************/
        /* Rotate Up or Down depend if yi<yf rotate up else down on   	        */
        /* graph matrix Horizontal Line                                         */
        /************************************************************************/
        void    RotateY(int xi,int xf,int yi,int yf,bool shiftOnly=false);
        /************************************************************************/
        /* Show R,G,B matrix on screen                                          */
        /************************************************************************/
        void Show(void);
        /************************************************************************/
        /* Locate position to write a character                                 */
        /************************************************************************/
        void Locate(int x,int y);
        /************************************************************************/
        /* Put Character on graph matrix at coords last x,y                     */
        /************************************************************************/
        void PutC(char c,bool useBackColor=false);
        /************************************************************************/
        /* Put Character String on graph matrix at last x,y                     */
        /************************************************************************/
        void PutS(const char *c,bool useBackColor=false);
        /************************************************************************/
        /* Put Character String from Flash on graph matrix at last x,y          */
        /************************************************************************/
		void PutSF(const char *c,bool useBackColor=false);
		/************************************************************************/
		/* Get Text Width in pixels                                             */
		/************************************************************************/
		uint8_t inline GetTextWidth(void)
		{
			#if defined(_GRAPH_NORMAL_CHAR_SIZE_)
				return 4;
			#elif defined(_GRAPH_BIG_CHAR_SIZE_)
				return 6;
			#else
				#error _GRAPH_NORMAL_CHAR_SIZE_ nor _GRAPH_BIG_CHAR_SIZE not defined
			#endif
		}
		/************************************************************************/
		/* Get Text Height in pixels                                            */
		/************************************************************************/
		uint8_t inline GetTextHeight(void)
		{
			#if defined(_GRAPH_NORMAL_CHAR_SIZE_)
				return 5;
			#elif defined(_GRAPH_BIG_CHAR_SIZE_)
				return 8;
			#else
				#error _GRAPH_NORMAL_CHAR_SIZE_ nor _GRAPH_BIG_CHAR_SIZE not defined
			#endif
		}
		
};

#endif //_GRAPH_BASIC_LIB_H_