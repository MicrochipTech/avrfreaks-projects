//_____  I N C L U D E S ___________________________________________________
#include "main.h"
#include "dali_lib.c"
#include "stdlib.h"
//_____ M A C R O S ________________________________________________________

//_____ D E F I N I T I O N S ______________________________________________


//_____ D E C L A R A T I O N S ____________________________________________
void main(void){
  
  init_ports();
  init_light_out();
  dali_init();
  Enable_interrupt(); //enable global interrupts
  
  do{        
    if(newRx == 1){
      dali_rx_state_machine();
      execute_dali();
    }
    
    if (fadeStepFlag == 1){
      fadeStepFlag = 0;
      step_output_power(dimSense);
    }
    
  update_output_power();
  }
  while(1);
}

void init_ports(void){
   //PD4 : DALI Rx PIN (input)
  
   //PB0..7 : LED bargraph (output)
   
  // ****** Init Ports
   // DDRx  = 1 output,     DDRx  = 0 input
   // PORTx = 1 output high, PORTx = 0 output low
   // PINx is the read value at the input of the microcontroller
   // Setting PINx if the port is configured as an output makes it toggled

   DDRB  = 0xff; PORTB = 0xff; 
   DDRD  = 0x00; PORTD = 0xff; 
   DDRE  = 0x00; PORTE = 0xff;
}

void init_light_out(void){	
  
  //configure registers to generate a PWM on lamp output
  set_gtccr();
  set_tccr0a();
  set_tccr0b();
  
  set_pwm_A_duty(0);
  set_pwm_B_duty(0xff);
  
  set_tifr0();
  set_timsk0();
}	
	
void update_output_power(void)
{
  if (dali.actualDimLevel >= dali.minLevel)
  {
    dali.status.lampOn = 1;
    PWM_POWER(0xff-DIMMING_CURVE[dali.actualDimLevel-1]);  //LED output is inverted (max : 0, min 0xff)
    BAR_POWER(DIMMING_CURVE[dali.actualDimLevel-1]);
  }
  else 
  {
    dali.status.lampOn = 0;
    PWM_POWER(0xff);
    BAR_POWER(0);
  }
}     

