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

#ifndef AVRPP_TYPELIST_H___
#define AVRPP_TYPELIST_H___

namespace avrpp {

/** @brief A namespace containing operations on type lists.
 *
 *  Basic types and a lot of classes to operate the lists.
 */
namespace typelist {

/** @brief A type containing an integer value. */
template <int i> struct integer {
	enum {
		/** @brief the integer value */
		result=i
	};
};

/** @brief Compares two classes and defines nested constant 'result'
 *
 * @return result is zero if classes are equal
 */
template <typename a, typename b> struct compare_classes {
	/** @brief The result */
	enum {result=1};
};
/** @brief Compares two classes and defines nested constant 'result'
 *
 * @return <b>result</b> is zero if classes are equal
 */
template <typename a> struct compare_classes <a,a> {
	/** @brief The result */
	enum {result=0};
};

/** @brief Used as an end marker for type lists */
struct null_list {
	typedef null_list head;
};

/** @brief A type list
 * 
 * Each type list has a head class and a tail that is a type list itself.
 */
template <typename head_class, typename tail_list=null_list>
struct typelist {
	/** @brief Head class */
	typedef head_class head;
	/** @brief Tail of a list, a list itself */
	typedef tail_list tail;
};

/** @brief Appends a class to a list */
template <typename class_list,typename some_class> struct append;

/** @brief Appends a class to a list.
 * 
 * @param class_list defines the class list
 * @param some_class defines the class
 * @return <b>result</b> is a nested type for the result type list
 */
template <typename head, typename tail, typename some_class>
struct append <typelist<head,tail>,some_class> {
	/** @brief The result */
	typedef typelist<head,typename append<tail,some_class>::result> result;
};

/** @brief Appends a class to a list.
 * 
 * @param class_list defines the class list
 * @param some_class defines the class
 * @return <b>result</b> is a nested type for the result type list
 */
template <typename some_class>
struct append <null_list,some_class> {
	/** @brief The result */
	typedef typelist<some_class,null_list> result;
};

template <typename list1, typename list2> struct join;

/** @brief Joins two type lists
 * 
 * @param list1 The first list
 * @param list2 The second list
 * @return <b>result</b> is a nested type for the result type list
 */
template <typename head1,typename tail1, typename list2>
struct join<typelist<head1,tail1>,list2> {
	/** @brief The result */
	typedef typelist<head1,typename join<tail1,list2>::result> result;
};
/** @brief Joins two type lists
 * 
 * @param list1 The first list
 * @param list2 The second list
 * @return <b>result</b> is a nested type for the result type list
 */
template <> struct join <null_list,null_list> {
	/** @brief The result */
	typedef null_list result;
};
/** @brief Joins two type lists
 * 
 * @param list1 The first list
 * @param list2 The second list
 * @return <b>result</b> is a nested type for the result type list
 */
template <typename list2> struct join <null_list,list2> {
	/** @brief The result */
	typedef list2 result;
};


template <typename class_list, typename some_class,int head_length=0>
struct get_index;

/** @brief Gets a type from a list by index (0-...)
 * 
 * @param list The list
 * @param index
 * @return <b>result</b> is a nested type for the result type
 */
template <typename head, typename tail, typename some_class,int head_length>
struct get_index <typelist<head,tail>,some_class,head_length> {
	/** @brief The result */
	enum {result = get_index<tail, some_class, head_length+1>::result};
};

/** @brief Gets index of a type
 * 
 * @param list is a list
 * @param some_class is a class to find in the list
 * @return <b>result</b> is a nested constant containing the result (-1 if not found)
 */
template <typename tail, typename some_class,int head_length>
struct get_index <typelist<some_class,tail>,some_class,head_length> {
	/** @brief The result */
	enum {result = head_length};
};

/** @brief Gets index of a type
 * 
 * @param list is a list
 * @param some_class is a class to find in the list
 * @return <b>result</b> is a nested constant containing the result (-1 if not found)
 */
template <typename some_class, int head_length>
struct get_index <null_list, some_class, head_length> {
	/** @brief The result */
	enum {result=-1};
};

/** @brief Gets index of a type
 * 
 * @param list is a list
 * @param some_class is a class to find in the list
 * @return <b>result</b> is a nested constant containing the result (-1 if not found)
 */
template <int head_length>
struct get_index <null_list, null_list, head_length> {
	/** @brief The result */
	enum {result=head_length};
};


/** @brief Gets length of a type list
 * 
 * @param class_list is a type list
 * @return <b>result</b> is its length
 */
template <typename class_list>
struct length {
	/** @brief The result */
	enum {result=get_index<class_list,null_list>::result};
};

template <typename class_list,int index>
struct get_by_index;

/** @brief Gets a type from a list by index (0-...)
 * 
 * @param list The list
 * @param index
 * @return <b>result</b> is a nested type for the result type
 */
template <typename head, typename tail, int index>
struct get_by_index <typelist<head,tail>, index> {
	/** @brief The result */
	typedef typename get_by_index<tail,index-1>::result result;
};

/** @brief Gets a type from a list by index (0-...)
 * 
 * @param list The list
 * @param index
 * @return <b>result</b> is a nested type for the result type
 */
template <typename head, typename tail>
struct get_by_index <typelist<head,tail>, 0> {
	/** @brief The result */
	typedef head result;
};

/** @brief Gets a type from a list by index (0-...)
 * 
 * @param list the list
 * @param index
 * @return <b>result</b> is a nested type for the result type
 */
template <int index>
struct get_by_index <null_list, index> {
	/** @brief The result */
	typedef null_list result;
};

/** @brief Removes all instances of a type from a type list
 * 
 * @param class_list the list
 * @param del type to be deleted
 * @return <b>result</b> is a nested type for the result type
 */
template <typename class_list, typename del>
struct erase {
	/** @brief The result */
	typedef typelist<typename class_list::head,typename erase<typename class_list::tail,del>::result> result;
};

/** @brief Removes all instances of a type from a type list
 * 
 * @param class_list the list
 * @param del type to be deleted
 * @return <b>result</b> is a nested type for the result type
 */
template <typename tail, typename del>
struct erase <typelist<del,tail>,del> {
	/** @brief The result */
	typedef typename erase<tail,del>::result result;
};

/** @brief Removes all instances of a type from a type list
 * 
 * @param class_list the list
 * @param del type to be deleted
 * @return <b>result</b> is a nested type for the result type
 */
template <typename del>
struct erase<null_list,del> {
	/** @brief The result */
	typedef null_list result;
};


/** @brief Removes all duplicate instances from a type list
 * 
 * @param class_list the list
 * @return <b>result</b> is a nested type for the result type
 */
template <typename class_list>
struct unique {
	typedef typename erase<typename class_list::tail,typename class_list::head>::result tail;
	/** @brief The result */
	typedef typelist<typename class_list::head,typename unique<tail>::result> result;
};

/** @brief Removes all duplicate instances from a type list
 * 
 * @param class_list the list
 * @return <b>result</b> is a nested type for the result type
 */
template <>
struct unique<null_list> {
	/** @brief The result */
	typedef null_list result;
};

/** @brief Instantiates a template with every type from a list and calls a static method of each instance
 * 
 * @param class_list the list
 * @param processor a template
 * @return <b>result</b> is a nested type for the result type
 */
template <typename class_list,template <typename target> class processor>
struct foreach {
	/** @brief This function is called when processing a type */
	static void call () {
		processor<typename class_list::head>::call();
		foreach <typename class_list::tail, processor>::call ();
	}
	/** @brief This function is called when processing a type */
	template <typename pt> static void call (pt p) {
		processor<typename class_list::head>::call(p);
		foreach <typename class_list::tail, processor>::call (p);
	}
	/** @brief This function is called when processing a type */
	template <typename pt_1, typename pt_2> static void call (pt_1 p1, pt_2 p2) {
		processor<typename class_list::head>::call(p1, p2);
		foreach <typename class_list::tail, processor>::call (p1, p2);
	}
};

/** @brief Instantiates a template with every type from a list and calls a static method of each instance
 *
 * @param class_list the list
 * @param processor a template
 * @return <b>result</b> is a nested type for the result type
 */
template <template <typename target> class processor>
struct foreach<null_list, processor> {
	/** @brief This function is called when processing a type */
	static void call () {}
	/** @brief This function is called when processing a type */
	template <typename pt> static void call (pt /*p*/) {}
	/** @brief This function is called when processing a type */
	template <typename pt_1, typename pt_2> static void call (pt_1 /*p1*/, pt_2 /*p2*/) {}
};


template <typename class_list, bool decapitate> struct decapitate_if;

/** @brief Removes head from a list if decapitate is true
 * 
 * @param class_list the list
 * @param decapitate remove head from the list if true
 * @return <b>result</b> is a nested type for the result type
 */
template <typename head, typename tail>
struct decapitate_if <typelist<head,tail>,true> {
	/** @brief The result */
	typedef tail result;
};
/** @brief Removes head from a list if decapitate is true
 * 
 * @param class_list the list
 * @param decapitate remove head from the list if true
 * @return <b>result</b> is a nested type for the result type
 */
template <typename head, typename tail>
struct decapitate_if <typelist<head,tail>,false> {
	/** @brief The result */
	typedef typelist<head,tail> result;
};
/** @brief Removes head from a list if decapitate is true
 * 
 * @param class_list the list
 * @param decapitate remove head from the list if true
 * @return <b>result</b> is a nested type for the result type
 */
template <bool decapitate>
struct decapitate_if <null_list,decapitate> {
	/** @brief The result */
	typedef null_list result;
};

template <typename class_list, template <typename cls> class filter_class> struct filter;

/** @brief Filters a type list with user's filter
 * 
 * @param class_list type list to filter
 * @param filter_class a template filter parameterized by a filtered class. Should define a constant named "result", class is removed if result is 0.
 * @return <b>result</b> is a nested type for the result type
 */
template <typename head, typename tail, template <typename cls> class filter_class>
struct filter <typelist<head,tail>,filter_class> {
	/** @brief The result */
	typedef typename decapitate_if<typelist<head,typename filter<tail,filter_class>::result >,
									!filter_class<head>::result >::result result;
};

/** @brief Filters a type list with user's filter
 * 
 * @param class_list type list to filter
 * @param filter_class a template filter parameterized by a filtered class. Should define a constant named "result", class is removed if result is 0.
 * @return <b>result</b> is a nested type for the result type
 */
template <template <typename cls> class filter_class>
struct filter<null_list,filter_class> {
	/** @brief The result */
	typedef null_list result;
};

template <typename class_list, template <typename source> class transformation>
struct transform;

/** @brief Transforms every type in the list using an utility given by user
 *
 * @param class_list type list to transform
 * @param transformation is a template class parameterized by a
 * class being transformed and producing a new (transformed) class
 * @return <b>result</b> is type list of transformed classes
 */
template <typename head, typename tail, template <typename source> class transformation>
struct transform<typelist<head,tail>,transformation> {
	/** @brief The result */
	typedef typelist <typename transformation<head>::result,
				typename transform <tail,transformation>::result> result;
};

/** @brief Transforms every type in the list using an utility given by user
 *
 * @param class_list type list to transform
 * @param transformation is a template class parameterized by a
 * class being transformed and producing a new (transformed) class
 * @return <b>result</b> is type list of transformed classes
 */
template <template <typename source> class transformation>
struct transform <null_list,transformation> {
	/** @brief The result */
	typedef null_list result;
};


template <typename class_list, typename cat_list, template <typename source> class transformation>
struct split_by_categories;

/** @brief Receives a type list, transformation that convert source types into
 * their category types, and a list of category types. List of source types
 * is split into several type lists (number of lists is equal to number of categories),
 * each containing all the types belonging to a single category. Order of category
 * lists is the same as order of categories. Order of classes in a list is the same as in
 * the source list.
 *
 *
 * @param class_list type list to categorize
 * @param cat_list type list of categories
 * @param transformation is a template class parameterized by a
 * class being transformed and producing a new (transformed) class. Here
 * it should produce a category class.
 * @return <b>result</b> is type list of type lists, one for each category
 */
template <typename class_list, typename clist_head, typename clist_tail, template <typename source> class transformation>
struct split_by_categories <class_list,typelist<clist_head,clist_tail>, transformation> {
	private:
		template <typename cls> struct current_filter {
			enum {result = !compare_classes<typename transformation<cls>::result,
					clist_head>::result};
		};
		typedef typename filter <class_list, current_filter>::result head_list;
	public:
		/** @brief The result */
		typedef typelist <head_list,
			typename split_by_categories<class_list, clist_tail, transformation>::result> result;
};

/** @brief Receives a type list, transformation that convert source types into
 * their category types, and a list of category types. List of source types
 * is split into several type lists (number of lists is equal to number of categories),
 * each containing all the types belonging to a single category. Order of category
 * lists is the same as order of categories. Order of classes in a list is the same as in
 * the source list.
 *
 *
 * @param class_list type list to categorize
 * @param cat_list type list of categories
 * @param transformation is a template class parameterized by a
 * class being transformed and producing a new (transformed) class. Here
 * it should produce a category class.
 * @return <b>result</b> is type list of type lists, one for each category
 */
template <typename class_list, template <typename source> class transformation>
struct split_by_categories <class_list, null_list, transformation> {
	/** @brief The result */
	typedef null_list result;
};


template <typename class_list, template <typename source> class transformation>
struct categorize_by_transformation;

/** @brief Splits a type list into categories defined by transformation.
 *
 * Performs a type transformation (see transform<>), then creates a
 * list of lists. Every sublist contains types that are transformed
 * to same type. Classes from different sublists are always transformed
 * to different types.
 *
 * @param class_list type list to categorize
 * @param transformation is a template class parameterized by a
 * class being transformed and producing a new (transformed) class
 * @return <b>result</b> is type list of type lists, one for each category
 */
template <typename head, typename tail, template <typename source> class transformation>
struct categorize_by_transformation <typelist<head,tail>,transformation> {
	/** @brief A list of categories */
	typedef typename unique<typename transform<typelist<head,tail>,
									transformation>::result>::result categories;
	/** @brief The result */
	typedef typename split_by_categories <typelist<head,tail>, categories, transformation >::result result;
};

/** @brief Splits a type list into categories defined by transformation.
 *
 * Performs a type transformation (see transform<>), then creates a
 * list of lists. Every sublist contains types that are transformed
 * to same type. Classes from different sublists are always transformed
 * to different types.
 *
 * @param class_list type list to categorize
 * @param transformation is a template class parameterized by a
 * class being transformed and producing a new (transformed) class
 * @return <b>result</b> is type list of type lists, one for each category
 */
template <template <typename source> class transformation>
struct categorize_by_transformation <null_list,transformation> {
	/** @brief The result */
	typedef null_list result;
};

template <long int if_val, typename cls_neg, typename cls_zero, typename cls_pos> class arithmetic_if;

/** @brief Selects one of three classes depending on value sign.
 *
 * Similar to old FORTRAN arithmetic if (except for the fact it selects types
 * during compilation, not control paths).
 *
 * @param if_val a long integer value.
 * @param cls_neg is selected when if_val is negative
 * @param cls_zero is selected when if_val is zero
 * @param cls_pos is selected when if_val is positive
 * @return <b>result</b> is the selected class
 */
template <long int if_val, typename cls_neg, typename cls_zero, typename cls_pos> class arithmetic_if {
	enum { /** @brief Sign of if_val */
		val_sign = if_val < 0 ? -1 : (if_val > 0 ? 1 : 0)
	};
	/** @brief A type list for internal use */
	typedef typelist<cls_neg,typelist<cls_zero,typelist<cls_pos> > > src_list;
public:
	/** @brief The result */
	typedef typename get_by_index<src_list, val_sign + 1>::result result;
};

template <typename class_list, template <typename cls1,typename cls2> class compare_classes> class max;

/** @brief Selects a maximum class (using a comparison class).
 * In case of equal classes, prefers the first one.
 *
 * @param class_list a class list
 * @param compare_classes is a comparison class. Accepts two classes and defines a 'result' constant.
 * @return <b>result</b> is the class selected.
 */
template <typename head, typename tail, template <typename cls1,typename cls2> class compare_classes>
class max < ::avrpp::typelist::typelist<head,tail>, compare_classes > {
	/** @brief max<> for the tail */
	typedef typename max <tail, compare_classes>::result tail_max;
	enum {
		/** @brief head compared to tail_max */
		tail_cmp = compare_classes <head, tail_max>::result
	};
public:
	/** @brief The result */
	typedef typename arithmetic_if <tail_cmp, tail_max, head, head>::result result;
};

/** @brief Selects a maximum class (using a comparison class).
 * In case of equal classes, prefers the first one.
 *
 * @param class_list a class list
 * @param compare_classes is a comparison class. Accepts two classes and defines a 'result' constant.
 * @return <b>result</b> is the class selected.
 */
template <typename head, template <typename cls1,typename cls2> class compare_classes>
class max < ::avrpp::typelist::typelist<head,null_list>, compare_classes > {
public:
	/** @brief The result */
	typedef head result;
};

}

}

