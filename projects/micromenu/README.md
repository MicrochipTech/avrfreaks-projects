# MicroMenu

Uploaded by abcminiuser on 2007-08-07 05:41:00 (rating 0 out of 5)

## Summary

 MICRO-MENU


 (C) Dean Camera, 2007  

[www.fourwalledcubicle.com](http://www.fourwalledcubicle.com)


 Royalty-free for all uses.


MicroMenu provides an easy-to-use hierarchical menu interface for AVR firmware written in the C language. It was written as a way to teach myself linked-lists, and I've cleaned it up and made it freely available for both commercial and non-commercial uses. It is designed to be contained in a single user-made menu loop, with helper functions for each menu item (if desired).


Menus are stored completely in flash memory, saving SRAM.


#### SETUP: ####


 MicroMenu is designed to interface with your existing string output routines. This routine should be of type (void)(const char*), and should use PROGMEM strings. If your string routine uses SRAM strings instead, you can switch MicroMenu over to SRAM buffered mode via defining MENU\_USE\_SRAM\_BUFFER at the top of the Menu.c file (alternatively in your makefile, passed to Menu.c via the -D switch).


 If used, MENU\_USE\_SRAM\_BUFFER should be defined to the maximum string size. If a menu string exceeds this, a stack overflow will occur. Please ensure that MENU\_USE\_SRAM\_BUFFER is of sufficient length - including the string's trailing null terminator - if used.


#### DEFINING MENUS: ####


 To create a menu, use the MAKE\_MENU macro. This takes as arguments the following: 


 Name, Next, Previous, Parent, Sibling, SelectFunc, EnterFunc, Text


 Name: Name of the menu. This is used when linking one menu to another, or to select a menu outright with the  

 SET\_MENU macro.


 Next: Name of the next menu item in the current menu list. If no menu exists, use the NULL\_ENTRY constant.


 Previous: Name of the previous menu item in the current list. If no menu exists, use the NULL\_ENTRY constant.


 Parent: Name of the current menu item's parent menu. If no menu exists, use the NULL\_ENTRY constant.


 Sibling: Name of the current menu item's sibling menu. If no menu exists, use the NULL\_ENTRY constant.


 SelectFunc: Name of the function to execute when the menu item is selected. If no function exists, use the  

 NULL\_FUNC constant.


 EnterFunc: Name of the function to execute when the menu item is shown. If no function exists, use the NULL\_FUNC  

 constant. This is executed AFTER the menu item's text string (if any) is shown, allowing menu string  

 modifications, or dynamic menu strings.


 Text: Text string to show when the menu is displayed. This is static; if dynamic strings are required they  

 should be constructed in the menu item's EnterFunc function. If no static string is to be shown, use  

 the NULL\_TEXT constant.


 If menus are to be referenced across C files, use the EXTERN\_MENU macro. This takes as a parameter the name of the menu to be exported across C files. It is recommended that this be placed in the header file included the C file not containing the full menu definition.


#### INTERFACE: ####


 Before calling a menu, the macro SET\_MENU\_WRITE\_FUNC should be used. This should take as a parameter the string output function name, of type (void)(const char*).


 To enter a menu, use the SET\_MENU macro. This takes as a parameter, either a menu name OR a transversal of the current menu function (PREVIOUS, NEXT, PARENT or SIBLING).


 To execute the current menu's function, use the GO\_MENU\_FUNC macro. This takes as a parameter the desired current menu function, either ENTERFUNC or SELECTFUNC.


>> Updated on the 7th of August 2007 to correct a minor typo.

## Compilers

- WinAVR (GNU GCC)

## Tags

- General Lib.functions
- WinAVR (GNU GCC)
