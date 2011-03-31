//_____ I N C L U D E S ____________________________________________________

#include "dali_lib.h"
#include "dali_code.h"
#include "stdlib.h"
#include "dali_dimming_curve.c"
#include "eeprom.c"


//_____ M A C R O S ________________________________________________________					

//_____ D E F I N I T I O N S ______________________________________________

/*U8 log_answer[128];
U8 index_answer = 0;*/

//--------------------------------------------------------------------------
//  @fn dali_rx_data(void)
//!
//! This interruption is executed each time a new byte is received.
//!
//! @warning none
//!
//! @param  none
//!
//! @return newDaliAdd, newDaliData (containing 2 received bytes), sets newRx flag.
//! 
//!
//--------------------------------------------------------------------------

#pragma vector = USART_RXC_vect
__interrupt void dali_rx_data(void);

__interrupt void dali_rx_data(void){
  newDaliAdd = EUDR;
  newDaliData = UDR;
  newRx = 1;
  return;
}

//--------------------------------------------------------------------------
//  @fn tick_1ms(void)
//!
//! This interruption is executed each ms.
//!
//! @warning none
//!
//! @param  none
//!
//! @return many flags used to generate events (fade event, timeout 100ms, timeout 15mn...)
//! 
//!
//--------------------------------------------------------------------------

#pragma vector = TIMER1_COMPA_vect
__interrupt void tick_1ms(void);

__interrupt void tick_1ms(void){
  
  //here every ms.
  if (timeOut != 0)
    timeOut--;
  
  if (prediv != 0)
    prediv--;
  else{
    //here every 1/4th second
    prediv = 250;
    if (timeOut2 != 0)
      timeOut2 --;
  }
  
  if (fadePeriod != 0){
    timeCounter++;
    if (timeCounter >= fadePeriod){
      fadeStepFlag = 1;
      timeCounter = 0;
    }
  }
  return;
}

//--------------------------------------------------------------------------
//  @fn dali_init_eusart(void)
//!
//! This function configures EUSART as described in config.h
//!
//! @warning none
//!
//! @param  none
//!
//! @return none
//!
//--------------------------------------------------------------------------

void dali_init_eusart(void){
  
  dali_set_eucsra();
  dali_set_eucsrb();
  dali_set_mubrrh();
  dali_set_mubrrl();
  dali_set_ubrrh(); 
  dali_set_ubrrl(); 
  dali_set_ucsra();
  dali_set_ucsrb();
  dali_set_ucsrc();
} 

//--------------------------------------------------------------------------
//  @fn dali_init_timer_1(void)
//!
//! This function configures TIMER 1 as described in config.h
//!
//! @warning none
//!
//! @param  none
//!
//! @return none
//!
//--------------------------------------------------------------------------

void dali_init_timer_1(void){
  
  dali_set_tccr1a();
  dali_set_tccr1b();
  dali_set_tccr1c();
  dali_set_t1_period();
  dali_set_tifr1();
  dali_set_timsk1();
} 

//--------------------------------------------------------------------------
//  @fn dali_init(void)
//!
//! This function initializes dali registers dali.xxx to the reset value
//! asked by DALI standard.
//! Launches EUSART configuration (dali_init_eusart(void))
//! and timer_1 configuration (dali_init_timer_1(void))
//!
//! @warning none
//!
//! @param  none
//!
//! @return none
//!
//--------------------------------------------------------------------------

