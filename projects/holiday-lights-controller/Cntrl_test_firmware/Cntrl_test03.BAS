'Cntrl_test03
'Holiday Lights Controller
'Peter J. Stonard
'December 12, 06
'Revised October 7, 07
'
'  ####################################################################
'  #  Initialize RTC, set to 1pps output (Red Led blinks)             #
'  #  Display "Welcome Test03" on LCD (LCD Required)                  #
'  #  Use LCD to display channel (1 to 32) and status (on or off)     #
'  #  Only uses IO card 1 (8 channels) IO card address = (A0     = 1) #
'  #  Use encoder to change channel and channel data                  #
'  #  Store channel data in EEPROM IC                                 #
'  #  IO channels over I2C to follow data in EEPROM                   #
'  #  To verify EEPROM hold data, set up pattern and reboot           #
'  #  Remove EEPROM and all channels turn on (default data = &HFF     #
'  #  Replace EEPROM and all channel revert to data stored in EEPROM  #
'  ####################################################################
'
'  ####################################################################
'  #  EEPROM Types. Important!                                        #
'  #  There are several different EEPROM devices. This program is     #
'  #  constructed for a two-byte address. Only use:                   #
'  #  24C32 (4k x 8)                                                  #
'  #  24C64 (8k x 8)                                                  #
'  #  24C128 (16k x 8)                                                #
'  #  24C256 (32k x 8)                                                #
'  ####################################################################
'****************************************************************************
'* Hardware Definition                                                      *
'****************************************************************************
'I2C bus connects a DS1307, an EEPROM, I2C output to other cards, LCD 16x1
'and a MUX two digit LED display
'Bit7 = MSB
'
'Encoder, Switch, 120Hz Int0, LCD data on Port D.
'PD7 Does Not Exist! Always Set to zero
'
'PD6 = DB0
'PD5 = DB1
'PD4 = DB2
'PD3 = DB3
'PD2 = 120Hz
'PD1 = ENCA
'PD0 = ENCB
'
'LCD on Port C.
'PC7 = Does Not Exist! Always Set to zero
'PC6 = RESET*
'PC5 = DB7
'PC4 = DB6
'PC3 = DB5
'PC2 = DB4
'PC1 = E
'PC0 = RS
'
'Prog, I2C, LED Anodes on Port B.
'PB7 = SEL_R*
'PB6 = SEL_L*
'PB5 = SCK
'PB4 = MISO
'PB3 = MOSI
'PB2 = not used
'PB1 = SCL  (I2C Data)
'PB0 = SDA  (I2C Clock)
'
'****************************************************************************
'* Define Variables                                                         *
'****************************************************************************
' Variables stored as a bit
Dim Direction As Bit
Dim Int_sec As Bit
Dim Int_3sec As Bit
Dim Int_hsec As Bit
Dim Int_qsec As Bit
Dim Lcd_en As Bit
Dim Led_en As Bit
Dim Led_inv As Bit
Dim New_sw As Bit
Dim Old_switch As Bit
Dim R_eeprom As Bit
Dim Read_sw As Bit
Dim Rtc_en As Bit
Dim W_eeprom As Bit
'
' Variables stored as a byte
Dim Adresh As Byte                      'EEPROM device read address
Dim Adresl As Byte                      'EEPROM device write address
Dim Cnt1 As Byte                        'Count timer cycles (64 = 1/2 AC cycle)
Dim Cnt2 As Byte                        'Count I2C bus access (AC 1/2 cycles)
Dim Cnt3 As Byte                        'LCD cursor counter ( 1 to 16)
Dim Ee_dat1 As Byte                     'EEPROM data lower nibble as byte
Dim Ee_dat2 As Byte                     'EEPROM data upper nibble as byte
Dim Ch_num As Byte                      'Location of active channel (1 to 32)
Dim Hsec_cnt As Byte                    'counter for half second interrupt
Dim Lcd_data As Byte                    'LCD data
Dim New_data As Byte
Dim On_data(32) As Byte                 'Output data array
Dim Pos_data As Byte                    'Temp Position data
Dim Qsec_cnt As Byte                    'counter for quarter second interrupt
Dim Sec_cnt As Byte                     'counter for one second interrupt
Dim Seed0 As Byte                       'Timer0 counter value
Dim Seed1 As Byte                       'Timer1 counter value
Dim Switch_data As Byte
Dim Tempb As Byte                       'Temp storage
Dim Tempc As Byte                       'Temp storage
Dim Tmp_enc1 As Byte                    'Temp storage
Dim Tmp_enc2 As Byte                    'Temp storage
Dim Tmp_ee1 As Byte                     'Temp storage
Dim Tmp_ee2 As Byte                     'Temp storage
Dim Tempg As Byte                       'Temp storage
Dim Temph As Byte                       'Temp storage
Dim Lamp_dat1(16) As Byte               'Output data array
'
' Variables stored as a word
Dim 3sec_cnt As Word                    '3 Sec interrupt counter
Dim Tempa As Word                       'Temp storage
Dim Tempd As Word                       'Temp storage
Dim Chars(10) As Word                   'Character look up table
'
' Variables stored as strings
Dim Lcd_msg As String * 16              'Current message storage space
Dim Msg1 As String * 16                 'Storage for a message
Dim Msg2 As String * 16                 'Storage for a message
Dim Msg3 As String * 3                  'Storage for a message
'
'****************************************************************************
'* Alias                                                                    *
'****************************************************************************
Switch Alias Pind.3
Enca Alias Pind.1
Encb Alias Pind.0
'****************************************************************************
'* Define Subroutines                                                       *
'****************************************************************************
Declare Sub Clock_init
Declare Sub Read_eeprom
Declare Sub Write_eeprom
'
'****************************************************************************
'* Define AVR                                                               *
'****************************************************************************
$regfile = "m168def.dat"                'Change for your AVR.
$crystal = 8000000                      '8 MHz Crystal.
Config Lcdpin = Pin , Db4 = Portc.2 , Db5 = Portc.3 , Db6 = Portc.4 , Db7 = Portc.5 , E = Portc.1 , Rs = Portc.0
Config Lcd = 16 * 1
Config I2cdelay = 1                     'Use the fastest I2C clock
'
'****************************************************************************
'* Define Ports & Data Direction Registers  (In = 0, Out = 1)               *
'****************************************************************************
' Port B
Portb = &B0000_0000                     'Set All Port Pins Low
Ddrb = &B1111_0111                      'Set encoder pins as inputs (tri-state)
'
' Port C
Portc = &B0000_0000                     'Set All Port Pins Low
Ddrc = &B1011_1111                      'Set pins as outputs
'
' Port D
Portd = &B0000_0000                     'Set All Port Pins Low
Ddrd = &B1111_0000                      'Set pins as inputs and outputs
Reset Ucr.3                             'Disable UART
Reset Ucr.4                             'Disable UART
'
'****************************************************************************
'* Define I2C pins                                                          *
'****************************************************************************
Config Sda = Portb.0                    ' I2C Data.
Config Scl = Portb.1                    ' I2C Clock
'
'****************************************************************************
Const Ds1307w = &HD0                    'Address Of DS1307 Write
Const Ds1307r = &HD1                    'Address Of DS1307 Read
'
Const Ds1307_add_sec = &H00             'Address Of DS1307 Seconds
Const Ds1307_add_min = &H01             'Address Of DS1307 Min
Const Ds1307_add_hour = &H02            'Address Of DS1307 Hour
Const Ds1307_add_control = &H07         'Address Of DS1307 Control
'
Const Io_1 = &H72                       'Address of IO Card1 (A0     = 1)
Const Io_2 = &H74                       'Address of IO Card2 (A1     = 1)
Const Io_3 = &H76                       'Address of IO Card3 (A0,A1  = 1)
Const Io_4 = &H78                       'Address of IO Card4 (A2     = 1)
'
Const Adresw = &HA0                     'write address of 24C256
Const Adresr = &HA1                     'read address of 24C256
'****************************************************************************
'* LED Inversion                                                            *
'****************************************************************************
Led_inv = 0                             'change to 1 for inverted LED display
'                                       'change to 0 for normal LED display
'****************************************************************************
'* Init Variables                                                           *
'****************************************************************************
Adresl = &H00                           'low byte address
Adresh = &H00                           'high byte address
Msg1 = "Welcome Test03  "
Msg2 = "Ch:         D:  "
Cnt2 = 2
New_sw = 1
Ch_num = 1
Switch_data = 1
Seed0 = 125                             'Use 125 max!
Seed1 = 133                             '133 for 60Hz free-running osc.
'****************************************************************************
'* Power On Reset                                                           *
'****************************************************************************
Gosub Clock_init                        'set up RTC
If Led_inv = 0 Then                     'Use Inverted LED data
   Restore Charz1
      For Tempa = 1 To 10
         Read Chars(tempa)
         Next Tempa
   End If
