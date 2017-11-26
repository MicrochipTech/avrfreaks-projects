/*	This tool allows control of Macro-keyboard from Linux OS (see usage function).
*   Copyright (C) 2017  Ondra Vlk
*
*   This program is free software: you can redistribute it and/or modify
*   it under the terms of the GNU General Public License as published by
*   the Free Software Foundation, either version 3 of the License, or
*   (at your option) any later version.
*
*   This program is distributed in the hope that it will be useful,
*   but WITHOUT ANY WARRANTY; without even the implied warranty of
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*   GNU General Public License for more details.
*
*   You should have received a copy of the GNU General Public License
*   along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#define IS_WORKING_AS_INTENDED      0           //If the program has all features (for example sensing hotkeys -> sending command to MCU over USB -> ...)
#define USE_DELAY_AT_START			1			//TODO not really necessary but it gives time to swith to text editor if receiving data

#include <iostream>
#include <cstring>
#include <stdlib.h>
#include <libusb-1.0/libusb.h>                  //USB library "libusb-1.0"

#if	USE_DELAY_AT_START == 1
#include <thread>
#include <chrono>
#endif // USE_DELAY_AT_START

#include "../firmware/hid_rq.h"           		// custom request numbers
#include "../firmware/main_config.h"
#include "../firmware/usbconfig.h"        		// device's VID/PID and names

//#include "../firmware/hid_keys.h"       		// custom key mapping - not necessary for the program but it is important for user

//#include <argp.h>   //Not supported by Windows - had to make custom solution, therefore it sucks -> also Windows sucks more since it doesn't allow me easily to control keyboard with libusb-1.0 (shuld have used argp in the first place and forget about Windows)

#define MAIN_BUFFER_SIZE    256

#if MAIN_BUFFER_SIZE < BUFFER_SIZE
#warning "Defined buffer is smaller than device buffer -> operation of this program may be limited."
#endif // MAIN_BUFFER_SIZE


typedef struct {
	int r;                  //reset
	int p;                  //printing protocol
	int lg;                 //get hardware LED status
	int ls;                 //set hardware LED status
	char* lls;              //set LED status value
	int c;                  //continuous run
	int s;                  //flag send s
	char* ss;               //send data on address s
	int d;                  //flag store d
	char* dd;               //store data block d (address is at the beggining - format see usage())
} arguments;



/*----------------------*/
/*--Prototypes----------*/
/*----------------------*/
//Help menu
static void usage(char *name);

//Argument parsing to determine operation
int argParse(arguments* arg, int argc, char **argv);

//Convert user input into data readable by teh device - return No. of characters + 1 (null terminator)
uint16_t dataTranslation(char* rawData, unsigned char* processedData);

//  command is "vendor" request located in "hid_rq.h" in device firmware
//  comModifier is special case for the command like address for VENDOR_RQ_GET_DATA
//  dataBuffer represent data that should be send to the device - Null if no bulk transfer is required
//  dataLen for checking if the buffer is not too large for the device
int usbProc(uint8_t command, uint8_t comModifier, uint8_t* dataBuffer, uint16_t dataLen);

/*----------------------*/
/*--- Main functions ---*/
/*----------------------*/

