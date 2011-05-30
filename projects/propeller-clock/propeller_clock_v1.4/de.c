#include <stdio.h>
#include <math.h>
#define PI 3.141592654
 
 
typedef struct coord{
	unsigned char x;
	unsigned char y;
}coord_t;
 
char posCOOR[91][32]={
{1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32},
{1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32},
{1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32},
{1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32},
{1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32},
{1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32},
{1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32},
{1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32},
{1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32},
{1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32},
{1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32},
{1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,27,28,29,30,31},
{1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,22,23,24,25,26,27,28,29,30,31},
{1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,19,20,21,22,23,24,25,26,27,28,29,30,31},
{1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31},
{1,2,3,4,5,6,7,8,9,10,11,12,13,14,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31},
{1,2,3,4,5,6,7,8,9,10,11,12,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31},
{1,2,3,4,5,6,7,8,9,10,11,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31},
{1,2,3,4,5,6,7,8,9,10,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,29,30},
{1,2,3,4,5,6,7,8,9,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,26,27,28,29,30},
{1,2,3,4,5,6,7,8,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,23,24,25,26,27,28,29,30},
{1,2,3,4,5,6,7,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,21,22,23,24,25,26,27,28,29,30},
{1,2,3,4,5,6,6,7,8,9,10,11,12,13,14,15,16,17,18,19,19,20,21,22,23,24,25,26,27,28,29,30},
{1,2,3,4,5,6,6,7,8,9,10,11,12,13,14,15,16,17,17,18,19,20,21,22,23,24,25,26,27,28,29,29},
{1,2,3,4,5,5,6,7,8,9,10,11,12,13,14,15,16,16,17,18,19,20,21,22,23,24,25,26,26,27,28,29},
{1,2,3,4,5,5,6,7,8,9,10,11,12,13,14,15,15,16,17,18,19,20,21,22,23,24,24,25,26,27,28,29},
{1,2,3,4,4,5,6,7,8,9,10,11,12,13,13,14,15,16,17,18,19,20,21,22,22,23,24,25,26,27,28,29},
{1,2,3,4,4,5,6,7,8,9,10,11,12,12,13,14,15,16,17,18,19,20,20,21,22,23,24,25,26,27,28,29},
{1,2,3,4,4,5,6,7,8,9,10,11,11,12,13,14,15,16,17,18,19,19,20,21,22,23,24,25,26,26,27,28},
{1,2,3,3,4,5,6,7,8,9,10,10,11,12,13,14,15,16,17,17,18,19,20,21,22,23,24,24,25,26,27,28},
{1,2,3,3,4,5,6,7,8,9,10,10,11,12,13,14,15,16,16,17,18,19,20,21,22,23,23,24,25,26,27,28},
{1,2,3,3,4,5,6,7,8,9,9,10,11,12,13,14,15,15,16,17,18,19,20,21,21,22,23,24,25,26,27,27},
{1,2,3,3,4,5,6,7,8,8,9,10,11,12,13,14,14,15,16,17,18,19,20,20,21,22,23,24,25,25,26,27},
{1,2,3,3,4,5,6,7,8,8,9,10,11,12,13,13,14,15,16,17,18,18,19,20,21,22,23,23,24,25,26,27},
{1,2,2,3,4,5,6,7,7,8,9,10,11,12,12,13,14,15,16,17,17,18,19,20,21,22,22,23,24,25,26,27},
{1,2,2,3,4,5,6,7,7,8,9,10,11,11,12,13,14,15,16,16,17,18,19,20,20,21,22,23,24,25,25,26},
{1,2,2,3,4,5,6,6,7,8,9,10,11,11,12,13,14,15,15,16,17,18,19,19,20,21,22,23,23,24,25,26},
{1,2,2,3,4,5,6,6,7,8,9,10,10,11,12,13,14,14,15,16,17,18,18,19,20,21,22,22,23,24,25,26},
{1,2,2,3,4,5,6,6,7,8,9,9,10,11,12,13,13,14,15,16,17,17,18,19,20,20,21,22,23,24,24,25},
{1,2,2,3,4,5,5,6,7,8,9,9,10,11,12,12,13,14,15,16,16,17,18,19,19,20,21,22,23,23,24,25},
{1,2,2,3,4,5,5,6,7,8,8,9,10,11,11,12,13,14,15,15,16,17,18,18,19,20,21,21,22,23,24,25},
{1,2,2,3,4,5,5,6,7,8,8,9,10,11,11,12,13,14,14,15,16,17,17,18,19,20,20,21,22,23,23,24},
{1,1,2,3,4,4,5,6,7,7,8,9,10,10,11,12,13,13,14,15,16,16,17,18,19,19,20,21,22,22,23,24},
{1,1,2,3,4,4,5,6,7,7,8,9,10,10,11,12,12,13,14,15,15,16,17,18,18,19,20,20,21,22,23,23},
{1,1,2,3,4,4,5,6,6,7,8,9,9,10,11,12,12,13,14,14,15,16,17,17,18,19,19,20,21,22,22,23},
{1,1,2,3,4,4,5,6,6,7,8,8,9,10,11,11,12,13,13,14,15,16,16,17,18,18,19,20,21,21,22,23},
{1,1,2,3,3,4,5,6,6,7,8,8,9,10,10,11,12,13,13,14,15,15,16,17,17,18,19,19,20,21,22,22},
{1,1,2,3,3,4,5,5,6,7,8,8,9,10,10,11,12,12,13,14,14,15,16,16,17,18,18,19,20,20,21,22},
{1,1,2,3,3,4,5,5,6,7,7,8,9,9,10,11,11,12,13,13,14,15,15,16,17,17,18,19,19,20,21,21},
{1,1,2,3,3,4,5,5,6,7,7,8,9,9,10,10,11,12,12,13,14,14,15,16,16,17,18,18,19,20,20,21},
{1,1,2,3,3,4,4,5,6,6,7,8,8,9,10,10,11,12,12,13,13,14,15,15,16,17,17,18,19,19,20,21},
{1,1,2,3,3,4,4,5,6,6,7,8,8,9,9,10,11,11,12,13,13,14,14,15,16,16,17,18,18,19,20,20},
{1,1,2,2,3,4,4,5,6,6,7,7,8,9,9,10,10,11,12,12,13,14,14,15,15,16,17,17,18,18,19,20},
{1,1,2,2,3,4,4,5,5,6,7,7,8,8,9,10,10,11,11,12,13,13,14,14,15,16,16,17,17,18,19,19},
{1,1,2,2,3,4,4,5,5,6,6,7,8,8,9,9,10,11,11,12,12,13,14,14,15,15,16,16,17,18,18,19},
{1,1,2,2,3,3,4,5,5,6,6,7,7,8,9,9,10,10,11,11,12,13,13,14,14,15,15,16,17,17,18,18},
{1,1,2,2,3,3,4,4,5,6,6,7,7,8,8,9,10,10,11,11,12,12,13,13,14,15,15,16,16,17,17,18},
{1,1,2,2,3,3,4,4,5,5,6,7,7,8,8,9,9,10,10,11,11,12,13,13,14,14,15,15,16,16,17,17},
{1,1,2,2,3,3,4,4,5,5,6,6,7,7,8,8,9,10,10,11,11,12,12,13,13,14,14,15,15,16,16,17},
{1,1,2,2,3,3,4,4,5,5,6,6,7,7,8,8,9,9,10,10,11,11,12,12,13,13,14,14,15,15,16,16},
{1,1,2,2,3,3,4,4,5,5,6,6,7,7,8,8,9,9,10,10,11,11,12,12,13,13,14,14,15,15,16,16},
{0,1,1,2,2,3,3,4,4,5,5,6,6,7,7,8,8,9,9,10,10,11,11,12,12,13,13,14,14,15,15,16},
{0,1,1,2,2,3,3,4,4,5,5,6,6,7,7,8,8,8,9,9,10,10,11,11,12,12,13,13,14,14,15,15},
{0,1,1,2,2,3,3,4,4,5,5,5,6,6,7,7,8,8,9,9,10,10,10,11,11,12,12,13,13,14,14,15},
{0,1,1,2,2,3,3,4,4,4,5,5,6,6,7,7,7,8,8,9,9,10,10,11,11,11,12,12,13,13,14,14},
{0,1,1,2,2,3,3,3,4,4,5,5,5,6,6,7,7,8,8,8,9,9,10,10,11,11,11,12,12,13,13,14},
{0,1,1,2,2,2,3,3,4,4,4,5,5,6,6,7,7,7,8,8,9,9,9,10,10,11,11,11,12,12,13,13},
{0,1,1,2,2,2,3,3,4,4,4,5,5,5,6,6,7,7,7,8,8,9,9,9,10,10,11,11,11,12,12,13},
{0,1,1,1,2,2,3,3,3,4,4,4,5,5,6,6,6,7,7,7,8,8,9,9,9,10,10,10,11,11,12,12},
{0,1,1,1,2,2,3,3,3,4,4,4,5,5,5,6,6,6,7,7,8,8,8,9,9,9,10,10,10,11,11,11},
{0,1,1,1,2,2,2,3,3,3,4,4,4,5,5,5,6,6,6,7,7,8,8,8,9,9,9,10,10,10,11,11},
{0,1,1,1,2,2,2,3,3,3,4,4,4,5,5,5,6,6,6,7,7,7,7,8,8,8,9,9,9,10,10,10},
{0,1,1,1,2,2,2,2,3,3,3,4,4,4,5,5,5,6,6,6,6,7,7,7,8,8,8,9,9,9,10,10},
{0,1,1,1,1,2,2,2,3,3,3,4,4,4,4,5,5,5,6,6,6,6,7,7,7,8,8,8,8,9,9,9},
{0,1,1,1,1,2,2,2,2,3,3,3,4,4,4,4,5,5,5,6,6,6,6,7,7,7,7,8,8,8,9,9},
{0,1,1,1,1,2,2,2,2,3,3,3,3,4,4,4,4,5,5,5,5,6,6,6,6,7,7,7,8,8,8,8},
{0,0,1,1,1,1,2,2,2,2,3,3,3,3,4,4,4,4,5,5,5,5,6,6,6,6,7,7,7,7,7,8},
{0,0,1,1,1,1,2,2,2,2,2,3,3,3,3,4,4,4,4,4,5,5,5,5,6,6,6,6,7,7,7,7},
{0,0,1,1,1,1,1,2,2,2,2,2,3,3,3,3,4,4,4,4,4,5,5,5,5,5,6,6,6,6,6,7},
{0,0,1,1,1,1,1,2,2,2,2,2,2,3,3,3,3,3,4,4,4,4,4,5,5,5,5,5,6,6,6,6},
{0,0,1,1,1,1,1,1,2,2,2,2,2,2,3,3,3,3,3,3,4,4,4,4,4,5,5,5,5,5,5,6},
{0,0,0,1,1,1,1,1,1,2,2,2,2,2,2,3,3,3,3,3,3,3,4,4,4,4,4,4,5,5,5,5},
{0,0,0,1,1,1,1,1,1,1,2,2,2,2,2,2,2,3,3,3,3,3,3,3,3,4,4,4,4,4,4,4},
{0,0,0,0,1,1,1,1,1,1,1,1,2,2,2,2,2,2,2,2,3,3,3,3,3,3,3,3,4,4,4,4},
{0,0,0,0,1,1,1,1,1,1,1,1,1,1,2,2,2,2,2,2,2,2,2,3,3,3,3,3,3,3,3,3},
{0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,2,2,2,2,2,2,2,2,2,2,2,3,3,3,3},
{0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,2,2,2,2,2,2,2,2,2,2,2},
{0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,2,2,2,2},
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1},
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}
};