'
If Led_inv = 1 Then                     'Use non-Inverted LED data
   Restore Charz2
      For Tempa = 1 To 10
         Read Chars(tempa)
         Next Tempa
   End If
'
' Following code is rem'd out. Used for program development only.
'
'Set Up Data In on_data array
'Tempb = &H01
'  For Tempc = 1 To 32                   'Fill 32 bytes in on_data array
'     On_data(tempc) = Tempb             'send a value
'     Incr Tempb
'     Next Tempc
'
'Set Up Test Data In Eeprom Memory from data table "Mem_data"
'Tempb = &HFF
'Adresh = 0
'   Restore Mem_data
'   For Adresl = 1 To 16                 'Fill 16 bytes in memory block
'     Read Tempb
'      I2cstart                          'generate start
'      I2cwbyte Adresw                   'send mem device address
'      I2cwbyte Adresh                   'send mem high address
'      I2cwbyte Adresl                   'send mem low address
'      I2cwbyte Tempb                    'send a value
'      I2cstop                           'generate stop
'      Waitms 10                         'wait 10 mS (write cycle time)
'      Next Adresl
'
'****************************************************************************
'* Main Program                                                             *
'****************************************************************************
Main:
Portb.6 = 1                             'Turn off LED Display
Portb.7 = 1
'
Initlcd                                 ' start LCD display
Wait 1                                  'Delay until LCD and uC clock running
Cls
Cursor Off
Locate 1 , 1
Lcd = Mid(msg1 , 1 , 16)                'Display Welcome message
Wait 3
Lcd_msg = Msg2                          'Write test03 template to LCD
'
Config Timer0 = Timer , Prescale = 8    'Timer0 generates 64*120Hz clk
Load Timer0 , Seed0
'
Config Timer1 = Timer , Prescale = 1024 'Timer1 lost sync watchdog
Load Timer1 , Seed1
'
'Read 16 bytes of EEPROM memory into on_data array (32 channels)
Adresh = &H00
' (
For Tempb = 1 To 16                     'Tempb = address of memory (1 to 16)
   Tempc = Tempb
   Shift Tempc , Left                   'Tempc = address of on_data (1 to 32)
   Adresl = Tempb
   Gosub Read_eeprom
   On_data(tempc) = Ee_dat1
   On_data(tempc - 1) = Ee_dat2
   Next Tempb
