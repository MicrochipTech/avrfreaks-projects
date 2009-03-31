// first 8 bytes of Bytes array is used to store returned controller data..  last 25 are used to send command sequence to
// the controller to request data Bytes[0..7]  contain data from controller.
// this is an example of how to use the two files to get the data from the controller.
// remember to include. the getdata.S file in the makefile under Assembler Sources.


uint8_t Bytes[]={1,2,3,4,5,6,7,8,0,1,0,0,0,0,0,0,0,0,0,0,0,0,1,1,0,0,0,0,0,0,1,0,1};

extern void GetData(uint16_t);

int main(void)
{
	GetData(Bytes);
	Byte[??] to access each byte
}
