#ifndef __CW
#define __CW

//------------------------------------------------------------------------------------------------------------
//
//	ascii to morse code translation array, containing cw characters.
//
//	char array starting with morse code char for ' ' (0x20) 
//	and ends with morse code char for '_'(0x5f).
//	index to access chars is = toupper( ascii value ) - 0x20.
//	morse code characters buildt as follows:
//	start sign = 1 + dash = 1 or dot = 0, right adjusted in byte
//
//------------------------------------------------------------------------------------------------------------
char chr[64] = { 	0x00, 0x6c, 0x52, 0x4c, 0x89, 0x4c, 0x28, 0x5e,		// !"#$%&' (#% replaced with ?(0x4c))
					0x36, 0x6d, 0x4c, 0x2a, 0x4c, 0x61, 0x55, 0x32, 	//()*+�-./ (*� replaced with ?)
					0x3f, 0x2f, 0x27, 0x23, 0x21, 0x20, 0x30, 0x38,		//01234567 
					0x3c, 0x3e, 0x78, 0x6a, 0x4c, 0x31, 0x4c, 0x4c, 	//89:;<=>? (<> replaced with ?)
					0x5a, 0x05, 0x18, 0x1a, 0x0c, 0x02, 0x12, 0x0e, 	//@ABCDEFG
					0x10, 0x04, 0x17, 0x0d, 0x14, 0x07, 0x06, 0x0f, 	//HIJKLMNO	
					0x16, 0x1d, 0x0a, 0x08, 0x03, 0x09, 0x11, 0x0b, 	//PQRSTUVW
					0x19, 0x1b, 0x1c, 0x4c, 0x4c, 0x4c, 0x4c, 0x4d }; 	//XYZ[\]^_ ([\]^ repaced with ?)
//
//prosigns
//
char cwAS = 0x28;	//.-...  ('�', wait, built as above)
char cwSK = 0x45;   //...-.- ('�', end of transmission, built as above)

#endif