' )
Enable Int0
Enable Timer1
'
On Timer0 Timer0_isr                    'Interrupt service routine vectors
On Timer1 Timer1_isr
On Int0 Int_isr
'
Enable Interrupts                       'Enable interrupts
'

Do
If Rtc_en = 1 Then                      'Read and write to I2C during AC-cross
' RTC read and write can only be done when the I2C bus is not required for
' triac control during the AC half-cycle. RTC_en = 1 when the I2C bus is
' available.
   If R_eeprom = 1 And W_eeprom = 0 Then
      Gosub Read_eeprom                 'Read one byte of the EEPROM
      R_eeprom = 0                      'Clear flags
      Rtc_en = 0
      End If
'
   If W_eeprom = 1 And R_eeprom = 0 Then
      Gosub Write_eeprom                'Write one byte of the EEPROM
      W_eeprom = 0                      'Clear flags
      Rtc_en = 0
      End If
'
' Prepare the lamp data to turn on selected channels at the right time-slot
' for correct brightness of each channel.
   For Tempc = 1 To 16                  'Reset lamp data to &HFF (lamps off)
         lamp_dat1(tempc) = &HFF
         Next tempc
   '
   Tempb = 17 - On_data(1)
   For Tempc = Tempb To 16
         lamp_dat1(tempc).0 = 0
         Next Tempc
   '
   Tempb = 17 - On_data(2)
   For Tempc = Tempb To 16
         lamp_dat1(tempc).1 = 0
         Next Tempc
   '
   Tempb = 17 - On_data(3)
   For Tempc = Tempb To 16
         lamp_dat1(tempc).2 = 0
         Next Tempc
   '
   Tempb = 17 - On_data(4)
   For Tempc = Tempb To 16
         lamp_dat1(tempc).3 = 0
         Next Tempc
   '
   Tempb = 17 - On_data(5)
   For Tempc = Tempb To 16
         lamp_dat1(tempc).4 = 0
         Next Tempc
   '
   Tempb = 17 - On_data(6)
   For Tempc = Tempb To 16
         lamp_dat1(tempc).5 = 0
         Next Tempc
   '
   Tempb = 17 - On_data(7)
   For Tempc = Tempb To 16
         lamp_dat1(tempc).6 = 0
         Next Tempc
   '
   Tempb = 17 - On_data(8)
   For Tempc = Tempb To 16
         lamp_dat1(tempc).7 = 0
         Next Tempc
   '
   End If
