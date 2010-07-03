/**
@author vektor dewanto
@ver 8nov09
*/
#include "slaveService.h"

extern signed long int _trackDispMM;
extern uint8_t slaveStatus;

uint16_t make_16_from_2_8 (uint8_t* a)
    {
        return (uint16_t)(a[0]) | (uint16_t)(a[1]) << 8;
    }    
bool echo(Packet* p){     
   packet_send (p);               
   
   return true;
}           
bool motorForward(uint16_t mmValue){
   fwdMilimeter(mmValue);   
   return true;
}  
bool motorBackward(uint16_t mmValue){
   bwdMilimeter(mmValue);   
   return true;
}
bool _getEncoderTick(){
    Packet p; 
    uint16_t word=0;
    
    word=getEncoderTick();
    
    p.payload [0] = (uint8_t) (0x00FF & word);//Lo byte
    p.payload [1] = (uint8_t) (word >> 8);//hi byte
    packet_compute_header (&p, 2);
    packet_send (&p);
    
    return true;
}
bool sendStatus(){
	Packet p;
	p.payload[0]=slaveStatus;
	packet_compute_header (&p, 1);
	packet_send (&p);
    
	return true;
}
bool sendTrackDispMM(){
	Packet p;          
	uint16_t uint16TrackDispMM=0;
	if(_trackDispMM>=0){//dir x plus/fwd
		p.payload[0]=0;
	}else{//dir x minus/bwd
		p.payload[0]=1;
	}
	uint16TrackDispMM=(uint16_t) labs(_trackDispMM);//convert from signed long int to uint16_t
	p.payload [1] = (uint8_t) (0x00FF & uint16TrackDispMM);//Lo byte
	p.payload [2] = (uint8_t) (uint16TrackDispMM >> 8);//hi byte
	packet_compute_header (&p, 3);
	packet_send (&p);
	
	_trackDispMM=0;//after being sent, clear its value
	return true;
}
bool masterPacketProcess (Packet* p)   
    {
        //Packet response;
        bool s = true;
        if (p->length >= 1)
            {
                uint8_t function = p->payload [0];
                switch (function)
                    {
                        case ECHO:
                            echo(p);
                            break;
                        case MTR_FWD:
                            if (p->length != 3) { return false; }
                            s = motorForward( make_16_from_2_8 (p->payload + 1) );
                            break;
                        case GET_ENCTICK:
                            if (p->length != 1) { return false; }
                            s = _getEncoderTick();
                           break;
								case GET_STATUS:
									if (p->length != 1) { return false; }
									s = sendStatus();
									break;
								case GET_TRACK_DISP_MM:
									if (p->length != 1) { return false; }
									s = sendTrackDispMM();
									break;	
								case MTR_BWD:
									if (p->length != 3) { return false; }
									s = motorBackward( make_16_from_2_8 (p->payload + 1) );
									break;
                        default:
                            s = false;
                    }
            }
        else
            {
                s = false;
            }
        /*
        response.payload[0] = s;
        packet_compute_header (&response, 1);
        packet_send (&response);
        */
        return s;
    }