void dali_init(void){
  
  U8 n;
  
  dali.cmdType = NONE;
  dali.addByte = 0;
  dali.dataByte = 0;
  dali.dtr = 0;
  //the following reset values are determined by dali standard
  dali.actualDimLevel     = 0xfe;
  dali.powerOnLevel       = 0xfe;
  dali.systemFailureLevel = 0xfe;
  dali.minLevel           = DALI_PHYSICAL_MIN_LEVEL;
  dali.maxLevel           = 0xfe;
  dali.fadeRate           = 0x07;
  dali.fadeTime           = 0;
  //dali.shortAdd           = 0xff;   
  dali.searchAddH         = 0xff;
  dali.searchAddM         = 0xff;
  dali.searchAddL         = 0xff;
  dali.randomAddH         = 0xff;
  dali.randomAddM         = 0xff;    
  dali.randomAddL         = 0xff;
  dali.group              = 0x0000;
  for (n=0;n<16;n++)
    dali.scene[n]         = 0xff;
  dali.scene[1] = 66;
  dali.scene[2] = 180;
  dali.scene[3] = 220;
  dali.scene[4] = 254;
/*status.statusInformation : MSB   
  dali.status.powerFailure    = NO; 
  dali.status.missingShortAdd = YES;
  dali.status.resetState      = YES;
  dali.status.fadeRunning     = NO;
  dali.status.limitError      = NO;
  dali.status.lampOn          = YES;
  dali.status.lampFailure     = NO;
  dali.status.ballastFailure  = NO;
LSB*/
  dali.status.statusInformation = 0xe4;
  
  if (ee_read(ADD_EEPROM_STATUS) == EEPROM_INITIALIZED){ //eeprom contains saved values, load
    dali.powerOnLevel = ee_read(ADD_POWER_ON_LEVEL); 
    dali.systemFailureLevel = ee_read(ADD_SYSTEM_FAILURE_LEVEL);
    dali.minLevel = ee_read(ADD_MIN_LEVEL);
    dali.maxLevel = ee_read(ADD_MAX_LEVEL);
    dali.fadeRate = ee_read(ADD_FADE_RATE);
    dali.fadeTime = ee_read(ADD_FADE_TIME);
    dali.shortAdd = ee_read(ADD_SHORT_ADD);
    dali.randomAddH = ee_read(ADD_RANDOM_ADDH);
    dali.randomAddM = ee_read(ADD_RANDOM_ADDM);
    dali.randomAddL = ee_read(ADD_RANDOM_ADDL);
    dali.group |= (ee_read(ADD_GROUPH))<<8;
    dali.group |= (ee_read(ADD_GROUPL));
    for (n=0; n<16; n++) 
      dali.scene[n] = ee_read(ADD_SCENE_0 + n);
  }
  else{  //eeprom is empty, save
    ee_write(ADD_POWER_ON_LEVEL, dali.powerOnLevel);
    ee_write(ADD_SYSTEM_FAILURE_LEVEL, dali.systemFailureLevel);
    ee_write(ADD_MIN_LEVEL, dali.minLevel);
    ee_write(ADD_MAX_LEVEL, dali.maxLevel);
    ee_write(ADD_FADE_RATE, dali.fadeRate);
    ee_write(ADD_FADE_TIME, dali.fadeTime);
    ee_write(ADD_SHORT_ADD, dali.shortAdd);
    ee_write(ADD_RANDOM_ADDH, dali.randomAddH);
    ee_write(ADD_RANDOM_ADDM, dali.randomAddM);
    ee_write(ADD_RANDOM_ADDL, dali.randomAddL);
    ee_write(ADD_GROUPH, ((U8)(dali.group>>8)));
    ee_write(ADD_GROUPL, ((U8)(dali.group)));
    for (n=0; n<16; n++) 
      ee_write((ADD_SCENE_0 + n), dali.scene[n]);
    ee_write(ADD_EEPROM_STATUS, EEPROM_INITIALIZED);
  }
    
  dali_init_eusart();
  dali_init_timer_1();
}

//--------------------------------------------------------------------------
//  @fn dali_rx_state_machine(void)
//!
//! DALI Frame decoding. This functions tests the address. If the received 
//! command corresponds to ballast address (short address, group or broadcast)
//! or if the command is a special command, dali.cmdType indicates the
//! type of command received.
//!
//! @warning none
//!
//! @param  none
//!
//! @return none
//!
//--------------------------------------------------------------------------

