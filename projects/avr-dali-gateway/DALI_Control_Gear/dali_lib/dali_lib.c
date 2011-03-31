//_____ I N C L U D E S ____________________________________________________

#include "dali_lib.h"
#include "dali_config.h"
#include "dali_code.h"
#include "stdlib.h"
#include "dali_dimming_curve.c"
#include "eeprom.c"
#include "dali_special_command.c"
#include "dali_indirect_arc_power.c"
#include "dali_query_command.c"



//_____ M A C R O S ________________________________________________________					

//_____ D E F I N I T I O N S ______________________________________________

  /*** this interrupt routine is called each time a new dali frame is received ***/

#pragma vector = USART_RXC_vect
__interrupt void dali_rx_data(void);

__interrupt void dali_rx_data(void){
  
  new_dali_add = EUDR;
  new_dali_data = UDR;
  new_rx = 1;
  return;
}

  /*** configure mcu peripherals. see "dali_config.h" ***/

void dali_init_eusart(void){
  
  Dali_set_eucsra();
  Dali_set_eucsrb();
  Dali_set_mubrrh();
  Dali_set_mubrrl();
  Dali_set_ubrrh();
  Dali_set_ubrrl(); 
  Dali_set_ucsra();
  Dali_set_ucsrb();
  Dali_set_ucsrc();
} 

  /*** initialise DALI registers ***/
  /*** load eeprom if previous values exist ***/

void dali_init(void){
  
  U8 n;
  
  dali.cmd_type = NONE;
  dali.add_byte = 0;
  dali.data_byte = 0;
  dali.dtr = 0;
  //the following reset values are determined by dali standard
  dali.actual_dim_level     = 0xfe;
  dali.power_on_level       = 0xfe;
  dali.system_failure_level = 0xfe;
  dali.min_level           = DALI_PHYSICAL_MIN_LEVEL;
  dali.max_level           = 0xfe;
  dali.fade_rate           = 0x07;
  dali.fade_time           = 0; 
  dali.search_add_H         = 0xff;
  dali.search_add_M         = 0xff;
  dali.search_add_L         = 0xff;
  dali.random_add_H         = 0xff;
  dali.random_add_M         = 0xff;    
  dali.random_add_L         = 0xff;
  dali.group              = 0x0000;
  for (n=0;n<16;n++)
    dali.scene[n]         = 0xff;
/*status.status_information : MSB   
  dali.status.power_failure    = NO; 
  dali.status.missing_short_add = YES;
  dali.status.reset_state      = YES;
  dali.status.fade_running     = NO;
  dali.status.limit_error      = NO;
  dali.status.lamp_on          = YES;
  dali.status.lamp_failure     = NO;
  dali.status.ballastPresent  = NO;
LSB*/
  dali.status.status_information = 0xe4;
  
  if (ee_read(ADD_EEPROM_STATUS) == EEPROM_INITIALIZED){ 
    //eeprom contains previsously saved values, load
    dali.power_on_level = ee_read(ADD_POWER_ON_LEVEL); 
    dali.system_failure_level = ee_read(ADD_SYSTEM_FAILURE_LEVEL);
    dali.min_level = ee_read(ADD_MIN_LEVEL);
    dali.max_level = ee_read(ADD_MAX_LEVEL);
    dali.fade_rate = ee_read(ADD_FADE_RATE);
    dali.fade_time = ee_read(ADD_FADE_TIME);
    dali.short_add = ee_read(ADD_SHORT_ADD);
    dali.random_add_H = ee_read(ADD_RANDOM_ADDH);
    dali.random_add_M = ee_read(ADD_RANDOM_ADDM);
    dali.random_add_L = ee_read(ADD_RANDOM_ADDL);
    dali.group |= (ee_read(ADD_GROUPH))<<8;
    dali.group |= (ee_read(ADD_GROUPL));
    for (n=0; n<16; n++) 
      dali.scene[n] = ee_read(ADD_SCENE_0 + n);
    //if eeprom is loaded, the device is not in reset state any more.
    dali.status.reset_state = 0;
  }
  else{  
    //eeprom is empty, save
    ee_write(ADD_POWER_ON_LEVEL, dali.power_on_level);
    ee_write(ADD_SYSTEM_FAILURE_LEVEL, dali.system_failure_level);
    ee_write(ADD_MIN_LEVEL, dali.min_level);
    ee_write(ADD_MAX_LEVEL, dali.max_level);
    ee_write(ADD_FADE_RATE, dali.fade_rate);
    ee_write(ADD_FADE_TIME, dali.fade_time);
    ee_write(ADD_SHORT_ADD, dali.short_add);
    ee_write(ADD_RANDOM_ADDH, dali.random_add_H);
    ee_write(ADD_RANDOM_ADDM, dali.random_add_M);
    ee_write(ADD_RANDOM_ADDL, dali.random_add_L);
    ee_write(ADD_GROUPH, ((U8)(dali.group>>8)));
    ee_write(ADD_GROUPL, ((U8)(dali.group)));
    for (n=0; n<16; n++) 
      ee_write((ADD_SCENE_0 + n), dali.scene[n]);
    ee_write(ADD_EEPROM_STATUS, EEPROM_INITIALIZED);
  }
  
  //check if short address exists :
  if (dali.short_add == 0xff)
    dali.status.missing_short_add = 1;
  else
    dali.status.missing_short_add = 0;
  
  dali.actual_dim_level = dali.power_on_level;
  
  dali_init_eusart();
}

  /*** this function is called every x ms and update counters ***/

