	//+------------------------------+
	//| SEGMENTS DEFINTIONS TABLE	 |
	//+------------------------------+--------------------------------------------------------------------------------------------
	_DISP7_SEGMENTS_TABLE:
	//ASCII SPACE 	     a
	//		   ----
	//		  |    |
	//		f |    | b
	//		  |  g |
	//		   ---- 
	//		  |    |
	//		e |    | c
	//		  |  d |
	//		   ---- 
	 (0<<_DISPLAY_SPI_SEG2_A_BIT) | (0<<_DISPLAY_SPI_SEG2_B_BIT) | (0<<_DISPLAY_SPI_SEG2_C_BIT) | (0<<_DISPLAY_SPI_SEG2_D_BIT) | (0<<_DISPLAY_SPI_SEG2_E_BIT) | (0<<_DISPLAY_SPI_SEG2_F_BIT) | (0<<_DISPLAY_SPI_SEG2_G_BIT) | (0<<_DISPLAY_SPI_SEG2_P_BIT)
	//ASCII . 	     a
	//		   ----
	//		  |    |
	//		f |    | b
	//		  |  g |
	//		   ---- 
	//		  |    |
	//		e |    | c
	//		  |  d |
	//		   ----  ##
	(0<<_DISPLAY_SPI_SEG2_A_BIT) | (0<<_DISPLAY_SPI_SEG2_B_BIT) | (0<<_DISPLAY_SPI_SEG2_C_BIT) | (0<<_DISPLAY_SPI_SEG2_D_BIT) | (0<<_DISPLAY_SPI_SEG2_E_BIT) | (0<<_DISPLAY_SPI_SEG2_F_BIT) | (0<<_DISPLAY_SPI_SEG2_G_BIT) | (1<<_DISPLAY_SPI_SEG2_P_BIT)
	//ASCII - 	     a
	//		   ----
	//		  |    |
	//		f |    | b
	//		  |  g |
	//		   #### 
	//		  |    |
	//		e |    | c
	//		  |  d |
	//		   ---- 
	(0<<_DISPLAY_SPI_SEG2_A_BIT) | (0<<_DISPLAY_SPI_SEG2_B_BIT) | (0<<_DISPLAY_SPI_SEG2_C_BIT) | (0<<_DISPLAY_SPI_SEG2_D_BIT) | (0<<_DISPLAY_SPI_SEG2_E_BIT) | (0<<_DISPLAY_SPI_SEG2_F_BIT) | (1<<_DISPLAY_SPI_SEG2_G_BIT) | (0<<_DISPLAY_SPI_SEG2_P_BIT)
	//ASCII 0 	     a
	//		   ####
	//		  #    #
	//		f #    # b
	//		  #  g #
	//		   ---- 
	//		  #    #
	//		e #    # c
	//		  #  d #
	//		   #### 
	(1<<_DISPLAY_SPI_SEG2_A_BIT) | (1<<_DISPLAY_SPI_SEG2_B_BIT) | (1<<_DISPLAY_SPI_SEG2_C_BIT) | (1<<_DISPLAY_SPI_SEG2_D_BIT) | (1<<_DISPLAY_SPI_SEG2_E_BIT) | (1<<_DISPLAY_SPI_SEG2_F_BIT) | (0<<_DISPLAY_SPI_SEG2_G_BIT) | (0<<_DISPLAY_SPI_SEG2_P_BIT)
	//ASCII 1 	     a
	//		   ----
	//		  |    #
	//		f |    # b
	//		  |  g #
	//		   ---- 
	//		  |    #
	//		e |    # c
	//		  |  d #
	//		   ---- 
	(0<<_DISPLAY_SPI_SEG2_A_BIT) | (1<<_DISPLAY_SPI_SEG2_B_BIT) | (1<<_DISPLAY_SPI_SEG2_C_BIT) | (0<<_DISPLAY_SPI_SEG2_D_BIT) | (0<<_DISPLAY_SPI_SEG2_E_BIT) | (0<<_DISPLAY_SPI_SEG2_F_BIT) | (0<<_DISPLAY_SPI_SEG2_G_BIT) | (0<<_DISPLAY_SPI_SEG2_P_BIT)
	//ASCII 2 	     a
	//		   ####
	//		  |    #
	//		f |    # b
	//		  |  g #
	//		   #### 
	//		  #    |
	//		e #    | c
	//		  #  d |
	//		   #### 
	(1<<_DISPLAY_SPI_SEG2_A_BIT) | (1<<_DISPLAY_SPI_SEG2_B_BIT) | (0<<_DISPLAY_SPI_SEG2_C_BIT) | (1<<_DISPLAY_SPI_SEG2_D_BIT) | (1<<_DISPLAY_SPI_SEG2_E_BIT) | (0<<_DISPLAY_SPI_SEG2_F_BIT) | (1<<_DISPLAY_SPI_SEG2_G_BIT) | (0<<_DISPLAY_SPI_SEG2_P_BIT)
	//ASCII 3 	     a
	//		   ####
	//		  |    #
	//		f |    # b
	//		  |  g #
	//		   #### 
	//		  |    #
	//		e |    # c
	//		  |  d #
	//		   #### 
	(1<<_DISPLAY_SPI_SEG2_A_BIT) | (1<<_DISPLAY_SPI_SEG2_B_BIT) | (1<<_DISPLAY_SPI_SEG2_C_BIT) | (1<<_DISPLAY_SPI_SEG2_D_BIT) | (0<<_DISPLAY_SPI_SEG2_E_BIT) | (0<<_DISPLAY_SPI_SEG2_F_BIT) | (1<<_DISPLAY_SPI_SEG2_G_BIT) | (0<<_DISPLAY_SPI_SEG2_P_BIT)
	//ASCII 4 	     a
	//		   ----
	//		  #    #
	//		f #    # b
	//		  #  g #
	//		   #### 
	//		  |    #
	//		e |    # c
	//		  |  d #
	//		   ---- 
	(0<<_DISPLAY_SPI_SEG2_A_BIT) | (1<<_DISPLAY_SPI_SEG2_B_BIT) | (1<<_DISPLAY_SPI_SEG2_C_BIT) | (0<<_DISPLAY_SPI_SEG2_D_BIT) | (0<<_DISPLAY_SPI_SEG2_E_BIT) | (1<<_DISPLAY_SPI_SEG2_F_BIT) | (1<<_DISPLAY_SPI_SEG2_G_BIT) | (0<<_DISPLAY_SPI_SEG2_P_BIT)
	//ASCII 5 	     a
	//		   ####
	//		  #    |
	//		f #    | b
	//		  #  g |
	//		   #### 
	//		  |    #
	//		e |    # c
	//		  |  d #
	//		   #### 
	(1<<_DISPLAY_SPI_SEG2_A_BIT) | (0<<_DISPLAY_SPI_SEG2_B_BIT) | (1<<_DISPLAY_SPI_SEG2_C_BIT) | (1<<_DISPLAY_SPI_SEG2_D_BIT) | (0<<_DISPLAY_SPI_SEG2_E_BIT) | (1<<_DISPLAY_SPI_SEG2_F_BIT) | (1<<_DISPLAY_SPI_SEG2_G_BIT) | (0<<_DISPLAY_SPI_SEG2_P_BIT)
	//ASCII 6 	     a
	//		   ####
	//		  #    |
	//		f #    | b
	//		  #  g |
	//		   #### 
	//		  #    #
	//		e #    # c
	//		  #  d #
	//		   #### 
	(1<<_DISPLAY_SPI_SEG2_A_BIT) | (0<<_DISPLAY_SPI_SEG2_B_BIT) | (1<<_DISPLAY_SPI_SEG2_C_BIT) | (1<<_DISPLAY_SPI_SEG2_D_BIT) | (1<<_DISPLAY_SPI_SEG2_E_BIT) | (1<<_DISPLAY_SPI_SEG2_F_BIT) | (1<<_DISPLAY_SPI_SEG2_G_BIT) | (0<<_DISPLAY_SPI_SEG2_P_BIT)
	//ASCII 7 	     a
	//		   ####
	//		  |    #
	//		f |    # b
	//		  |  g #
	//		   ---- 
	//		  |    #
	//		e |    # c
	//		  |  d #
	//		   ---- 
	(1<<_DISPLAY_SPI_SEG2_A_BIT) | (1<<_DISPLAY_SPI_SEG2_B_BIT) | (1<<_DISPLAY_SPI_SEG2_C_BIT) | (0<<_DISPLAY_SPI_SEG2_D_BIT) | (0<<_DISPLAY_SPI_SEG2_E_BIT) | (0<<_DISPLAY_SPI_SEG2_F_BIT) | (0<<_DISPLAY_SPI_SEG2_G_BIT) | (0<<_DISPLAY_SPI_SEG2_P_BIT)
	//ASCII 8 	     a
	//		   ####
	//		  #    #
	//		f #    # b
	//		  #  g #
	//		   #### 
	//		  #    #
	//		e #    # c
	//		  #  d #
	//		   #### 
	(1<<_DISPLAY_SPI_SEG2_A_BIT) | (1<<_DISPLAY_SPI_SEG2_B_BIT) | (1<<_DISPLAY_SPI_SEG2_C_BIT) | (1<<_DISPLAY_SPI_SEG2_D_BIT) | (1<<_DISPLAY_SPI_SEG2_E_BIT) | (1<<_DISPLAY_SPI_SEG2_F_BIT) | (1<<_DISPLAY_SPI_SEG2_G_BIT) | (0<<_DISPLAY_SPI_SEG2_P_BIT)
	//ASCII 9	     a
	//		   ####
	//		  #    #
	//		f #    # b
	//		  #  g #
	//		   #### 
	//		  |    #
	//		e |    # c
	//		  |  d #
	//		   #### 
	(1<<_DISPLAY_SPI_SEG2_A_BIT) | (1<<_DISPLAY_SPI_SEG2_B_BIT) | (1<<_DISPLAY_SPI_SEG2_C_BIT) | (1<<_DISPLAY_SPI_SEG2_D_BIT) | (0<<_DISPLAY_SPI_SEG2_E_BIT) | (1<<_DISPLAY_SPI_SEG2_F_BIT) | (1<<_DISPLAY_SPI_SEG2_G_BIT) | (0<<_DISPLAY_SPI_SEG2_P_BIT)
	//ASCII A 	     a
	//		   ####
	//		  #    #
	//		f #    # b
	//		  #  g #
	//		   #### 
	//		  #    #
	//		e #    # c
	//		  #  d #
	//		   ---- 
	(1<<_DISPLAY_SPI_SEG2_A_BIT) | (1<<_DISPLAY_SPI_SEG2_B_BIT) | (1<<_DISPLAY_SPI_SEG2_C_BIT) | (0<<_DISPLAY_SPI_SEG2_D_BIT) | (1<<_DISPLAY_SPI_SEG2_E_BIT) | (1<<_DISPLAY_SPI_SEG2_F_BIT) | (1<<_DISPLAY_SPI_SEG2_G_BIT) | (0<<_DISPLAY_SPI_SEG2_P_BIT)
	//ASCII B 	     a
	//		   ----
	//		  #    |
	//		f #    | b
	//		  #  g |
	//		   #### 
	//		  #    #
	//		e #    # c
	//		  #  d #
	//		   #### 
	(0<<_DISPLAY_SPI_SEG2_A_BIT) | (0<<_DISPLAY_SPI_SEG2_B_BIT) | (1<<_DISPLAY_SPI_SEG2_C_BIT) | (1<<_DISPLAY_SPI_SEG2_D_BIT) | (1<<_DISPLAY_SPI_SEG2_E_BIT) | (1<<_DISPLAY_SPI_SEG2_F_BIT) | (1<<_DISPLAY_SPI_SEG2_G_BIT) | (0<<_DISPLAY_SPI_SEG2_P_BIT)
	//ASCII C 	     a
	//		   ####
	//		  #    |
	//		f #    | b
	//		  #  g |
	//		   ---- 
	//		  #    |
	//		e #    | c
	//		  #  d |
	//		   #### 
	(1<<_DISPLAY_SPI_SEG2_A_BIT) | (0<<_DISPLAY_SPI_SEG2_B_BIT) | (0<<_DISPLAY_SPI_SEG2_C_BIT) | (1<<_DISPLAY_SPI_SEG2_D_BIT) | (1<<_DISPLAY_SPI_SEG2_E_BIT) | (1<<_DISPLAY_SPI_SEG2_F_BIT) | (0<<_DISPLAY_SPI_SEG2_G_BIT) | (0<<_DISPLAY_SPI_SEG2_P_BIT)
	//ASCII D 	     a
	//		   ----
	//		  |    #
	//		f |    # b
	//		  |  g #
	//		   #### 
	//		  #    #
	//		e #    # c
	//		  #  d #
	//		   #### 
	(0<<_DISPLAY_SPI_SEG2_A_BIT) | (1<<_DISPLAY_SPI_SEG2_B_BIT) | (1<<_DISPLAY_SPI_SEG2_C_BIT) | (1<<_DISPLAY_SPI_SEG2_D_BIT) | (1<<_DISPLAY_SPI_SEG2_E_BIT) | (0<<_DISPLAY_SPI_SEG2_F_BIT) | (1<<_DISPLAY_SPI_SEG2_G_BIT) | (0<<_DISPLAY_SPI_SEG2_P_BIT)
	//ASCII E 	     a
	//		   ####
	//		  #    |
	//		f #    | b
	//		  #  g |
	//		   #### 
	//		  #    |
	//		e #    | c
	//		  #  d |
	//		   #### 
	(1<<_DISPLAY_SPI_SEG2_A_BIT) | (0<<_DISPLAY_SPI_SEG2_B_BIT) | (0<<_DISPLAY_SPI_SEG2_C_BIT) | (1<<_DISPLAY_SPI_SEG2_D_BIT) | (1<<_DISPLAY_SPI_SEG2_E_BIT) | (1<<_DISPLAY_SPI_SEG2_F_BIT) | (1<<_DISPLAY_SPI_SEG2_G_BIT) | (0<<_DISPLAY_SPI_SEG2_P_BIT)
	//ASCII F 	     a
	//		   ####
	//		  #    |
	//		f #    | b
	//		  #  g |
	//		   #### 
	//		  #    |
	//		e #    | c
	//		  #  d |
	//		   ---- 
	(1<<_DISPLAY_SPI_SEG2_A_BIT) | (0<<_DISPLAY_SPI_SEG2_B_BIT) | (0<<_DISPLAY_SPI_SEG2_C_BIT) | (0<<_DISPLAY_SPI_SEG2_D_BIT) | (1<<_DISPLAY_SPI_SEG2_E_BIT) | (1<<_DISPLAY_SPI_SEG2_F_BIT) | (1<<_DISPLAY_SPI_SEG2_G_BIT) | (0<<_DISPLAY_SPI_SEG2_P_BIT)
	//ASCII G 	     a
	//		   ####
	//		  #    #
	//		f #    # b
	//		  #  g #
	//		   #### 
	//		  |    #
	//		e |    # c
	//		  |  d #
	//		   #### 
	(1<<_DISPLAY_SPI_SEG2_A_BIT) | (1<<_DISPLAY_SPI_SEG2_B_BIT) | (1<<_DISPLAY_SPI_SEG2_C_BIT) | (1<<_DISPLAY_SPI_SEG2_D_BIT) | (0<<_DISPLAY_SPI_SEG2_E_BIT) | (1<<_DISPLAY_SPI_SEG2_F_BIT) | (1<<_DISPLAY_SPI_SEG2_G_BIT) | (0<<_DISPLAY_SPI_SEG2_P_BIT)
	//ASCII H 	     a
	//		   ----
	//		  #    #
	//		f #    # b
	//		  #  g #
	//		   #### 
	//		  #    #
	//		e #    # c
	//		  #  d #
	//		   ---- 
	(0<<_DISPLAY_SPI_SEG2_A_BIT) | (1<<_DISPLAY_SPI_SEG2_B_BIT) | (1<<_DISPLAY_SPI_SEG2_C_BIT) | (0<<_DISPLAY_SPI_SEG2_D_BIT) | (1<<_DISPLAY_SPI_SEG2_E_BIT) | (1<<_DISPLAY_SPI_SEG2_F_BIT) | (1<<_DISPLAY_SPI_SEG2_G_BIT) | (0<<_DISPLAY_SPI_SEG2_P_BIT)
	//ASCII I 	     a
	//		   ----
	//		  |    #
	//		f |    # b
	//		  |  g #
	//		   ---- 
	//		  |    #
	//		e |    # c
	//		  |  d #
	//		   ---- 
	(0<<_DISPLAY_SPI_SEG2_A_BIT) | (1<<_DISPLAY_SPI_SEG2_B_BIT) | (1<<_DISPLAY_SPI_SEG2_C_BIT) | (0<<_DISPLAY_SPI_SEG2_D_BIT) | (0<<_DISPLAY_SPI_SEG2_E_BIT) | (0<<_DISPLAY_SPI_SEG2_F_BIT) | (0<<_DISPLAY_SPI_SEG2_G_BIT) | (0<<_DISPLAY_SPI_SEG2_P_BIT)
	//ASCII J 	     a
	//		   ----
	//		  |    #
	//		f |    # b
	//		  |  g #
	//		   ---- 
	//		  #    #
	//		e #    # c
	//		  #  d #
	//		   #### 
	(0<<_DISPLAY_SPI_SEG2_A_BIT) | (1<<_DISPLAY_SPI_SEG2_B_BIT) | (1<<_DISPLAY_SPI_SEG2_C_BIT) | (1<<_DISPLAY_SPI_SEG2_D_BIT) | (1<<_DISPLAY_SPI_SEG2_E_BIT) | (0<<_DISPLAY_SPI_SEG2_F_BIT) | (0<<_DISPLAY_SPI_SEG2_G_BIT) | (0<<_DISPLAY_SPI_SEG2_P_BIT)
	//ASCII L 	     a
	//		   ----
	//		  #    |
	//		f #    | b
	//		  #  g |
	//		   ---- 
	//		  #    |
	//		e #    | c
	//		  #  d |
	//		   #### 
	(0<<_DISPLAY_SPI_SEG2_A_BIT) | (0<<_DISPLAY_SPI_SEG2_B_BIT) | (0<<_DISPLAY_SPI_SEG2_C_BIT) | (1<<_DISPLAY_SPI_SEG2_D_BIT) | (1<<_DISPLAY_SPI_SEG2_E_BIT) | (1<<_DISPLAY_SPI_SEG2_F_BIT) | (0<<_DISPLAY_SPI_SEG2_G_BIT) | (0<<_DISPLAY_SPI_SEG2_P_BIT)
	//ASCII N 	     a
	//		   ####
	//		  #    #
	//		f #    # b
	//		  #  g #
	//		   ---- 
	//		  #    #
	//		e #    # c
	//		  #  d #
	//		   ---- 
	(1<<_DISPLAY_SPI_SEG2_A_BIT) | (1<<_DISPLAY_SPI_SEG2_B_BIT) | (1<<_DISPLAY_SPI_SEG2_C_BIT) | (0<<_DISPLAY_SPI_SEG2_D_BIT) | (1<<_DISPLAY_SPI_SEG2_E_BIT) | (1<<_DISPLAY_SPI_SEG2_F_BIT) | (0<<_DISPLAY_SPI_SEG2_G_BIT) | (0<<_DISPLAY_SPI_SEG2_P_BIT)
	//ASCII O 	     a
	//		   ####
	//		  #    #
	//		f #    # b
	//		  #  g #
	//		   ---- 
	//		  #    #
	//		e #    # c
	//		  #  d #
	//		   #### 
	(1<<_DISPLAY_SPI_SEG2_A_BIT) | (1<<_DISPLAY_SPI_SEG2_B_BIT) | (1<<_DISPLAY_SPI_SEG2_C_BIT) | (1<<_DISPLAY_SPI_SEG2_D_BIT) | (1<<_DISPLAY_SPI_SEG2_E_BIT) | (1<<_DISPLAY_SPI_SEG2_F_BIT) | (0<<_DISPLAY_SPI_SEG2_G_BIT) | (0<<_DISPLAY_SPI_SEG2_P_BIT)
	//ASCII P 	     a
	//		   ####
	//		  #    #
	//		f #    # b
	//		  #  g #
	//		   #### 
	//		  #    |
	//		e #    | c
	//		  #  d |
	//		   ---- 
	(1<<_DISPLAY_SPI_SEG2_A_BIT) | (1<<_DISPLAY_SPI_SEG2_B_BIT) | (0<<_DISPLAY_SPI_SEG2_C_BIT) | (0<<_DISPLAY_SPI_SEG2_D_BIT) | (1<<_DISPLAY_SPI_SEG2_E_BIT) | (1<<_DISPLAY_SPI_SEG2_F_BIT) | (1<<_DISPLAY_SPI_SEG2_G_BIT) | (0<<_DISPLAY_SPI_SEG2_P_BIT)
	//ASCII Q 	     a
	//		   ####
	//		  #    #
	//		f #    # b
	//		  #  g #
	//		   #### 
	//		  |    #
	//		e |    # c
	//		  |  d #
	//		   ---- 
	(1<<_DISPLAY_SPI_SEG2_A_BIT) | (1<<_DISPLAY_SPI_SEG2_B_BIT) | (1<<_DISPLAY_SPI_SEG2_C_BIT) | (0<<_DISPLAY_SPI_SEG2_D_BIT) | (0<<_DISPLAY_SPI_SEG2_E_BIT) | (1<<_DISPLAY_SPI_SEG2_F_BIT) | (1<<_DISPLAY_SPI_SEG2_G_BIT) | (0<<_DISPLAY_SPI_SEG2_P_BIT)
	//ASCII R 	     a
	//		   ####
	//		  #    #
	//		f #    #b
	//		  #  g #
	//		   ---- 
	//		  #    |
	//		e #    | c
	//		  #  d |
	//		   ---- 
	(1<<_DISPLAY_SPI_SEG2_A_BIT) | (1<<_DISPLAY_SPI_SEG2_B_BIT) | (0<<_DISPLAY_SPI_SEG2_C_BIT) | (0<<_DISPLAY_SPI_SEG2_D_BIT) | (1<<_DISPLAY_SPI_SEG2_E_BIT) | (1<<_DISPLAY_SPI_SEG2_F_BIT) | (0<<_DISPLAY_SPI_SEG2_G_BIT) | (0<<_DISPLAY_SPI_SEG2_P_BIT)
	//ASCII S 	     a
	//		   ####
	//		  #    |
	//		f #    | b
	//		  #  g |
	//		   #### 
	//		  |    #
	//		e |    # c
	//		  |  d #
	//		   #### 
	(1<<_DISPLAY_SPI_SEG2_A_BIT) | (0<<_DISPLAY_SPI_SEG2_B_BIT) | (1<<_DISPLAY_SPI_SEG2_C_BIT) | (1<<_DISPLAY_SPI_SEG2_D_BIT) | (0<<_DISPLAY_SPI_SEG2_E_BIT) | (1<<_DISPLAY_SPI_SEG2_F_BIT) | (1<<_DISPLAY_SPI_SEG2_G_BIT) | (0<<_DISPLAY_SPI_SEG2_P_BIT)
	//ASCII U 	     a
	//		   ----
	//		  #    #
	//		f #    # b
	//		  #  g #
	//		   ---- 
	//		  #    #
	//		e #    # c
	//		  #  d #
	//		   #### 
	(0<<_DISPLAY_SPI_SEG2_A_BIT) | (1<<_DISPLAY_SPI_SEG2_B_BIT) | (1<<_DISPLAY_SPI_SEG2_C_BIT) | (1<<_DISPLAY_SPI_SEG2_D_BIT) | (1<<_DISPLAY_SPI_SEG2_E_BIT) | (1<<_DISPLAY_SPI_SEG2_F_BIT) | (0<<_DISPLAY_SPI_SEG2_G_BIT) | (0<<_DISPLAY_SPI_SEG2_P_BIT)
	//ASCII Y	     a 
	//		   ----
	//		  #    #
	//		f #    # b
	//		  #  g #
	//		   #### 
	//		  |    #
	//		e |    # c
	//		  |  d #
	//		   ---- 
	(0<<_DISPLAY_SPI_SEG2_A_BIT) | (1<<_DISPLAY_SPI_SEG2_B_BIT) | (1<<_DISPLAY_SPI_SEG2_C_BIT) | (0<<_DISPLAY_SPI_SEG2_D_BIT) | (0<<_DISPLAY_SPI_SEG2_E_BIT) | (1<<_DISPLAY_SPI_SEG2_F_BIT) | (1<<_DISPLAY_SPI_SEG2_G_BIT) | (0<<_DISPLAY_SPI_SEG2_P_BIT)
	//ASCII Z 	     a
	//		   ####
	//		  |    #
	//		f |    # b
	//		  |  g #
	//		   #### 
	//		  #    |
	//		e #    | c
	//		  #  d |
	//		   #### 
	(1<<_DISPLAY_SPI_SEG2_A_BIT) | (1<<_DISPLAY_SPI_SEG2_B_BIT) | (0<<_DISPLAY_SPI_SEG2_C_BIT) | (1<<_DISPLAY_SPI_SEG2_D_BIT) | (1<<_DISPLAY_SPI_SEG2_E_BIT) | (0<<_DISPLAY_SPI_SEG2_F_BIT) | (1<<_DISPLAY_SPI_SEG2_G_BIT) | (0<<_DISPLAY_SPI_SEG2_P_BIT)