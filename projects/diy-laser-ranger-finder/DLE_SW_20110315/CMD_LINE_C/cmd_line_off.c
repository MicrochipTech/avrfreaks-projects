#include <stdio.h>
#include <string.h>
#include "cmd_line.h"

#define TRUE 1
#define FALSE 0

void main(){

    int32   l_rc;
    
    void    *pv_cl_info;
    void    *pv_cl_hist_buff;
    uint32  ul_cl_hist_size, ul_gl_flag;
    char    ca_cmd_line[512];  

    char    ca_cmd_hist_file_name[] = "cmd_hist.dat";
    FILE    *pf_hist;
    
    pf_hist = fopen(ca_cmd_hist_file_name, "rb+");
    if (!pf_hist){
        pf_hist = fopen(ca_cmd_hist_file_name, "wb+");
        if (!pf_hist){
            printf("Error opening history file\n");
            return;
        }
    }

    // Init get line
    pv_cl_info = init_get_line("\n>>", &pv_cl_hist_buff, &ul_cl_hist_size);

    // Fill history buffer
    fread(pv_cl_hist_buff, 1, ul_cl_hist_size, pf_hist);

    ca_cmd_line[0] = '\0';
    ul_gl_flag = 0;
    while(1){
        l_rc = get_line(pv_cl_info, ca_cmd_line, sizeof(ca_cmd_line), ul_gl_flag);
        ul_gl_flag = 0;
        _strupr(ca_cmd_line);
        switch (l_rc){
            case GL_RC_RETURN:
                if (strcmp(ca_cmd_line, "QUIT") == 0){
                    l_rc = FALSE;
                }else{
                    // --- Proceed command here ---
                    printf("\n%s",ca_cmd_line);
                    // ----------------------------
                    ca_cmd_line[0] = 0;
                    ul_gl_flag = 0;
                    l_rc = TRUE;
                }
                break;
            case GL_RC_TAB:
                strncat(ca_cmd_line, "_AUTO_COMPLETE_", sizeof(ca_cmd_line));
                ul_gl_flag = GL_FLAG_CONTINUE;
                l_rc = TRUE;
                break;
            case GL_RC_TAB_TAB:
                puts("\nFollowing commands expected");
                puts("XXX XXX XXX");
                puts("XXX XXX XXX");
                ul_gl_flag = 0;
                l_rc = TRUE;
                break;
            default:
                printf("\nSomething wrong\n");
                l_rc = FALSE;
                break;
        } // End of switch

        if (!l_rc) break;

    } // End of While forever

    // save history buffer
    rewind(pf_hist);
    fwrite(pv_cl_hist_buff, 1, ul_cl_hist_size, pf_hist);
    fclose(pf_hist);

    free_get_line(pv_cl_info);

}