void dali_tick(void){
#if (TICK_PERIOD > 1000)
  tick_1ms_counter += ((U8)(TICK_PERIOD/1000));
  if (time_out >= ((U8)(TICK_PERIOD/1000))){ 
    time_out -= ((U8)(TICK_PERIOD/1000));
  }else{ 
    time_out = 0;
  }
#else
  static U8 prediv = ((U8)((1000/TICK_PERIOD) - 1));
  if (prediv != 0){
    prediv--;
  }else{  
    prediv = ((U8)((1000/TICK_PERIOD) - 1));
    tick_1ms_counter ++;
    if (time_out != 0){ 
      time_out --;
    }
  }
#endif
  return;
}

  /*** address_byte processing. updates cmd_type value ***/

void dali_rx_state_machine(void){      
  
  static U8 stored_dali_add = 0;
  static U8 stored_dali_data = 0;

  dali.add_byte = new_dali_add;      //store received dali frame
  dali.data_byte = new_dali_data;    
      
  if (((dali.add_byte&0xe1) == 0xa1) || ((dali.add_byte&0xe1) == 0xc1)){
    
    //SPECIAL CMD RECEIVED (101x xxx1 or 110x xxx1)
    if ((dali.add_byte == INITIALISE) || (dali.add_byte == RANDOMISE)){
      if (time_out == 0){   //first time command is received
        time_out = FW_FW_DELAY;     //100ms delay (84 + FW frame duration)
        stored_dali_add = dali.add_byte;
        stored_dali_data = dali.data_byte;
        dali.cmd_type = NONE;
      }else{                //second time command is received
        time_out = 0;      //disable 100ms timer
        if ((dali.add_byte == stored_dali_add) && (dali.data_byte == stored_dali_data))
          dali.cmd_type = SPECIAL_CMD;
        else 
          dali.cmd_type = NONE;
      }
    }
    else
      dali.cmd_type = SPECIAL_CMD;  
    return;
  }//END SPECIAL CMD
  else{
    if  (((dali.add_byte&0xfe) == DALI_BROADCAST) ||            
        (((dali.add_byte&0x80) == 0x00) && ((dali.add_byte & 0xfe) == ((dali.short_add<<1) & 0xfe))) ||
        (((dali.add_byte&0x80) == 0x80) && (dali.group&(1<<((dali.add_byte>>1)&0x0f)))!=0)){
      //ADDRESS IS ACCEPTED
      if ((dali.add_byte&0x01) == 0x00){
        //DIRECT ARC POWER COMMAND RECEIVED
        if (dali.data_byte == 0xff){
          dali.cmd_type = NONE;
          return;
        }
        dali.cmd_type = DIRECT_ARC_POWER;
        return;
      }
      else{
        //CLASSIC COMMAND RECEIVED
        if (dali.data_byte <= 31){
          //INDIRECT ARC POWER COMMAND RECEIVED
          dali.cmd_type = INDIRECT_ARC_POWER;
          return;
        }
        else{
          if (dali.data_byte <= 128){
            //CONFIGURATION COMMAND RECEIVED (FIRST)
            //COMMAND NEEDS TO BE CONFIRMED WITHIN 100ms
            if (time_out == 0){ //first time command is received
              time_out = FW_FW_DELAY;
              stored_dali_add = dali.add_byte;
              stored_dali_data = dali.data_byte;
              dali.cmd_type = NONE;
            }
            else{             //second time command is received
              time_out = 0; //disable 100ms timer
              if ((dali.add_byte == stored_dali_add) && (dali.data_byte == stored_dali_data))
                dali.cmd_type = CONFIG_CMD;
              else 
                dali.cmd_type = NONE;
            }
            return;
          }
          else{
            if (dali.data_byte <= 196){
              //QUERY COMMAND RECEIVED
              dali.cmd_type = QUERY_CMD;
              return;
            }    
          }
        }
      }
    }
  }
  dali.cmd_type = NONE;
  return;
}

  /*** updates arc_power_level during fading ***/