void execute_dali(void)
{
  switch(dali.cmdType)
  {  
//NO COMMAND  
    case NONE: 
      break;
//DIRECT ARC POWER      
    case DIRECT_ARC_POWER:
    { 
      //WHAT IF INDIRECT ARC POWER RECEIVED DURING FADING ????????????
      requestedLevel = dali.dataByte;
      change_output_w_fadetime();
      dali.status.resetState = 0;
      dali.status.powerFailure = 0;
      break;
    }
//SPECIAL COMMAND
    case SPECIAL_CMD:
    {
      switch(dali.addByte){
        case TERMINATE:{
          timeOut2 = 0;
          compareMode = 0;
          physicalSelectionMode = 0;
          break;
        }
        case DATA_TRANSFER_REGISTER:{
          dali.dtr = dali.dataByte;
          break;
        }
        case INITIALISE:{
          if ((dali.dataByte == 0) ||                     //broadcast
              ((dali.dataByte & 0x7e) == (dali.shortAdd & 0x7e)))           //short address OK
          {
            timeOut2 = 3600;    //enables special commands for 3600*1/4th second = 15min.
            compareMode = 1;
          }
          break; 
        }
        case RANDOMISE:{
          if (timeOut2 != 0)    //specialMode enabled
          {
            srand(TCNT1L);
            dali.randomAddH = /*rand(TCNT1L)*/rand();
            dali.randomAddM = /*rand(dali.randomAddH)*/rand();
            dali.randomAddL = /*rand(dali.randomAddM)*/rand();  
          }
          break;
        }
        case COMPARE:{
          if ((timeOut2 != 0) && (compareMode == 1))    //specialMode enabled
          {
            if (dali.randomAddH > dali.searchAddH)
              break;
            else
            {
              if (dali.randomAddH == dali.searchAddH)
              {
                if (dali.randomAddM > dali.searchAddM)
                  break;
                else
                {
                  if (dali.randomAddM == dali.searchAddM)
                  {
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
          if (timeOut2 != 0)    //specialMode enabled
          {          
            if ((dali.randomAddH == dali.searchAddH) &&
                (dali.randomAddM == dali.searchAddM) &&
                (dali.randomAddL == dali.searchAddL))
              compareMode = 0;
          }
          break;
        }
        case SEARCHADDRH:{
          if (timeOut2 != 0)    //specialMode enabled
          {          
            dali.searchAddH = dali.dataByte;
          }
          break;
        }
        case SEARCHADDRM:{
          if (timeOut2 != 0)    //specialMode enabled
          {          
            dali.searchAddM = dali.dataByte;
          }
          break;
        }
        case SEARCHADDRL:{
          if (timeOut2 != 0)    //specialMode enabled
          {
            dali.searchAddL = dali.dataByte;
          }
          break;
        }
        case PROGRAM_SHORT_ADDRESS:{
          if (timeOut2 != 0)    //specialMode enabled
          {
            if (((dali.randomAddH == dali.searchAddH) &&
                (dali.randomAddM == dali.searchAddM) &&
                (dali.randomAddL == dali.searchAddL)) ||
                (physicalSelectionMode == 1))
            {
                dali.shortAdd = (dali.dataByte & 0x7f);
                if ((dali.shortAdd & 0x7e) == 0x7e)
                  dali.status.missingShortAdd = 1;
                else
                  dali.status.missingShortAdd = 0;     
            }
          }
          break;
        }
        
        case VERIFY_SHORT_ADDRESS:{
          if (timeOut2 != 0)    //specialMode enabled
          {
            if ((dali.shortAdd & 0x7e) == (dali.dataByte & 0x7e))
              dali_answer(YES);
          }
          break;
        }
        
        case QUERY_SHORT_ADDRESS:{
          if (timeOut2 != 0)    //specialMode enabled
          {
            if (((dali.randomAddH == dali.searchAddH) &&
                (dali.randomAddM == dali.searchAddM) &&
                (dali.randomAddL == dali.searchAddL)) ||
                (physicalSelectionMode == 1))
            {              
              dali_answer(dali.shortAdd);
            }
          }
          break;
        }
        
        case PHYSICAL_SELECTION:{
          if (timeOut2 != 0)    //specialMode enabled
          {
            physicalSelectionMode = 1;
          }
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
          break;}
        case UP_200MS:{
          up_output_w_faderate();
          break;}
        case DOWN_200MS:{
          down_output_w_faderate();
          break;}
        case STEP_UP:
        {
          if (dali.actualDimLevel < dali.maxLevel && dali.status.lampOn == ON)
            dali.actualDimLevel++;
          break;
        } 
        case STEP_DOWN:
        {
          if (dali.actualDimLevel > dali.minLevel && dali.status.lampOn == ON)
            dali.actualDimLevel--;      
          break;
        }        
        case RECALL_MAX_LEVEL:
        {
          dali.actualDimLevel = dali.maxLevel;         
          break;
        } 
        case RECALL_MIN_LEVEL:
        {
          dali.actualDimLevel = dali.minLevel;          
          break;
        } 
        case STEP_DOWN_AND_OFF:
        {
          if (dali.status.lampOn == ON)
          {
            if (dali.actualDimLevel > dali.minLevel)
              dali.actualDimLevel--;
            else
              dali.actualDimLevel = 0;
          }
          break;
        }  
        case ON_AND_STEP_UP:
        {
          if (dali.status.lampOn == ON)
          {
            if (dali.actualDimLevel < dali.maxLevel)
              dali.actualDimLevel++;
          }
          else
            dali.actualDimLevel = dali.minLevel;          
          break;
        }  
        default:
        {
          //case GO_TO_SCENE:
          if ((dali.dataByte & 0xf0) == GO_TO_SCENE)
          {
            requestedLevel = dali.scene[(dali.dataByte&0x0f)];
            change_output_w_fadetime();            
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
          break;
        }
        case STORE_THE_DTR_AS_MIN_LEVEL:{
          dali.minLevel = dali.dtr;
          if (dali.minLevel >= dali.maxLevel)
            dali.minLevel = dali.maxLevel-1;
          if (dali.actualDimLevel < dali.minLevel)
            dali.actualDimLevel = dali.minLevel;
          break;
        }
        case STORE_THE_DTR_AS_SYSTEM_FAILURE_LEVEL:{
          dali.systemFailureLevel = dali.dtr; 
          break;
        }
        case STORE_THE_DTR_AS_POWER_ON_LEVEL:{
          dali.powerOnLevel = dali.dtr; 
          break;
        }
        case STORE_THE_DTR_AS_FADE_TIME:{
          dali.fadeTime = dali.dtr & 0xf; 
          break;
        }
        case STORE_THE_DTR_AS_FADE_RATE:{
          if (dali.dtr != 0)
            dali.fadeRate = dali.dtr & 0xf; 
          break;
        }
        case STORE_DTR_AS_SHORT_ADDRESS:{
          if ((dali.shortAdd & 0x7e) == 0x7e)
            dali.status.missingShortAdd = 1;
          else
            dali.status.missingShortAdd = 0;
          break;
        }
        
        default:{
          //case STORE_THE_DTR_AS_SCENE:
          if ((dali.dataByte & 0xf0) == STORE_THE_DTR_AS_SCENE){
            dali.scene[(dali.dataByte&0x0f)] = dali.dtr;
            break;
          }
          //case REMOVE_FROM_SCENE:
          if ((dali.dataByte & 0xf0) == REMOVE_FROM_SCENE){
            dali.scene[(dali.dataByte&0x0f)] = 0xff;
            break;
          }
          if ((dali.dataByte & 0xf0) == ADD_TO_GROUP){
            dali.group = 1<<(dali.dataByte&0xf);
            break;
          }
          if ((dali.dataByte & 0xf0) == REMOVE_FROM_GROUP){
            dali.group = 0x0000;
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
