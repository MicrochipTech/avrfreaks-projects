/*
   AT Command. h
   
   GSM_ControlSystem "GSA" V1.0.0  Copyright (C) 2012 Easy System
	

    ***************************************************************************
     *    >>> See http://www.esyste.com for Hw Documentation <<<              *
     *                                                                                                                         *
     *    Thank you for using GSA, and thank you for your support!                    *
     *                                                                                                                         *
    ***************************************************************************


    This file is part of the GSM_ControlSystem "GSA" distribution.

    GSA is free software; you can redistribute it and/or modify it under
    the terms of the GNU General Public License (version 2) as published by the
    Free Software Foundation AND MODIFIED BY the GSA exception.
    >>>NOTE<<< The modification to the GPL is included to allow you to
    distribute a combined work that includes GSA without being obliged to
    provide the source code for proprietary components outside of the GSA
    kernel.  GSA is distributed in the hope that it will be useful, but
    WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
    or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
    more details. 
    You should have received a copy of the GNU General Public
    License and the GSA license exception along with GSA.
    
    GNU General Public License Exception:
    Any GSA source code, whether modified or in its original release form, or whether in whole or in part, 
    can only be distributed by you under the terms of the GNU General Public License plus this exception. 
    An independent module is a module which is not derived from or based on GSA.
    
    EXCEPTION TEXT:
    Clause 1
    Linking GSA statically or dynamically with other modules is making a combined work based on GSA. Thus, 
    the terms and conditions of the GNU General Public License cover the whole combination.
    As a special exception, the copyright holder of GSA gives you permission to link GSA with independent modules that communicate with GSA solely 
    through the GSA or FreeRTOS API interface, regardless of the license terms of these independent modules, and to copy and distribute the resulting 
    combined work under terms of your choice, provided that:
    
    1)   Every copy of the combined work is accompanied by a written statement that details to the recipient the version of GSA used and an offer by yourself 
          to provide the GSA source code (including any modifications you may have made) should the recipient request it.
    2)  The combined work is not itself an RTOS, scheduler, kernel or related product.
    3)  The independent modules add significant and primary functionality to GSA and do not merely extend the existing functionality already present in GSA.

    Clause 2
    GSA may not be used for any competitive or comparative purpose, 
    including the publication of any form of run time or compile time metric, 
    without the express permission of Easy System (this is the norm within the industry and is intended to ensure information accuracy). 
    
    All FreeRTOS files follow their own licence.
    
    1 tab == 4 spaces!

	****************************************************************************
	*   									AT COMMAND REFERENCE              							*
	****************************************************************************
*/