#define TLIST1(t1) avrpp::typelist::typelist<t1>
#define TLIST2(t1,t2) avrpp::typelist::typelist<t1,avrpp::typelist::typelist<t2> >
#define TLIST3(t1,t2,t3) avrpp::typelist::typelist<t1,avrpp::typelist::typelist<t2,avrpp::typelist::typelist<t3> > >
#define TLIST4(t1,t2,t3,t4) avrpp::typelist::typelist<t1,avrpp::typelist::typelist<t2,avrpp::typelist::typelist<t3,avrpp::typelist::typelist<t4> > > >
#define TLIST5(t1,t2,t3,t4,t5) avrpp::typelist::typelist<t1,avrpp::typelist::typelist<t2,avrpp::typelist::typelist<t3,avrpp::typelist::typelist<t4,avrpp::typelist::typelist<t5> > > > >
#define TLIST6(t1,t2,t3,t4,t5,t6) avrpp::typelist::typelist<t1,avrpp::typelist::typelist<t2,avrpp::typelist::typelist<t3,avrpp::typelist::typelist<t4,avrpp::typelist::typelist<t5,avrpp::typelist::typelist<t6> > > > > >
#define TLIST7(t1,t2,t3,t4,t5,t6,t7) avrpp::typelist::typelist<t1,avrpp::typelist::typelist<t2,avrpp::typelist::typelist<t3,avrpp::typelist::typelist<t4,avrpp::typelist::typelist<t5,avrpp::typelist::typelist<t6,avrpp::typelist::typelist<t7> > > > > > >
#define TLIST8(t1,t2,t3,t4,t5,t6,t7,t8) avrpp::typelist::typelist<t1,avrpp::typelist::typelist<t2,avrpp::typelist::typelist<t3,avrpp::typelist::typelist<t4,avrpp::typelist::typelist<t5,avrpp::typelist::typelist<t6,avrpp::typelist::typelist<t7,avrpp::typelist::typelist<t8> > > > > > > >
#define TLIST9(t1,t2,t3,t4,t5,t6,t7,t8,t9) avrpp::typelist::typelist<t1,avrpp::typelist::typelist<t2,avrpp::typelist::typelist<t3,avrpp::typelist::typelist<t4,avrpp::typelist::typelist<t5,avrpp::typelist::typelist<t6,avrpp::typelist::typelist<t7,avrpp::typelist::typelist<t8,avrpp::typelist::typelist<t9> > > > > > > > >






#endif // AVRPP_H___