void dali_rx_state_machine(void){      
  
  dali.addByte = newDaliAdd;      //store received dali frame
  dali.dataByte = newDaliData;
  newRx = 0;                      //ready to receive a new frame
      
  if (((dali.addByte&0xe1) == 0xa1) || ((dali.addByte&0xe1) == 0xc1)){
    
    //SPECIAL CMD RECEIVED
    if ((dali.addByte == INITIALISE) || (dali.addByte == RANDOMISE)){
      if (timeOut == 0){   //first time command is received
        timeOut = 100;
        storedDaliAdd = dali.addByte;
        storedDaliData = dali.dataByte;
        dali.cmdType = NONE;
      }else{              //second time command is received
        timeOut = 0;      //disable 100ms timer
        if ((dali.addByte == storedDaliAdd) && (dali.dataByte == storedDaliData))
          dali.cmdType = SPECIAL_CMD;
        else 
          dali.cmdType = NONE;
      }
    }
    else
      dali.cmdType = SPECIAL_CMD;  
    return;
  }//END SPECIAL CMD
  else{
    if  (((dali.addByte&0xfe) == DALI_BROADCAST) ||            
        (((dali.addByte&0x80) == 0x00) && ((dali.addByte & 0xfe) == ((dali.shortAdd<<1) & 0xfe))) ||
        (((dali.addByte&0x80) == 0x80) && (dali.group&(1<<((dali.addByte>>1)&0x0f)))!=0)){
      //ADDRESS IS ACCEPTED
      if ((dali.addByte&0x01) == 0x00){
        //DIRECT ARC POWER COMMAND RECEIVED
        if (dali.dataByte == 0xff){
          dali.cmdType = NONE;
          return;
        }
        dali.cmdType = DIRECT_ARC_POWER;
        return;
      }
      else{
        //CLASSIC COMMAND RECEIVED
        if (dali.dataByte <= 31){
          //INDIRECT ARC POWER COMMAND RECEIVED
          dali.cmdType = INDIRECT_ARC_POWER;
          return;
        }
        else{
          if (dali.dataByte <= 128){
            //CONFIGURATION COMMAND RECEIVED (FIRST)
            //COMMAND NEEDS TO BE CONFIRMED WITHIN 100ms
            if (timeOut == 0){ //first time command is received
              timeOut = 100;
              storedDaliAdd = dali.addByte;
              storedDaliData = dali.dataByte;
              dali.cmdType = NONE;
            }
            else{              //second time command is received
              timeOut = 0; //disable 100ms timer
              if ((dali.addByte == storedDaliAdd) && (dali.dataByte == storedDaliData))
                dali.cmdType = CONFIG_CMD;
              else 
                dali.cmdType = NONE;
            }
            return;
          }
          else{
            if (dali.dataByte <= 196){
              //QUERY COMMAND RECEIVED
              dali.cmdType = QUERY_CMD;
              return;
            }    
          }
        }
      }
    }
  }
  dali.cmdType = NONE;
  return;
}

void dali_step_output_power(bit sense){
  
  dali.status.fadeRunning = 1;

  if (sense == UP){
    if (dali.actualDimLevel == 0)
      dali.actualDimLevel = dali.minLevel;
    else
      if (dali.actualDimLevel < dali.maxLevel)
        dali.actualDimLevel++;
  }
  else{
    if (dali.actualDimLevel > dali.minLevel)
      dali.actualDimLevel--;
    else
      dali.actualDimLevel = 0;
  }
  if (dali.actualDimLevel == requestedLevel){
    fadePeriod = 0;
    dali.status.fadeRunning = 0;
  }
}

void dali_change_output_w_fadetime(void){
  
  if (requestedLevel != 0xff){
    //limits checking
    dali.status.limitError = NO;
    if (requestedLevel < dali.minLevel && requestedLevel != 0){    
      requestedLevel = dali.minLevel;
      dali.status.limitError = YES;
    }
    if (requestedLevel > dali.maxLevel){
      requestedLevel = dali.maxLevel;
      dali.status.limitError = YES;
    }
    
    //fading != 0 ?
    if (dali.fadeTime != 0){      
      if (requestedLevel >  dali.actualDimLevel+1){ //minimum of 2 steps (avoid fadePeriod > maxU16)
        if (dali.actualDimLevel == 0)
          fadePeriod = FADE_TIME[dali.fadeTime]/(requestedLevel - dali.minLevel+1);           
        else
          fadePeriod = FADE_TIME[dali.fadeTime]/(requestedLevel - dali.actualDimLevel);
          
        dimSense = UP;
      }
      else{ 
        if (requestedLevel <  dali.actualDimLevel-1){ //minimum of 2 steps (avoid fadePeriod > maxU16)
          if (dali.dataByte == 0)
            fadePeriod = FADE_TIME[dali.fadeTime]/(dali.actualDimLevel - dali.minLevel+1);
          else
            fadePeriod = FADE_TIME[dali.fadeTime]/(dali.actualDimLevel - requestedLevel);
          
          dimSense = DOWN;
        }
        else{
          fadePeriod = 0;     //setting fadePeriod to 0 disables fading !
          dali.actualDimLevel = requestedLevel;
        }
      }
    }
    else{
      fadePeriod = 0;     //setting fadePeriod to 0 disables fading !
      dali.actualDimLevel = requestedLevel;
    }
  }
  else{
    requestedLevel = dali.actualDimLevel;
    fadePeriod = 0;
  }
}

