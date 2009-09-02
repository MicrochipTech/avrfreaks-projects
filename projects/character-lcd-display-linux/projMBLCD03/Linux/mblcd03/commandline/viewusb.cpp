/*
 * Include Headers
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <usb.h>

extern "C" {
#include "hiddata.h"
}

static int usbhidGetStringAscii(usb_dev_handle *dev, int index, char *buf, int buflen)
{
char    buffer[256];
int     rval, i;

    if((rval = usb_get_string_simple(dev, index, buf, buflen)) >= 0) /* use libusb version if it works */
        return rval;
    if((rval = usb_control_msg(dev, USB_ENDPOINT_IN, USB_REQ_GET_DESCRIPTOR, (USB_DT_STRING << 8) + index, 0x0409, buffer, sizeof(buffer), 5000)) < 0)
        return rval;
    if(buffer[1] != USB_DT_STRING){
        *buf = 0;
        return 0;
    }
    if((unsigned char)buffer[0] < rval)
        rval = (unsigned char)buffer[0];
    rval /= 2;
    /* lossy conversion to ISO Latin1: */
    for(i=1;i<rval;i++){
        if(i > buflen)              /* destination buffer overflow */
            break;
        buf[i-1] = buffer[2 * i];
        if(buffer[2 * i + 1] != 0)  /* outside of ISO Latin1 range */
            buf[i-1] = '?';
    }
    buf[i-1] = 0;
    return i-1;
}

int	main( int argc, char *argv[] )
{
	struct usb_bus      *bus;
	struct usb_device   *dev;
	usb_dev_handle      *handle = NULL;
    char    strbuf[256];
    int errorCode = USBOPEN_ERR_ACCESS;

	usb_init();
	usb_find_busses();
	usb_find_devices();
	for ( bus=usb_get_busses(); bus; bus=bus->next ) {
		printf("bus:[%s]\n", bus->dirname );
		for ( dev=bus->devices; dev; dev=dev->next ) {
  			struct usb_device_descriptor *descp = &dev->descriptor;
			printf( "-vid=%.4x, pid=%.4x, serial=%d\n",
				descp->idVendor, descp->idProduct,
				descp->iSerialNumber );

            handle = usb_open(dev); /* we need to open the device in order to query strings */
            if(!handle){
                errorCode = USBOPEN_ERR_ACCESS;
                fprintf(stderr, "Warning: cannot open USB device: %s\n", usb_strerror());
                continue;
            }
            int len = usbhidGetStringAscii(handle,
						dev->descriptor.iManufacturer, strbuf, sizeof(strbuf));
            if(len < 0){
                errorCode = USBOPEN_ERR_IO;
                fprintf(stderr,
						"Warning: cannot query manufacturer for device: %s\n",
						usb_strerror());
            }else{
				fprintf( stdout, " Manufact: [%s]\n", strbuf );
			}

            len = usbhidGetStringAscii(handle,
						dev->descriptor.iProduct, strbuf, sizeof(strbuf));
            if(len < 0){
                errorCode = USBOPEN_ERR_IO;
                fprintf(stderr,
						"Warning: cannot query manufacturer for device: %s\n",
						usb_strerror());
            }else{
				fprintf( stdout, " Product : [%s]\n", strbuf );
			}

			usb_close( handle );
		}
	}
}