coord_t polar_2_rect(short int angulo,short int raio)
{
	coord_t coord;
	
	if (angulo>=270)
	{
		angulo=360-angulo;
		coord.y=-1;
		coord.x=1;
	}
	else 	if (angulo>=180)
			{
				angulo=angulo-180;
				coord.y=-1;
				coord.x=-1;
			}
			else 	if (angulo>=90)
					{
						angulo=180-angulo;
						coord.y=1;
						coord.x=-1;
					}
					else
					{
						coord.x=1;
						coord.y=1;
					}

					coord.x=coord.x*posCOOR[angulo][raio-1]+32;//o raio � s� de 0 a 31
	coord.y=-1*coord.y*posCOOR[90-angulo][raio-1]+32; // o angulo � de 0 a 90
	
	return coord;
}


coord_t polar_to_rectangular(int theta,int radius){
	coord_t pos;
	float ta=theta*PI/180;
	pos.x=(int)((float)radius*(float)cos(ta)+32);
	pos.y=(int)(-(float)radius*(float)sin(ta)+32);
	return pos;
}


int main(int argc, char **argv)
{
	if (argc!=3)
	{
		printf ("\n\n...ARGUMENTO INSUFICIENTES...\n\n");
		return -1;
	}
	
	int raio=atoi(argv[1]);
	int angulo=atoi(argv[2]);
	coord_t a;
	a=polar_to_rectangular(angulo,raio);
	printf ("\nORIGINAL : \n X: %d\n Y: %d",a.x,a.y);
	a=polar_2_rect(angulo,raio);
	printf ("\n\nCALC : \n X: %d\n Y: %d\n",a.x,a.y);
	return 0;
}