'
' Read data for current ch_num from EEPROM  store into on_data array
If Int_qsec = 1 Then                    'Update once every qtr sec
'
   If Switch_data = 0 Then
      Adresh = &H00
      Tempb = Ch_num
      Shift Tempb , Right , 1
      Adresl = Tempb
      R_eeprom = 1                      'Request EEPROM read
      If R_eeprom = 0 Then
         If Ch_num.0 = 1 Then           'ch_num is even
            On_data(ch_num - 1) = Ee_dat1
            End If
         If Ch_num.0 = 0 Then           'ch_num is odd
            On_data(ch_num - 1) = Ee_dat2
            End If
         End If
      End If
' Prepare the LCD for data
   Lcd_msg = Msg2                       'Write template to LCD
   Temph = Makebcd(ch_num)
   Tempb = Temph And &H0F
   Tempb = Tempb + 48                   'Convert Channel no. LSD data to ASCII
   Mid(lcd_msg , 5 , 1) = Tempb         'Write channel number data to LCD
   Tempb = Temph And &HF0
   Shift Tempb , Right , 4
   Tempb = Tempb + 48                   'Convert Channel no. MSD data to ASCII
   Mid(lcd_msg , 4 , 1) = Tempb         'Write Channel number data to LCD
'
   Tempb = Ch_num                       'Display 'off' if on_data = 0, or 'on'
   If On_data(ch_num) = 0 Then Msg3 = "Off"
   If On_data(ch_num) > 0 Then Msg3 = "On "
   Mid(lcd_msg , 8 , 3) = Msg3          'Write on or off data to LCD
'
   If On_data(ch_num) < &H0A Then Lcd_data = On_data(ch_num) + &H30
   If On_data(ch_num) > &H09 Then Lcd_data = On_data(ch_num) + &H37
   Mid(lcd_msg , 15 , 1) = Lcd_data     'Write EEPROM data to LCD
'
      Mid(lcd_msg , Pos_data , 1) = "<" 'Use brackets to indicate which
      Tempb = Pos_data + 4              'data set is controlled by the encoder
      Mid(lcd_msg , Tempb , 1) = ">"
'
   If Switch_data = 2 Then              'Write new on_data back to eeprom
      Adresh = &H00
      Tempb = Ch_num
      Tempb = Tempb + 1
      Shift Tempb , Right , 1
      Adresl = Tempb
         If Ch_num.0 = 1 Then           'ch_num is even
            Ee_dat1 = On_data(ch_num + 1)
            Ee_dat2 = On_data(ch_num)
            End If
         If Ch_num.0 = 0 Then           'ch_num is odd
            Ee_dat1 = On_data(ch_num)
            Ee_dat2 = On_data(ch_num - 1)
            End If
      W_eeprom = 1                      'Request EEPROM write
      Switch_data = 0                   'Force back to idle mode
      End If
'
   Int_qsec = 0
   End If
'
If Read_sw = 1 Then                     'Poll the switch every AC half-cycle
  If Switch <> New_sw Then Toggle New_sw       'debounce switch
  If New_sw = 0 And Old_switch = 1 Then
     Incr Switch_data
     If Switch_data = 3 Then Switch_data = 0       'Force back to idle mode
     End If
  Old_switch = New_sw
  '#####################################################################
  '#     Switch Data:                                                  #
  '#     0 Select channel LCD displays data from encoder and EEPROM    #
  '#     1 channel turns on                                            #
  '#     2 New data saved to EEPROM                                    #
  '#     3 Force reset to switch_data = 0                              #
  '#####################################################################