void dali_up_output_w_faderate(void){  
  
    //UP
    if ((dali.fadeRate != 0) && (dali.status.lampOn == 1)){      
      if (dali.actualDimLevel < (dali.maxLevel-FADE_RATE[dali.fadeRate]))
        requestedLevel = FADE_RATE[dali.fadeRate] + dali.actualDimLevel;
      else
        requestedLevel = dali.maxLevel;
      
      if (requestedLevel != dali.actualDimLevel){
        fadePeriod = 200/(requestedLevel-dali.actualDimLevel);          
        dimSense = UP;
      }
      else
        fadePeriod = 0;
    }
    else
      fadePeriod = 0;     //setting fadePeriod to 0 disables fading !
}

void dali_down_output_w_faderate(void)
{  
    //DOWN
    if ((dali.fadeRate != 0) && (dali.status.lampOn == 1)){      
      if (dali.actualDimLevel > (dali.minLevel+FADE_RATE[dali.fadeRate]))
        requestedLevel =  dali.actualDimLevel - FADE_RATE[dali.fadeRate];
      else
        requestedLevel = dali.minLevel;
      
      if (requestedLevel != dali.actualDimLevel){
        fadePeriod = 200/(dali.actualDimLevel-requestedLevel);         
        dimSense = DOWN;
      }
      else
        fadePeriod = 0;
    }
    else
      fadePeriod = 0;     //setting fadePeriod to 0 disables fading !
}

void dali_answer(U8 answer){
  
    timeOut = 3;          //delay of 3ms (minimum time between forward & backward frames)
    while (timeOut != 0);
    dali_disable_rx();    //rx is disabled
    
    UDR = answer;         //writing UDR starts byte transmission
    
  //  log_answer[index_answer++] = answer;
    
    timeOut = 10;         //delay of 10ms (backward frame duration)
    while (timeOut != 0);
    dali_enable_rx();
}

