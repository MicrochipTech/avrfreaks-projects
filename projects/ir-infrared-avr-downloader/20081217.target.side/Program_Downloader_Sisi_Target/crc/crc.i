

{
	unsigned int sum= 0;
	unsigned char j;
	  
	{
		unsigned char i;
			
			
		{
			if(sum & 1)
				sum= (sum >> 1) ^ 0xA001;
			else
				sum= (sum >> 1);
		};
	};
	  
}