'
' Check Encoder for new encoder data
     tmp_enc1 = tmp_enc1 And &B0000_0011
     tmp_enc2 = tmp_enc1
     Shift Tmp_enc1 , Left , 2          'save last setting
     Tempg.0 = Enca                     'tmp_enc1 = old & Tempd = new
     Tempg.1 = Encb
     tmp_enc1 = tmp_enc1 Or Tempg
' Test the new encoder data to determine encoder direction
     Select Case tmp_enc1
     Case 1
     Direction = 0
     Case 2
     Direction = 1
     Case 4
     Direction = 1
     Case 7
     Direction = 0
     Case 8
     Direction = 0
     Case 11
     Direction = 1
     Case 13
     Direction = 1
     Case 14
     Direction = 0
     End Select
'
     If Switch_data = 0 Then
' Use the new encoder data to change the channel number by one
' Roll over the ch_num at 32 -> 1, and 1 -> 32
        Pos_data = 3                    'Point to on/off data on the LCD
        Tempb = Ch_num
        If tmp_enc2 <> Tempg And Direction = 1 Then Decr Tempb
        If Tempb = 33 Then Tempb = 1
        If tmp_enc2 <> Tempg And Direction = 0 Then Incr Tempb
        If Tempb = 0 Then Tempb = 32
        Ch_num = Tempb
        New_data = Makebcd(ch_num)      'Save the encoder data for LED display
        End If
  '
     If Switch_data = 1 Then
' Use the new encoder data to change the channel data by one
' Roll over the on_data at 15 -> 0, and 0 -> 15
        Pos_data = 7                       'Point to on/off data on the LCD
        Tempb = Ch_num
        Temph = On_data(ch_num)
        If tmp_enc2 <> Tempg And Direction = 1 Then Decr Temph
        If Temph = 16 Then Temph = 0
        If tmp_enc2 <> Tempg And Direction = 0 Then Incr Temph
        If Temph = 255 Then Temph = 15
        New_data = Makebcd(temph)
        On_data(ch_num) = Temph
        End If
'
   Read_sw = 0                          'Clear the Read_sw flag
   End If
'
If Lcd_en = 1 Then
' Write message data to LCD module. Note that writing a string takes too
' much time, so only write one character at each AC half-cycle.
   'tempb = Switch_data + 48             'Convert switch_data to ASCII
   'Mid(lcd_msg , 16 , 1) = tempb        'Write switch_data to LCD
   Locate 1 , Cnt3                      'Move to next LCD character
   Lcd = Mid(lcd_msg , Cnt3 , 1)        'Write one character
   Incr Cnt3                            ' Advance LCD cursor counter
   If Cnt3 = 17 Then Cnt3 = 1           'Reset after 16 characters
   End If
'
Loop                                    'Repeat forever
'
'****************************************************************************
'* ISR Interrupt Service Routines                                           *
'****************************************************************************
Int_isr:
' Handles 120Hz activity. Detects first crossing of each AC Half-cycle
' This interrupt is level not edge triggered so it must be locked out
' immediately after the first hit. The Int0 is enables again near the
' expected next crossing (cnt1 = 48). If AC crossing sync is lost the
' watchdog timer (timer1) will generate a fake reset until sync is
' regained.
Disable Int0
Enable Timer0                           'Start AC Half-cycle counter
Load Timer1 , Seed1                     'Start AC watchdog timer
Cnt1 = 0
Read_sw = 1                             'Read the switch on each AC crossing
Incr Cnt2                               'MUX counter for LED & LCD
If Cnt2 = 3 Then
   Cnt2 = 0
   Lcd_en = 0
   Led_en = 1
   'Portb.2 = 1
   End If
'
If Cnt2 = 2 Then
   Lcd_en = 1
   Led_en = 0
   'Portb.2 = 0
   End If
'
Incr Sec_cnt
   If Sec_cnt = 120 Then
      Int_sec = 1                       'Generate one second interrupt
      Sec_cnt = 0
      End If
'
Incr Qsec_cnt                           'Generate qtr second interrupt
If Qsec_cnt = 15 Then
   Qsec_cnt = 0
   Int_qsec = 1
   End If
'
Incr Hsec_cnt                           'Generate half second interrupt
If Hsec_cnt = 30 Then
   Hsec_cnt = 0
   Int_hsec = 1
   End If
