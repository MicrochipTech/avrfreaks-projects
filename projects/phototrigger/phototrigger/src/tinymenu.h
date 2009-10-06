/*
 * tinymenu.h -- header file for routines for implementing a uC menu interface
 *
 *  (c) 2004 Tymm Twillman (tymm@booyaka.com)
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef __TINYMENU_H__
#define __TINYMENU_H__


#define MENU_FLAG_SUBMENU   1  // This entry calls a submenu
#ifndef CONFIG_TINYMENU_COMPACT
# define MENU_FLAG_HIDDEN   2  // don't display this entry
#endif
#define MENU_FLAG_EXITMENU  4  // exit current menu
#define MENU_FLAG_FORGETPOS 8  // reset cursor position after coming back to this menu


#define MENU_ENTRY_NAMELEN 16 // Max size of a menu entry's name

typedef struct menu_entry_s {
	uint8_t flags;                            // see flag definitions above
	void (*select)(void *arg, char *name);    // routine to call when selected
	char name[MENU_ENTRY_NAMELEN];            // name to display for this entry
	void *value;                              // value to pass to select function
} menu_entry_t;

/* Information on a specific menu.  Previous is for the menu
 *  that this is a submenu of, or NULL if this is the main menu
 */

typedef struct menu_s {
	uint8_t top_entry;        // top displayed entry
	uint8_t current_entry;    // currently highlighted entry
	uint8_t num_entries;      // total # of entries in menu
	struct menu_s *previous;  // previous menu (for backtracking)
	menu_entry_t entry[];
} menu_t;


/* Used for passing around information about a set of menus */

typedef struct {
	uint8_t x_loc;   // X location of menu
	uint8_t y_loc;   // y location of menu
	uint8_t height;  // max height of menu
	uint8_t width;   // max width of menu
	menu_t *menu;    // menu currently in use
} menu_context_t;

/* Select the currently highlighted entry in the menu (call it's select func) */

void menu_select(menu_context_t *context);

/* Display the current menu in context */

void menu_display(menu_context_t *context);

/* Move highlighting to previous entry in menu */

void menu_prev_entry(menu_context_t *context);

/* Move highlighting to next entry in menu */

void menu_next_entry(menu_context_t *context);

/* Go to the previous menu, if there is one (otherwise do nothing) */

void menu_exit(menu_context_t *context);

/* Enter a new menu (set up the menu in the context and display it) */

void menu_enter(menu_context_t *context, menu_t *menu);

#endif // #ifndef __TINYMENU_H__