int main(int argc, char **argv)
{
#if	USE_DELAY_AT_START == 1
	std::cout << "sleep" << std::endl;
	std::this_thread::sleep_for(std::chrono::milliseconds(3000));
	std::cout << "wake" << std::endl;
#endif // USE_DELAY_AT_START


	int r = 0;                              //for return values
	uint16_t len = 0;                       //holds number string length in data buffer (don't count buffer[0] which is address but counts null termination)
	unsigned char buffer[MAIN_BUFFER_SIZE] = {0};       //Generic buffer for data handling

	arguments arg = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};     // Default - no argument

	//Argument parsing
	r = argParse(&arg, argc, argv);

	if(r) {return r;}   // Argument parsing error

	//Argument 2 values parsing
	if(arg.s == 1) {
		len = dataTranslation(arg.ss, buffer);
	} else if(arg.d == 1) {
		len = dataTranslation(arg.dd, buffer);
#if HWLED_CONTROL_LED == 1
	} else if(arg.ls == 1) {

		if(arg.lls[0] == '0') {
			buffer[0] = 0;
		} else if(arg.lls[0] == '1') {
			buffer[0] = 1;
		} else {
			std::cerr << "Not a valid input of hardware LED control protocol" << std::endl;
			arg.ls = 0;     //Not a valid input
			arg.lls = nullptr;
			exit(1);
		}

#endif // HWLED_CONTROL_LED
	}

	std::cout << "dataLen: " << len << std::endl;
	int jjj = 0;

	while(buffer[jjj] != 0) {std::cout << static_cast<int>(buffer[jjj]) << ", "; ++jjj;};

	std::cout << std::endl << std::endl;

	if(arg.r == 1) {
		r = usbProc(VENDOR_RQ_MCU_RESET, 0, 0, 0);
#if PRINT_ENABLED == 1
	} else if(arg.p == 1) {
		r = usbProc(VENDOR_RQ_PRINT_MEMORY, 0, 0, 0);
#endif // PRINT_ENABLED
#if HWLED_CONTROL_LED == 1
	} else if(arg.ls == 1) {
		r = usbProc(VENDOR_RQ_SET_LED_STATUS, buffer[0], 0, 0);   //Set LED to 0:OFF or 1:ON
	} else if(arg.lg == 1) {
		r = usbProc(VENDOR_RQ_GET_LED_STATUS, 0, buffer, sizeof(buffer));   //Address of datablock is passed in command modifier - handled on device side
#endif // HWLED_CONTROL_LED
	} else if(arg.s == 1) {
		r = usbProc(VENDOR_RQ_GET_DATA, buffer[0], 0, 0);   //Address of datablock is passed in command modifier - handled on device side

	} else if(arg.d == 1) {
		if(len >= BUFFER_SIZE) {
			std::cerr << "Device buffer too small for incoming data. Transfer skipped." << std::endl;
			return 10;
		}

		while(len < BUFFER_SIZE) {buffer[len++] = 0;} //fill remaining space with 0 - send always full size buffer to the device

		r = usbProc(VENDOR_RQ_SET_BULK, 0, buffer, BUFFER_SIZE);    //Address of datablock is passed as buffer[0] - handled on device side, (len+1) is (data+address)

#if IS_WORKING_AS_INTENDED == 1
	} else if(arg.c == 1) {
		//skip now - enter infinite loop with hotkey sensing
		//Not implemented since Windows is a mess
#endif // IS_WORKING_AS_INTENDED
	}

	return r;
}


//Help menu
static void usage(char *name)
{

	fprintf(stderr, "\n"
	        "usage:\n"
#if IS_WORKING_AS_INTENDED == 1
	        "  %1$s -c ............. continuous mode with hotkey sensing.\n"
#endif // IS_WORKING_AS_INTENDED
	        "  %1$s -d xxxyyyyy .... store data block in device.\n"
	        "  %1$s -s xxx ......... send data block from device (switch to text editor).\n"
	        "  %1$s -r ............. reboot USB device.\n"
#if PRINT_ENABLED == 1
	        "  %1$s -p ............. print content of device eeprom (switch to text editor to receive).\n"
#endif // PRINT_ENABLED
#if HWLED_CONTROL_LED == 1
	        "  %1$s -lg ............ get status of hardware LED of the device.\n"
	        "  %1$s -ls z .......... set status of hardware LED (z == 0 or 1).\n"
#endif // HWLED_CONTROL_LED
	        "\n"
	        "Data block format xxxyyyyy where X represent decimal number (always 3 digit)\n"
	        "of device address and Y is ASCII value or its hexadecimal representation\n"
	        "in 3 digit format #yy (lowercase abcdef). ASCII # symbol is represented by ##.\n"
	        "\n"
	        "Example: 012asd#21fgh##98 - store asd!fgh#98 at address 12.\n"
	        "\n"
	        "Special configuration data\n"
	        "%1$s -d 255#xx#yy... where #xx#yy is hex sequence of:\n"
	        "\tCount_NumLock_trigger\n"
	        "\tCount_CapsLock_trigger\n"
	        "\tCount_NumCaps_trigger\n"
	        "\tCount_CapsNum_trigger\n"
	        "\tCounter_timeout\n"
	        "\tDelay_after_trigger\n", name);
}


