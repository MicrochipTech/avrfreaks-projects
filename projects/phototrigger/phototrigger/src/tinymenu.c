/*
 * tinymenu.c -- routines for implementing a uC menu interface
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


#include <inttypes.h>

#include "lcd.h"
#include "tinymenu.h"
#include "tinymenu_hw.h"


/*
 * Execute function for currently selected menu entry (or if it's a submenu,
 *  enter the submenu)
 *
 */

void menu_select(menu_context_t *context)
{
	menu_entry_t *entry;

	entry = &context->menu->entry[context->menu->current_entry];

	// reset position if appropriate flag is set (kind of a hack)
	if(entry->flags & MENU_FLAG_FORGETPOS)
	{
		context->menu->current_entry = 0;
		context->menu->top_entry = 0;
	}

	if (entry->flags & MENU_FLAG_SUBMENU) {
		// Submenu -- enter it

		menu_enter(context, (menu_t *)entry->value);
	} else if (entry->flags & MENU_FLAG_EXITMENU) {
		// "go back entry" -- exit menu!

		menu_exit(context);
		return; // exit, since last func already displayed menu
	} else {
		// Regular entry -- execute function

		entry->select(entry->value, entry->name);
	}

	// Re-display menu on return

	menu_display(context);
}


/*
 * Print an entry in a menu on display; selected specifies whether it's the
 *  currently highlighted entry (in which case it should be made inverse, or
 *  have an asterisk prepended to let user know it's "highlighted")
 */

static void menu_print_entry(menu_entry_t *entry,
                             uint8_t max_width,
                             uint8_t selected)
{
	uint8_t i;


#ifdef CONFIG_TINYMENU_HAS_INVERSE
	if (selected)
		menu_set_inverse();
#else
	// No inverse; do workaround

	max_width--;
	if (selected) {
		menu_putchar(LCDCHARARRHORIZ);
	} else {
		menu_putchar(' ');
	}
#endif

	// Print the characters in the name; fill out to width with
	//  spaces (mainly for inverse)

	for (i = 0; i < max_width; i++) {
		if (!entry->name[i])
			break;

		menu_putchar(entry->name[i]);
	}

	for (; i < max_width; i++)
		menu_putchar(' ');

#ifdef CONFIG_TINYMENU_HAS_INVERSE
	// Restore non-inverse printing

	menu_set_normal();
#endif
}


/*
 * Display the current menu in the context
 */

void menu_display(menu_context_t *context)
{
	uint8_t i;
	menu_t *menu = context->menu;
	menu_entry_t *disp_entry;
	uint8_t dindex = 0;

#ifndef CONFIG_TINYMENU_USE_CLEAR
	uint8_t j;
#else

	menu_clear();
#endif

	// Display only those entries that will fit on the display

	for (i = 0; i < context->height; i++) {

#ifndef CONFIG_TINYMENU_COMPACT
		// Don't display hidden menu entries

		do {
			disp_entry = &menu->entry[menu->top_entry + dindex];
			if (dindex++ >= menu->num_entries - menu->top_entry)
				goto entries_done;
		} while (disp_entry->flags & MENU_FLAG_HIDDEN);

#else
		disp_entry = &menu->entry[menu->top_entry + dindex];
		if (dindex++ >= menu->num_entries - menu->top_entry)
			return;
#endif

		// Go to correct x,y locations and print the entry

		menu_set_pos(context->x_loc, context->y_loc + i);
		menu_print_entry(disp_entry, context->width,
		  (menu->current_entry + 1 == dindex + menu->top_entry));
	}

entries_done:

#ifndef CONFIG_TINYMENU_USE_CLEAR
	// Fill rest of menu screen space with spaces

	for (; i < context->height; i++) {
		menu_set_pos(context->x_loc, context->y_loc + i);
		for (j = 0; j < context->width; j++) {
			menu_putchar(' ');
		}
	}
#endif
}


/*
 * Move down currently highlighted to next entry, without going out of bounds.
 *  Also adjust current top entry in display if needed to fit new entry
 *  on display
 */

void menu_next_entry(menu_context_t *context)
{
	menu_t *menu = context->menu;
	uint8_t new_entry = menu->current_entry;


#ifndef CONFIG_TINYMENU_COMPACT
	while(1) {
		if (++new_entry >= menu->num_entries) // watch bounds
			return;
		if (!(menu->entry[new_entry].flags & MENU_FLAG_HIDDEN))
			break;
	}
#else
	if (++new_entry >= menu->num_entries)
		return;
#endif

	menu->current_entry = new_entry;

	if (menu->current_entry >= menu->top_entry + context->height)
		menu->top_entry = menu->current_entry - context->height + 1;

	menu_display(context);
}


/*
 * Move up currently highlighted to previous entry, without going out of
 *  bounds.  Also adjust current top entry in display if needed to fit new
 *  entry on display.
 */

void menu_prev_entry(menu_context_t *context)
{
	menu_t *menu = context->menu;
	uint8_t new_entry = menu->current_entry;


#ifndef CONFIG_TINYMENU_COMPACT
	while(1) {
		if (new_entry-- == 0) // Watch bounds
			return;

		if (!(menu->entry[new_entry].flags & MENU_FLAG_HIDDEN))
			break;
	}
#else
	if (new_entry-- == 0)
		return;
#endif

	menu->current_entry = new_entry;

	if (menu->current_entry < menu->top_entry)
		menu->top_entry = menu->current_entry;

	menu_display(context);
}


/*
 * Exit a menu (go to the previous menu) -- if there is no previous
 *  menu, don't do anything.
 */

void menu_exit(menu_context_t *context)
{
	if (context->menu->previous) {
		context->menu = context->menu->previous;
		menu_display(context);
	}
}


/*
 * Enter a menu -- save current menu in the menu's previous pointer
 *  so when we exit we can go back, and update the menu context to
 *  reflect new menu... then display it.
 */

void menu_enter(menu_context_t *context, menu_t *menu)
{
	menu->previous = context->menu;
	context->menu = menu;
	menu_display(context);
}