void dali_step_output_power(bit sense){
  
  dali.status.fade_running = 1;

  if (sense == UP){
    //step up arc power level
    if (dali.actual_dim_level == 0)
      dali.actual_dim_level = dali.min_level;
    else
      if (dali.actual_dim_level < dali.max_level)
        dali.actual_dim_level++;
  }
  else{
    //step down arc power level
    if (dali.actual_dim_level > dali.min_level)
      dali.actual_dim_level--;
    else
      dali.actual_dim_level = 0;
  }
  //disable fading if requested level is reached
  if (dali.actual_dim_level == requested_level){
    fade_period = 0;
    dali.status.fade_running = 0;
  }
}

  /*** used with 'DIRECT ARC POWER' command ***/

void dali_change_output_w_fade_time(void){
  
  if (requested_level != 0xff){
    //limits checking
    dali.status.limit_error = NO;
    if (requested_level < dali.min_level && requested_level != 0){    
      requested_level = dali.min_level;
      dali.status.limit_error = YES;
    }
    if (requested_level > dali.max_level){
      requested_level = dali.max_level;
      dali.status.limit_error = YES;
    }
    
    //fading != 0 ?
    if (dali.fade_time != 0){      
      if (requested_level >  dali.actual_dim_level+1){ //minimum of 2 steps (avoid fade_period > maxU16)
        if (dali.actual_dim_level == 0)
          fade_period = FADE_TIME[dali.fade_time]/(requested_level - dali.min_level+1);           
        else
          fade_period = FADE_TIME[dali.fade_time]/(requested_level - dali.actual_dim_level);
          
        dim_sense = UP;
      }else{ 
        if (requested_level <  dali.actual_dim_level-1){ //minimum of 2 steps (avoid fade_period > maxU16)
          if (dali.data_byte == 0)
            fade_period = FADE_TIME[dali.fade_time]/(dali.actual_dim_level - dali.min_level+1);
          else
            fade_period = FADE_TIME[dali.fade_time]/(dali.actual_dim_level - requested_level);
          
          dim_sense = DOWN;
        }else{
          //requested level is closed to actual level, fading will not be enabled
          fade_period = 0;     //setting fade_period to 0 disables fading !
          dali.actual_dim_level = requested_level;
        }
      }
      if (dali.fade_time == 15){
        //90.510s value does not fit in U16. 
        //FADE_TIME[15] = 45.255 and is then multiplied by 2
        fade_period = fade_period<<1;
      }
    }
    else{
      fade_period = 0;     //setting fade_period to 0 disables fading !
      dali.actual_dim_level = requested_level;
    }
  }
  else{
    //fade time == 0, immediately set the requested level
    requested_level = dali.actual_dim_level;
    fade_period = 0;
  }
}

  /*** used with 'UP' command ***/