void dali_execute(void)
{
  switch(dali.cmdType)
  {  
    
//NO COMMAND  
    case NONE: 
      break;
      
//DIRECT ARC POWER      
    case DIRECT_ARC_POWER:{ 
      requestedLevel = dali.dataByte;
      dali_change_output_w_fadetime();
      dali.status.resetState = 0;
      dali.status.powerFailure = 0;
      break;
    }
    
//SPECIAL COMMAND
    case SPECIAL_CMD:{
      
      switch(dali.addByte){
        
        case TERMINATE:{
          timeOut2 = 0;
          compareMode = 0;
          physicalSelectionMode = PHYSICAL_SELECTION_DISABLED;
          break;
        }
        case DATA_TRANSFER_REGISTER:{
          dali.dtr = dali.dataByte;
          break;
        }                
        case INITIALISE:{
          if ((dali.dataByte == 0) ||                             //broadcast
              ((dali.dataByte & 0xfe) == ((dali.shortAdd<<1) & 0xfe))) //short address OK
          {
            timeOut2 = 3600;    //enables special commands for 3600*1/4th second = 15min.
            compareMode = 1;
          }
          break; 
        }               
        case RANDOMISE:{
          if (timeOut2 != 0){    //specialMode enabled
            srand(TCNT1L);
            dali.randomAddH = /*rand(TCNT1L)*/rand();
            dali.randomAddM = /*rand(dali.randomAddH)*/rand();
            dali.randomAddL = /*rand(dali.randomAddM)*/rand();  
            ee_write(ADD_RANDOM_ADDH, dali.randomAddH);
            ee_write(ADD_RANDOM_ADDM, dali.randomAddM);
            ee_write(ADD_RANDOM_ADDL, dali.randomAddL);
          }
          break;
        }             
        case COMPARE:{
          if ((timeOut2 != 0) && (compareMode == 1)){    //specialMode enabled
            if (dali.randomAddH > dali.searchAddH)
              break;
            else{
              if (dali.randomAddH == dali.searchAddH){
                if (dali.randomAddM > dali.searchAddM)
                  break;
                else{
                  if (dali.randomAddM == dali.searchAddM){
                    if (dali.randomAddL > dali.searchAddL)
                      break;
                  }
                }
              }
            }     
            dali_answer(YES);           
          }
          break;
        }       
        case WITHDRAW:{
          if (timeOut2 != 0){    //specialMode enabled        
            if ((dali.randomAddH == dali.searchAddH) &&
                (dali.randomAddM == dali.searchAddM) &&
                (dali.randomAddL == dali.searchAddL))
              compareMode = 0;
          }
          break;
        }    
        case SEARCHADDRH:{
          if (timeOut2 != 0){    //specialMode enabled        
            dali.searchAddH = dali.dataByte;
          }
          break;
        }               
        case SEARCHADDRM:{
          if (timeOut2 != 0){    //specialMode enabled         
            dali.searchAddM = dali.dataByte;
          }
          break;
        }              
        case SEARCHADDRL:{
          if (timeOut2 != 0){    //specialMode enabled
            dali.searchAddL = dali.dataByte;
          }
          break;
        }                
        case PROGRAM_SHORT_ADDRESS:{
          if (timeOut2 != 0){    //specialMode enabled
            if (((dali.randomAddH == dali.searchAddH) &&
                (dali.randomAddM == dali.searchAddM) &&
                (dali.randomAddL == dali.searchAddL) &&
                (physicalSelectionMode == PHYSICAL_SELECTION_DISABLED)) ||
                (physicalSelectionMode == PHYSICAL_SELECTION_ENABLED))
            {
                if (dali.dataByte == 0xff){
                  dali.shortAdd = 0xff;
                  dali.status.missingShortAdd = 1;
                }
                else{
                  dali.shortAdd = dali.dataByte>>1;
                  dali.status.missingShortAdd = 0; 
                }
                ee_write(ADD_SHORT_ADD, dali.shortAdd);
            }
          }
          break;
        }       
        case VERIFY_SHORT_ADDRESS:{
          if (timeOut2 != 0){    //specialMode enabled
            if ((dali.dataByte & 0xfe) == ((dali.shortAdd<<1) & 0xfe))
              dali_answer(YES);
          }
          break;
        }               
        case QUERY_SHORT_ADDRESS:{
          if (timeOut2 != 0){    //specialMode enabled
            if (((dali.randomAddH == dali.searchAddH) &&
                (dali.randomAddM == dali.searchAddM) &&
                (dali.randomAddL == dali.searchAddL) &&
                (physicalSelectionMode == PHYSICAL_SELECTION_DISABLED)) ||
                (physicalSelectionMode == PHYSICAL_SELECTION_ENABLED))
            {              
              dali_answer((dali.shortAdd<<1) | 1);
            }
          }
          break;
        }               
        case PHYSICAL_SELECTION:{
          if (timeOut2 != 0){    //specialMode enabled
            if (physicalSelectionMode == PHYSICAL_SELECTION_DISABLED)
              physicalSelectionMode = PHYSICAL_SELECTION_REQUESTED;  //toggles physical selection mode
            else 
              physicalSelectionMode = PHYSICAL_SELECTION_DISABLED;
          }
          break;
        }
        
        default:{
          break;
        }
      }
      dali.status.resetState = 0;      
      break;
    }
    
//INDIRECT ARC POWER   
    case INDIRECT_ARC_POWER:
    {
      switch(dali.dataByte)
      {        
        case IMMEDIATE_OFF:{
          dali.actualDimLevel = 0;
          break;
        }        
        case UP_200MS:{
          dali_up_output_w_faderate();
          break;
        }        
        case DOWN_200MS:{
          dali_down_output_w_faderate();
          break;
        }        
        case STEP_UP:{
          if (dali.actualDimLevel < dali.maxLevel && dali.status.lampOn == ON)
            dali.actualDimLevel++;
          break;
        }        
        case STEP_DOWN:{
          if (dali.actualDimLevel > dali.minLevel && dali.status.lampOn == ON)
            dali.actualDimLevel--;      
          break;
        }         
        case RECALL_MAX_LEVEL:{
          dali.actualDimLevel = dali.maxLevel;         
          break;
        }         
        case RECALL_MIN_LEVEL:{
          dali.actualDimLevel = dali.minLevel;          
          break;
        }        
        case STEP_DOWN_AND_OFF:{
          if (dali.status.lampOn == ON){
            if (dali.actualDimLevel > dali.minLevel)
              dali.actualDimLevel--;
            else
              dali.actualDimLevel = 0;
          }
          break;
        }        
        case ON_AND_STEP_UP:{
          if (dali.status.lampOn == ON){
            if (dali.actualDimLevel < dali.maxLevel)
              dali.actualDimLevel++;
          }
          else
            dali.actualDimLevel = dali.minLevel;          
          break;
        } 
        default:{
          //case GO_TO_SCENE:
          if ((dali.dataByte & 0xf0) == GO_TO_SCENE){
            requestedLevel = dali.scene[(dali.dataByte&0x0f)];
            dali_change_output_w_fadetime();            
          }
          break;
        }
      }
      dali.status.resetState = 0; 
      dali.status.powerFailure = 0;
      break;
    }
    
    case CONFIG_CMD:{
      switch(dali.dataByte){
        case RESET:{
          dali_init();
          break;
        }
        case STORE_ACTUAL_DIM_LEVEL_IN_DTR:{
          dali.dtr = dali.actualDimLevel; 
          break;
        }
        case STORE_THE_DTR_AS_MAX_LEVEL:{
          dali.maxLevel = dali.dtr;
          if (dali.maxLevel <= dali.minLevel)
            dali.maxLevel = dali.minLevel+1;
          if (dali.actualDimLevel > dali.maxLevel)
            dali.actualDimLevel = dali.maxLevel;
          ee_write(ADD_MAX_LEVEL, dali.maxLevel);
          break;
        }
        case STORE_THE_DTR_AS_MIN_LEVEL:{
          dali.minLevel = dali.dtr;
          if (dali.minLevel >= dali.maxLevel)
            dali.minLevel = dali.maxLevel-1;
          if (dali.actualDimLevel < dali.minLevel)
            dali.actualDimLevel = dali.minLevel;
          ee_write(ADD_MIN_LEVEL, dali.minLevel);
          break;
        }
        case STORE_THE_DTR_AS_SYSTEM_FAILURE_LEVEL:{
          dali.systemFailureLevel = dali.dtr; 
          ee_write(ADD_SYSTEM_FAILURE_LEVEL, dali.systemFailureLevel);
          break;
        }
        case STORE_THE_DTR_AS_POWER_ON_LEVEL:{
          dali.powerOnLevel = dali.dtr; 
          ee_write(ADD_POWER_ON_LEVEL, dali.powerOnLevel);
          break;
        }
        case STORE_THE_DTR_AS_FADE_TIME:{
          dali.fadeTime = dali.dtr & 0xf; 
          ee_write(ADD_FADE_TIME, dali.fadeTime);
          break;
        }
        case STORE_THE_DTR_AS_FADE_RATE:{
          if (dali.dtr != 0)
          {
            dali.fadeRate = dali.dtr & 0xf; 
            ee_write(ADD_FADE_RATE, dali.fadeRate);
          }
          break;
        }
        case STORE_DTR_AS_SHORT_ADDRESS:{
          if (dali.dtr == 0xff)
          {
            dali.shortAdd = 0xff;
            dali.status.missingShortAdd = 1;
          }
          else
          {
            dali.shortAdd = dali.dtr & 0x3f;
            dali.status.missingShortAdd = 0;
          }
          ee_write(ADD_SHORT_ADD, dali.shortAdd);
          break;
        }
        
        default:{
          //case STORE_THE_DTR_AS_SCENE:
          if ((dali.dataByte & 0xf0) == STORE_THE_DTR_AS_SCENE){
            dali.scene[(dali.dataByte&0x0f)] = dali.dtr;
            ee_write((ADD_SCENE_0 + (dali.dataByte&0x0f)), dali.scene[(dali.dataByte&0xf)]);
            break;
          }
          //case REMOVE_FROM_SCENE:
          if ((dali.dataByte & 0xf0) == REMOVE_FROM_SCENE){
            dali.scene[(dali.dataByte&0x0f)] = 0xff;
            ee_write((ADD_SCENE_0 + (dali.dataByte&0x0f)), 0xff);
            break;
          }
          if ((dali.dataByte & 0xf0) == ADD_TO_GROUP){
            dali.group = 1<<(dali.dataByte&0xf);
            ee_write(ADD_GROUPH, ((U8)(dali.group>>8)));
            ee_write(ADD_GROUPL, ((U8)(dali.group)));
            break;
          }
          if ((dali.dataByte & 0xf0) == REMOVE_FROM_GROUP){
            dali.group = 0x0000;
            ee_write(ADD_GROUPH, 0);
            ee_write(ADD_GROUPL, 0);
            break;
          }
        }//END default:
      }//END switch(dali.databyte); 
      dali.status.resetState = 0;      
      break;
    }//END CASE CONFIG_CMD
    
    case QUERY_CMD:{
      
      switch(dali.dataByte){
        case QUERY_STATUS:{      
          dali_answer(dali.status.statusInformation);
          break;
        }
        case QUERY_BALLAST:{
          dali_answer(YES);
          break;
        }
        case QUERY_LAMP_FAILURE:{
          if (dali.status.lampFailure == 1)
            dali_answer(YES);
          //else NO = no answer...
          break;
        }
        case QUERY_LAMP_POWER_ON:{
          if (dali.status.lampOn == 1)
            dali_answer(YES);
          break;
        }
        case QUERY_LIMIT_ERROR:{
          if (dali.status.limitError == 1)
            dali_answer(YES);
          break;
        }
        case QUERY_RESET_STATE:{
          if (dali.status.resetState == 1)
            dali_answer(YES);
          break;
        }
        case QUERY_MISSING_SHORT_ADDRESS:{
          if (dali.status.missingShortAdd == 1)
            dali_answer(YES);
          break;
        }       
        case QUERY_VERSION_NUMBER:{
          dali_answer(DALI_VERSION_NUMBER);
          break;
        }    
        case QUERY_CONTENT_DTR:{
          dali_answer(dali.dtr);
          break;
        }   
        case QUERY_DEVICE_TYPE:{
          dali_answer(DALI_DEVICE_TYPE);
          break;
        }  
        case QUERY_PHYSICAL_MINIMUM_LEVEL :{
          dali_answer(DALI_PHYSICAL_MIN_LEVEL);
          break;
        }  
        case QUERY_POWER_FAILURE :{
          dali_answer(dali.status.powerFailure);
          break;
        }  
        case QUERY_ACTUAL_LEVEL :{
          dali_answer(dali.actualDimLevel);
          break;
        }  
        case QUERY_MAX_LEVEL :{
          dali_answer(dali.maxLevel);
          break;
        } 
        case QUERY_MIN_LEVEL :{
          dali_answer(dali.minLevel);
          break;
        }         
        case QUERY_POWER_ON_LEVEL :{
          dali_answer(dali.powerOnLevel);
          break;
        }  
        case QUERY_SYSTEM_FAILURE_LEVEL :{
          dali_answer(dali.systemFailureLevel);
          break;
        }   
        case QUERY_FADE :{
          dali_answer(dali.fadeTime<<4 | dali.fadeRate);
          break;
        }  
        case QUERY_GROUPS_0_7:{
          dali_answer((U8)(dali.group));
          break;
        }
        case QUERY_GROUPS_8_15:{
          dali_answer((U8)(dali.group>>8));
          break;
        } 
        case QUERY_RANDOM_ADDRESS_H:{
          dali_answer(dali.randomAddH);
          break;
        }  
        case QUERY_RANDOM_ADDRESS_M:{
          dali_answer(dali.randomAddM);
          break;
        }   
        case QUERY_RANDOM_ADDRESS_L:{
          dali_answer(dali.randomAddL);
          break;
        }   
        
        default:{
          if ((dali.dataByte & 0xf0) == QUERY_SCENE_LEVEL){
            dali_answer(dali.scene[(dali.dataByte&0x0f)]);
            break;
          } 
          break;
        }
      }//END switch(dali.dataByte)
      break;
    }//END CASE QUERY_CMD
      
    default:break;
  }
}


