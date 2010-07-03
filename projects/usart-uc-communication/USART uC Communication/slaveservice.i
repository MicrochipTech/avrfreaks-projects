
typedef signed char int8_t;
typedef unsigned char uint8_t;

typedef short int16_t;
typedef unsigned short uint16_t;

typedef long int32_t;
typedef unsigned long uint32_t;

typedef short intptr_t;
typedef unsigned short uintptr_t;

typedef short flash_intptr_t;
typedef unsigned short flash_uintptr_t;

typedef short eeprom_intptr_t;
typedef unsigned short eeprom_uintptr_t;

typedef struct
_Packet
{
uint8_t checksum;
uint8_t length; 
uint8_t payload [14];
}
Packet;

#pragma used+
_Bool packet_check (Packet* packet);
_Bool packet_compute_header (Packet* packet, uint8_t length);
_Bool packet_send (Packet* packet);
_Bool packet_receive (Packet* packet);
#pragma used-

#pragma used+
uint16_t make_16_from_2_8 (uint8_t* a);
_Bool echo(Packet* p);
_Bool motorForward(uint16_t mmValue);
_Bool motorBackward(uint16_t mmValue);
_Bool _getEncoderTick();
_Bool sendStatus();
_Bool sendTrackDispMM();
_Bool masterPacketProcess (Packet* p);
#pragma used-

extern signed long int _trackDispMM;
extern uint8_t slaveStatus;

uint16_t make_16_from_2_8 (uint8_t* a)
{
return (uint16_t)(a[0]) | (uint16_t)(a[1]) << 8;
}    
_Bool echo(Packet* p){     
packet_send (p);               

return 1;
}           
_Bool motorForward(uint16_t mmValue){
fwdMilimeter(mmValue);   
return 1;
}  
_Bool motorBackward(uint16_t mmValue){
bwdMilimeter(mmValue);   
return 1;
}
_Bool _getEncoderTick(){
Packet p; 
uint16_t word=0;

word=getEncoderTick();

p.payload [0] = (uint8_t) (0x00FF & word);
p.payload [1] = (uint8_t) (word >> 8);
packet_compute_header (&p, 2);
packet_send (&p);

return 1;
}
_Bool sendStatus(){
Packet p;
p.payload[0]=slaveStatus;
packet_compute_header (&p, 1);
packet_send (&p);

return 1;
}
_Bool sendTrackDispMM(){
Packet p;          
uint16_t uint16TrackDispMM=0;
if(_trackDispMM>=0){
p.payload[0]=0;
}else{
p.payload[0]=1;
}
uint16TrackDispMM=(uint16_t) labs(_trackDispMM);
p.payload [1] = (uint8_t) (0x00FF & uint16TrackDispMM);
p.payload [2] = (uint8_t) (uint16TrackDispMM >> 8);
packet_compute_header (&p, 3);
packet_send (&p);

_trackDispMM=0;
return 1;
}
_Bool masterPacketProcess (Packet* p)   
{

_Bool s = 1;
if (p->length >= 1)
{
uint8_t function = p->payload [0];
switch (function)
{
case 0:
echo(p);
break;
case 1:
if (p->length != 3) { return 0; }
s = motorForward( make_16_from_2_8 (p->payload + 1) );
break;
case 2:
if (p->length != 1) { return 0; }
s = _getEncoderTick();
break;
case 3:
if (p->length != 1) { return 0; }
s = sendStatus();
break;
case 4:
if (p->length != 1) { return 0; }
s = sendTrackDispMM();
break;	
case 5:
if (p->length != 3) { return 0; }
s = motorBackward( make_16_from_2_8 (p->payload + 1) );
break;
default:
s = 0;
}
}
else
{
s = 0;
}

return s;
}