void dali_up_output_w_fade_rate(void){  
  
    //check if the lamp is on, and fade rate is != 0
    if ((dali.fade_rate != 0) && (dali.status.lamp_on == 1)){   
      //check limits
      if (dali.actual_dim_level < (dali.max_level-FADE_RATE[dali.fade_rate]))
        requested_level = FADE_RATE[dali.fade_rate] + dali.actual_dim_level;
      else
        requested_level = dali.max_level;
      
      if (requested_level != dali.actual_dim_level){
        fade_period = 200/(requested_level-dali.actual_dim_level);          
        dim_sense = UP;
      }
      else
        fade_period = 0;
    }
    else
      fade_period = 0;     //setting fade_period to 0 disables fading !
}

  /*** used with 'UP' command ***/

void dali_down_output_w_fade_rate(void)
{  
    //check if the lamp is on, and fade rate is != 0
    if ((dali.fade_rate != 0) && (dali.status.lamp_on == 1)){    
      //check limits
      if (dali.actual_dim_level > (dali.min_level+FADE_RATE[dali.fade_rate]))
        requested_level =  dali.actual_dim_level - FADE_RATE[dali.fade_rate];
      else
        requested_level = dali.min_level;
      
      if (requested_level != dali.actual_dim_level){
        fade_period = 200/(dali.actual_dim_level-requested_level);         
        dim_sense = DOWN;
      }
      else
        fade_period = 0;
    }
    else
      fade_period = 0;     //setting fade_period to 0 disables fading !
}

  /*** sends a byte in a BW frame ***/

void dali_answer(U8 answer){
  
    time_out = 3;          //delay of 3ms (minimum time between forward & backward frames)
    while (time_out != 0);
    Dali_disable_rx();    //rx is disabled
    
    UDR = answer;         //writing UDR starts byte transmission
    
    time_out = 10;        //delay of 10ms (backward frame duration)
    while (time_out != 0);
    Dali_enable_rx();
}

  /*** Data_byte processing ***/

