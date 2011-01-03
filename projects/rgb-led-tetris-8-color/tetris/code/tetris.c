// 16x16 pixels screen output commands
// for nerdkits with ATmega168
// hexorgstas@gmail.com

#include <avr/io.h>
#include <avr/eeprom.h>
#include <stdint.h>
#include "tetris.h"
#include "draw.h"


void CheckX(uint8_t *x, uint8_t shape, uint8_t rotation)
{
	switch (shape)
	{
		case 0: switch (rotation) 
		{ 
			case 0: case 2: if ((*x) > 10) (*x) = 10; break;
			case 1: case 3: if ((*x) > 7) (*x) = 7; break;
		} break;
		case 1: switch (rotation) 
		{ 
			case 0: case 2: if ((*x) > 8) (*x) = 8; break;	
			case 1: case 3: if ((*x) > 9) (*x) = 9; break;
		} break;
		case 2: switch (rotation) 
		{ 
			case 0: case 2: if ((*x) > 9) (*x) = 9; break;	
			case 1: case 3: if ((*x) > 8) (*x) = 8; break;
		} break;
		case 3: if ((*x) > 8) *x = 8; break;
		case 4: case 5:switch (rotation) 
		{ 
			case 0: case 2: if ((*x) > 8) (*x) = 8; break;	
			case 1: case 3: if ((*x) > 9) (*x) = 9; break;
		} break;
		case 6: switch (rotation) 
		{ 
			case 0: case 2: if ((*x) > 8) (*x) = 8; break;	
			case 1: case 3: if ((*x) > 9) (*x) = 9; break;
		} break;
	}	
}

