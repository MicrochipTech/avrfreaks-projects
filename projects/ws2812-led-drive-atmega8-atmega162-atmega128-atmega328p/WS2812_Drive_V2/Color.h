 /*
  ////////////////////////////////////////////////////////////////////////////
                 _____       _                   _
                /  __ \     | |                 | |
                | /  \/ ___ | | ___  _ __    ___| | __ _ ___ ___
                | |    / _ \| |/ _ \| '__|  / __| |/ _` / __/ __|
                | \__/\ (_) | | (_) | |    | (__| | (_| \__ \__ \
                \____/\___/|_|\___/|_|     \___|_|\__,_|___/___/

  ////////////////////////////////////////////////////////////////////////////
 */              


#include <avr/io.h>

#ifndef _COLOR_H_
#define _COLOR_H_
//
//         +-------------------------------------------------------+
//         |        CLASS 24 bits Color  COMMON IMPLEMENTATION     |
//         +-------------------------------------------------------+
//         | @PURPOSE      : Implements Color Class with 24 bits   |
//         | @OBSERVATIONS :									   |
//         +-------------------------------------------------------+
//

class Color
{
    public:
    uint8_t red;
    uint8_t green;
    uint8_t blue;
    /************************************************************************/
    /* 1o Constructor set r,g,b=0                                           */
    /************************************************************************/
    Color(void)
    {
        this->red=0;
        this->green=0;
        this->blue=0;
    }
    /************************************************************************/
    /* 2o constructor set r=red,g=green,b=blue                              */
    /************************************************************************/
    Color(uint8_t red,uint8_t green,uint8_t blue)
    {
        this->red=red;
        this->green=green;
        this->blue=blue;
    }
    /************************************************************************/
    /* Set local red,green,blue with red,green,blue arguments               */
    /************************************************************************/
    Color FromRGB(uint8_t red,uint8_t green,uint8_t blue)
    {
        Color tmp;
        tmp.red=red;
        tmp.green=green;
        tmp.blue=blue;
        this->red=red;
        this->green=green;
        this->blue=blue;
        return tmp;
    }
    /************************************************************************/
    /* Compare two colors and return true if r,g,b source iqual r,g,b destine*/
    /************************************************************************/
    bool operator== (Color color)
    {
        if (this->red==color.red && this->green==color.green && this->blue==color.blue)
        {
            return true;
        }
        else
        {
            return false;
        }
    }
};

#endif /* _COLOR_H_ */