void dali_execute(void)
{
  U8 cmd_number = 0;
  
  switch(dali.cmd_type)
  {  
    
//NO COMMAND  
    case NONE: 
      break;
      
//DIRECT ARC POWER      
    case DIRECT_ARC_POWER:{ 
      requested_level = dali.data_byte;
      dali_change_output_w_fade_time();
      dali.status.reset_state = 0;
      dali.status.power_failure = 0;
      break;
    }
    
//SPECIAL COMMAND 
    case SPECIAL_CMD:{
      //special_command[] contains function pointers
      //cmd_number calculates the appropriate index of special_command[] to 
      //call the appropriate function
      cmd_number = (dali.add_byte - TERMINATE)>>1;
      if (cmd_number < 15){
        special_command[cmd_number]();
      }
      dali.status.reset_state = 0;      
      break;
    }
    
//INDIRECT ARC POWER   
    case INDIRECT_ARC_POWER:
    {
      //indirect_arc_power[] contains function pointers
      //cmd_number calculates the appropriate index of indirect_arc_power[] to 
      //call the appropriate function
      cmd_number = dali.data_byte&0x1f;
      
      if (cmd_number <= 8)
        indirect_arc_power[cmd_number]();  //9 first indirect power functions
      else
        indirect_arc_power[10]();          //case go_to_scene;
      
      dali.status.reset_state = 0; 
      dali.status.power_failure = 0;
      break;
    }

//CONFIGURATION COMMANDS
    case CONFIG_CMD:{
      dali.status.reset_state = 0;
      
      switch(dali.data_byte){
      case RESET:{
        ee_write(ADD_EEPROM_STATUS, 0); //clears the flag EEPROM_INITIALIZED
        dali_init();                    //reset values will be stored in eeprom
        dali.status.reset_state = 1; 
        break;
      }
      case STORE_ACTUAL_DIM_LEVEL_IN_DTR:{
        dali.dtr = dali.actual_dim_level; 
        break;
      }
      case STORE_THE_DTR_AS_MAX_LEVEL:{
        dali.max_level = dali.dtr;
        if (dali.max_level <= dali.min_level)
          dali.max_level = dali.min_level+1;
        if (dali.actual_dim_level > dali.max_level)
          dali.actual_dim_level = dali.max_level;
        ee_write(ADD_MAX_LEVEL, dali.max_level);
        break;
      }
      case STORE_THE_DTR_AS_MIN_LEVEL:{
        dali.min_level = dali.dtr;
        if (dali.min_level >= dali.max_level)
          dali.min_level = dali.max_level-1;
        if (dali.actual_dim_level < dali.min_level)
          dali.actual_dim_level = dali.min_level;
        ee_write(ADD_MIN_LEVEL, dali.min_level);
        break;
      }
      case STORE_THE_DTR_AS_SYSTEM_FAILURE_LEVEL:{
        dali.system_failure_level = dali.dtr; 
        ee_write(ADD_SYSTEM_FAILURE_LEVEL, dali.system_failure_level);
        break;
      }
      case STORE_THE_DTR_AS_POWER_ON_LEVEL:{
        dali.power_on_level = dali.dtr; 
        if (dali.power_on_level > 254)
          dali.power_on_level = 254;
        ee_write(ADD_POWER_ON_LEVEL, dali.power_on_level);
        break;
      }
      case STORE_THE_DTR_AS_FADE_TIME:{
        dali.fade_time = dali.dtr & 0xf; 
        ee_write(ADD_FADE_TIME, dali.fade_time);
        break;
      }
      case STORE_THE_DTR_AS_FADE_RATE:{
        if (dali.dtr != 0)  //value 0 is not allowed for fade_rate
        {
          dali.fade_rate = dali.dtr & 0xf; 
          ee_write(ADD_FADE_RATE, dali.fade_rate);
        }
        break;
      }
      case STORE_DTR_AS_SHORT_ADDRESS:{
        if (dali.dtr == 0xff)
        {
          dali.short_add = 0xff;
          dali.status.missing_short_add = 1;
        }
        else
        {
          dali.short_add = dali.dtr & 0x3f;
          dali.status.missing_short_add = 0;
        }
        ee_write(ADD_SHORT_ADD, dali.short_add);
        break;
      }
      
      default:{
        //case STORE_THE_DTR_AS_SCENE:
        if ((dali.data_byte & 0xf0) == STORE_THE_DTR_AS_SCENE){
          dali.scene[(dali.data_byte&0x0f)] = dali.dtr;
          ee_write((ADD_SCENE_0 + (dali.data_byte&0x0f)), dali.scene[(dali.data_byte&0xf)]);
          break;
        }
        //case REMOVE_FROM_SCENE:
        if ((dali.data_byte & 0xf0) == REMOVE_FROM_SCENE){
          dali.scene[(dali.data_byte&0x0f)] = 0xff;
          ee_write((ADD_SCENE_0 + (dali.data_byte&0x0f)), 0xff);
          break;
        }
        //case ADD_TO_GROUP
        if ((dali.data_byte & 0xf0) == ADD_TO_GROUP){
          dali.group = 1<<(dali.data_byte&0xf);
          ee_write(ADD_GROUPH, ((U8)(dali.group>>8)));
          ee_write(ADD_GROUPL, ((U8)(dali.group)));
          break;
        }
        //case REMOVE_FROM_GROUP
        if ((dali.data_byte & 0xf0) == REMOVE_FROM_GROUP){
          dali.group = 0x0000;
          ee_write(ADD_GROUPH, 0);
          ee_write(ADD_GROUPL, 0);
          break;
        }
      }//END default:
    }//END switch(dali.data_byte);      
    break;
  }//END CASE CONFIG_CMD
  
//QUERY COMMANDS
    case QUERY_CMD:{
      
      cmd_number = dali.data_byte;
      
      if ((cmd_number > QUERY_FADE) && (cmd_number < QUERY_SCENE_LEVEL))
        break; //reserved functions
      if ((cmd_number & 0xf0) == QUERY_SCENE_LEVEL)
        cmd_number &= 0xf0;
      
      if (cmd_number >= QUERY_GROUPS_0_7){     //recalculates index considering that SCENE_LEVEL is actually 16 functions
        cmd_number -= 25;
      }
      else{
        if (cmd_number >= QUERY_SCENE_LEVEL)    //recalculates index considering that commands 166-175 are reserved        
          cmd_number -= 10;
      }
      
      cmd_number = cmd_number - QUERY_STATUS;
      if (cmd_number < 28){
        query_command[cmd_number](); 
      }  
  
      break;
    }//END CASE QUERY_CMD
    
    default:break;
  }
}

  /*** update arc power level accordingly to the dimming curve ***/
  /* returns the output level required, in percentage */
  /* 0 : lamp off, 255 : 100% power */

