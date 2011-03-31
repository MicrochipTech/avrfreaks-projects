void none(){
  return;
}

void terminate(){
  time_out_2 = 0;
  compare_mode = 0;
  physical_selection_mode = PHYSICAL_SELECTION_DISABLED;
  return;
}

void data_transfer_register(){
  dali.dtr = dali.data_byte;
  return;
}            

void initialise(){
  if ((dali.data_byte == 0) ||                                     //broadcast
      ((dali.data_byte & 0xfe) == ((dali.short_add<<1) & 0xfe)) ||  //short address OK
      ((dali.data_byte == 0xff) && (dali.short_add == 0xff)))       //no short address
  {
    time_out_2 = 3600;    //enables special commands for 3600*1/4th second = 15min.
    compare_mode = 1;
  }
  return; 
}   

void randomise(){
  if (time_out_2 != 0){    //if time_out_2 >0 , specialMode is enabled
    srand(TCNT1L);        //initialise the first random value of a list
    dali.random_add_H = rand(); //take the next random value
    dali.random_add_M = rand();
    dali.random_add_L = rand();  
    ee_write(ADD_RANDOM_ADDH, dali.random_add_H); 
    ee_write(ADD_RANDOM_ADDM, dali.random_add_M);
    ee_write(ADD_RANDOM_ADDL, dali.random_add_L);
  }
  return;
}     

void compare(){
  if ((time_out_2 != 0) && (compare_mode == 1)){    
    //specialMode enabled && compare_mode enabled
    if (dali.random_add_H > dali.search_add_H)
      return; //answer 'NO'
    else{
      if (dali.random_add_H == dali.search_add_H){
        if (dali.random_add_M > dali.search_add_M)
          return; //answer 'NO'
        else{
          if (dali.random_add_M == dali.search_add_M){
            if (dali.random_add_L > dali.search_add_L)
              return; //answer 'NO'
          }
        }
      }
    }     
    dali_answer(YES);           
  }
  return;
}   

void withdraw(){
  if (time_out_2 != 0){    //specialMode enabled        
    if ((dali.random_add_H == dali.search_add_H) &&
        (dali.random_add_M == dali.search_add_M) &&
        (dali.random_add_L == dali.search_add_L))
      compare_mode = 0;   //disable compare
  }
  return;
}   

void search_addr_h(){
  if (time_out_2 != 0){    //specialMode enabled        
    dali.search_add_H = dali.data_byte;
  }
  return;
}    

void search_addr_m(){
  if (time_out_2 != 0){    //specialMode enabled         
    dali.search_add_M = dali.data_byte;
  }
  return;
}      

void search_addr_l(){
  if (time_out_2 != 0){    //specialMode enabled
    dali.search_add_L = dali.data_byte;
  }
  return;
}   

void program_short_address(){
  if (time_out_2 != 0){    //specialMode enabled
    if (((dali.random_add_H == dali.search_add_H) &&
        (dali.random_add_M == dali.search_add_M) &&
        (dali.random_add_L == dali.search_add_L) &&
        (physical_selection_mode == PHYSICAL_SELECTION_DISABLED)) ||
        (physical_selection_mode == PHYSICAL_SELECTION_ENABLED))
    {
        if (dali.data_byte == 0xff){
          //clear short address
          dali.short_add = 0xff;
          dali.status.missing_short_add = 1;
        }
        else{
          dali.short_add = dali.data_byte>>1;
          dali.status.missing_short_add = 0; 
        }
        ee_write(ADD_SHORT_ADD, dali.short_add);
    }
  }
  return;
}  

void verify_short_address(){
  if (time_out_2 != 0){    //specialMode enabled
    if ((dali.data_byte & 0xfe) == ((dali.short_add<<1) & 0xfe))
      dali_answer(YES);
  }
  return;
}     

void query_short_address(){
  if (time_out_2 != 0){    //specialMode enabled
    if (((dali.random_add_H == dali.search_add_H) &&
        (dali.random_add_M == dali.search_add_M) &&
        (dali.random_add_L == dali.search_add_L) &&
        (physical_selection_mode == PHYSICAL_SELECTION_DISABLED)) ||
        (physical_selection_mode == PHYSICAL_SELECTION_ENABLED))
    {              
      dali_answer((dali.short_add<<1) | 1);
    }
  }
  return;
}    

void physical_selection(){
  if (time_out_2 != 0){    //specialMode enabled
    if (physical_selection_mode == PHYSICAL_SELECTION_DISABLED){
      physical_selection_mode = PHYSICAL_SELECTION_REQUESTED;  //toggles physical selection mode
      compare_mode = 0;
    }else{ 
      physical_selection_mode = PHYSICAL_SELECTION_DISABLED;
      compare_mode = 1;   //reactivate search & random address comparison
    }
  }
  return;
}

//function pointer
//special_command[] contains pointers on special_command functions.
//this method is better than switch() case: in terms of execution cycles

typedef void (*p_special_command)();
code p_special_command special_command[15] = {
  terminate,
  data_transfer_register,
  initialise,
  randomise,
  compare,
  withdraw,
  none,
  none,
  search_addr_h,
  search_addr_m,
  search_addr_l,
  program_short_address,
  verify_short_address,
  query_short_address,
  physical_selection
};

