void get_gps_info(float *lat,float *lon,float *utc_mo, float *utc_da, float *utc_hr, float *utc_mn){

	/*
      get gps info from Parallax GPS receiver module -- coded for baby orangutan B
		inputs: none
		outputs: latitude,longitude,utc_month,utc_day,utc_hour,utc_minute
		GPS Info: http://www.parallax.com/Portals/0/Downloads/docs/prod/acc/GPSManualV1.1.pdf
      AVR Library Command Reference: http://www.robotshop.ca/content/PDF/pololu-avr-library-command-reference-1220.pdf
		Module must have clear view of sky and be level
		
		Baby O  <->    GPS	<->	7805	<->	Battery (14AH, AGM)
		PD0------------sio			Vin---------+12V
		vin------------vin---------+5Vout
		Gnd------------Gnd---------Gnd---------Gnd
							|	
		PC0---LED		Raw
	*/

   //format of Parallax GPS GPMRC string...
   //$GPMRC,HHMMSS,A,DDMM.MMMM,S,DDDMM.MMMM,S,KKK.K,HHH.H,DDMMYY,,,A*64
   //012345678901234567890123456789012345678901234567890123456789012345
   //          1         2         3         4         5         6      
   // $GPRMC string locations for following data items...

   int utc_hours=7, utc_minutes=9, latitude=16, latitude_hemisphere=26, longitude=28, longitude_hemisphere=39, utc_day=53, utc_month=55;

   serial_set_baud_rate(4800);
   char gps[66]; // array to store $GPRMS string
   char buf[1];
   char type[6] = "$GPRMC";
   int type_matched = 0;
   int byte_count;
   int i;

   while(!type_matched)
	{
		do // test for valid data before continuing...
		{
			serial_receive(buf,1);
			byte_count = serial_get_received_bytes();
			while(byte_count == 0) byte_count = serial_get_received_bytes();
		} while (buf[0] > 127); // invalid data
		
      while (buf[0]!='$')
      { // scan forward to next $...
         serial_receive(buf,1);
   	   byte_count = serial_get_received_bytes();
	      while(byte_count == 0) byte_count = serial_get_received_bytes();
      }
      gps[0]=buf[0];
      // is this $GPRMC?
      type_matched = 1;
      for (i=1;i<sizeof(type);i++)
      {
         serial_receive(buf,1);
   	   byte_count = serial_get_received_bytes();
	      while(byte_count == 0) byte_count = serial_get_received_bytes();
         gps[i]=buf[0];
         if(type[i]!=gps[i])
         {
            type_matched = 0;
            break;
         }
      }
      if (type_matched)
      {
         // get the rest of the line...
         for (i=sizeof(type);i<sizeof(gps);i++)
         {
            serial_receive(buf,1);
      	   byte_count = serial_get_received_bytes();
	         while(byte_count == 0) byte_count = serial_get_received_bytes();
            gps[i] = buf[0];
         }
      }
   }
 
   *lat = (gps[latitude+0]-48)*10+(gps[latitude+1]-48)+(float)((gps[latitude+2]-48)*10+(gps[latitude+3]-48))/60;
   if(gps[latitude_hemisphere]=='S') *lat=-*lat; // apply sign

   *lon = (gps[longitude+0]-48)*100+(gps[longitude+1]-48)*10+(gps[longitude+2]-48) \
         +(float)((gps[longitude+3]-48)* 10+(gps[longitude+4]-48))/60;
   if(gps[longitude_hemisphere]=='W') *lon = -*lon; // apply sign

   *utc_mo = (gps[utc_month   + 0]-48)*10+(gps[utc_month   + 1]-48);
   *utc_da = (gps[utc_day     + 0]-48)*10+(gps[utc_day     + 1]-48);
   *utc_hr = (gps[utc_hours   + 0]-48)*10+(gps[utc_hours   + 1]-48);
   *utc_mn = (gps[utc_minutes + 0]-48)*10+(gps[utc_minutes + 1]-48);
} 