'
Incr 3sec_cnt                           'Generate three second interrupt
If 3sec_cnt = 360 Then
   3sec_cnt = 0
   Int_3sec = 1
   End If
'
Return
'****************************************************************************
Timer0_isr:
' This timer generates a string of 64 pulses (cnt1) during the AC
' Half-cycle. These are used to determine the active triac firing range
' and to inhibit any other I2C bus activity when the triacs have priority.
Load Timer0 , Seed0                     'reset timer0
Incr Cnt1                               'Advance AC half-cycle timer
'
Select Case Cnt1                        'Decode one 1/2 cycle
   Case 1
      I2csend Io_1 , &HFF               'Turn off all Io_1 channels
      Rtc_en = 1                        'Enable RTC traffic on I2C bus
      Portb.2 = 1                       'Trigger 'scope for testing
   Case 2
      I2csend Io_2 , &HFF               'Turn off all Io_2 channels
   Case 3
      I2csend Io_3 , &HFF               'Turn off all Io_3 channels
   Case 4
      I2csend Io_4 , &HFF               'Turn off all IO_4 channels
   Case 10
      Rtc_en = 0                        'Disable RTC traffic on I2C bus
      Portb.2 = 0
   Case 11                              'Display MSD LED data
   If Led_en = 1 Then
      Tempd = New_data
      Tempd = Tempd And &H0F
      Tempd = Chars(tempd + 1)
      Portd = Tempd And &H00_F0
      Shift Tempd , Right , 8
      Portc = Tempd And &H00_7C
      If Led_inv = 0 Then
         Portb = Portb And &B0111_1111  'Turn on left LED
         Else
         Portb = Portb And &B1011_1111  'Turn on right LED
         End If
      End If
   Case 12
      I2csend Io_1 , Lamp_dat1(1)       '100%
   Case 22
      I2csend Io_1 , Lamp_dat1(2)       '95%
   Case 26
      I2csend Io_1 , Lamp_dat1(3)       '91%
   Case 30
      I2csend Io_1 , Lamp_dat1(4)       '85%
   Case 35
      Portb = Portb Or &B1100_0000      'Turn off both LED digits
   Case 36
   If Led_en = 1 Then                   'Display LSD LED data
      Tempd = New_data
      Tempd = Tempd And &HF0
      Shift Tempd , Right , 4
      Tempd = Chars(tempd + 1)
      Portd = Tempd And &H00_F0
      Shift Tempd , Right , 8
      Portc = Tempd And &H00_7C
      If Led_inv = 0 Then
         Portb = Portb And &B1011_1111  'Turn on right LED
         Else
         Portb = Portb And &B0111_1111  'Turn on left LED
         End If
      End If
   Case 37
      I2csend Io_1 , lamp_dat1(5)       '74%
   Case 40
      I2csend Io_1 , lamp_dat1(6)       '68%
   Case 44
      I2csend Io_1 , lamp_dat1(7)       '64%
   Case 45
      I2csend Io_1 , lamp_dat1(8)       '55%
   Case 48
      Enable Int0                       'Hunt for next AC crossing
   Case 49
      I2csend Io_1 , lamp_dat1(9)       '50%
   Case 50
      I2csend Io_1 , lamp_dat1(10)      '44%
   Case 52
      I2csend Io_1 , lamp_dat1(11)      '38%
   Case 54
      I2csend Io_1 , Lamp_dat1(12)      '32%
   Case 56
      I2csend Io_1 , lamp_dat1(13)      '27%
   Case 57
      I2csend Io_1 , Lamp_dat1(14)      '21%
   Case 59
      I2csend Io_1 , Lamp_dat1(15)      '16%
   Case 60
      Portb = Portb Or &B1100_0000      'Turn off both LED digits
   Case 63
      Cnt1 = 0
      Disable Timer0
   End Select
