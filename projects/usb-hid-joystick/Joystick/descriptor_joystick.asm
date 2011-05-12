;*******************************************************************
;                      Joystick Descriptor Module
;*******************************************************************
;
; Author:               Gunter Hatze
; Date of creation:     17-APR-2011
; Version:				1.0
; Revision history:     None
;
;*******************************************************************
; This is the descriptor for an analog CH Flightstick Pro Joystick
; that has been converted to a USB version. It uses the generic HID
; class. 

device_descriptor:
	.DB		18, 1		; Descriptor is 18 bytes long (including this byte)\
						; Identifies a Device Descriptor
	.DB		0x10, 0x01	; Version number of USB Standard that device complies with (V1.1)
	.DB		0x00, 0x00	; HID Class is defined in Interface Descriptor\
						; Subclass is Vendor Specific
	.DB		0x00, 8		; Device Protocol is Vendor Specific\
						; 8 bytes maximum Packet size for Endpoint 0
	.DB		0xA0, 0x01	; Vendor ID
	.DB		0x02, 0x00	; Product ID
	.DB		0x00, 0x01	; Release ID
	.DB		1, 2		; Index of String Descriptor describing manufacturer\
						; Index of String Descriptor describing product
	.DB		0, 1		; No String Descriptor for device serial number\
						; Number of possible configurations


configuration_descriptor:
	.DB		9, 0x02		; Descriptor is 9 bytes long (including this byte)\
						; Identifies a Configuration Descriptor
	.DB		34, 0		; Total length in bytes of data returned. This includes the length of:

						;		- This descriptor
						;		- Interface descriptor
						;		- HID Descriptor
						;		- Endpoint Descriptor

	.DB		0x01, 0x01	; Number of Interfaces supported by this Configuration\
						; Value to use to select this configuration via the SetConfiguration()
						; Request.
	.DB		0, 0x80		; No String Descriptor for this configuration\
						; Configuration characteristics:

						;		- Bit 7: always 1
						;		- Bit 6: 1 if not bus powered, 0 otherwise
						;		- Bit 5: 1 if Remote Wakeup is supported, 0 otherwise
						;		- Bits 4-0: always 0

	.DB		25, 0		; Amount of bus power the device will use\
						; Padding byte so the Atmel Compiler is happy


interface_descriptor:
	.DB		9, 0x04		; Descriptor is 9 bytes long (including this byte)\
						; Identifies an Interface Descriptor
	.DB		0x00, 0x00	; Number of this Interface Descriptor (zero based index)\
						; Value used to select this alternate setting for the Interface
						; listed in the prior byte
	.DB		0x01, 0x03	; Number of Endpoints used (excluding EP0)\
						; Class Code is HID
	.DB		0x00, 0x00	; Subclass Code is none\
						; Protocol Code is none
	.DB		0, 0		; No String Descriptor for this Interface\
						; Padding byte so the Atmel Compiler is happy


hid_descriptor:
	.DB		9, 0x21		; Descriptor is 9 bytes long (including this byte)\
						; Identifies a HID Descriptor
	.DB		0x11, 0x01	; HID Class Specification Release number (V1.1)
	.DB		0x00, 0x01	; Hardware target number\
						; Number of HID class descriptors to follow (Report Descriptor)
	.DB		0x22, 78	; Type of HID descriptor to follow is a Report Descriptor\
						; Low byte of total length of Report descriptor
	.DB		0, 0		; High byte of total length of Report descriptor\
						; Padding byte so the Atmel Compiler is happy


endpoint1_descriptor:	; EP1, IN, Interrupt
	.DB		7, 0x05		; Descriptor is 7 bytes long (including this byte)\
						; Identifies an Endpoint Descriptor
	.DB		0x81, 0x03	; Endpoint Address (0x81):

						;		- Bit 7: 1 for direction IN, 0 for direction OUT
						;		- Bits 6-4: always 0
						;		- Bits 3-0: Endpoint number

						; Endpoint Attributes 0x03):

						;		- Bits 7-2: always 0
						;		- Bits 1,0: Endpoint Transfer type (3 = Interrupt)

	.DB		8, 0		; Maximum Packet Size is 8 bytes
	.DB		10, 0		; Endpoint polling interval selectable in 1ms increments\
						;(set to 10 ms)
						; Padding byte so the Atmel Compiler is happy


;***********************************************************************************

; Report Descriptor for the Joystick:

;	- X and Y axis controller
;	- Throttle
;	- Four buttons

.INCLUDE	"joystick_new.inc"		; Report Descriptor for joystick


;Byte 	Bits 		Description
;0 		0 to 7 		Throttle
;1 		0 to 7 		X position
;2 		0 to 7 		Y position
;3 		0 to 3 		Hat switch
;		4 			Button 1
;		5 			Button 2
;		6 			Button 3
;		7 			Button 4


;While the hat switch item only requires 3 bits, it is allocated 4 bits in the
;report. This conveniently byte-aligns the remainder of the report.

;***********************************************************************************

; The following String Descriptor specifies the supported language(s)
string0_descriptor:
	.DB		4, 0x03		; Descriptor is 4 bytes long (including this byte)\
						; Identifies a String Descriptor
	.DB		0x09, 0x04	; Supported language is English - US (LANGID 0409)


; The following String Descriptor specifies the manufacturer
string1_descriptor:
	.DB		18, 0x03	; Descriptor is 18 bytes long (including this byte)\
						; Identifies a String Descriptor
	.DB		'G',0,'R',0,'P',0,'C',0,' ',0,'E',0,'n',0,'g',0		; "GRPC Eng"


; The following String Descriptor specifies the product
string2_descriptor:
	.DB		18, 0x03	; Descriptor is 18 bytes long (including this byte)\
						; Identifies a String Descriptor
	.DB		'J',0,'o',0,'y',0,'s',0,'t',0,'i',0,'c',0,'k',0
						; "Joystick"