int argParse(arguments* arg, int argc, char **argv)
{
	if(argc < 2) {
		std::cerr << "Too few arguments." << std::endl;
		usage(argv[0]);
		return 1;
	} else if(argc > 3) {
		std::cerr << "Too many arguments." << std::endl;
		usage(argv[0]);
		return 2;
	}

	if(strcasecmp(argv[1], "-r") == 0) {
		arg->r = 1;
		return 0;
#if PRINT_ENABLED == 1
	} else if(strcasecmp(argv[1], "-p") == 0) {
		arg->p = 1;
		return 0;
#endif // PRINT_ENABLED
#if HWLED_CONTROL_LED == 1
	} else if((strcasecmp(argv[1], "-ls") == 0) && (argc == 3)) {
		arg->ls = 1;
		arg->lls = argv[2];
		return 0;
	} else if(strcasecmp(argv[1], "-lg") == 0) {
		arg->lg = 1;
		return 0;
#endif // HWLED_CONTROL_LED
#if IS_WORKING_AS_INTENDED == 1
	} else if(strcasecmp(argv[1], "-c") == 0) {
		arg->c = 1;
		return 0;
#endif // IS_WORKING_AS_INTENDED
	} else if((strcasecmp(argv[1], "-s") == 0) && (argc == 3)) {
		arg->ss = argv[2];
		arg->s = 1;
		return 0;
	} else if((strcasecmp(argv[1], "-d") == 0) && (argc == 3)) {
		arg->dd = argv[2];
		arg->d = 1;
		return 0;
	} else {
		std::cerr << "Unknown arguments." << std::endl;
		usage(argv[0]);
		return 3;
	}
}


uint16_t dataTranslation(char* rawData, unsigned char* processedData)
{
	char tempBuf[4] = {0};      //Temporary buffer for data processing
	uint16_t dataCnt = 0;            //Count data in processedData + 1 (null terminated)

	//First 3 digists should represent address (000-255 since that is maximal size of uint8_t which is used in the device)
	if(rawData[0] < '0' || rawData[0] > '9' ||
	   rawData[1] < '0' || rawData[1] > '9' ||
	   rawData[2] < '0' || rawData[2] > '9') {
		std::cerr << "Not a proper address of the device memory - require 3 digit decimal number 000-255." << std::endl;
		exit(1);
	} else {
		for(int i = 0; i < 3; i++) {
			tempBuf[i] = rawData[i];
		}
	}

	//First position in buffer is address for data block inside the device
	processedData[0] = strtol(tempBuf, nullptr, 10);

	//Now processing data itself
	int iii = 3;

	while(rawData[iii] != 0) {

		if(rawData[iii] == '#') {
			if(rawData[iii + 1] == '#') {       //Special character # should be stored in the device
				processedData[dataCnt + 1] = '#';
				iii++;

			} else if(((rawData[iii + 1] >= '0' && rawData[iii + 1] <= '9') || (rawData[iii + 1] >= 'a' && rawData[iii + 1] <= 'f')) && //Hex value
			          ((rawData[iii + 2] >= '0' && rawData[iii + 2] <= '9') || (rawData[iii + 2] >= 'a' && rawData[iii + 2] <= 'f'))) {
				tempBuf[0] = rawData[iii + 1];
				tempBuf[1] = rawData[iii + 2];
				tempBuf[2] = 0x00;

				processedData[dataCnt + 1] = strtol(tempBuf, nullptr, 16);
				iii += 2;

			} else {     //data mishmash - exit
				std::cerr << "Data mishmash - can't decipher #xx sequence" << std::endl;
				exit(1);
			}
		} else {
			processedData[dataCnt + 1] = rawData[iii];
		}

		//Progress buffer "pointers"
		dataCnt++;
		iii++;

		if(dataCnt >= MAIN_BUFFER_SIZE) {
			std::cerr << "Data sequence too long to process in the device." << std::endl;
			exit(1);
		}
	}

	processedData[dataCnt + 2] = 0x00; //Null termination
	return (dataCnt + 1);
}