'
Return
'****************************************************************************
Timer1_isr:
' This watchdog timer is set to timeout just after an expected AC crossing.
' Normally it is never used unless AC crossing is lost (or during boot)
Load Timer1 , Seed1                     'Reset timer1
Enable Int0                             'Hunt for next AC crossing
Enable Timer0                           'Start AC Half-cycle counter
Return
'****************************************************************************
'* Subroutines                                                              *
'****************************************************************************
Clock_init:
'
Tempb = &B0100_0000                     ' Oscillator Enable
I2cstart                                ' Generate start
I2cwbyte Ds1307w                        ' IO_ adsress
I2cwbyte Ds1307_add_sec                 ' Address of Seconds RTC
I2cwbyte Tempb                          '
I2cstop                                 ' Generate Stop
'
Tempb = &B0001_0001                     ' Set 12/24 hr & Hours = "11"
I2cstart                                ' Generate start
I2cwbyte Ds1307w                        ' IO_ adsress
I2cwbyte Ds1307_add_hour                ' Address of Hour RTC
I2cwbyte Tempb                          '
I2cstop                                 ' Generate Stop
'
Tempb = &B0101_1001                     ' Set Minutes = "59"
I2cstart                                ' Generate start
I2cwbyte Ds1307w                        ' IO_ adsress
I2cwbyte Ds1307_add_min                 ' Address of Min RTC
I2cwbyte Tempb                          '
I2cstop                                 ' Generate Stop
'
Tempb = &B10010000                      ' 1 Hz SQW Output
I2cstart                                ' Generate start
I2cwbyte Ds1307w                        ' IO_ adsress
I2cwbyte Ds1307_add_control             ' Address of Control RTC
I2cwbyte Tempb                          '
I2cstop                                 ' Generate Stop
'
Return
'*******************************************************************************
Read_eeprom:                            'Read from EEPROM
I2cstart                                'generate start
I2cwbyte Adresw                         'send mem address
I2cwbyte Adresh                         'high address
I2cwbyte Adresl                         'low address
I2cstart                                'generate repeated start
I2cwbyte &HA1                           'Set EEPROM to read
I2crbyte ee_dat1 , Nack                   'receive value, last byte
I2cstop                                 'generate stop
Ee_dat2 = Ee_dat1 And &HF0              'unpack byte to two nibbles
Shift Ee_dat2 , Right , 4
Ee_dat1 = Ee_dat1 And &H0F
Return
'****************************************************************************
Write_eeprom:                           'Write to EEPROM
Tmp_ee1 = Ee_dat1 And &H0F              'pack two nibbles into byte
Tmp_ee2 = Ee_dat2 And &H0F
Shift Tmp_ee2 , Left , 4
Tmp_ee1 = Tmp_ee1 Or Tmp_ee2
I2cstart                                'generate start
I2cwbyte Adresw                         'send mem address
I2cwbyte Adresh                         'send mem low address
I2cwbyte Adresl                         'send address of EEPROM
I2cwbyte Tmp_ee1                        'send a value
I2cstop                                 'generate stop
Waitms 10                               'wait 10 mS (write cycle time)
Return
'****************************************************************************
End                                     'end program                        *
'****************************************************************************
Charz1:                                 'Normal character map
Data &H0F , &HF0                        '0
Data &H9F , &HFC                        '1
Data &H2F , &HE8                        '2
Data &H0F , &HEC                        '3
Data &H9F , &HE4                        '4
Data &H4F , &HE4                        '5
Data &H4F , &HE0                        '6
Data &H1F , &HFC                        '7
Data &H0F , &HE0                        '8
Data &H0F , &HE4                        '9
'Data &HEF , &HFC                        'bot seg
'Data &H7F , &HFC                        'top seg
'Data &HFF , &HEF                        'mid seg
'
Charz2:                                 'Inverted character map
Data &H0F , &HF0                        '0
Data &HFF , &HF0                        '1
Data &H2F , &HE8                        '2
Data &H6F , &HE0                        '3
Data &HDF , &HE0                        '4
Data &H4F , &HE4                        '5
Data &H0F , &HE4                        '6
Data &HEF , &HF0                        '7
Data &H0F , &HE0                        '8
Data &H4F , &HE0                        '9
'
'Charz:                                  'Segment map
Data &H7F , &HFF                        'A
Data &HBF , &HFF                        'B
Data &HDF , &HFF                        'C
Data &HEF , &HFF                        'D
Data &HFF , &HFB                        'E
Data &HFF , &HF7                        'F
'
Mem_data:                               'Memory preset map
Data &H12
Data &H34
Data &H56
Data &H78
'
Data &H9A
Data &HBC
Data &HDE
Data &HF1
'
Data &H23
Data &H45
Data &H67
Data &H89
'
Data &HAB
Data &HCD
Data &HEF
Data &H12
