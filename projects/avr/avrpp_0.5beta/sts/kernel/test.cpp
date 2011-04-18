/*   
   Copyright 2011 Elijah M. Merkin aka Ellioh

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
*/

#ifndef AVRPP_KTEST
#define AVRPP_AVR_GCC
#endif

#ifdef AVRPP_AVR_GCC
#include <avr/io.h>
#endif

#include <avrpp/typelist.hpp>
#include <avrpp/datatypes.hpp>
#include <avrpp/assert.hpp>
#include <avrpp/ports.hpp>
#include <avrpp/bus.hpp>
#include <avrpp/rtassert.hpp>



namespace avrpp_tests {

typedef TLIST4(char,int,float,double) cifd;

static int foreach_counter = 0;

/**
* @brief Used to test avrpp::typelist::foreach
*/
template <typename cls> struct test_foreach_processor {
	static void call () {
		foreach_counter += 1 + ::avrpp::typelist::get_index<cifd,cls>::result;
	}
	static void call (unsigned char v) {
		foreach_counter += v;
	}
	static void call (unsigned char v1, unsigned char v2) {
		foreach_counter += v1 - v2;
	}
};

template <typename cls> struct test_filter {
	enum {result=cls::pass};
};

struct pass1 {enum{pass=1};};
struct pass2 {enum{pass=1};};
struct no_pass1 {enum{pass=0};};
struct no_pass2 {enum{pass=0};};

template <typename cls> struct test_transform;
template <> struct test_transform<int>{typedef char result;};
template <> struct test_transform<char>{typedef int result;};

template <typename t> struct wrap_4_acc_bits {
	typedef t bus_type;
};

template <typename cls> struct test_accumulate_bits_processor {
	static unsigned char call () {
		return 0;
	}
};
template <> struct test_accumulate_bits_processor< wrap_4_acc_bits<char> > {
	static unsigned char call () {
		return 1;
	}
};

template <> struct test_accumulate_bits_processor< wrap_4_acc_bits<int> > {
	static unsigned char call () {
		return 2;
	}
};

template <typename cls1, typename cls2> struct test_compare_classes {
	enum {
		result = int(sizeof(cls1))-int(sizeof(cls2))
	};
};

void test_typelists () {
	typedef TLIST1(char) c;
	typedef TLIST1(int) i;
	typedef TLIST2(int,char) ic;
	typedef TLIST2(char,int) ci;
	typedef TLIST2(int,int) ii;
	typedef TLIST2(char,char) cc;
	typedef TLIST3(int,int,int) iii;
	typedef TLIST3(char,char,char) ccc;
	typedef TLIST3(char,int,int) cii;
	typedef TLIST3(int,char,int) ici;
	typedef TLIST3(int,int,char) iic;
	typedef TLIST3(char,char,int) cci;
	typedef TLIST3(char,int,char) cic;
	typedef TLIST3(int,char,char) icc;
	typedef TLIST4(int,char,char,int) icci;
	typedef TLIST4(char,char,char,int) ccci;
	typedef TLIST1(pass1) p1;
	typedef TLIST1(pass2) p2;
	typedef TLIST1(no_pass1) np1;
	typedef TLIST1(no_pass2) np2;
	typedef TLIST2(pass1,pass2) p1p2;
	typedef TLIST2(pass1,no_pass1) p1np1;
	typedef TLIST2(no_pass1,no_pass2) np1np2;
	typedef TLIST2(no_pass1,pass1) np1p1;

	using namespace ::avrpp::typelist;

//compare_classes
	{
		static const int i_not_equals_i = compare_classes<int,int>::result;
		COMPILE_ASSERT(!i_not_equals_i, int_equals_int)
		static const int i_not_equals_c = compare_classes<int,char>::result;
		COMPILE_ASSERT(i_not_equals_c, int_not_equals_char)
	}

//append
	{
		typedef append<i,char>::result i_append_char;
		SAME_TYPE_ASSERT (i_append_char,ic,i_append_c_is_ic)
		typedef append<c,int>::result c_append_int;
		SAME_TYPE_ASSERT (c_append_int,ci,c_append_int_is_ci)
		typedef append<null_list,int>::result null_append_int;
		SAME_TYPE_ASSERT (null_append_int,i,null_append_int_is_i)
	}

//join
	{
		typedef join<i,c>::result i_join_c;
		SAME_TYPE_ASSERT (i_join_c,ic,i_join_c_is_ic)
		typedef join<ii,c>::result ii_join_c;
		SAME_TYPE_ASSERT (ii_join_c,iic,ii_join_c_is_iic)
		typedef join<i,cc>::result i_join_cc;
		SAME_TYPE_ASSERT (i_join_cc,icc,i_join_cc_is_icc)
		typedef join<ic,ci>::result ic_join_ci;
		SAME_TYPE_ASSERT (ic_join_ci,icci,ic_join_ci_is_icci)
		typedef join<null_list,ci>::result null_join_ci;
		SAME_TYPE_ASSERT (null_join_ci,ci,null_join_ci_is_ci)
		typedef join<ci,null_list>::result ci_join_null;
		SAME_TYPE_ASSERT (ci_join_null,ci,ci_join_null_is_ci)
		typedef join<null_list,c>::result null_join_c;
		SAME_TYPE_ASSERT (null_join_c,c,null_join_c_is_c)
		typedef join<c,null_list>::result c_join_null;
		SAME_TYPE_ASSERT (c_join_null,c,c_join_null_is_c)
		typedef join<null_list,null_list>::result null_join_null;
		SAME_TYPE_ASSERT (null_join_null,null_list,null_join_null_is_null)
	}

//get_index
	{
		const int null_in_null = get_index<null_list,null_list>::result;
		COMPILE_ASSERT(null_in_null==0, null_in_null_is_0)
		const int null_in_i = get_index<i,null_list>::result;
		COMPILE_ASSERT(null_in_i==1, null_in_i_is_1)
		const int null_in_ii = get_index<ii,null_list>::result;
		COMPILE_ASSERT(null_in_ii==2, null_in_ii_is_2)
		const int null_in_icci = get_index<icci,null_list>::result;
		COMPILE_ASSERT(null_in_icci==4, null_in_icci_is_4)
		const int int_in_i = get_index<i,int>::result;
		COMPILE_ASSERT(int_in_i==0, int_in_i_is_0)
		const int int_in_ic = get_index<ic,int>::result;
		COMPILE_ASSERT(int_in_ic==0, int_in_ic_is_0)
		const int int_in_ci = get_index<ci,int>::result;
		COMPILE_ASSERT(int_in_ci==1, int_in_ci_is_1)
		const int int_in_ii = get_index<ii,int>::result;
		COMPILE_ASSERT(int_in_ii==0, int_in_ii_is_0)
		const int int_in_cii = get_index<cii,int>::result;
		COMPILE_ASSERT(int_in_cii==1, int_in_cii_is_1)
		const int int_in_ici = get_index<ici,int>::result;
		COMPILE_ASSERT(int_in_ici==0, int_in_ici_is_0)
		const int int_in_icc = get_index<icc,int>::result;
		COMPILE_ASSERT(int_in_icc==0, int_in_icc_is_0)
		const int int_in_cic = get_index<cic,int>::result;
		COMPILE_ASSERT(int_in_cic==1, int_in_cic_is_1)
		const int int_in_cci = get_index<cci,int>::result;
		COMPILE_ASSERT(int_in_cci==2, int_in_cci_is_2)
		const int int_in_null = get_index<null_list,int>::result;
		COMPILE_ASSERT(int_in_null==-1, int_in_null_is_minus1)
		const int int_in_c = get_index<c,int>::result;
		COMPILE_ASSERT(int_in_c==-1, int_in_c_is_minus1)
		const int int_in_cc = get_index<cc,int>::result;
		COMPILE_ASSERT(int_in_cc==-1, int_in_cc_is_minus1)
		const int int_in_ccc = get_index<ccc,int>::result;
		COMPILE_ASSERT(int_in_ccc==-1, int_in_ccc_is_minus1)
	}

//length
	{
		const int len_of_null = length<null_list>::result;
		COMPILE_ASSERT(len_of_null==0, len_of_null_is_0)
		const int len_of_i = length<i>::result;
		COMPILE_ASSERT(len_of_i==1, len_of_i_is_1)
		const int len_of_ci = length<ci>::result;
		COMPILE_ASSERT(len_of_ci==2, len_of_ci_is_2)
		const int len_of_ici = length<ici>::result;
		COMPILE_ASSERT(len_of_ici==3, len_of_ici_is_3)
	}

//get_by_index
	{
		typedef get_by_index<null_list,0>::result null_0;
		SAME_TYPE_ASSERT (null_0,null_list,null_0_is_null)
		typedef get_by_index<null_list,1>::result null_1;
		SAME_TYPE_ASSERT (null_1,null_list,null_1_is_null)
		typedef get_by_index<null_list,2>::result null_2;
		SAME_TYPE_ASSERT (null_2,null_list,null_2_is_null)
		typedef get_by_index<i, 0>::result i_0;
		SAME_TYPE_ASSERT (i_0,int,i_0_is_int)
		typedef get_by_index<i, 1>::result i_1;
		SAME_TYPE_ASSERT (i_1,null_list,i_1_is_null)
		typedef get_by_index<i, 2>::result i_2;
		SAME_TYPE_ASSERT (i_2,null_list,i_2_is_null)
		typedef get_by_index<i, 3>::result i_3;
		SAME_TYPE_ASSERT (i_3,null_list,i_3_is_null)
		typedef get_by_index<ic, 0>::result ic_0;
		SAME_TYPE_ASSERT (ic_0,int,ic_0_is_int)
		typedef get_by_index<ic, 1>::result ic_1;
		SAME_TYPE_ASSERT (ic_1,char,ic_1_is_char)
		typedef get_by_index<ic, 2>::result ic_2;
		SAME_TYPE_ASSERT (ic_2,null_list,ic_2_is_null)
		typedef get_by_index<ic, 3>::result ic_3;
		SAME_TYPE_ASSERT (ic_3,null_list,ic_3_is_null)

	}

//erase
	{
		typedef erase<null_list,int>::result null_minus_int;
		SAME_TYPE_ASSERT (null_minus_int,null_list,null_minus_int_is_null)
		typedef erase<null_list,null_list>::result null_minus_null;
		SAME_TYPE_ASSERT (null_minus_null,null_list,null_minus_null_is_null)
		typedef erase<c,null_list>::result c_minus_null;
		SAME_TYPE_ASSERT (c_minus_null,c,c_minus_null_is_c)
		typedef erase<cc,null_list>::result cc_minus_null;
		SAME_TYPE_ASSERT (cc_minus_null,cc,cc_minus_null_is_cc)
		typedef erase<cic,null_list>::result cic_minus_null;
		SAME_TYPE_ASSERT (cic_minus_null,cic,cic_minus_null_is_cic)
		typedef erase<c,char>::result c_minus_char;
		SAME_TYPE_ASSERT (c_minus_char,null_list,c_minus_char_is_null)
		typedef erase<c,int>::result c_minus_int;
		SAME_TYPE_ASSERT (c_minus_int,c,c_minus_int_is_c)
		typedef erase<ci,int>::result ci_minus_int;
		SAME_TYPE_ASSERT (ci_minus_int,c,ci_minus_int_is_c)
		typedef erase<ci,char>::result ci_minus_char;
		SAME_TYPE_ASSERT (ci_minus_char,i,ci_minus_char_is_i)
		typedef erase<ci,double>::result ci_minus_double;
		SAME_TYPE_ASSERT (ci_minus_double,ci,ci_minus_double_is_ci)
		typedef erase<cic,int>::result cic_minus_int;
		SAME_TYPE_ASSERT (cic_minus_int,cc,cic_minus_int_is_cc)
		typedef erase<cic,char>::result cic_minus_char;
		SAME_TYPE_ASSERT (cic_minus_char,i,cic_minus_char_is_i)
		typedef erase<cii,int>::result cii_minus_int;
		SAME_TYPE_ASSERT (cii_minus_int,c,cii_minus_int_is_c)
		typedef erase<cii,char>::result cii_minus_char;
		SAME_TYPE_ASSERT (cii_minus_char,ii,cic_minus_char_is_ii)
	}

//unique
	{
		typedef unique<null_list>::result unique_null;
		SAME_TYPE_ASSERT (unique_null,null_list,unique_null_is_null)
		typedef unique<c>::result unique_c;
		SAME_TYPE_ASSERT (unique_c,c,unique_c_is_c)
		typedef unique<cc>::result unique_cc;
		SAME_TYPE_ASSERT (unique_cc,c,unique_cc_is_c)
		typedef unique<ci>::result unique_ci;
		SAME_TYPE_ASSERT (unique_ci,ci,unique_ci_is_ci)
		typedef unique<cic>::result unique_cic;
		SAME_TYPE_ASSERT (unique_cic,ci,unique_cic_is_ci)
		typedef unique<cci>::result unique_cci;
		SAME_TYPE_ASSERT (unique_cci,ci,unique_cci_is_ci)
		typedef unique<icc>::result unique_icc;
		SAME_TYPE_ASSERT (unique_icc,ic,unique_icc_is_ic)
	}

//foreach
	{
		foreach_counter = 0; foreach<null_list,test_foreach_processor>::call ();
		RUNTIME_ASSERT (foreach_counter==0, null_list_foreach_is_0);
		foreach_counter = 0; foreach<c,test_foreach_processor>::call ();
		RUNTIME_ASSERT (foreach_counter==1, c_foreach_is_1);
		foreach_counter = 0; foreach<ci,test_foreach_processor>::call ();
		RUNTIME_ASSERT (foreach_counter==3, ci_foreach_is_3);
		foreach_counter = 0; foreach<ic,test_foreach_processor>::call ();
		RUNTIME_ASSERT (foreach_counter==3, ic_foreach_is_3);
		foreach_counter = 0; foreach<icci,test_foreach_processor>::call ();
		RUNTIME_ASSERT (foreach_counter==6, icci_foreach_is_6);
		foreach_counter = 0; foreach<icci,test_foreach_processor>::call (1);
		RUNTIME_ASSERT (foreach_counter==4, icci_foreach_is_6);
		foreach_counter = 0; foreach<icci,test_foreach_processor>::call (2,1);
		RUNTIME_ASSERT (foreach_counter==4, icci_foreach_is_6);
	}

//decapitate_if
	{
		typedef decapitate_if<null_list,true>::result decapitated_null;
		SAME_TYPE_ASSERT (decapitated_null,null_list,decapitated_null_is_null);
		typedef decapitate_if<null_list,false>::result not_decapitated_null;
		SAME_TYPE_ASSERT (not_decapitated_null,null_list,not_decapitated_null_is_null);
		typedef decapitate_if<i,true>::result decapitated_i;
		SAME_TYPE_ASSERT (decapitated_i,null_list,decapitated_i_is_null);
		typedef decapitate_if<i,false>::result not_decapitated_i;
		SAME_TYPE_ASSERT (not_decapitated_i,i,not_decapitated_i_is_i);
		typedef decapitate_if<ii,true>::result decapitated_ii;
		SAME_TYPE_ASSERT (decapitated_ii,i,decapitated_ii_is_i);
		typedef decapitate_if<ii,false>::result not_decapitated_ii;
		SAME_TYPE_ASSERT (not_decapitated_ii,ii,not_decapitated_ii_is_ii);
		typedef decapitate_if<ic,true>::result decapitated_ic;
		SAME_TYPE_ASSERT (decapitated_ii,i,decapitated_ic_is_c);
		typedef decapitate_if<ic,false>::result not_decapitated_ic;
		SAME_TYPE_ASSERT (not_decapitated_ic,ic,not_decapitated_ic_is_ic);
	}

//filter
	{
		typedef filter <null_list,test_filter>::result null_filtered;
		SAME_TYPE_ASSERT (null_filtered,null_list,null_filtered_is_null);
		typedef filter <p1,test_filter>::result p1_filtered;
		SAME_TYPE_ASSERT (p1_filtered,p1,p1_filtered_is_p1);
		typedef filter <np1,test_filter>::result np1_filtered;
		SAME_TYPE_ASSERT (np1_filtered,null_list,np1_filtered_is_null);
		typedef filter <p1p2,test_filter>::result p1p2_filtered;
		SAME_TYPE_ASSERT (p1p2_filtered,p1p2,p1p2_filtered_is_p1p2);
		typedef filter <np1np2,test_filter>::result np1np2_filtered;
		SAME_TYPE_ASSERT (np1np2_filtered,null_list,np1np2_filtered_is_null);
		typedef filter <p1np1,test_filter>::result p1np1_filtered;
		SAME_TYPE_ASSERT (p1np1_filtered,p1,p1np1_filtered_is_p1);
		typedef filter <np1p1,test_filter>::result np1p1_filtered;
		SAME_TYPE_ASSERT (np1p1_filtered,p1,np1p1_filtered_is_p1);
	}

//transform
	{
		typedef transform<null_list,test_transform>::result transform_null;
		SAME_TYPE_ASSERT (transform_null,null_list,transform_null_is_null);
		typedef transform<c,test_transform>::result transform_c;
		SAME_TYPE_ASSERT (transform_c,i,transform_c_is_i);
		typedef transform<i,test_transform>::result transform_i;
		SAME_TYPE_ASSERT (transform_i,c,transform_i_is_c);
		typedef transform<ic,test_transform>::result transform_ic;
		SAME_TYPE_ASSERT (transform_ic,ci,transform_ic_is_ci);
		typedef transform<ci,test_transform>::result transform_ci;
		SAME_TYPE_ASSERT (transform_ci,ic,transform_ci_is_ic);
	}

//split_by_categories
	{
		typedef split_by_categories <null_list,null_list,test_transform>::result null_split_by_null;
		SAME_TYPE_ASSERT (null_split_by_null,null_list,null_split_by_null_is_null);
		typedef split_by_categories <i,null_list,test_transform>::result i_split_by_null;
		SAME_TYPE_ASSERT (i_split_by_null,null_list,i_split_by_null_is_null);
		typedef split_by_categories <null_list,i,test_transform>::result null_split_by_i;
		SAME_TYPE_ASSERT (null_split_by_i,typelist<null_list>,null_split_by_i_is_typelist_null);
		typedef split_by_categories <i,i,test_transform>::result i_split_by_i;
		SAME_TYPE_ASSERT (i_split_by_i,typelist<null_list>,i_split_by_i_is_typelist_null);
		typedef split_by_categories <i,c,test_transform>::result i_split_by_c;
		SAME_TYPE_ASSERT (i_split_by_c,typelist<i>,i_split_by_c_is_typelist_i);
		typedef split_by_categories <ic,c,test_transform>::result ic_split_by_c;
		typedef typelist<i> ll_i;
		SAME_TYPE_ASSERT (ic_split_by_c,ll_i,ic_split_by_c_is_typelist_i);
		typedef split_by_categories <ic,ci,test_transform>::result ic_split_by_ci;
		typedef typelist<i,typelist<c> > ll_i_c;
		SAME_TYPE_ASSERT (ic_split_by_ci,ll_i_c,ic_split_by_ci_is_some_typelist);
		typedef split_by_categories <icci,ci,test_transform>::result icci_split_by_ci;
		typedef typelist<ii,typelist<cc> > ll_ii_cc;
		SAME_TYPE_ASSERT (icci_split_by_ci,ll_ii_cc,icci_split_by_ci_is_some_typelist);
		typedef split_by_categories <icci,ic,test_transform>::result icci_split_by_ic;
		typedef typelist<cc,typelist<ii> > ll_cc_ii;
		SAME_TYPE_ASSERT (icci_split_by_ic,ll_cc_ii,icci_split_by_ic_is_some_typelist);
	}

	//categorize_by_transformation
	{
		typedef categorize_by_transformation <null_list,test_transform>::result null_cat;
		SAME_TYPE_ASSERT (null_cat,null_list,null_cat_is_null);
		typedef categorize_by_transformation <i,test_transform> i_cat;
		typedef typelist <i> i_cat_res;
		SAME_TYPE_ASSERT (i_cat::result,i_cat_res,i_cat_res_is_ok)
		SAME_TYPE_ASSERT (i_cat::categories,c,i_cat_cat_is_c);
		typedef categorize_by_transformation <icci,test_transform> icci_cat;
		typedef typelist<ii,typelist<cc> > icci_cat_res;
		SAME_TYPE_ASSERT (icci_cat::result,icci_cat_res,icci_cat_res_is_ok)
		SAME_TYPE_ASSERT (icci_cat::categories,ci,icci_cat_cat_is_ci);
		typedef categorize_by_transformation <ccci,test_transform> ccci_cat;
		typedef typelist<ccc,typelist<i> > ccci_cat_res;
		SAME_TYPE_ASSERT (ccci_cat::result,ccci_cat_res,ccci_cat_res_is_ok)
		SAME_TYPE_ASSERT (ccci_cat::categories,ic,ccci_cat_cat_is_ic)
	}

	//arithmetic_if
	{
		typedef arithmetic_if<-5,char,int,float>::result c;
		typedef arithmetic_if<0,char,int,float>::result i;
		typedef arithmetic_if<500,char,int,float>::result f;
		SAME_TYPE_ASSERT (c,char,neg_is_ok)
		SAME_TYPE_ASSERT (i,int,zero_is_ok)
		SAME_TYPE_ASSERT (f,float,pos_is_ok)
	}

	//max
	{
		typedef max<TLIST1(char),test_compare_classes>::result c;
		SAME_TYPE_ASSERT (c,char,c_is_ok)
		typedef max<TLIST2(char, unsigned char),test_compare_classes>::result cu;
		SAME_TYPE_ASSERT (cu,char,cu_is_ok)
		typedef max<TLIST2(unsigned char, char),test_compare_classes>::result uc;
		SAME_TYPE_ASSERT (uc,unsigned char,uc_is_ok)
		typedef max<TLIST3(int, unsigned char, char),test_compare_classes>::result iuc;
		SAME_TYPE_ASSERT (iuc,int,iuc_is_ok)
		typedef max<TLIST3(unsigned char, int, char),test_compare_classes>::result uic;
		SAME_TYPE_ASSERT (uic,int,uic_is_ok)
		typedef max<TLIST3(unsigned char, char, int),test_compare_classes>::result uci;
		SAME_TYPE_ASSERT (uci,int,uci_is_ok)
	}
}

static void test_datatypes () {
	using namespace ::avrpp::datatypes;
	using namespace ::avrpp::config;
	using namespace ::avrpp::typelist;
	typedef TLIST2(char, long) cl; //I hope they never have the same type :-)
	typedef TLIST2(long, char) lc; //I hope they never have the same type :-)
	typedef TLIST2(unsigned char, char) ucc;
	typedef TLIST2(char, unsigned char) cuc;
	//get_enclosing_type_by_sizeof
	{
		typedef get_enclosing_type_by_sizeof <null_list, 1>::result null;
		SAME_TYPE_ASSERT (null,null_list,null_list_gives_nothing)
		typedef get_enclosing_type_by_sizeof <cl, 1>::result cl_1;
		SAME_TYPE_ASSERT (cl_1,char,cl_1_is_char)
		typedef get_enclosing_type_by_sizeof <cl, sizeof(long)>::result cl_l;
		SAME_TYPE_ASSERT (cl_l,long,cl_l_is_long)
		typedef get_enclosing_type_by_sizeof <cl, sizeof(long)*2>::result cl_l2;
		SAME_TYPE_ASSERT (cl_l2,null_list,cl_l2_is_null)
	}
	//get_widest_type
	{
		typedef get_widest_type<cl>::result w_cl;
		SAME_TYPE_ASSERT (w_cl,long,w_cl_is_l)
		typedef get_widest_type<lc>::result w_lc;
		SAME_TYPE_ASSERT (w_lc,long,w_lc_is_l)
		typedef get_widest_type<cuc>::result w_cuc;
		SAME_TYPE_ASSERT (w_cuc,char,w_cuc_is_c)
		typedef get_widest_type<ucc>::result w_ucc;
		SAME_TYPE_ASSERT (w_ucc,unsigned char,w_ucc_is_uc)
	}
}

static volatile unsigned char portt1=0, ddrt1=0, pint1=0;
static volatile unsigned char portt2=0, ddrt2=0, pint2=0;

static unsigned char up_t1 = 0, up_t2 = 0, up_it1 = 0, up_it2 = 0;

struct port_t1 {
	typedef unsigned char bulk_data_type;
	typedef ::avrpp::ports::bulk_port_operations <port_t1> bulk;
	static volatile unsigned char & port () {return portt1;}
	static volatile unsigned char & pin () {return pint1;}
	static volatile unsigned char & ddr () {return ddrt1;}
	static void update_outputs () {up_t1++;}
	static void update_inputs () {up_it1++;}
};

struct port_t2 {
	typedef unsigned char bulk_data_type;
	typedef ::avrpp::ports::bulk_port_operations <port_t2> bulk;
	static volatile unsigned char & port () {return portt2;}
	static volatile unsigned char & pin () {return pint2;}
	static volatile unsigned char & ddr () {return ddrt2;}
	static void update_outputs () {up_t2++;}
	static void update_inputs () {up_it2++;}
};


static volatile unsigned long portw1=0, ddrw1=0, pinw1=0;

static volatile unsigned long up_w1 = 0, up_iw1 = 0;

struct port_w1 {
	typedef unsigned long bulk_data_type;
	typedef ::avrpp::ports::bulk_port_operations <port_w1> bulk;
	static volatile bulk_data_type & port () {return portw1;}
	static volatile bulk_data_type & pin () {return pinw1;}
	static volatile bulk_data_type & ddr () {return ddrw1;}
	static void update_outputs () {up_w1++;}
	static void update_inputs () {up_iw1++;}
};

static unsigned char lock_count = 0, unlock_count = 0;
struct pseudo_lock {
	static void lock () {lock_count++;};
	static void unlock () {unlock_count++;};
	static void init () {lock_count=unlock_count=0;}
};
/**
* @brief Tests operations on physical i/o ports from ports.hpp
*/
#ifdef AVRPP_KTEST //No test on target platform: limited flash
void test_ports_hpp () {
	using namespace ::avrpp::ports;
	//input<port,pin,state>
	{
		typedef input<port_t1,3,hi_z,pseudo_lock> p3;
		typedef input<port_t1,3,pullup,pseudo_lock> p3p;
		SAME_TYPE_ASSERT (p3::options, hi_z<p3::bulk>, hi_z_ok)
		SAME_TYPE_ASSERT (p3p::options, pullup<p3::bulk>, pullup_ok)
		COMPILE_ASSERT (p3::port_pin==3,input_port_pin)
		COMPILE_ASSERT (p3p::port_pin==3,input_port_pin)
		ddrt1 = 1<<3;
		portt1 = 1<<3;
		pseudo_lock::init ();
		p3::config ();
		RUNTIME_ASSERT (ddrt1==0,input_clears_ddr_bit);
		RUNTIME_ASSERT (lock_count==1 && unlock_count==1,locking);
		RUNTIME_ASSERT (portt1==0,hi_z_clears_pullup_bit);
		portt1 = 0xff;
		ddrt1 = 0xff;
		pseudo_lock::init ();
		p3::config ();
		RUNTIME_ASSERT (lock_count==1 && unlock_count==1,locking);
		RUNTIME_ASSERT (ddrt1==(0xff^(1<<3)),input_clears_only_own_ddr_bit);
		RUNTIME_ASSERT (portt1==(0xff^(1<<3)),hi_z_clears_only_own_pullup_bit);
		pseudo_lock::init ();
		p3p::config ();
		RUNTIME_ASSERT (lock_count==1 && unlock_count==1,locking);
		RUNTIME_ASSERT (portt1==0xff,pullup_sets_pullup_bit);
		portt1 = 0;
		pseudo_lock::init ();
		p3p::config ();
		RUNTIME_ASSERT (lock_count==1 && unlock_count==1,locking);
		RUNTIME_ASSERT (portt1==(1<<3),pullup_sets_only_own_pullup_bit);
		portt1 = 0;
		pseudo_lock::init ();
		p3p::config_options ();
		RUNTIME_ASSERT (portt1==(1<<3),pullup_config_pullup);
		RUNTIME_ASSERT (lock_count==1 && unlock_count==1,locking);
		pseudo_lock::init ();
		p3::config_options ();
		RUNTIME_ASSERT (lock_count==1 && unlock_count==1,locking);
		RUNTIME_ASSERT (portt1==0,hi_z_config_pullup);
		p3::update ();
		pint1 = 0xff^(1<<3);
		RUNTIME_ASSERT (!p3::get(),get_input_0);
		pint1 = 1<<3;
		RUNTIME_ASSERT (p3::get(),get_input_1);
	}
	//output<port,pin,state>
	{
		typedef output<port_t1,3,pseudo_lock> p3;
		COMPILE_ASSERT (p3::port_pin==3,outut_port_pin);
		ddrt1 = portt1 = 0;
		pseudo_lock::init ();
		p3::config ();
		RUNTIME_ASSERT (lock_count==1 && unlock_count==1,locking);
		RUNTIME_ASSERT (ddrt1==(1<<3),output_config);
		pseudo_lock::init ();
		p3::set(true);
		RUNTIME_ASSERT (lock_count==1 && unlock_count==1,locking);
		RUNTIME_ASSERT (portt1==(1<<3),output_set_true);
		RUNTIME_ASSERT (p3::get(),output_set_bit_is_read)
		pseudo_lock::init ();
		p3::set(false);
		RUNTIME_ASSERT (lock_count==1 && unlock_count==1,locking);
		RUNTIME_ASSERT (portt1==0,output_set_false);
		RUNTIME_ASSERT (!p3::get(),output_cleared_bit_is_read)
		pseudo_lock::init ();
		portt1=0xff;
		p3::set(false);
		RUNTIME_ASSERT (lock_count==1 && unlock_count==1,locking);
		RUNTIME_ASSERT (portt1==(0xff^(1<<3)),output_set_false_single_bit);
		RUNTIME_ASSERT (!p3::get(),output_cleared_bit_is_read)
		pseudo_lock::init ();
		p3::set(true);
		RUNTIME_ASSERT (lock_count==1 && unlock_count==1,locking);
		RUNTIME_ASSERT (portt1==0xff,output_set_true_single_bit);
		RUNTIME_ASSERT (p3::get(),output_set_bit_is_read)
		p3::update ();
	}
}
#endif

void test_bus_hpp () {
	using namespace ::avrpp::bus;
	using namespace ::avrpp::ports;
	using namespace ::avrpp::typelist;
	typedef output<port_t1,3> p3;
	typedef output<port_t1,4> p4;
	typedef output<port_t1,7> p7;
	typedef bus_pin <p3,1> p3_1;
	typedef bus_pin <p3,7> p3_7;
	typedef bus_pin <p4,2> p4_2;

	//bus_pin<pin_class,bit_num>
	{
		COMPILE_ASSERT(p3_1::bus_bit==1,bit_1_is_1);
		COMPILE_ASSERT(p3_7::bus_bit==7,bit_7_is_7);
		COMPILE_ASSERT(p3_1::port_pin==3,pin_3_is_3);
		COMPILE_ASSERT(p3_7::port_pin==3,pin_3_is_3);
		SAME_TYPE_ASSERT(p3_1::pin,p3,bus_pin_is_pin);
		SAME_TYPE_ASSERT(p3_7::pin,p3,bus_pin_is_pin);
		SAME_TYPE_ASSERT(p3_1::bulk,p3::bulk,bus_pin_bulk_ok);
		SAME_TYPE_ASSERT(p3_7::bulk,p3::bulk,bus_pin_bulk_ok);
		SAME_TYPE_ASSERT(p3_1::port,p3::port,bus_pin_port_ok);
		SAME_TYPE_ASSERT(p3_7::port,p3::port,bus_pin_port_ok);
		SAME_TYPE_ASSERT(p3_1::port,p3::port,bus_pin_port_ok);
		SAME_TYPE_ASSERT(p3_7::port,p3::port,bus_pin_port_ok);
		COMPILE_ASSERT(p3_1::src_mask==0x02,p3_1_src_mask_ok);
		COMPILE_ASSERT(p3_7::src_mask==0x80,p3_7_src_mask_ok);
		COMPILE_ASSERT(p3_1::dst_mask==0x08,p3_1_dst_mask_ok);
		COMPILE_ASSERT(p3_7::dst_mask==0x08,p3_7_dst_mask_ok);
		COMPILE_ASSERT(p3_1::bit_shift==2,p3_1_bit_shift_ok);
		COMPILE_ASSERT(p3_7::bit_shift==-4,p3_7_bit_shift_ok);
		COMPILE_ASSERT(p3_1::lshift==2,p3_1_lshift_ok);
		COMPILE_ASSERT(p3_7::lshift==0,p3_7_lshift_ok);
		COMPILE_ASSERT(p3_1::rshift==0,p3_1_rshift_ok);
		COMPILE_ASSERT(p3_7::rshift==4,p3_7_rshift_ok);
	}

	//group_output<out_pin_list>, group_update_outputs<out_pin_list>
	{
		typedef TLIST1(p4_2) bus_list1;
		typedef TLIST2(p3_1, p4_2) bus_list2;
		typedef group_output <bus_list1,unsigned char> out_group1;
		typedef group_output <bus_list2,unsigned char> out_group2;
		COMPILE_ASSERT(out_group1::dst_mask==0x10,out_group1_dst_is_ok);
		COMPILE_ASSERT(out_group2::dst_mask==0x18,out_group2_dst_is_ok);
		COMPILE_ASSERT(out_group1::src_mask==0x04,out_group1_src_is_ok);
		COMPILE_ASSERT(out_group2::src_mask==0x06,out_group2_src_is_ok);
		COMPILE_ASSERT(out_group1::lshift==2,out_group1_lshift_is_ok);
		COMPILE_ASSERT(out_group2::lshift==2,out_group2_lshift_is_ok);
		COMPILE_ASSERT(out_group1::rshift==0,out_group1_rshift_is_ok);
		COMPILE_ASSERT(out_group2::rshift==0,out_group2_rshift_is_ok);
		portt1 = 0;
		out_group2::out(0x06);
		RUNTIME_ASSERT(portt1==0x18,group_out_is_ok);
		out_group2::out(~0x06);
		RUNTIME_ASSERT(portt1==0x00,group_out_is_ok);
		out_group2::out(0x02);
		RUNTIME_ASSERT(portt1==0x08,group_out_is_ok);
		out_group2::out(0x04);
		RUNTIME_ASSERT(portt1==0x10,group_out_is_ok);
		ddrt1 = 0;
		out_group2::config ();
		RUNTIME_ASSERT(ddrt1==0x18,group_out_is_ok);
		up_t1 = 0;
		group_update_outputs<bus_list2,unsigned char>::update ();
		RUNTIME_ASSERT(up_t1==1,group_upd_out_is_ok);
	}

	//group_input<out_pin_list>, group_update_inputs<out_pin_list>
	{ // The test indirectly relies on the previous one (group_output, group_update_outputs)
		typedef bus_pin< input <port_t1,2,pullup>, 1 > p1;
		typedef bus_pin< input <port_t1,1,hi_z>, 0 > p0;
		typedef TLIST2(p1,p0) in_list;
		typedef group_input <in_list,unsigned char> in_group1;
		typedef group_update_inputs <in_list,unsigned char> uin_group1;
		COMPILE_ASSERT(in_group1::dst_mask==0x06,ingroup_dst_mask_ok)
		COMPILE_ASSERT(in_group1::src_mask==0x03,ingroup_src_mask_ok)
		enum {num_opt_groups=length<in_group1::option_groups>::result};
		COMPILE_ASSERT(num_opt_groups==2,ingroup_pullup_mask_ok)
		COMPILE_ASSERT(in_group1::lshift==1,in_group1_lshift_is_ok);
		COMPILE_ASSERT(in_group1::rshift==0,in_group1_lshift_is_ok);

		pint1 = portt1 = 0; ddrt1 = 0xff;
		in_group1::config ();
		RUNTIME_ASSERT(portt1==0x04,pullup_is_ok);
		RUNTIME_ASSERT(ddrt1==static_cast<unsigned char>(~0x06),ddr_is_ok);
		up_it1 = 0;
		uin_group1::update ();
		RUNTIME_ASSERT(up_it1==1,group_upd_in_is_ok);

	}

	//bus_pin_wrapper<pin_list>
	{
		typedef TLIST3(p4, p3, p7) bus_list;
		typedef bus_pin<p4,2> p4_2;
		typedef bus_pin<p3,1> p3_1;
		typedef bus_pin<p7,0> p7_0;
		typedef TLIST3(p4_2,p3_1,p7_0) bus_list_w;
		typedef bus_pin_wrapper<null_list>::result wrapped_null;
		SAME_TYPE_ASSERT(wrapped_null,null_list,wrapped_null_is_null);
		typedef bus_pin_wrapper<bus_list>::result bus_wrap;
		SAME_TYPE_ASSERT (bus_wrap,bus_list_w,bus_pins_wrapping_ok);
	}

	//get_port,get_bulk,type_and_number,get_bulk_and_shift
	{
		typedef bus_pin<p4,2> p4_2;
		typedef get_port<p4_2>::result p_port;
		typedef get_bulk<p4_2>::result p_bulk;
		typedef get_bulk_and_shift<p4_2>::result p_bulk_shift;
		typedef type_and_number<port_t1::bulk,2> bsh;
		SAME_TYPE_ASSERT(p_port,port_t1,get_port_ok);
		SAME_TYPE_ASSERT(p_bulk,port_t1::bulk,get_bulk_ok);
		SAME_TYPE_ASSERT(p_bulk_shift,bsh,get_bulk_and_shift);
	}

	//accumulate_bits<class_list,processor>
	{

		typedef TLIST1(wrap_4_acc_bits<char>) c;
		typedef TLIST1(wrap_4_acc_bits<int>) i;
		typedef TLIST1(wrap_4_acc_bits<long>) f;
		typedef TLIST2(wrap_4_acc_bits<long>,wrap_4_acc_bits<char>) fc;
		typedef TLIST2(wrap_4_acc_bits<char>,wrap_4_acc_bits<long>) cf;
		typedef TLIST2(wrap_4_acc_bits<int>,wrap_4_acc_bits<char>) ic;
		typedef TLIST2(wrap_4_acc_bits<char>,wrap_4_acc_bits<int>) ci;
		typedef TLIST2(wrap_4_acc_bits<int>,wrap_4_acc_bits<int>) ii;
		typedef TLIST2(wrap_4_acc_bits<char>,wrap_4_acc_bits<char>) cc;

		typedef accumulate_bits <null_list,test_accumulate_bits_processor> ab_null;
		typedef accumulate_bits <i,test_accumulate_bits_processor> ab_i;
		typedef accumulate_bits <ii,test_accumulate_bits_processor> ab_ii;
		typedef accumulate_bits <c,test_accumulate_bits_processor> ab_c;
		typedef accumulate_bits <cc,test_accumulate_bits_processor> ab_cc;
		typedef accumulate_bits <ic,test_accumulate_bits_processor> ab_ic;
		typedef accumulate_bits <f,test_accumulate_bits_processor> ab_f;
		typedef accumulate_bits <cf,test_accumulate_bits_processor> ab_cf;
		typedef accumulate_bits <fc,test_accumulate_bits_processor> ab_fc;
		RUNTIME_ASSERT(ab_null::call ()==0,ab_null_0)
		RUNTIME_ASSERT(ab_i::call ()==2,ab_i_2)
		RUNTIME_ASSERT(ab_ii::call ()==2,ab_ii_2)
		RUNTIME_ASSERT(ab_c::call ()==1,ab_c_1)
		RUNTIME_ASSERT(ab_cc::call ()==1,ab_cc_1)
		RUNTIME_ASSERT(ab_ic::call ()==3,ab_ic_3)
		RUNTIME_ASSERT(ab_f::call ()==0,ab_f_0)
		RUNTIME_ASSERT(ab_cf::call ()==1,ab_cf_1)
		RUNTIME_ASSERT(ab_fc::call ()==1,ab_fc_1)
	}

	//output_bus<pin_list>
	{
		typedef output<port_t2,1> p21;
		typedef TLIST4(p4,p3,p7,p21) bus_list;
		typedef output_bus<bus_list> bus;
		enum { lug = length<bus::pins_by_bulk>::result };
		enum { log = length<bus::pins_by_bulk_and_shift>::result };
		COMPILE_ASSERT(lug==2,two_ports_two_updates);
		COMPILE_ASSERT(log==3,and_3_outs);
		ddrt1=ddrt2=0x00;
		bus::config ();
		RUNTIME_ASSERT (ddrt1==0x98,ddrt1_ok);
		RUNTIME_ASSERT (ddrt2==0x02,ddrt2_ok);
		portt1=portt2=0x00;
		bus::out(0x00);
		RUNTIME_ASSERT (portt1==0x00,portt1_ok)
		RUNTIME_ASSERT (portt2==0x00,portt2_ok)
		RUNTIME_ASSERT (bus::get_outputs ()==0x00,get_outputs_ok)
		bus::out(0xff);
		RUNTIME_ASSERT (portt1==0x98,portt1_ok);
		RUNTIME_ASSERT (portt2==0x02,portt2_ok);
		RUNTIME_ASSERT (bus::get_outputs ()==0x0f,get_outputs_ok)
		portt1=portt2=0xff;
		bus::out(0x00);
		RUNTIME_ASSERT (portt1==static_cast<unsigned char>(~0x98),portt1_ok);
		RUNTIME_ASSERT (portt2==static_cast<unsigned char>(~0x02),portt2_ok);
		portt1=portt2=0x00;
		bus::out(0x01);
		RUNTIME_ASSERT (portt1==0x00,portt1_ok);
		RUNTIME_ASSERT (portt2==0x02,portt2_ok);
		RUNTIME_ASSERT (bus::get_outputs ()==0x01,get_outputs_ok)
		bus::out(0x02);
		RUNTIME_ASSERT (portt1==0x80,portt1_ok);
		RUNTIME_ASSERT (portt2==0x00,portt2_ok);
		RUNTIME_ASSERT (bus::get_outputs ()==0x02,get_outputs_ok)
		bus::out(0x04);
		RUNTIME_ASSERT (portt1==0x08,portt1_ok);
		RUNTIME_ASSERT (portt2==0x00,portt2_ok);
		RUNTIME_ASSERT (bus::get_outputs ()==0x04,get_outputs_ok)
		bus::out(0x08);
		RUNTIME_ASSERT (portt1==0x10,portt1_ok);
		RUNTIME_ASSERT (portt2==0x00,portt2_ok);
		RUNTIME_ASSERT (bus::get_outputs ()==0x08,get_outputs_ok)
	}

	//output_bus<pin_list>, wide version
	{
		typedef output<port_w1,8> pw1_8;
		typedef output<port_t1,7> p1_7;
		typedef output<port_t1,6> p1_6;
		typedef output<port_t1,5> p1_5;
		typedef output<port_t1,4> p1_4;
		typedef output<port_t1,3> p1_3;
		typedef output<port_t1,2> p1_2;
		typedef output<port_t1,1> p1_1;
		typedef output<port_t1,0> p1_0;
		typedef output<port_t2,0> p2_0;
		typedef TLIST9(pw1_8,p1_0,p1_1,p1_2,p1_3,p1_4,p1_5,p1_6,p1_7) pinlist;
		typedef TLIST9(p1_7,p1_6,p1_5,p1_4,p1_3,p1_2,p1_1,p1_0,p2_0) wpinlist_np;
		typedef TLIST1(pw1_8) npinlist_wp;
		typedef output_bus<pinlist> bus;
		typedef output_bus<wpinlist_np> wbus_np;
		typedef output_bus<npinlist_wp> nbus_wp;
		portw1 = ddrw1 = 0;
		enum { lug = length<bus::pins_by_bulk>::result };
		enum { log = length<bus::pins_by_bulk_and_shift>::result };
		COMPILE_ASSERT(lug==2,two_ports_two_updates);
		COMPILE_ASSERT(log==9,and_9_outs);
		bus::config ();
		//int qq=portw1;
		//int qq1=portt1;
		RUNTIME_ASSERT (ddrw1==0x100,ddrw1_ok)
		RUNTIME_ASSERT (ddrt1==0xff,ddrt1_ok)
		up_t1 = up_w1 = 0;
		bus::just_out(0x1ff);
		RUNTIME_ASSERT (up_t1==0 && up_w1==0, just_out_does_not_update);
		//int qq1 = portw1;
		//int qq2 = portt1;
		RUNTIME_ASSERT (portw1==0x100,portw1_ok)
		RUNTIME_ASSERT (portt1==0xff,portt1_ok)
		RUNTIME_ASSERT (bus::get_outputs()==0x1ff,get_outs_ok);
		up_t1 = up_w1 = 0;
		bus::update_outputs();
		RUNTIME_ASSERT (up_t1==1 && up_w1==1,upd_out_ok)

		up_t1 = up_w1 = 0;
		portt1 = 0; portw1 = 0;
		bus::out(0x1ff);
		RUNTIME_ASSERT (up_t1==1 && up_w1==1, out_updates);
		RUNTIME_ASSERT (bus::get_outputs()==0x1ff,get_outs_ok);

		//narrow bus, wide port
		portw1=ddrw1=0;
		nbus_wp::config ();
		RUNTIME_ASSERT (ddrw1==0x100,ddrw1_ok)
		nbus_wp::out (0x01);
		RUNTIME_ASSERT (portw1==0x100,portw1_ok)
		RUNTIME_ASSERT (nbus_wp::get_outputs()==0x01,get_outs_ok);

		//wide bus, narrow port
		portt1=portt2=ddrt1=ddrt2=0;
		wbus_np::config ();
		RUNTIME_ASSERT (ddrt1==0xff,ddrt1_ok)
		RUNTIME_ASSERT (ddrt2==0x01,ddrt2_ok)
		wbus_np::out (0x1ff);
		RUNTIME_ASSERT (portt1==0xff,portt1_ok)
		RUNTIME_ASSERT (portt2==0x01,portt2_ok)
		RUNTIME_ASSERT (wbus_np::get_outputs()==0x1ff,get_outs_ok);
	}

	//input_bus<pin_list>, wide version
	{
		typedef input<port_w1,8,pullup> pw1_8;
		typedef input<port_t1,7,hi_z> p1_7;
		typedef input<port_t1,6,pullup> p1_6;
		typedef input<port_t1,5,hi_z> p1_5;
		typedef input<port_t1,4,pullup> p1_4;
		typedef input<port_t1,3,hi_z> p1_3;
		typedef input<port_t1,2,pullup> p1_2;
		typedef input<port_t1,1,hi_z> p1_1;
		typedef input<port_t1,0,pullup> p1_0;
		typedef input<port_t2,0,pullup> p2_0;
		typedef TLIST9(pw1_8,p1_0,p1_1,p1_2,p1_3,p1_4,p1_5,p1_6,p1_7) pinlist;
		typedef TLIST9(p1_7,p1_6,p1_5,p1_4,p1_3,p1_2,p1_1,p1_0,p2_0) wpinlist_np;
		typedef TLIST1(pw1_8) npinlist_wp;
		typedef input_bus<pinlist> bus;
		typedef input_bus<wpinlist_np> wbus_np;
		typedef input_bus<npinlist_wp> nbus_wp;
		enum { lug = length<bus::pins_by_bulk>::result };
		enum { log = length<bus::pins_by_bulk_and_shift>::result };
		COMPILE_ASSERT(lug==2,two_ports_two_updates);
		COMPILE_ASSERT(log==9,and_9_outs);
		ddrw1 = 0x100; ddrt1 = 0xff;
		portw1 = 0x00; portt1 = 0xaa;
		bus::config ();
		RUNTIME_ASSERT (portw1==0x100,portw1_ok)
		RUNTIME_ASSERT (portt1==0x55,portt1_ok)
		RUNTIME_ASSERT (ddrw1==0x00,ddrw1_ok)
		RUNTIME_ASSERT (ddrt1==0x00,ddrt1_ok)
		pinw1 = 0x100;
		pint1 = 0xff;
		up_it1 = up_iw1 = 0;
		RUNTIME_ASSERT (bus::just_in()==0x1ff, in_ok)
		RUNTIME_ASSERT (up_it1==0 && up_iw1==0,upd_in_ok)
		up_it1 = up_iw1 = 0;
		RUNTIME_ASSERT (bus::in()==0x1ff, in_ok)
		RUNTIME_ASSERT (up_it1==1 && up_iw1==1,in_updates)

		pinw1 = ~0x100;
		pint1 = static_cast<unsigned char> (~0xff);
		RUNTIME_ASSERT (bus::in()==0x00, in_ok)
		up_it1 = up_iw1 = 0;
		bus::update_inputs();
		RUNTIME_ASSERT (up_it1==1 && up_iw1==1,upd_in_ok)


		//narrow bus, wide port
		portw1=0x00; ddrw1=0x100;
		nbus_wp::config ();
		RUNTIME_ASSERT (ddrw1==0x00,ddrw1_ok)
		RUNTIME_ASSERT (portw1==0x100,portw1_ok)
		pinw1 = 0x100;
		RUNTIME_ASSERT (nbus_wp::in ()==0x01,pinw1_ok)
		pinw1 = ~0x100;
		RUNTIME_ASSERT (nbus_wp::in ()==0x00,pinw1_ok)

		//wide bus, narrow port
		portt1 = 0xaa;
		portt2 = 0x00;
		ddrt1 = ddrt2 = 0xff;
		wbus_np::config ();
		RUNTIME_ASSERT (ddrt1==0x00,ddrt1_ok)
		RUNTIME_ASSERT (ddrt2==0xfe,ddrt2_ok)
		RUNTIME_ASSERT (portt1==0x55,portt1_ok)
		RUNTIME_ASSERT (portt2==0x01,portt2_ok)
		pint1 = 0xff;
		pint2 = 0x01;
		RUNTIME_ASSERT (wbus_np::in()==0x1ff, in_ok)
		pint1 = 0x00;
		pint2 = 0xfe;
		RUNTIME_ASSERT (wbus_np::in()==0x00, in_ok)
	}

}
/*
template <int i>
struct Number {
	enum {result = i};
};

template <int n>
struct SourceList {
	typedef typename 
		::avrpp::typelist::append <
				typename SourceList<n-1>::result,Number<n> >::result result;
};

template <>
struct SourceList<1> {
	typedef ::avrpp::typelist::typelist<Number<1> > result;
};

template <int n, typename cls> struct div_filter {
	enum {result=((cls::result % n || cls::result < 3 || n == 1) ? 1:0)};
};


template <typename num_list>
struct Prime;

template <typename head, typename tail>
struct Prime < ::avrpp::typelist::typelist<head,tail> > {
	template <typename cls> struct temp : div_filter<head::result,cls> {};
	typedef typename Prime< typename ::avrpp::typelist::filter<tail, temp >::result >::result new_tail;
	typedef ::avrpp::typelist::typelist <head, new_tail> result;
};

template <>
struct Prime < ::avrpp::typelist::null_list> {
	typedef ::avrpp::typelist::null_list result;
};

void test_prime () {
	using namespace ::avrpp::typelist;

	COMPILE_ASSERT (length<SourceList<5>::result>::result==5,res_prime);

	COMPILE_ASSERT (length< Prime<SourceList<10>::result>::result>::result==5,res_prime);
	COMPILE_ASSERT (length< Prime<SourceList<15>::result>::result>::result==7,res_prime);

	typedef get_by_index < Prime<SourceList<15>::result >::result, 0>::result i0;
	SAME_TYPE_ASSERT (i0, Number<1>,res_prime);
	typedef get_by_index < Prime<SourceList<15>::result >::result, 1>::result i1;
	SAME_TYPE_ASSERT (i1, Number<2>,res_prime);
	typedef get_by_index < Prime<SourceList<15>::result >::result, 2>::result i2;
	SAME_TYPE_ASSERT (i2, Number<3>,res_prime);
	typedef get_by_index < Prime<SourceList<15>::result >::result, 3>::result i3;
	SAME_TYPE_ASSERT (i3, Number<5>,res_prime);
	typedef get_by_index < Prime<SourceList<15>::result >::result, 4>::result i4;
	SAME_TYPE_ASSERT (i4, Number<7>,res_prime);
	typedef get_by_index < Prime<SourceList<15>::result >::result, 5>::result i5;
	SAME_TYPE_ASSERT (i5, Number<11>,res_prime);
	typedef get_by_index < Prime<SourceList<15>::result >::result, 6>::result i6;
	SAME_TYPE_ASSERT (i6, Number<13>,res_prime);
}
*/
}

#include <avrpp/delay.hpp>

int main () {
	//avrpp_tests::test_prime ();
	avrpp_tests::test_typelists ();
	avrpp_tests::test_datatypes ();
#ifdef AVRPP_KTEST //No test on target platform: limited flash
	avrpp_tests::test_ports_hpp ();
#endif
	avrpp_tests::test_bus_hpp ();
	RUNTIME_ASSERT(1,asdfsd)
	::avrpp::delay::ns<1000>::delay< ::avrpp::delay::default_delay_policy> ();
#ifndef AVRPP_KTEST
#ifdef AUTOTEST
	EXIT_PROGRAM ();
#else
	while(1)
		avrpp_tests::foreach_counter++;
#endif
#endif
	return 0;
}