int usbProc(uint8_t command, uint8_t comModifier, uint8_t* dataBuffer, uint16_t dataLen)
{

	libusb_device **devs;               //pointer to pointer of device, used to retrieve a list of devices
	libusb_device_handle *dev_handle;   //a device handle
	libusb_context *ctx = NULL;         //a libusb session

	int r;                              //for return values
	ssize_t cnt;                        //holding number of devices in list

	//Device marking - Vendor ID, Product ID - from "usbconfig.h"
	const uint8_t rawVid[2] = {USB_CFG_VENDOR_ID};
	const uint8_t rawPid[2] = {USB_CFG_DEVICE_ID};
	const uint16_t vid = rawVid[1] * 256 + rawVid[0];
	const uint16_t pid = rawPid[1] * 256 + rawPid[0];

	r = libusb_init(&ctx);                                          //Initialize the library for the session

	if(r < 0) {
		std::cerr << "USBlib Init Error " << r << std::endl;        //There was an error
		return r;
	}

	libusb_set_debug(ctx, LIBUSB_LOG_LEVEL_INFO);                   //Set verbosity level to 3, as suggested in the documentation

	cnt = libusb_get_device_list(ctx, &devs);                       //Get the list of devices

	if(cnt < 0) {
		libusb_exit(ctx);                                           //Close USB session - error exit
		std::cerr << "Get Device Error" << std::endl;
		return 1;
	}

	std::cout << cnt << " Devices in list." << std::endl;

	dev_handle = libusb_open_device_with_vid_pid(ctx, vid, pid);    //These are vendorID and productID I found for my usb device

	libusb_free_device_list(devs, 1);                               //Free the list, unref the devices in it

	if(dev_handle == NULL) {
		libusb_exit(ctx);                                           //Close USB session - error exit
		std::cerr << "Cannot open device" << std::endl;
		return 1;
	} else {
		std::cout << "Device Opened" << std::endl;
	}

	libusb_set_auto_detach_kernel_driver(dev_handle, 1);            //Detach kernel drives if necessary

//TODO - delete - already using autodetach kernel driver
//	if(libusb_kernel_driver_active(dev_handle, 0) == 1) { //find out if kernel driver is attached
//		std::cout << "Kernel Driver Active" << std::endl;
//
//		if(libusb_detach_kernel_driver(dev_handle, 0) == 0) //detach it
//			std::cout << "Kernel Driver Detached!" << std::endl;
//	}

	r = libusb_claim_interface(dev_handle, 0);                      //Claim interface 0 (the first) of device (mine had just 1)

	if(r < 0) {
		libusb_close(dev_handle);                                   //Close the device we opened - error exit
		libusb_exit(ctx);                                           //Close USB session - error exit
		std::cerr << "Cannot Claim Interface" << std::endl;
		return r;
	} else {
		std::cout << "Claimed Interface" << std::endl;
	}

	std::cout << "Writing Data..." << std::endl;

	switch(command) {
		case VENDOR_RQ_MCU_RESET:
		case VENDOR_RQ_GET_DATA:
#if PRINT_ENABLED == 1
		case VENDOR_RQ_PRINT_MEMORY:
#endif // PRINT_ENABLED
#if HWLED_CONTROL_LED == 1
		case VENDOR_RQ_SET_LED_STATUS:
#endif // HWLED_CONTROL_LED
			r = libusb_control_transfer(dev_handle, LIBUSB_REQUEST_TYPE_VENDOR | LIBUSB_RECIPIENT_DEVICE | LIBUSB_ENDPOINT_OUT, command, comModifier, 0, dataBuffer, 0, 5000);
			break;

		case VENDOR_RQ_SET_BULK:
			r = libusb_control_transfer(dev_handle, LIBUSB_REQUEST_TYPE_VENDOR | LIBUSB_RECIPIENT_DEVICE | LIBUSB_ENDPOINT_OUT, command, 0, 0, dataBuffer, dataLen, 5000);
			break;

#if HWLED_CONTROL_LED == 1

		case VENDOR_RQ_GET_LED_STATUS:
			r = libusb_control_transfer(dev_handle, LIBUSB_REQUEST_TYPE_VENDOR | LIBUSB_RECIPIENT_DEVICE | LIBUSB_ENDPOINT_IN, command, 0, 0, dataBuffer, dataLen, 5000);
			break;
#endif // HWLED_CONTROL_LED

	}

	std::cout << "Transfered " << r << " bytes of data." << std::endl;

#if HWLED_CONTROL_LED == 1

	if(command == VENDOR_RQ_GET_LED_STATUS && r > 0) {
		std::cout << "LED Status: ";

		for(int i = 0; i < r; ++i) {
			std::cout << static_cast<int>(dataBuffer[i]) << ", ";
		}

		std::cout << std::endl << std::endl;
	}

#endif // HWLED_CONTROL_LED

	r = libusb_release_interface(dev_handle, 0); //Release the claimed interface

	if(r != 0) {
		std::cerr << "Cannot Release Interface" << std::endl;
		return r;
	} else {
		std::cout << "Released Interface" << std::endl;
	}

	libusb_close(dev_handle); //Close the device we opened
	libusb_exit(ctx); //Close USB session

	return r;
}
