#include <inttypes.h>
#include <string.h>


#include "tinymenu.h"
#include "tinymenu_hw.h"

void menu_select(menu_context_t *context)
{
	menu_entry_t *entry = &context->menu->entry[context->menu->current_entry];

	if (entry->flags & MENU_FLAG_SUBMENU) {
		menu_enter(context, (menu_t *)entry->value);
	} else {
		entry->select(entry->value, entry->name);
	}
	
	menu_display(context);
}


static void menu_print_entry(menu_entry_t *entry, uint8_t max_width, uint8_t selected)
{
	uint8_t i;
	

#ifdef MENU_HAS_INVERSE
	if (selected)
		menu_set_inverse();
#else
	max_width--;
	if (selected) {
		menu_putchar('*');
	} else {
		menu_putchar(' ');
	}
#endif		
	
	for (i = 0; i < max_width; i++) {
		if (!entry->name[i])
			break;
			
		menu_putchar(entry->name[i]);
	}
	
	for (; i < max_width; i++)
		menu_putchar(' ');
		
#ifdef MENU_HAS_INVERSE
	menu_set_normal();
#endif
}

void menu_display(menu_context_t *context)
{
	uint8_t i;
	menu_t *menu = context->menu;
	menu_entry_t *disp_entry;
	uint8_t dindex = 0;


	menu_clear();

	for (i = 0; i < context->height; i++) {
		do {
			disp_entry = &menu->entry[menu->top_entry + dindex];
			if (dindex++ >= menu->num_entries - menu->top_entry)
				return;
		} while (disp_entry->flags & MENU_FLAG_HIDDEN);
		
		menu_set_xy(context->x_loc, context->y_loc + i);
		menu_print_entry(disp_entry, context->width, (menu->current_entry == dindex - 1));
	}
}


void menu_next_entry(menu_context_t *context)
{
	menu_t *menu = context->menu;
	uint8_t new_entry = menu->current_entry;
	

	while(1) {
		if (++new_entry >= menu->num_entries)
			return;
		if (!(menu->entry[new_entry].flags & MENU_FLAG_HIDDEN))
			break;
	}
	
	menu->current_entry = new_entry;
	
	if (menu->current_entry >= menu->top_entry + context->height)
		menu->top_entry = menu->current_entry - context->height;
	
	menu_display(context);
}


void menu_prev_entry(menu_context_t *context)
{
	menu_t *menu = context->menu;
	uint8_t new_entry = menu->current_entry;
	

	while(1) {
		if (new_entry-- == 0)
			return;
			
		if (!(menu->entry[new_entry].flags & MENU_FLAG_HIDDEN))
			break;
	}
	
	menu->current_entry = new_entry;
	
	if (menu->current_entry < menu->top_entry) 
		menu->top_entry = menu->current_entry;
		
	menu_display(context);
}


void menu_exit(menu_context_t *context)
{
	if (context->menu->previous)
		context->menu = context->menu->previous;
		
	menu_display(context);
}


void menu_enter(menu_context_t *context, menu_t *menu)
{
	menu->previous = context->menu;
	context->menu = menu;
	menu_display(context);
}

