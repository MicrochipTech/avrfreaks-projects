         

#device 10
	
	DDRB := $0F;
	repeat

		inc W30
		if CPU.Zero then
			inc R16
		end if

		if R16.0 then
			PORTB := $00;
		else
			PORTB := $0F;
		end if


	until false;












