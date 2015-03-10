
/*
*
*	tcj 3-9-2105
*
*	usbbulktest.c
*
*	15030901
*
*	host-side program to exercise usbb_bulk_example.c on AT32UC3B controller
*
*	written for Linux host; may or may not require adaptation for other OS
*
*
*	compile with:
*
*	gcc usbbulktest.c -o usbbulktest -lusb-1.0
*
*
*	run with:
*
*	sudo ./usbbulktest  x  y                where x and y are each numbers in range 0 - 255
*
*	reply from controller echoing these numbers should be displayed
*
*
*	Note that this uses libusb-1.0 library, not to be confused with the 0.1 version, which
*	has different functions
*
*	For Linux, libusb-1.0 and libusb-1.0-dev must be installed from repository (e.g. with Synaptic)
*
*	Linking libusb library as shown on compile line above is mandatory
*
*
*
*	In Linux, this program requires root access; therefore either use sudo or
*	use setuid to allow execution by non-root user
*
*	For setuid, first change owner of program to root:  	sudo chown root program_name
*	then change the user suid bit of the program:   		sudo chmod u+s program_name
*
*
*/

#include <stdio.h>
#include <string.h>
#include <libusb-1.0/libusb.h>

#define VENDOR_ID   0x6666				// prototype VID
#define PRODUCT_ID  0x89ab


libusb_device **devs; 					// pointer to pointer of device, used to retrieve a list of devices

libusb_device_handle *dev_handle;		// device handle (which must be closed with libusb_close(dev_handle)

libusb_context *ctx = NULL; 			// libusb session (which must be exited with libusb_exit(ctx)

int r; // return values
int cnt; // for holding number of devices in list

uint8_t open_interface(void)
{
	r = libusb_init(&ctx); // initialize the library for the session we just declared
	if(r < 0)
	{
		printf("init error\n");
		return 1;
	}

	libusb_set_debug(ctx, 3); // debug verbosity level = 3

	cnt = libusb_get_device_list(ctx, &devs); // get device list
	if(cnt < 0)
	{
		printf("get device error\n");
		return 1;
	}

	printf("%d devices in List\n", cnt);

	dev_handle = libusb_open_device_with_vid_pid(ctx, VENDOR_ID, PRODUCT_ID);

	if(dev_handle == NULL) printf("cannot open device\n");
		
	else printf("device opened\n");
		
	libusb_free_device_list(devs, 1); // free the list
	
	if(libusb_kernel_driver_active(dev_handle, 0) == 1)  // is kernel driver attached?
	{
		printf("kernel driver is active\n");
		if(libusb_detach_kernel_driver(dev_handle, 0) == 0) printf("kernel driver detached\n");  // detach it
	}

	r = libusb_claim_interface(dev_handle, 0); // claim interface 0
	if(r < 0)
	{
		printf("interface claim failed\n");
		return 1;
	}

	printf("interface claimed\n");

	return 0;
}


uint8_t close_interface(void)
{
	r = libusb_release_interface(dev_handle, 0); // release interface

	if(r!=0)
	{
		printf("cannot release interface\n");
		return 1;
	}
	printf("interface released\n");

	libusb_close(dev_handle); // close open device
	libusb_exit(ctx); // must be called after closing

	return 0;
}



uint8_t main(argc,argv)
uint8_t argc;
uint8_t *argv[];
{
	int num_written; //number of bytes written
	int num_read; //number of bytes read

	uint8_t out_data[2], in_data[2] = {0,0};

	out_data[0] = atoi(argv[1]);	

	out_data[1] = atoi(argv[2]);

	if(open_interface()) return 1;


	r = libusb_bulk_transfer(dev_handle, (4 | LIBUSB_ENDPOINT_OUT), out_data, 2, &num_written, 0); // device's out endpoint is 4; we're writing 2 bytes; infinite timeout

	if(r == 0 && num_written == 2) printf("success on write\n");  //we wrote the 2 bytes successfully
	else printf("Write Error\n");


	r = libusb_bulk_transfer(dev_handle, (3 | LIBUSB_ENDPOINT_IN), in_data, 2, &num_read, 0); // device's in endpoint is 3; we're reading 2 bytes; infinite timeout

	if(r == 0 && num_read == 2) printf("success on read\n");  //we read the 2 bytes successfully
	else printf("Read Error\n");


	printf("\n\nback from USB: %d %d\n\n", in_data[0], in_data[1]);

		
	if(close_interface()) return 1;

	return 0;
}