/*
"AT"										// Starting A Command Line 31
"A/"										// Last Command Automatic Repetition Prefix 31

//General Configuration Commands - AT Interface Backward Compatibility
"#SELINT"							// Select Interface Style 33

//Hayes AT Commands - Generic Modem Control
"&F"									// Set To Factory-Defined Configuration
"Z"									// Soft Reset 34
"+FCLASS"						// Select Active Service Class 34
"&Y"									// Designate A Default Reset Basic Profile 35
"&P"									// Designate A Default Reset Full Profile 35
"&W"								// Store Current Configuration 36
"&Z"								// Store Telephone Number In The Module

//Internal Phonebook
"&N"								// Display Internal Phonebook Stored Numbers 36
"+GMI"							// Manufacturer Identification 36
"+GMM"							// Model Identification 37
"+GMR"							// Revision Identification 37
"+GCAP"							// Capabilities List 37
"+GSN"							// Serial Number 37
"&V"								// Display Current Base Configuration And

//Profile
"&V1"								// S Registers Display 38
"&V0"								// Display Current Configuration And Profile 38
"&V3"								// Extended S Registers Display 38
"&V2"								// Display Last Connection Statistics 39
"\V"									// Single Line Connect Message 39
"+GCI"								// Country Of Installation 39
"%L"								// Line Signal Level 39
"%Q"								// Line Quality 40
"L"									// Speaker Loudness 40
"M"									// Speaker Mode 40

//Hayes AT Commands - DTE-Modem Interface Control
"E"									// Command Echo 40
"Q"									// Quiet Result Codes 40
"V"									// Response Format 41
"X"									// Extended Result Codes 42
"I"									// Identification Information 43
"&C"								// Data Carrier Detect (DCD) Control 43
"&D"								// Data Terminal Ready (DTR) Control 43
"\\Q"								// Standard Flow Control 45
"&K"									// Flow Control 45
"&S"									// Data Set Ready (DSR) Control 46
"\\R"									// Ring (RI) Control 46
"+IPR"								// Fixed DTE Interface Rate 47
"+IFC"								// DTE-Modem Local Flow Control 48
"+ILRR"							// DTE-Modem Local Rate Reporting 49
"+ICF"								// DTE-Modem Character Framing 49

//Hayes AT Commands - Call Control
"D"									// Dial 50
"T"									// Tone Dial 54
"P"									// Pulse Dial 55
"A"									// Answer 55
"H"									// Disconnect 55
"O"									// Return To On Line Mode 55
"&G"								// Guard Tone 56
"&Q"								// Sync/Async Mode 56

//Hayes AT Commands - Modulation Control
"+MS"								// Modulation Selection 56
"%E"								// Line Quality Monitor And Auto Retrain Or Fallback/Fallforward

//Hayes AT Commands - Compression Control
"+DS"								// Data Compression 57
"+DR"								// Data Compression Reporting 57

//Hayes AT Commands - Break Control
"\\B"									// Transmit Break To Remote 58
"\\K"									// Break Handling 58
"\\N"									// Operating Mode 58

//Hayes AT Commands - S Parameters
"S0"									// Number Of Rings To Auto Answer 59
"S1"									// Ring Counter 60
"S2"									// Escape Character 60
"S3"									// Command Line Termination Character 61
"S4"									// Response Formatting Character 62
"S5"									// Command Line Editing Character 63
"S7"									// Connection Completion Time-Out 63
"S12"								// Escape Prompt Delay 64
"S25"								// Delay To DTR Off 65
"S30"								// Disconnect Inactivity Timer 66
"S38"								// Delay Before Forced Hang Up 66

//ETSI GSM 07.07 - General
"+CGMI"							// Request Manufacturer Identification 68
"+CGMM"						// Request Model Identification 68
"+CGMR"						// Request Revision Identification 68
"+CGSN"							// Request Product Serial Number Identification 69
"+CSCS"							// Select TE Character Set 69
"+CIMI"							// Request International Mobile Subscriber

//Identity (IMSI) 70
"+CMUX"							// Multiplexing Mode 70
"+WS46"							// PCCA STD-101 Select Wireless Network 71

//ETSI GSM 07.07 - Call Control
"+CHUP"							// Hang Up Call 71
"+CBST"							// Select Bearer Service Type 71
"+CRLP"							// Radio Link Protocol 73
"+CR"								// Service Reporting Control 74
"+CEER"							// Extended Error Report 75
"+CRC"							// Cellular Result Codes 76
"+CSNS"							// Single Numbering Scheme 77
"+CVHU"							// Voice Hang Up Control 77

//ETSI GSM 07.07 - Network Service Handling
"+CNUM"							// Subscriber Number 78
"+COPN"								// Read Operator Names 79
"+CREG"								// Network Registration Report 80
"+COPS"								// Operator Selection 83
"+CLCK"								// Facility Lock/Unlock 87
"@CLCK"								// Facility Improved Lock/Unlock 89
"+CPWD"							// Change Facility Password 91
"+CLIP"								// Calling Line Identification Presentation 92
"+CLIR"								// Calling Line Identification Restriction 95
"+CCFC"								// Call Forwarding Number And Conditions 96
"+CCWA"							// Call Waiting 97
"+CHLD"								// Call Holding Services 101
"+CUSD"								// Unstructured Supplementary Service Data 102
"+CAOC"								// Advice Of Charge 104
"+CLCC"								// List Current Calls 106
"+CSSN"								// SS Notification 108
"+CCUG"								// Closed User Group Supplementary Service Control 110
"+CPOL"								// Preferred Operator List 111

//ETSI GSM 07.07 - Mobile Equipment Control
"+CPAS"								// Phone Activity Status 111
"+CFUN"								// Set Phone Functionality 112
"+CPIN"								// Enter PIN 114
"+CSQ"								// Signal Quality 120
"+CIND"								// Indicator Control 121
"+CMER"								// Mobile Equipment Event Reporting 123
"+CPBS"								// Select Phonebook Memory Storage 123
"+CPBR"								// Read Phonebook Entries 125
"+CPBF"								// Find Phonebook Entries 128
"+CPBW"								// Write Phonebook Entry 129
"+CCLK"								// Clock Management 131
"+CALA"								// Alarm Management 133
"+CRSM"								// Restricted SIM Access 137
"+CALM"								// Alert Sound Mode 138
"+CRSL"								// Ringer Sound Level 139
"+CLVL"								// Loudspeaker Volume Level 141
"+CMUT"								// Microphone Mute Control 142
"+CACM"								// Accumulated Call Meter 143
"+CAMM"							// Accumulated Call Meter Maximum 144
"+CPUC"								// Price Per Unit And Currency Table 145
"+CLAC"								// Available AT commands 146
"+CALD"								// Delete Alarm 146
"+CCID"								// Read ICCID (Integrated Circuit Card Identification)

//ETSI GSM 07.07 - Mobile Equipment Errors
"+CMEE"								// Report Mobile Equipment Error 147

//ETSI GSM 07.07 - Voice Control
"+VTS"								// DTMF Tones Transmission 148
"+VTD"								// Tone Duration 149

//ETSI GSM 07.07 - Commands For GPRS
"+CGCLASS"						// GPRS Mobile Station Class 150
"+CGATT"							// GPRS Attach Or Detach 151
"+CGEREP"							// GPRS Event Reporting 152
"+CGREG"							// GPRS Network Registration Status 153
"+CGDCONT"						// Define PDP Context 155
"+CGQMIN"							// Quality Of Service Profile (Minimum Acceptable) 157
"+CGQREQ"						// Quality Of Service Profile (Requested) 160
"+CGACT"							// PDP Context Activate Or Deactivate 162
"+CGPADDR"						// Show PDP Address 163
"+CGDATA"							// Enter Data State 165

//ETSI GSM 07.07 - Commands For Battery Charger
"+CBC"								// Battery Charge

// ETSI GSM 07.05 - General Configuration
"+CSMS"								// Select Message Service 169
"+CPMS"								// Preferred Message Storage 170
"+CMGF"								// Message Format 173

//ETSI GSM 07.05 - Message Configuration
"+CSCA"								// Service Center Address 174
"+CSMP"								// Set Text Mode Parameters 175
"+CSDH"								// Show Text Mode Parameters 180
"+CSCB"								// Select Cell Broadcast Message Types 181
"+CSAS"								// Save Settings 183
"+CRES"								// Restore Settings 184

//ETSI GSM 07.05 - Message Receiving And Reading
"+CNMI"								// New Message Indications To Terminal Equipment
"+CMGL"								// List Messages 196
"@CMGL"							// List Messages Improved 202
"+CMGR"							// Read Message 204
"@CMGR"							// Read Message Improved 211

//ETSI GSM 07.05 - Message Sending And Writing
"+CMGS"								// Send Message 215
"+CMSS"								// Send Message From Storage 222
"+CMGW"							// Write Message To Memory 223
"+CMGD"							// Delete Message 230

//FAX AT Commands - General Configuration
"+FMI"									// Manufacturer ID 233
"+FMM"								// Model ID 233
"+FMR"								// Revision ID 233

//FAX AT Commands - Transmission/Reception Control
"+FTS"								// Stop Transmission And Pause 233
"+FRS"								// Wait For Receive Silence 234
"+FTM"								// Transmit Data Modulation 234
"+FRM"								// Receive Data Modulation 234
"+FTH"								// Transmit Data With HDLC Framing 235
"+FRH"								// Receive Data With HDLC Framing 236

//FAX AT Commands - Serial Port Control
"+FLO"								// Select Flow Control Specified By Type 236
"+FPR"								// Select Serial Port Rate 237
"+FDD"								// Double Escape Character Replacement Control

//Custom AT Commands - General Configuration
"+PACSP"							// Network Selection Menu Availability 238
"#CGMI"								// Manufacturer Identification 238
"#CGMM"							//Model Identification 238
"#CGMR"							// Revision Identification 239
"#CGSN"								// Product Serial Number Identification 239
"#CIMI"								// International Mobile Subscriber Identity (IMSI)
"#CCID"								// Read ICCID (Integrated Circuit Card 239
"#SPN"								// Service Provider Name 240
"#CEER"								// Extended Numeric Error Report 240
"#CAP"								// Change Audio Path 242
"#SRS"								// Select Ringer Sound 243
"#SRP"								// Select Ringer Path 245
"#STM"								// Signaling Tones Mode 246
"#TONE"								// Tone Playback 247
"#TSVOL"							// Tone Classes Volume 247
"#REGMODE"						// Select Registration Operation Mode 249
"#SMSMODE"						// SMS Commands Operation Mode 249
"#PLMNMODE"					// PLMN List Selection 250
"#PCT"								// Display PIN Counter 250
"#SHDN"								// Software Shut Down 251
"#Z"									// Extended Reset 251
"#WAKE"								// Wake From Alarm Mode 251
"#QTEMP"							// Query Temperature Overflow 253
"#TEMPMON"						// Temperature Monitor 254
"#SGPO"								// Set General Purpose Output 256
"#GGPI"								// General Purpose Input 256
"#GPIO"								// General Purpose Input/Output Pin Control 257
"#SLED"								// STAT_LED GPIO Setting 261
"#SLEDSAV"						// Save STAT_LED GPIO Setting 261
"#DVI"									// Digital Voiceband Interface 261
"#E2SMSRI"						// SMS Ring Indicator 262
"#ADC"								// Analog/Digital Converter Input 263
"#DAC"								// Digital/Analog Converter Control 265
"#VAUX"								// Auxiliary Voltage Output Control 266
"#VAUXSAV"						// #VAUX Saving 268
"#V24CFG"							// V24 Output Pins Configuration 268
"#V24"								// V24 Output Pins Control 268
"#AXE"								// AXE Pin Reading 269
"#TXMONMODE"				// TTY-CTM-DSP Operating Mode 270
"#CBC"								// Battery and Charger Status 270
"#AUTOATT"						// GPRS Auto-Attach Property 271
"#MSCLASS"						// Multislot Class Control 272
"#MONI"								// Cell Monitor 273
"#SERVINFO"						// Serving Cell Information 278
"#COPSMODE"					// +COPS Mode 279
"#QSS"								// Query SIM Status 280
"#DIALMODE"						// ATD Dialing Mode 281
"#ACAL"								// Automatic Call 283
"#ACALEXT"						// Extended Automatic Call 284
"#ECAM"								// Extended Call Monitoring 284
"#SMOV"								// SMS Overflow 286
"#MBN"								// Mailbox Numbers 287
"#MWI"								// Message Waiting Indicator 288
"#CODEC"							// Audio Codec 289
"#SHFEC"							// Handsfree Echo Canceller 290
"#HFMICG"							// Handsfree Microphone Gain 291
"#HSMICG"							// Handset Microphone Gain 292
"#SHFSD"							// Set Headset Sidetone 293
"#SPKMUT"							// Speaker Mute Control 294
"#HFRECG"							// Handsfree Receiver Gain 294
"#HSRECG"						// Handset Receiver Gain 294
"#PRST"								// Audio Profile Factory Configuration 295
"#PSAV"								// Audio Profile Configuration Save 295
"#PSEL"								// Audio Profile Selection 296
"#PSET"								// Audio Profile Setting 296
"#SHFAGC"							// Handsfree Automatic Gain Control 297
"#SHFNR"							// Handsfree Noise Reduction 297
"#SHSAGC"						// Handset Automatic Gain 298
"#SHSEC"							// Handset Echo Canceller 298
"#SHSNR"							// Handset Noise Reduction 299
"#SHSSD"							// Set Handset Sidetone 299
"#/"										// Repeat Last Command 299
"#NITZ"								// Network Timezone 299
"#ENS"								// Enhanced Network Selection 301
"#BND"								// Select Band 302
"#AUTOBND"						// Automatic Band Selection 303
"#SKIPESC"							// Skip Escape Sequence 304
"#E2ESC"							// Escape Sequence Guard Time 305
"#GAUTH"							// PPP-GPRS Connection Authentication Type 306
"#GPPPCFG"						// PPP-GPRS Parameters Configuration 307
"#RTCSTAT"						// RTC Status 308
"#GSMAD"							// GSM Antenna Detection 309
"#SIMDET"							// SIM Detection Mode 310
"#ENHSIM"							// SIM Enhanced Speed 311
"#TTY"									// Teletype Writer Support 312
"#CPUMODE"						// CPU Clock Mode 312
"#GSMCONT"						// GSM Context Definition 312
"#CGPADDR"						// Show Address 313
"#NWSCANTMR"				// Network Selection Timer 314

//Custom AT Commands - General Configuration - Special Issues
"#OSC32KHZ"    				// External 32kHz Oscillator 315

//Custom AT Commands - Multisocket
"#SS"                           // Socket Status 315
"#SI"                           // Socket Info 316
"#SGACT"                        // Context Activation 317
"#SH"                           // Socket Shutdown 318
"#SCFG"                         // Socket Configuration 319
"#SCFGEXT"                      // Socket Configuration Extended 320
"#SD"                           // Socket Dial 321
"#SO"                           // Socket Restore 322
"#SL"                           // Socket Listen 323
"#SA"                           // Socket Accept 323
"#SRECV"                        // Receive Data In Command Mode 324
"#SSEND"                        // Send Data In Command Mode 325

//Custom AT Commands - FTP
"#FTPTO"                        // FTP Time-Out 326
"#FTPOPEN"                      // FTP Open 326
"#FTPCLOSE"                     // FTP Close 327
"#FTPPUT"                       // FTP Put 327
"#FTPGET"                       // FTP Get 328
"#FTPTYPE"                      // FTP Type 329
"#FTPMSG"                       // FTP Read Message 330
"#FTPDELE"                      // FTP Delete 330
"#FTPPWD"                       // FTP Print Working Directory 330
"#FTPCWD"                       // FTP Change Working Directory 331
"#FTPLIST"                      // FTP List 331

//Custom AT Commands - Enhanced Easy GPRS(r) Extension
"#USERID"                       // Authentication User ID 332
"#PASSW"                        // Authentication Password 333
"#PKTSZ"                        // Packet Size 334
"#DSTO"                         // Data Sending Time-Out 335
"#SKTTO"                        // Socket Inactivity Time-Out 336
"#SKTSET"                       // Socket Definition 338
"#SKTOP"                        // Socket Open 340
"#QDNS"                         // Query DNS 341
"#CACHEDNS"                     // DNS Response Caching 342
"#DNS"                          // Manual DNS Selection 342
"#SKTCT"                        // Socket TCP Connection Time-Out 344
"#SKTSAV"                       // Socket Parameters Save 345
"#SKTRST"                       // Socket Parameters Reset 346
"#GPRS"                         // GPRS fext Activation 346
"#SKTD"                         // Socket Dial 349
"#SKTL"                         // Socket Listen 352
"@SKTL"                         // Socket Listen Improved 356
"#E2SLRI"                       // Socket Listen Ring Indicator 358
"#FRWL"                         // Firewall Setup 358
"#GDATAVOL"                     // GPRS Data Volume 360
"#ICMP"                         // ICMP Support 362
"#TCPMAXDAT"                    // Maximum TCP Payload Size 362
"#TCPREASS"                     // TCP Reassembly 363

//Custom AT Commands - E-Mail Management
"#ESMTP"                        // E-mail SMTP Server 363
"#EADDR"                        // E-mail Sender Address 364
"#EUSER"                        // E-mail Authentication User Name 365
"#EPASSW"                       // E-mail Authentication Password 366
"#SEMAIL"                       // E-mail Sending With GPRS Context Activation 367
"#EMAILACT"                     // E-mail GPRS Context Activation 369
"#EMAILD"                       // E-mail Sending 371
"#ESAV"                         // E-mail Parameters Save 373
"#ERST"                         // E-mail Parameters Reset 374
"#EMAILMSG"                     // SMTP Read Message 375

//Custom AT Commands - Easy Scan(r) Extension
"#CSURV"                        // Network Survey 375
"#CSURVC"                       // Network Survey (Numeric Format) 381
"#CSURVU"                       // Network Survey Of User Defined Channels 387
"#CSURVUC"                      // Network Survey Of User Defined Channels (Numeric Format)
"#CSURVB"                       // BCCH Network Survey 390
"#CSURVBC"                      // BCCH Network Survey (Numeric Format) 390
"#CSURVF"                       // Network Survey Format 391
"#CSURVNLF"                     // <CR><LF> Removing On Easy Scan(r)

//Commands Family 392
"#CSURVEXT"                     // Extended Network Survey 393
"#CSURVP"                       // PLMN Network Survey 393
"#CSURVPC"                      // PLMN Network Survey (Numeric Format) 394

//Custom AT Commands - SIM Toolkit
"#STIA"                         // SIM Toolkit Interface Activation 394
"#STGI"                         // SIM Toolkit Get Information 399
"#STSR"                         // SIM Toolkit Send Response 404

//Jammed Detect & Report AT commands
"#JDR"                          // Jammed Detect & Report 406

//Custom AT Commands - Easy Script(r) Extension - Python Interpreter13
"#WSCRIPT"                      // Write Script 409
"#ESCRIPT"                      // Select Active Script 411
"#STARTMODESCR"                 // Script Execution Start Mode 412
"#EXECSCR"                      // Execute Active Script 414
"#RSCRIPT"                      // Read Script 414
"#LSCRIPT"                      // List Script Names 415
"#DSCRIPT"                      // Delete Script 416
"#REBOOT"                       // Reboot 417
"#CMUXSCR"                      // CMUX Interface Enable 417

//Custom AT Commands - GPS Application
"$GPSP"                         // GPS Controller Power Management 418
"$GPSR"       								  // GPS Reset 419
"$GPSD"                         // GPS Device Type Set 419
"$GPSSW"                        // GPS Software Version 422
"$GPSAT"                        // GPS Antenna Type Definition 420
"$GPSAV"                        // GPS Antenna Supply Voltage Readout 421
"$GPSAI"                        // GPS Antenna Current Readout 421
"$GPSAP"                        // GPS Antenna Protection 421
"$GPSS14"                       // GPS NMEA Serial Port Speed 422
"$GPSNMUN"                      // Unsolicited GPS NMEA Data Configuration 423
"$GPSACP"                       // GPS Actual Position Information 424
"$GPSCON"                       // Direct Access To GPS Module 425
"$GPSPRG"                       // Set The GPS Module In Programming Mode 426
"$GPSPS"                        // Set the GPS Module In Power Saving Mode 426
"$GPSWK"                        // Wake Up GPS From Power Saving Mode 427
"$GPSSAV"                       // Save GPS Parameters Configuration 427
"$GPSRST"                       // Restore Default GPS Parameters 427
"$GPSCMODE"                     // GPS Controller Disabled at Start-up With Charger Inserted 428

//Custom AT Commands - SAP
"#RSEN"                         // Remote SIM Enable 428

//Custom AT Commands - Telefonica OpenGate M2M
"#OGCFG"                        // OG Protocol Parameters Configuration 430
"#OGPLATCFG"                    // OG Platform Parameters Configuration 430
"#OGBEGINMSG"                   // OG Total Message Creation Start 430
"#OGBEGINOGMSG"                 // OGMessage Creation Start 430
"#OGADDPAR"                     // OGMessage Parameter Insertion 430
"#OGBEGINARRY"                  // OGMessage Array Insertion Start 430
"#OGADDARRY"                    // OGMessage Array Parameter Insertion 430
"#OGENDARRAY"                   // OGMessage Array Parameter Insertion End 430
"#OGENDOGMSG"                   // OGMessage Creation End 430
"#OGABORTMSG"                   // Message Creation Abort 430
"#OGENDMSG"                     // Message Creation End 430
"#OGSENDMSG"                    // Send OG Total Message 430
"#OGMSGSTATUS"                  // Get Pending OGMessage's Status 430
"#OGRETOGMSG"                   // Decode Received OGMessage 430
"#OGERASEALL"                   // Erase OGMessage's Status List 430
"#OGMSG"                        // OGMessage Received Indication 430
"#OGMSGTOUT"                    // OGMessage Sending Timeout Indication 430
*/
