#include <windows.h>
#include <wchar.h>
#include <stdio.h>
#include "cmd_line_wc.h"

#define TRUE 1
#define FALSE 0

void main(){

    int   n_rc;
    
    void    *pv_cl_info;
    void    *pv_cl_hist_buff;
    int     n_cl_hist_size, n_flag;
    WCHAR   ca_cmd_line[512];  

    WCHAR   ca_cmd_hist_file_name[] = L"cmd_hist.dat";
    FILE    *pf_hist;
    
    pf_hist = _wfopen(ca_cmd_hist_file_name, L"rb+"); 
    if (!pf_hist){
        pf_hist = _wfopen(ca_cmd_hist_file_name, L"wb+"); 
        if (!pf_hist){
            wprintf(L"Error opening history file\n"); 
            return;
        }
    }

    // Init get line
    pv_cl_info = init_get_line(L"\n>>", &pv_cl_hist_buff, &n_cl_hist_size); 

    // Fill history buffer
    fread(pv_cl_hist_buff, 1, n_cl_hist_size, pf_hist); 

    ca_cmd_line[0] = '\0';
    n_flag = 0;
    while(1){
        n_rc = get_line(pv_cl_info, ca_cmd_line, wcs_sizeof(ca_cmd_line), n_flag); 
        n_flag = 0;
        _wcsupr(ca_cmd_line); 
        switch (n_rc){
            case GL_RC_RETURN:
                if (wcscmp(ca_cmd_line, L"QUIT") == 0){
                    n_rc = FALSE;
                }else{
                    // --- Proceed command here ---
                    wprintf(L"\n%s",ca_cmd_line); 
                    // ----------------------------
                    ca_cmd_line[0] = 0;
                    n_flag = 0;
                    n_rc = TRUE;
                }
                break;
            case GL_RC_TAB:
                wcsncat(ca_cmd_line, L"_AUTO_COMPLETE_", wcs_sizeof(ca_cmd_line)); 
                n_flag = GL_FLAG_CONTINUE;
                n_rc = TRUE;
                break;
            case GL_RC_TAB_TAB:
                _putws(L"\nFollowing commands expected"); 
                _putws(L"XXX XXX XXX"); 
                _putws(L"XXX XXX XXX"); 
                n_flag = 0;
                n_rc = TRUE;
                break;
            default:
                wprintf(L"\nSomething wrong\n"); 
                n_rc = FALSE;
                break;
        } // End of switch

        if (!n_rc) break;

    } // End of While forever

    // save history buffer
    rewind(pf_hist); 
    fwrite(pv_cl_hist_buff, 1, n_cl_hist_size, pf_hist); 
    fclose(pf_hist); 

    free_get_line(pv_cl_info); 

}