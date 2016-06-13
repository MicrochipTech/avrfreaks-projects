#ifndef _emm_IRremote_h_
#define _emm_IRremote_h_

#include <avr/pgmspace.h>

//------------------------------//
// Codes for LG Remote          //
//------------------------------//
#define LG_Again        0xDA5A0000
#define LG_TV_VolUp     0xF070700F
#define LG_TV_VolDown   0xF0706817
#define LG_TV_On        0xF070205F
#define LG_TV_ProgNext  0xF070245B
#define LG_TV_ProgPrev  0xF070047B
#define LG_TV_AV        0xF070403F
#define LG_Open         0xDA5A3649
#define LG_On           0xDA5A0679
#define LG_1            0xDA5A6E11
#define LG_2            0xDA5A1E61
#define LG_3            0xDA5A5E21
#define LG_4            0xDA5A3E41
#define LG_5            0xDA5A7E01
#define LG_6            0xDA5A017E
#define LG_7            0xDA5A413E
#define LG_8            0xDA5A215E
#define LG_9            0xDA5A611E
#define LG_0            0xDA5A116E
#define LG_Clear        0xDA5A314E
#define LG_ScanPrev     0xDA5A2659
#define LG_ScanNext     0xDA5A6619
#define LG_SkipPrev     0xDA5A5629
#define LG_SkipNext     0xDA5A1669
#define LG_Pause        0xDA5A0E71
#define LG_Play         0xDA5A4639
#define LG_Stop         0xDA5A4E31
#define LG_Menu         0xDA5A6916
#define LG_Title        0xDA5A2956
#define LG_Display      0xDA5A2E51
#define LG_Return       0xDA5A512E
#define LG_Up           0xDA5A710E
#define LG_Down         0xDA5A0976
#define LG_Left         0xDA5A4D32
#define LG_Right        0xDA5A2D52
#define LG_Enter        0xDA5A0D72
#define LG_Audio        0xDA5A7906
#define LG_Subtitle     0xDA5A057A
#define LG_Angle        0xDA5A1966
#define LG_Setup        0xDA5A354A
#define LG_Marker       0xDA5A433C
#define LG_Search       0xDA5A235C
#define LG_Program      0xDA5A750A
#define LG_Zoom         0xDA5A136C
#define LG_Repeat       0xDA5A651A
#define LG_AB           0xDA5A552A
#define LG_Random       0xDA5A453A
#define LG_Star         0xDA5A3D42

void      IRinit(void);
uint32_t  IRcode(void);

extern uint32_t IRdigits[] PROGMEM;

#endif