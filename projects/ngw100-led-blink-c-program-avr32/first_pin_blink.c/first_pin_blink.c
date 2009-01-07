// Blink pin 3 on port B at 1 Hz
// Just add an LED and see the light! ;)
//
//Created by Dingo_aus, 7 January 2009
//email: dingo_aus [at] internode <dot> on /dot/ net
//
//Created in AVR32 Studio (version 2.0.2) running on Ubuntu 8.04

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>




FILE *fp; 


int main(int argc, char** argv)
{

	printf("\n**********************************\n"
			"*  Welcome to PIN Blink program  *\n"
			"*  ....blinking pin 3 on port B  *\n"
			"*  ....rate of 1 Hz............  *\n"
			"**********************************\n");
	
	//create a variable to store whether we are sending a '1' or a '0'
	char set_value[4]; 
	//Integer to keep track of whether we want on or off
	int toggle = 0;
	
	//Using sysfs we need to write "37" to /sys/class/gpio/export
	//This will create the folder /sys/class/gpio/gpio37
	if ((fp = fopen("/sys/class/gpio/export", "ab")) == NULL)
		{
			printf("Cannot open export file.\n");
			exit(1);
		}
	//Set pointer to begining of the file
		rewind(fp);
		//Write our value of "37" to the file
		strcpy(set_value,"37");
		fwrite(&set_value, sizeof(char), 2, fp);
		fclose(fp);
	
	printf("...export file accessed, new pin now accessible\n");
	
	//SET DIRECTION
	//Open the LED's sysfs file in binary for reading and writing, store file pointer in fp
	if ((fp = fopen("/sys/class/gpio/gpio37/direction", "rb+")) == NULL)
	{
		printf("Cannot open direction file.\n");
		exit(1);
	}
	//Set pointer to begining of the file
	rewind(fp);
	//Write our value of "out" to the file
	strcpy(set_value,"out");
	fwrite(&set_value, sizeof(char), 3, fp);
	fclose(fp);
	printf("...direction set to output\n");
	
	//SET VALUE
	//Open the LED's sysfs file in binary for reading and writing, store file pointer in fp
	if ((fp = fopen("/sys/class/gpio/gpio37/value", "rb+")) == NULL)
		{
			printf("Cannot open value file.\n");
			exit(1);
		}
	//Set pointer to begining of the file
	rewind(fp);
	//Write our value of "1" to the file 
	strcpy(set_value,"1");
	fwrite(&set_value, sizeof(char), 1, fp);
	fclose(fp);
	printf("...value set to 1...\n");
			
	//Run an infinite loop - will require Ctrl-C to exit this program
	while(1)
	{
		//Set it so we know the starting value in case something above doesn't leave it as 1
		strcpy(set_value,"1");
		
		if ((fp = fopen("/sys/class/gpio/gpio37/value", "rb+")) == NULL)
		{
			printf("Cannot open value file.\n");
			exit(1);
		}	
		toggle = !toggle;
		if(toggle)
		{
			//Set pointer to begining of the file
			rewind(fp);
			//Write our value of "1" to the file 
			strcpy(set_value,"1");
			fwrite(&set_value, sizeof(char), 1, fp);
			fclose(fp);
			printf("...value set to 1...\n");
		}
		else
		{
			//Set pointer to begining of the file
			rewind(fp);
			//Write our value of "0" to the file 
			strcpy(set_value,"0");
			fwrite(&set_value, sizeof(char), 1, fp);
			fclose(fp);
			printf("...value set to 0...\n");
		}
		//Pause for one second
		sleep(1);
	
	}
		
	
	return 0;
	
}