U8 dali_output_power(void){
  //output power level is contained in dali.actual_dim_level
  //if dali.actual_dim_level < dali.min_level, the lamp is off
  
  if (dali.actual_dim_level >= dali.min_level)
  {
    dali.status.lamp_on = 1;
    return DIMMING_CURVE[dali.actual_dim_level-1];
  }
  else 
  {
    dali.status.lamp_on = 0;
    return 0;
  }
} 

  /*** dali main function ***/
  /* shall be called every 1 ms */

U8 dali_control_gear(U8 lamp_failure)
{
  U8 output_level;        
  static U16 time_counter = 0;  //counts nb of ms elapsed...
  static U8 prediv = 0;
  
  if (tick_1ms_counter != 0){
    //a new tick occured
  
    if (prediv >= tick_1ms_counter){
      prediv -= tick_1ms_counter;
    }else{
      //here every 1/4th second
      prediv = 250;
      if (time_out_2 != 0){
        time_out_2 --;
      }
    }
  
    if (fade_period != 0){
      //check if fading is enabled
      time_counter += tick_1ms_counter;
      if (time_counter >= fade_period){
        //update arc power level for fading        
        dali_step_output_power(dim_sense);
        time_counter = 0;
      }
    }
    tick_1ms_counter = 0;
  }
  //if a new frame is received,
  //if there was no frame error,
  //then process
  if((new_rx == 1) /*&& ((EUCSRC & (1<<FEM)) == 0)*/){
    new_rx = 0;
    dali_rx_state_machine();
    dali_execute();     
  }

  //output level is the arc_power_level required, in percentage
  // 0 : lamp off, 255 : 100% power
  output_level = dali_output_power();
  
  //check if a lamp is physically disconnected
  if (lamp_failure == 1)
  {
    dali.status.lamp_failure = 1;
    if (physical_selection_mode == PHYSICAL_SELECTION_REQUESTED)
      physical_selection_mode = PHYSICAL_SELECTION_ENABLED;
  }
  else
  {
    dali.status.lamp_failure = 0;
    if (physical_selection_mode == PHYSICAL_SELECTION_ENABLED)
      physical_selection_mode = PHYSICAL_SELECTION_REQUESTED;
  }
  
  return output_level;
}
