specification lcd {

    required {
	parameter	controller	tag
	parameter 	columns
	parameter	lines
	bidir		DB
	output		/CE
	output		/RD
	output		/WR
	output		C/D
    }	
	
    optional output	/PWR 	

}

