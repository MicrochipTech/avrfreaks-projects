/***C*********************************************************************************************
**
** SRC-FILE     :   dle_actions.h
**                                        
** PROJECT      :   DLE Control
**                                                                
** SRC-VERSION  :   0
**              
** DATE         :   01/20/2011
**
** AUTHOR       :   AV
**
** DESCRIPTION  :   
**                  
** COPYRIGHT    :   Andrejs Vasiljevs (c) 2010
**
****C*E******************************************************************************************/
#ifndef __ACTIONS_H__
#define __ACTIONS_H__
/*

*/
#include <stdint.h>
typedef int8_t      int8;   
typedef uint8_t     uint8;  
typedef int16_t     int16;
typedef uint16_t    uint16;
typedef int32_t     int32;
typedef uint32_t    uint32;
typedef int64_t     int64;
typedef uint64_t    uint64; 

typedef void (*PF_PVOID)();

#ifndef NULL
#define NULL ((void*)0)
#endif

#define SIGN_LEN 16

typedef struct {
        uint8  uc_ver_maj;
        uint8  uc_ver_min;
        char   ca_signature[SIGN_LEN];
        uint8  reserved[128-44];
}HW_INFO;


typedef union{
        uint8   b[2];
        uint16  s;
}UINT16;

typedef union{
        uint8   b[4];
        uint16  s[2];
        uint32  l;
}UINT32;


typedef struct action_tag{
    char     ca_name[4];
    uint8    uc_cmd;
    void     (*pf_func)(void);
} T_ACTION;

typedef struct lcd_digit_decode_tag{
    uint8    uc_digit;
    uint8    uc_bitmask;
} T_LCD_DIGIT_DECODE;



typedef struct t_seg_map_tag {
    uint8 uc_addr;
    uint8 uc_mask;
} T_SEG_MAP;

//typedef struct t_seg_map1_tag {
//    _SFR_MEM8(uc_offset);
//    uint8 uc_mask;
//} T_SEG_MAP1;



typedef struct lcd_digit_segment_map_tag{
    uint8     uc_digit_pos;
    T_SEG_MAP ta_seg_map[7];
}T_DIGIT_MAP;

typedef struct {
    uint16 us_freq;

    int16 s_sin;
    int16 s_cos;

    uint16 us_pwr;

    // atan

} T_FREQ_DATA;

extern T_FREQ_DATA gta_freq_table[];
extern volatile T_FREQ_DATA *gpt_freq;


extern HW_INFO gt_hw_info;
extern T_ACTION gta_action_table[];

#define LCD_SET_DIGITS_NUM 5
#define LCD_SET_DIGITS_NUM 5
#define LCD_SIGNS_NUM 32

#define LCD_DIGITS0_INP_IDX 3
#define LCD_DIGITS1_INP_IDX (LCD_DIGITS0_INP_IDX + LCD_SET_DIGITS_NUM)
#define LCD_SIGNS_INP_IDX (LCD_DIGITS1_INP_IDX + LCD_SET_DIGITS_NUM)

extern uint8 gta_digit_decode[];
extern T_DIGIT_MAP gta_digit_set0_map[LCD_SET_DIGITS_NUM];
extern T_DIGIT_MAP gta_digit_set1_map[LCD_SET_DIGITS_NUM];
extern T_SEG_MAP gta_signs_map[32];

extern char  gca_version[2];
extern char  gca_signature[SIGN_LEN];

#define CLK_FRQ 8              // MHz

#define ADC_MSG_MUX_BIT 10

// ------------------------------- specific --------------------------------

#define MEAS_SAMPLES_P  6       // 2^P Number of samples for measurement
#define DELAY_SAMPLES_NUM  16   // Delay before measure start

#define MEAS_TYPE_STREAM    1
#define MEAS_TYPE_SINCOS    2
#define MEAS_TYPE_DISTANCE  4   // Not ready yet

// -------------------------------------------------------------------------
#endif /* __ACTIONS_H__ */
