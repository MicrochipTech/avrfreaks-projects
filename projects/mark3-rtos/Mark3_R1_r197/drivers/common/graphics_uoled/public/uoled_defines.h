#ifndef __UOLED_DEFINES_H__
#define __UOLED_DEFINES_H__

//---------------------------------------------------------------------------
/*!
    Text commands defines
*/
//---------------------------------------------------------------------------
#define TEXT_MOVE_CURSOR        (0xFFE4)
#define TEXT_PUT_CHAR           (0xFFFE)
#define TEXT_PUT_STRING         (0x0006)
#define TEXT_CHAR_WIDTH         (0x0002)
#define TEXT_CHAR_HEIGHT        (0x0001)
#define TEXT_FG_COLOR           (0xFF7F)
#define TEXT_BG_COLOR           (0xFF7E)
#define TEXT_FONT_SET           (0xFFE5)
#define TEXT_WIDTH              (0xFF7C)
#define TEXT_HEIGHT             (0xFF7B)
#define TEXT_X_GAP              (0xFF7A)
#define TEXT_Y_GAP              (0xFF79)
#define TEXT_BOLD               (0xFF76)
#define TEXT_INVERSE            (0xFF74)
#define TEXT_ITALIC             (0xFF75)
#define TEXT_OPACITY            (0xFF77)
#define TEXT_UNDERLINE          (0xFF73)
#define TEXT_ATTRIBUTES         (0xFF72)
#define TEXT_SET_PARAMS         (0xFFE3)


//---------------------------------------------------------------------------
/*!
    Graphics command defines
*/
//---------------------------------------------------------------------------
#define GFX_CLEAR_SCREEN        (0xFFD7)
#define GFX_CHANGE_COLOR        (0xFFBE)
#define GFX_DRAW_CIRCLE         (0xFFCD)
#define GFX_DRAW_CIRCLE_FILLED  (0xFFCC)
#define GFX_DRAW_LINE           (0xFFD2)
#define GFX_DRAW_RECTANGLE      (0xFFCF)
#define GFX_DRAW_RECTANGLE_FILLED   (0xFFCE)
#define GFX_DRAW_POLYLINE       (0x0005)
#define GFX_DRAW_POLYGON        (0x0004)
#define GFX_DRAW_TRIANGLE       (0xFFC9)
#define GFX_CALCULATE_ORBIT     (0x0003)
#define GFX_PUT_PIXEL           (0xFFCB)
#define GFX_READ_PIXEL          (0xFFCA)
#define GFX_MOVE_ORIGIN         (0xFFD6)
#define GFX_LINE_MOVE_ORIGIN    (0xFFD4)
#define GFX_CLIPPING_SET        (0xFF6C)
#define GFX_SET_CLIP_WINDOW     (0xFFBF)
#define GFX_EXTEND_CLIP_WINDOW  (0xFFBC)
#define GFX_SET_BG_COLOR        (0xFF6E)
#define GFX_SET_OUTLINE_COLOR   (0xFF67)
#define GFX_SET_CONTRAST        (0xFF66)
#define GFX_SET_FRAME_DELAY     (0xFF69)
#define GFX_LINE_PATTERN        (0xFF65)
#define GFX_SCREEN_MODE         (0xFF68)
#define GFX_SET_PARAMS          (0xFFD8)
#define GFX_BLIT                (0x000A)

//---------------------------------------------------------------------------
/*!
    Serial port commands
*/
//---------------------------------------------------------------------------
#define SERIAL_SET_BAUD         (0x000B)

#define SERIAL_BAUD_110         (0)
#define SERIAL_BAUD_300         (1)
#define SERIAL_BAUD_600         (2)
#define SERIAL_BAUD_1200        (3)
#define SERIAL_BAUD_2400        (4)
#define SERIAL_BAUD_4800        (5)
#define SERIAL_BAUD_9600        (6)
#define SERIAL_BAUD_14400       (7)
#define SERIAL_BAUD_19200       (8)
#define SERIAL_BAUD_31250       (9)
#define SERIAL_BAUD_38400       (10)
#define SERIAL_BAUD_56000       (11)
#define SERIAL_BAUD_57600       (12)
#define SERIAL_BAUD_115200      (13)
#define SERIAL_BAUD_128000      (14)
#define SERIAL_BAUD_256000      (15)
#define SERIAL_BAUD_300000      (16)
#define SERIAL_BAUD_375000      (17)
#define SERIAL_BAUD_500000      (18)
#define SERIAL_BAUD_600000      (19)


#define ACK_BYTE                (0x06)
//---------------------------------------------------------------------------
/*!
    General defines
*/
#define SERIAL_TIMEOUT_MAX      (1000)   //millisecond
#define COMMAND_RETRY_COUNT     (5)     //attempts

#endif

