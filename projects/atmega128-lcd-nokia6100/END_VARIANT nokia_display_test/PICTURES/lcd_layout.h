/* ***********************************************************************
**
**  Copyright (C) 2004 	Romuald Bialy (MIS) <romek_b@o2.pl>.
**
**
**  Yampp-7/USB - LCD display layout configuration file
**
**  File lcd_layout.h - Default English skin
**
*************************************************************************
**
**   This file is part of the yampp system by
**   Jesper Hansen <jesperh@telia.com>
**
**  This program is free software; you can redistribute it and/or
**  modify it under the terms of the GNU General Public License
**  as published by the Free Software Foundation; either version 2
**  of the License, or (at your option) any later version.
**
**  This program is distributed in the hope that it will be useful,
**  but WITHOUT ANY WARRANTY; without even the implied warranty of
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**  GNU General Public License for more details.
**
**  You should have received a copy of the GNU General Public License
**  along with this program; if not, write to the Free Software Foundation, 
**  Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
**
************************************************************************* */

// OBJECTS POSITIONS

#define ICONS_YPOS		109

#define PLAYLIST_NAME_CENTERED		// comment this if you don't need
					// autocentering of playlist name

#define PLAYLIST_NAME_YPOS	1
#define ARTIST_NAME_YPOS	33
#define SONG_NAME_YPOS		47

#define PLIST_NUMBER_XPOS	4
#define PLIST_NUMBER_YPOS	72

#define SONG_NUMBER_XPOS	68
#define SONG_NUMBER_YPOS	72

#define SONG_TIME_XPOS		76
#define SONG_TIME_YPOS		90

#define SOFT_VER_INFO_XPOS	8
#define SOFT_VER_INFO_YPOS	85

#define BATTCHARGE_TEXT_XPOS	8
#define BATTCHARGE_TEXT_YPOS	85

#define LINK_ACTIV_TEXT_XPOS	8
#define LINK_ACTIV_TEXT_YPOS	85

#define MENUITEMS_YOFFSET	18	// Y offset of Menu And Songs list

#define PROGRESS_BAR_XPOS	4
#define PROGRESS_BAR_YPOS	90
#define PROGRESS_BAR_LEN	68

#define POWER_OFF_BAR_XPOS	5
#define POWER_OFF_BAR_YPOS	105
#define POWER_OFF_BAR_LEN	114

#define CONTRAST_BAR_XPOS	15
#define CONTRAST_BAR_YPOS	106

//-------------------------------------------------------------------
// STRINGS - You may edit it if you need texsts in other language

// Menu strings
u08 fn0[] __attribute__ ((progmem)) = "Find Song";
u08 fn1[] __attribute__ ((progmem)) = "Next Playlist";
u08 fn2[] __attribute__ ((progmem)) = "Prev Playlist";
u08 fn3[] __attribute__ ((progmem)) = "Balance";
u08 fn4[] __attribute__ ((progmem)) = "Loudness";
u08 fn5[] __attribute__ ((progmem)) = "Random";
u08 fn6[] __attribute__ ((progmem)) = "Repeat Mode";
u08 fn7[] __attribute__ ((progmem)) = "Time Mode";
u08 fn8[] __attribute__ ((progmem)) = "LCD Contrast";
u08 fn9[] __attribute__ ((progmem)) = "Autolock Keys";
u08 fn10[] __attribute__ ((progmem))= "BacklightMode";

// Backlight modes
u08 Blight0[] __attribute__ ((progmem)) = "Always OFF";
u08 Blight1[] __attribute__ ((progmem)) = "Auto OFF/DIM";
u08 Blight2[] __attribute__ ((progmem)) = "Auto OFF/ON";
u08 Blight3[] __attribute__ ((progmem)) = "Always DIM";
u08 Blight4[] __attribute__ ((progmem)) = "Auto DIM/ON";
u08 Blight5[] __attribute__ ((progmem)) = "Always ON";

// Time mode strings
u08 TimRem_txt[] __attribute__ ((progmem)) = "REMAIN";
u08 TimNor_txt[] __attribute__ ((progmem)) = "NORMAL";

// Other miscleanous strings
u08 Pause_txt[] __attribute__ ((progmem))   = "Paused";
u08 PClink_txt[] __attribute__ ((progmem))  = "PC Link Active";
u08 Lobatt_txt[] __attribute__ ((progmem))  = " Low Battery ! ";
u08 Charge_txt[] __attribute__ ((progmem))  = "Battery charge";
u08 Format_txt[] __attribute__ ((progmem))  = "Unknown Card,\nPress STOP for\nformat to YADL\n";
u08 FormOK_txt[] __attribute__ ((progmem))  = "Formatting OK.";
u08 FormER_txt[] __attribute__ ((progmem))  = "Format ERROR !";
u08 FirmUpd_txt[] __attribute__ ((progmem)) = "\nFirmware version\ninfo updated!\n";
u08 BaseErr_txt[] __attribute__ ((progmem)) = "SongBase Error!";
u08 CardInit_txt[] __attribute__ ((progmem))= "Init Card...";



//-------------------------------------------------------------------
// OBJECTS SEQUENCE DEFINITIONS, DON'T CHANGE THIS

//At playing screen (bmp1)
#define PLAYLIST_NAME		0
#define ARTIST_NAME		1
#define SONG_NAME		2
#define PL_SO_NUMBERS		3
#define SONG_TIME		4
#define LOBAT_ALERT		5
#define PAUSED_TEXT1		6
#define PAUSED_TEXT2		7
#define BALANCE_INDICATOR	8

//At stop screen (bmp0)
#define SOFT_VER_INFO		9
#define BATTCHARGE_TEXT		10
#define PCLINK_TEXT		11
#define LOBATT_TEXT		12

//At menu and song browse screens (bmp2, bmp3)
#define MENU_WINDOW		13
#define BROWSE_WINDOW		14
#define BL_TI_MODES		15

//Bars
#define PROGRESS_BAR		32
#define VOLUME_BAR		34
#define POWER_OFF_BAR		36
#define CONTRAST_BAR		38
