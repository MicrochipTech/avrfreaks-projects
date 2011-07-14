/***C*********************************************************************************************
**
** SRC-FILE     :   
**                                        
** PROJECT      :   DLE
**                                                                
** SRC-VERSION  :   
**              
** DATE         :   
**
** AUTHOR       :   AV
**
** DESCRIPTION  :   
**                  
** FUNCTIONS    :   
**                  
** COMPILER     :   
**
** REFERENCES   :   
**
** COPYRIGHT    :   
**
****C*E******************************************************************************************/
#ifndef __DLE_CMD_LIB_H__
#define __DLE_CMD_LIB_H__
typedef enum e_cmd_field_type_tag {
    CFT_NUM,  
    CFT_TXT, 
    CFT_RESERVED = 4, 
    CFT_LAST = -1
}E_CMD_FIELD_TYPE;


typedef struct dle_cmd_field_tag{

    WCHAR   *pc_name;
    E_CMD_FIELD_TYPE e_type;

    int     n_len;
    union {
        DWORD   dw_val;
        WCHAR   *pc_str;
    };
} T_DLE_CMD_FIELD;

typedef struct t_dle_cmd_tag {
    WCHAR   *pc_name;
    T_DLE_CMD_FIELD  *pt_fields;
}T_DLE_CMD;

extern T_DLE_CMD gta_cmd_lib[];


//----------------------------------------------------------
struct t_cmd_init_tag{
    T_DLE_CMD_FIELD   eomsg;
};                          

extern struct t_cmd_init_tag gt_cmd_init;

//----------------------------------------------------------
struct t_cmd_dev_sign_tag{
    T_DLE_CMD_FIELD   eomsg;
};

extern struct t_cmd_dev_sign_tag gt_cmd_dev_sign;

//----------------------------------------------------------
struct t_cmd_spi_rd_tag{
    T_DLE_CMD_FIELD   addr;
    T_DLE_CMD_FIELD   len;
    T_DLE_CMD_FIELD   eomsg;
};

extern struct t_cmd_spi_rd_tag gt_cmd_spi_rd;

//----------------------------------------------------------
struct t_cmd_spi_wr_tag{
    T_DLE_CMD_FIELD   data;
    T_DLE_CMD_FIELD   eomsg;
};

extern struct t_cmd_spi_wr_tag gt_cmd_spi_wr;

//----------------------------------------------------------
struct t_cmd_apd_on_tag{
    T_DLE_CMD_FIELD   bias_value;
    T_DLE_CMD_FIELD   bias_ctrl_level;
    T_DLE_CMD_FIELD   eomsg;
};

extern struct t_cmd_apd_on_tag gt_cmd_apd_on;

//----------------------------------------------------------
struct t_cmd_apd_off_tag{
    T_DLE_CMD_FIELD   eomsg;
};

extern struct t_cmd_apd_off_tag gt_cmd_apd_off;

//----------------------------------------------------------
struct t_cmd_aim_tag{
    T_DLE_CMD_FIELD   act;
    T_DLE_CMD_FIELD   eomsg;
};

extern struct t_cmd_aim_tag gt_cmd_aim;

//----------------------------------------------------------
struct t_cmd_curtain_tag{
    T_DLE_CMD_FIELD   eomsg;
};

extern struct t_cmd_curtain_tag gt_cmd_curtain;

//----------------------------------------------------------
struct t_cmd_measure_tag{
    T_DLE_CMD_FIELD   stream;
    T_DLE_CMD_FIELD   sincos;
    T_DLE_CMD_FIELD   eomsg;
};

extern struct t_cmd_measure_tag gt_cmd_measure;

//----------------------------------------------------------
struct t_cmd_lcd_tag{
    T_DLE_CMD_FIELD   dig0;
    T_DLE_CMD_FIELD   dig1;
    T_DLE_CMD_FIELD   signs;
    T_DLE_CMD_FIELD   eomsg;
};

extern struct t_cmd_lcd_tag gt_cmd_lcd;


#if 0
//----------------------------------------------------------
struct t_cmd_???_tag{
    T_DLE_CMD_FIELD   ???;
    T_DLE_CMD_FIELD   eomsg;
};

extern struct t_cmd_???_tag gt_cmd_???;
#endif // #if 0



// --- Functions -----------

T_DLE_CMD* lookup_dle_cmd(WCHAR *pc_cmd_arg, T_DLE_CMD *pt_cmd_lib);
int check_dle_cmd(WCHAR *pc_cmd_arg, T_DLE_CMD_FIELD *pt_fields);
T_DLE_CMD* decomposite_dle_cmd(WCHAR *pc_cmd_arg, T_DLE_CMD *pt_cmd, int n_update);

#endif // __DLE_CMD_LIB_H__