uint8_t CanMove(uint8_t *field, uint8_t x, int8_t y, uint8_t shape, uint8_t rotation, uint8_t direction) // direction == 0: left, else - right
{
	if (direction) // moving right
	{
		switch (shape)
		{
			case 0: switch (rotation) 
			{ 
				case 0: case 2: return !(GetPixel(field, (x+1), y) || GetPixel(field, (x+1), (y+1)) || GetPixel(field, (x+1), (y+2)) || GetPixel(field, (x+1), (y+3))); break;	
				case 1: case 3: return !(GetPixel(field, (x+4), y)); break;
			} break;
			case 1: switch (rotation) 
			{ 
				case 0: return !(GetPixel(field, (x+3), y) || GetPixel(field, (x+3), (y+1))); break;	
				case 1: return !(GetPixel(field, (x+1), y) || GetPixel(field, (x+1), (y+1)) || GetPixel(field, (x+2), (y+2))); break;
				case 2: return !(GetPixel(field, (x+3), y) || GetPixel(field, (x+1), (y+1))); break;
				case 3: return !(GetPixel(field, (x+2), y) || GetPixel(field, (x+2), (y+1)) || GetPixel(field, (x+2), (y+2))); break;
			} break;
			case 2: switch (rotation) 
			{ 
				case 0: return !(GetPixel(field, (x+2), y) || GetPixel(field, (x+1), (y+1)) || GetPixel(field, (x+1), (y+2))); break;	
				case 1: return !(GetPixel(field, (x+1), y) || GetPixel(field, (x+3), (y+1))); break;
				case 2: return !(GetPixel(field, (x+2), y) || GetPixel(field, (x+2), (y+1)) || GetPixel(field, (x+2), (y+2))); break;
				case 3: return !(GetPixel(field, (x+3), y) || GetPixel(field, (x+3), (y+1))); break;	
			} break;
			case 3: return !(GetPixel(field, (x+2), y) || GetPixel(field, (x+2), (y+1))); break;
			case 4: switch (rotation) 
			{ 
				case 0: case 2: return !(GetPixel(field, (x+2), y) || GetPixel(field, (x+3), (y+1))); break;	
				case 1: case 3: return !(GetPixel(field, (x+2), y) || GetPixel(field, (x+2), (y+1)) || GetPixel(field, (x+1), (y+2))); break;
			} break;
			case 5: switch (rotation) 
			{ 
				case 0: case 2: return !(GetPixel(field, (x+3), y) || GetPixel(field, (x+2), (y+1))); break;	
				case 1: case 3: return !(GetPixel(field, (x+1), y) || GetPixel(field, (x+2), (y+1)) || GetPixel(field, (x+2), (y+2))); break;
			} break;
			case 6: switch (rotation) 
			{ 
				case 0: return !(GetPixel(field, (x+2), y) || GetPixel(field, (x+3), (y+1))); break;	
				case 1: return !(GetPixel(field, (x+1), y) || GetPixel(field, (x+2), (y+1)) || GetPixel(field, (x+1), (y+2))); break;
				case 2: return !(GetPixel(field, (x+3), y) || GetPixel(field, (x+2), (y+1))); break;
				case 3: return !(GetPixel(field, (x+2), y) || GetPixel(field, (x+2), (y+1)) || GetPixel(field, (x+2), (y+2))); break;	
			} break;
		}
	}
	else // moving left
	{
		if (x < 2) 
			return 0;
		switch (shape)
		{
			case 0: switch (rotation) 
			{ 
				case 0: case 2: return !(GetPixel(field, (x-1), y) || GetPixel(field, (x-1), (y+1)) || GetPixel(field, (x-1), (y+2)) || GetPixel(field, (x-1), (y+3))); break;	
				case 1: case 3: return !(GetPixel(field, (x-1), y)); break;
			} break;
			case 1: switch (rotation) 
			{ 
				case 0: return !(GetPixel(field, (x+1), y) || GetPixel(field, (x-1), (y+1))); break;	
				case 1: return !(GetPixel(field, (x-1), y) || GetPixel(field, (x-1), (y+1)) || GetPixel(field, (x-1), (y+2))); break;
				case 2: return !(GetPixel(field, (x-1), y) || GetPixel(field, (x-1), (y+1))); break;
				case 3: return !(GetPixel(field, (x-1), y) || GetPixel(field, x, (y+1)) || GetPixel(field, x, (y+2))); break;
			} break;
			case 2: switch (rotation) 
			{ 
				case 0: return !(GetPixel(field, (x-1), y) || GetPixel(field, (x-1), (y+1)) || GetPixel(field, (x-1), (y+2))); break;	
				case 1: return !(GetPixel(field, (x-1), y) || GetPixel(field, (x-1), (y+1))); break;
				case 2: return !(GetPixel(field, x, y) || GetPixel(field, x, (y+1)) || GetPixel(field, (x-1), (y+2))); break;
				case 3: return !(GetPixel(field, (x-1), y) || GetPixel(field, (x+1), (y+1))); break;	
			} break;
			case 3: return !(GetPixel(field, (x-1), y) || GetPixel(field, (x-1), (y+1))); break;
			case 4: switch (rotation) 
			{ 
				case 0: case 2: return !(GetPixel(field, (x-1), y) || GetPixel(field, x, (y+1))); break;	
				case 1: case 3: return !(GetPixel(field, x, y) || GetPixel(field, (x-1), (y+1)) || GetPixel(field, (x-1), (y+2))); break;
			} break;
			case 5: switch (rotation) 
			{ 
				case 0: case 2: return !(GetPixel(field, x, y) || GetPixel(field, (x-1), (y+1))); break;	
				case 1: case 3: return !(GetPixel(field, (x-1), y) || GetPixel(field, (x-1), (y+1)) || GetPixel(field, x, (y+2))); break;
			} break;
			case 6: switch (rotation) 
			{ 
				case 0: return !(GetPixel(field, x, y) || GetPixel(field, (x-1), (y+1))); break;	
				case 1: return !(GetPixel(field, (x-1), y) || GetPixel(field, (x-1), (y+1)) || GetPixel(field, (x-1), (y+2))); break;
				case 2: return !(GetPixel(field, (x-1), y) || GetPixel(field, x, (y+1))); break;
				case 3: return !(GetPixel(field, x, y) || GetPixel(field, (x-1), (y+1)) || GetPixel(field, x, (y+2))); break;	
			} break;
		}
	}
}


