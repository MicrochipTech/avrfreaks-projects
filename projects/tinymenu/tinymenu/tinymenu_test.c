#include <inttypes.h>
#include <stdlib.h>
#include <string.h>

#include "spin_delay.h"
#include "lcd_44780.h"
#include "tinymenu.h"
#include "joystick.h"

void my_select(void *arg, char *name);

menu_t sub1_menu = {
	.top_entry = 0,
	.current_entry = 0,
	.entry =  {
		{.flags = 0,
		 .select = my_select,
		 .name = "select 2",
		 .value = 0,
		},
		{.flags = 0,
		 .select = my_select,
		 .name = "select 3",
		 .value = 0,
		},
		{.flags = 0,
		 .select = my_select,
		 .name = "select 4",
		 .value = 0,
		},
	},
	.num_entries = 3,
	.previous = NULL,
};

menu_t sub2_menu = {
	.top_entry = 0,
	.current_entry = 0,
	.entry = {
		{.flags = 0,
		 .select = my_select,
		 .name = "select 5",
		 .value = 0,
		},
		{.flags = 0,
		 .select = my_select,
		 .name = "select 6",
		 .value = 0,
		},
		{.flags = 0,
		 .select = my_select,
		 .name = "select 7",
		 .value = 0,
		}
	},
	.num_entries = 3,
	.previous = NULL,
};

menu_t main_menu = {
	.top_entry = 0,
	.current_entry = 0,
	.entry = {
		{.flags = MENU_FLAG_SUBMENU,
		 .select = NULL,
		 .name = "submenu 1",
		 .value = 0,
		},
		{.flags = MENU_FLAG_SUBMENU,
		 .select = NULL,
		 .name = "submenu 2",
		 .value = 0,
		},
		{.flags = 0,
		 .select = my_select,
		 .name = "select 1",
		 .value = 0,
		}
	},
	.num_entries = 3,
	.previous = NULL,
};

menu_context_t menu_context = {
	.x_loc = 0,
	.y_loc = 0,
	.height = 2,
	.width = 14,
};

void my_select(void *arg, char *name)
{
	lcd_set_pos(0,20);
	lcd_puts(name);
}

int main()
{
	uint8_t j;

	
	joystick_init();
	
	menu_enter(&menu_context, NULL);
	
	while (1) {
		ms_spin(50);
		j = joystick_read();
	
		switch(j) {
			case JOYSTICK_UP:
				lcd_clear();
				menu_next_entry(&menu_context);
				break;
			case JOYSTICK_DOWN:
				menu_prev_entry(&menu_context);
				break;
			case JOYSTICK_LEFT:
				menu_exit(&menu_context);
				break;
			case JOYSTICK_RIGHT:
			case JOYSTICK_BUTTON:
				menu_select(&menu_context);
				break;
		}
	}
}
