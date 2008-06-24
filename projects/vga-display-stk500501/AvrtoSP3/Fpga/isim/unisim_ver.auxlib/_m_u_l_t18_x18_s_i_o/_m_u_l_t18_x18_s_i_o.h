////////////////////////////////////////////////////////////////////////////////
//   ____  ____  
//  /   /\/   /  
// /___/  \  /   
// \   \   \/    
//  \   \        Copyright (c) 2003-2004 Xilinx, Inc.
//  /   /        All Right Reserved. 
// /___/   /\   
// \   \  /  \  
//  \___\/\___\ 
////////////////////////////////////////////////////////////////////////////////

#ifndef H_unisim_ver_auxlibM_m_u_l_t18_x18_s_i_o_H
#define H_unisim_ver_auxlibM_m_u_l_t18_x18_s_i_o_H

#ifdef _MSC_VER
#pragma warning(disable: 4355)
#endif

#ifdef __MINGW32__
#include "xsimMinGW.h"
#else
#include "xsim.h"
#endif

class unisim_ver_auxlibM_m_u_l_t18_x18_s_i_o : public HSim__s5{
public: 
    unisim_ver_auxlibM_m_u_l_t18_x18_s_i_o(const char *instname);
    ~unisim_ver_auxlibM_m_u_l_t18_x18_s_i_o();
    void setDefparam();
    void constructObject();
    void moduleInstantiate(HSimConfigDecl *cfg);
    void connectSigs();
    void reset();
    virtual void archImplement();
    HSim::ValueS* up0Func(HSim::VlogVarType& outVarType, int& outNumScalars, int inNumScalars);
    HSim::ValueS* up1Func(HSim::VlogVarType& outVarType, int& outNumScalars, int inNumScalars);
    HSim::ValueS* up3Func(HSim::VlogVarType& outVarType, int& outNumScalars, int inNumScalars);
    HSim__s1 us[25];
    HSim__s3 uv[7];
    HSimVlogParam up[4];
};

#endif
