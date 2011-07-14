/***C*********************************************************************************************
**
** SRC-FILE     :   dle_scan_actions.h
**                                        
** PROJECT      :   DLE SCAN
**                                                                
** SRC-VERSION  :   0
**              
** DATE         :   01/20/2011
**
** AUTHOR       :   AV
**
** DESCRIPTION  :   
**                  
** COPYRIGHT    :   Andrejs Vasiljevs (c) 2011
**
****C*E******************************************************************************************/
#ifndef __DLE_SCAN_ACTIONS_H__
#define __DLE_SCAN_ACTIONS_H__

#define SIGN_LEN 32

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


extern HW_INFO gt_hw_info;
extern T_ACTION gta_action_table[];

extern char  gca_version[2];
extern char  gca_signature[SIGN_LEN];

#define LCD_DIGITS0_INP_IDX 3
#define LCD_DIGITS1_INP_IDX (LCD_DIGITS0_INP_IDX + LCD_SET_DIGITS_NUM)
#define LCD_SIGNS_INP_IDX (LCD_DIGITS1_INP_IDX + LCD_SET_DIGITS_NUM)

#define CLK_FRQ 8              // MHz

#define ADC_MSG_MUX_BIT 10


extern void disable_modulator();


// -------------------------------------------------------------------------
#endif /* __DLE_SCAN_ACTIONS_H__ */
