void query_status(){      
  dali_answer(dali.status.status_information);
  return;
}
void query_ballast(){
  dali_answer(YES);
  return;
}
void query_lamp_failure(){
  if (dali.status.lamp_failure == 1)
    dali_answer(YES);
    //else answer 'NO'
  return;
}

void query_lamp_power_on(){
  if (dali.status.lamp_on == 1)
    dali_answer(YES);
  //else answer 'NO'
  return;
}

void query_limit_error(){
  if (dali.status.limit_error == 1)
    dali_answer(YES);
    //else answer 'NO'
  return;
}

void query_reset_state(){
  if (dali.status.reset_state == 1)
    dali_answer(YES);
    //else answer 'NO'
  return;
}

void query_missing_short_address(){
  if (dali.status.missing_short_add == 1)
    dali_answer(YES);
    //else answer 'NO'
  return;
}  

void query_version_number(){
  dali_answer(DALI_VERSION_NUMBER);
  return;
}   

void query_content_dtr(){
  dali_answer(dali.dtr);
  return;
}   

void query_device_type(){
  dali_answer(DALI_DEVICE_TYPE);
  return;
}  

void query_physical_minimum_level (){
  dali_answer(DALI_PHYSICAL_MIN_LEVEL);
  return;
}  

void query_power_failure (){
  dali_answer(dali.status.power_failure);
  return;
}  

void query_actual_level (){
  dali_answer(dali.actual_dim_level);
  return;
}  

void query_max_level (){
  dali_answer(dali.max_level);
  return;
} 

void query_min_level (){
  dali_answer(dali.min_level);
  return;
}   

void query_power_on_level (){
  dali_answer(dali.power_on_level);
  return;
}  

void query_system_failure_level (){
  dali_answer(dali.system_failure_level);
  return;
}   

void query_fade (){
  dali_answer(dali.fade_time<<4 | dali.fade_rate);
  return;
}  

void query_scene_level(){
  dali_answer(dali.scene[(dali.data_byte&0x0f)]);
  return;
} 

void query_groups_0_7(){
  dali_answer((U8)(dali.group));
  return;
}

void query_groups_8_15(){
  dali_answer((U8)(dali.group>>8));
  return;
} 

void query_random_address_h(){
  dali_answer(dali.random_add_H);
  return;
}  

void query_random_address_m(){
  dali_answer(dali.random_add_M);
  return;
}   

void query_random_address_l(){
  dali_answer(dali.random_add_L);
  return;
}   

//function pointer
//query_command[] contains pointers on query_command functions.
//this method is better than switch() case: in terms of execution cycles

typedef void (*p_query_command)();
code p_query_command query_command[28] = {
  query_status,
  query_ballast,
  query_lamp_failure,
  query_lamp_power_on,
  query_limit_error,
  query_reset_state,
  query_missing_short_address,
  query_version_number,
  query_content_dtr,
  query_device_type,
  query_physical_minimum_level,
  query_power_failure,
  none,
  none,
  none,
  none,
  query_actual_level,
  query_max_level,
  query_min_level,
  query_power_on_level,
  query_system_failure_level,
  query_fade,
  query_scene_level,
  query_groups_0_7,
  query_groups_8_15,
  query_random_address_h,
  query_random_address_m,
  query_random_address_l,  
};