uint8_t ShouldPlace(uint8_t *field, uint8_t x, int8_t y, uint8_t shape, uint8_t rotation)
{
	if ((x>=1) && (x<=16) && (y<=16) && (shape <=6) && (rotation<=3))
	{
		switch (shape)
		{
			case 0: switch (rotation)
			{
				case 0: case 2:
				{
					if (GetPixel(field, x, (y+4)) || (y > 12))
						return 1;
					break;
				}
				case 1: case 3:
				{
					if (GetPixel(field, x, (y+1)) || GetPixel(field, (x+1), (y+1)) ||
						GetPixel(field, (x+2), (y+1)) || GetPixel(field, (x+3), (y+1)) ||
						(y > 15))
						return 1;
					break;
				}
				
			} break;
			case 1: switch (rotation)
			{
				case 0:
				{
					if (GetPixel(field, x, (y+2)) || GetPixel(field, (x+1), (y+2)) ||
						GetPixel(field, (x+2), (y+2)) || (y > 14))
						return 1;
					break;
				}
				case 1:
				{
					if (GetPixel(field, x, (y+3)) || GetPixel(field, (x+1), (y+3)) ||
						(y > 13))
						return 1;
					break;
				}
				case 2:
				{
					if (GetPixel(field, x, (y+2)) || GetPixel(field, (x+1), (y+1)) ||
						GetPixel(field, (x+2), (y+1)) || (y>14))
						return 1;
					break;
				}
				case 3:
				{
					if (GetPixel(field, x, (y+1)) || GetPixel(field, (x+1), (y+3)) ||
						(y>13))
						return 1;
					break;
				}
			} break;
			case 2: switch (rotation)
			{
				case 0:
				{
					if (GetPixel(field, x, (y+3)) || GetPixel(field, (x+1), (y+1)) ||
						(y > 13)) 
						return 1;
					break;
				}
				case 1:
				{
					if (GetPixel(field, x, (y+2)) || GetPixel(field, (x+1), (y+2)) ||
						GetPixel(field, (x+2), (y+2)) || (y > 14))
						return 1;
					break;
				}
				case 2:
				{
					if (GetPixel(field, x, (y+3)) || GetPixel(field, (x+1), (y+3)) ||
						(y>13))
						return 1;
					break;
				}
				case 3:
				{
					if (GetPixel(field, x, (y+1)) || GetPixel(field, (x+1), (y+1)) ||
						GetPixel(field, (x+2), (y+2)) || (y>14))
						return 1;
					break;
				}
			} break;
			case 3: 
			{
				if (GetPixel(field, x, (y+2)) || GetPixel(field, (x+1), (y+2)) ||
					(y > 14)) 
					return 1;
				break;
			}
			case 4: switch (rotation)
			{
				case 0: case 2:
				{
					if (GetPixel(field, x, (y+1)) || GetPixel(field, (x+1), (y+2)) ||
						GetPixel(field, (x+2), (y+2)) || (y > 14)) 
						return 1;
					break;
				}
				case 1: case 3:
				{
					if (GetPixel(field, x, (y+3)) || GetPixel(field, (x+1), (y+2)) ||
						(y > 13))
						return 1;
					break;
				}
			} break;
			case 5: switch (rotation)
			{
				case 0: case 2:
				{
					if (GetPixel(field, x, (y+2)) || GetPixel(field, (x+1), (y+2)) ||
						GetPixel(field, (x+2), (y+1)) || (y > 14)) 
						return 1;
					break;
				}
				case 1: case 3:
				{
					if (GetPixel(field, x, (y+2)) || GetPixel(field, (x+1), (y+3)) ||
						(y > 13))
						return 1;
					break;
				}
			} break;
			case 6: switch (rotation)
			{
				case 0:
				{
					if (GetPixel(field, x, (y+2)) || GetPixel(field, (x+1), (y+2)) ||
						GetPixel(field, (x+2), (y+2)) || (y > 14)) 
						return 1;
					break;
				}
				case 1:
				{
					if (GetPixel(field, x, (y+3)) || GetPixel(field, (x+1), (y+2)) ||
						(y > 13))
						return 1;
					break;
				}
				case 2:
				{
					if (GetPixel(field, x, (y+1)) || GetPixel(field, (x+1), (y+2)) ||
						GetPixel(field, (x+2), (y+1)) || (y>14))
						return 1;
					break;
				}
				case 3:
				{
					if (GetPixel(field, x, (y+2)) || GetPixel(field, (x+1), (y+3)) ||
						(y>13))
						return 1;
					break;
				}
			} break;
		} // end of shape switch
	} // end of sanity check
	return 0;

}


