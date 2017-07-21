void loop()//loop runs continuously so really no start to it
{
  Top:
  restart = false;//be sure restart flag is cleared
  initCommandPortal();//sets command to 6 and sets oldUnits to current value. Is used by next function
  
  if(odID != ID){//then it is OD or OD and ID which means do OD; default is OD 
  //Serial.println("1");
    displayRadiusAndWait();//radius is displayed; 
                          //caliper jaws have to close by Limit before we start to look for local minimum; 
                          //also look for command to run.After command runs, it will return here.
    if(restart){goto Top;}//after a command is run, restart loop()
                             //Serial.println("2");
    startODmeasPrompt(); //ID measurement persists on display until caliper jaws start to close. When they stop closing
                         //and start opening, we lock in OD measurement
                           //Serial.println("3");
    noReadyPrompt = false;//tells next function to output "to meas." prompt
    initCommandPortal();//sets command to 6 and sets oldUnits to current value. Is used by next function
      //Serial.println("4");
    localMinRead(); //prompt user, monitors caliper for local minimum and detect a possible push of inch/mm which means 
                    //command will be executed. When done, we will return to this point in the loop; output is oldResult
                      //Serial.println("5");
    if(restart){goto Top;}//after a command is run, restart loop()
    interpolation(); //improve accuracy of caliper reading using gage blocks; input and output are oldResult; pass through if data not present 
  //Serial.println("6");
    displayAnswer(); //display best reading; input is oldResult and output is displayAnswer which is what is displayed
  }

  
  if(odID != OD){//then it is ID or OD and ID so do ID
    //Serial.println("7");
    displayRadiusAndWait();//radius is displayed;
                         //caliper jaws have to open by Limit before we start to look for local maximum; 
                         //also looking for command to run. After command runs, it will return here.
    if(restart){goto Top;}//after a command is run, restart loop()
    startIDmeasPrompt(); //OD measurement persists on display until caliper jaws start to open. When they stop opening
                           //and start closing, we lock in ID measurement
    noReadyPrompt = false;//tells next function to output "to meas." prompt
    initCommandPortal();//sets command to 6 and sets oldUnits to current value. Is used by next function
    localMaxRead(); //monitor caliper for local maximum; then signal we are ready for new local minimum; 
                    //also looking for command to run.After command runs, it will return here.
        if(restart){goto Top;}//after a command is run, restart loop()
    interpolation(); //improve accuracy of caliper reading using gage blocks; input and output are oldResult; pass through if data not present    
    displayAnswer(); //display best reading; input is oldResult and no output to program  
                     //hold displayed result until user signals for a new reading or signals to enter calibration mode             
                     //when user has signaled that they are done with last reading. See if they want to enter calibration mode or take another reading
  }
}
