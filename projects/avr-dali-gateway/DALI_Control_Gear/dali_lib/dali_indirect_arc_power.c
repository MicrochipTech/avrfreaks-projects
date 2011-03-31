void immediate_off(){
  dali.actual_dim_level = 0;
  return;
}        

void up_200ms(){
  dali_up_output_w_fade_rate();
  return;
}     

void down_200ms(){
  dali_down_output_w_fade_rate();
  return;
}      

void step_up(){
  if (dali.actual_dim_level < dali.max_level && dali.status.lamp_on == ON)
    dali.actual_dim_level++;
  return;
}     

void step_down(){
  if (dali.actual_dim_level > dali.min_level && dali.status.lamp_on == ON)
    dali.actual_dim_level--;      
  return;
}       

void recall_max_level(){
  dali.actual_dim_level = dali.max_level;         
  return;
}   

void recall_min_level(){
  dali.actual_dim_level = dali.min_level;          
  return;
}   

void step_down_and_off(){
  if (dali.status.lamp_on == ON){
    if (dali.actual_dim_level > dali.min_level)
      dali.actual_dim_level--;
    else
      dali.actual_dim_level = 0;
  }
  return;
}  

void on_and_step_up(){
  if (dali.status.lamp_on == ON){
    if (dali.actual_dim_level < dali.max_level)
      dali.actual_dim_level++;
  }
  else
    dali.actual_dim_level = dali.min_level;          
  return;
} 

void go_to_scene(){
  requested_level = dali.scene[(dali.data_byte&0x0f)];
  dali_change_output_w_fade_time();            
  return;
}    

//function pointer
//indirect_arc_power[] contains pointers on indirect_arc_power functions.
//this method is better than switch() case: in terms of execution cycles

typedef void (*p_indirect_arc_power)();
code p_indirect_arc_power indirect_arc_power[11] = {
  immediate_off,
  up_200ms,
  down_200ms,
  step_up,
  step_down,
  recall_max_level,
  recall_min_level,
  step_down_and_off,
  on_and_step_up,
  none,
  go_to_scene  
};