void DrawShape(uint8_t *image, uint8_t x, uint8_t y, uint8_t shape, uint8_t rotation, uint8_t color)
{
	if ((x<=16) && (y<=16))
	{
		switch (shape)
		{
			case 0: case 'I' : switch (rotation) // I
			{
				case 0: case 2:
				{
					SetPixel(image, x, y, color);
					SetPixel(image, x, (y+1), color);
					SetPixel(image, x, (y+2), color);
					SetPixel(image, x, (y+3), color);
				} break;
				case 1: case 3:
				{
					SetPixel(image, x, y, color);
					SetPixel(image, (x+1), y, color);
					SetPixel(image, (x+2), y, color);
					SetPixel(image, (x+3), y, color);
				} break;
			} break;
			case 1: case 'L' : switch (rotation) // L 
			{
				case 0:
			 	{
					SetPixel(image, x, (y+1), color);
					SetPixel(image, (x+1), (y+1), color);
					SetPixel(image, (x+2), (y+1), color);
					SetPixel(image, (x+2), y, color);
				} break;
				case 1:
				{
					SetPixel(image, x, y, color);
					SetPixel(image, x, (y+1), color);
					SetPixel(image, x, (y+2), color);
					SetPixel(image, (x+1), (y+2), color);
				} break;
				case 2:
				{
					SetPixel(image, x, y, color);
					SetPixel(image, (x+1), y, color);
					SetPixel(image, (x+2), y, color);
					SetPixel(image, x, (y+1), color);
				} break;
				case 3:
				{
					SetPixel(image, x, y, color);
					SetPixel(image, (x+1), y, color);
					SetPixel(image, (x+1), (y+1), color);
					SetPixel(image, (x+1), (y+2), color);
				} break;
			} break;
			case 2: case 'J': switch (rotation) // J
			{
				case 0:
				{
					SetPixel(image, x, y, color);
					SetPixel(image, x+1, y, color);
					SetPixel(image, x, y+1, color);
					SetPixel(image, x, y+2, color);
				} break;
				case 1:
				{
					SetPixel(image, x, y, color);
					SetPixel(image, x, (y+1), color);
					SetPixel(image, (x+1), (y+1), color);
					SetPixel(image, (x+2), (y+1), color);
				} break;
				case 2:
				{
					SetPixel(image, (x+1), y, color);
					SetPixel(image, (x+1), (y+1), color);
					SetPixel(image, (x+1), (y+2), color);
					SetPixel(image, x, (y+2), color);
				} break;
				case 3:
				{
					SetPixel(image, x, y, color);
					SetPixel(image, (x+1), y, color);
					SetPixel(image, (x+2), y, color);
					SetPixel(image, (x+2), (y+1), color);
				} break;
			} break;
			case 3: case 'O':  // O
			{
				SetPixel(image, x, y, color);
				SetPixel(image, x, (y+1), color);
				SetPixel(image, (x+1), (y+1), color);
				SetPixel(image, (x+1), y, color);
					 	 
			} break;
			case 4: case 'Z': switch (rotation) // Z
			{
				case 0: case 2:
				{
					SetPixel(image, x, y, color);
					SetPixel(image, (x+1), y, color);
					SetPixel(image, (x+1), (y+1), color);
					SetPixel(image, (x+2), (y+1), color);
				} break;
				case 1: case 3:
				{
					SetPixel(image, (x+1), y, color);
					SetPixel(image, (x+1), (y+1), color);
					SetPixel(image, x, (y+1), color);
					SetPixel(image, x, (y+2), color);
				} break;
			} break;
			case 5: case 'S': switch (rotation) // S
			{
				case 0: case 2:
				{
					SetPixel(image, (x+1), y, color);
					SetPixel(image, (x+2), y, color);
					SetPixel(image, x, (y+1), color);
					SetPixel(image, (x+1), (y+1), color);
				} break;
				case 1: case 3:
				{
					SetPixel(image, x, y, color);
					SetPixel(image, x, (y+1), color);
					SetPixel(image, (x+1), (y+1), color);
					SetPixel(image, (x+1), (y+2), color);
				} break;
			} break;
			case 6: case 'T': switch (rotation) // T
			{
				case 0:
				{
					SetPixel(image, x, (y+1), color);
					SetPixel(image, (x+1), (y+1), color);
					SetPixel(image, (x+2), (y+1), color);
					SetPixel(image, (x+1), y, color);
				} break;
				case 1:
				{
					SetPixel(image, x, y, color);
					SetPixel(image, x, (y+1), color);
					SetPixel(image, x, (y+2), color);
					SetPixel(image, (x+1), (y+1), color);
				} break;
				case 2:
				{
					SetPixel(image, x, y, color);
					SetPixel(image, (x+1), y, color);
					SetPixel(image, (x+2), y, color);
					SetPixel(image, (x+1), (y+1), color);
				} break;
				case 3:
				{
					SetPixel(image, (x+1), y, color);
					SetPixel(image, (x+1), (y+1), color);
					SetPixel(image, (x+1), (y+2), color);
					SetPixel(image, x, (y+1), color);
				} break;
			} break;
		}
	}
}
