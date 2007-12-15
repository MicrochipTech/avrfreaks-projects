'Cntrl_test02
'Holiday Lights Controller
'Peter J. Stonard
'December 12, 06
'Revised September 1, 07
'
'  ####################################################################
'  #  Initialize RTC, set to 1pps output (Red Led blinks)             #
'  #  Use AC zero crossing and timers to derive multiple interrupts   #
'  #  Use LED_inv bit (line 172) to turn LED upside down as required  #
'  #  Display "Welcome" on optional LCD                               #
'  #  Detect AC crossing and generate 64 step clock (cnt1)            #
'  #  Read encoder position and display on LED                        #
'  #  Display RTC data on LCD                                         #
'  #  Encoder and switch allow setting of RTC data on LCD and LED     #
'  ####################################################################
'
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
Dim Led_en As Bit
Dim Led_inv As Bit
Dim Lcd_en As Bit
Dim New_switch As Bit
Dim Old_switch As Bit
Dim R_rtc As Bit
Dim Read_sw As Bit
Dim Rtc_en As Bit
Dim W_data As Bit
Dim W_rtc As Bit
'
' Variables stored as a byte
Dim Adresr As Byte                      'EEPROM device read address
Dim Adresw As Byte                      'EEPROM device write address
Dim Cnt1 As Byte                        'Count timer cycles (64 = 1/2 AC cycle)
Dim Cnt2 As Byte                        'Count I2C bus access (AC 1/2 cycles)
Dim Cnt3 As Byte                        'LCD cursor counter ( 1 to 16)
Dim Data_hr As Byte                     'Temp Hours data
Dim Data_min As Byte                    'Temp Minute data
Dim Data_sec As Byte                    'Temp Minute data
Dim Enc_data As Byte
Dim Hsec_cnt As Byte                    'counter for half second interrupt
Dim New_data As Byte
Dim Pos_data As Byte                    'Temp Pos data
Dim Qsec_cnt As Byte                    'counter for quarter second interrupt
Dim Rtc_sec As Byte                     'RTC Second
Dim Rtc_min As Byte                     'RTC Minute
Dim Rtc_hr As Byte                      'RTC Hour
Dim Rtc_mode As Byte                    'RTC Mode
Dim Sec_cnt As Byte                     'counter for one second interrupt
Dim Seed0 As Byte                       'Timer0 counter value
Dim Seed1 As Byte                       'Timer1 counter value
Dim Switch_data As Byte
Dim Tempc As Byte                       'Temp storage
Dim Tempe As Byte                       'Temp storage
Dim Tempf As Byte                       'Temp storage
Dim Tempg As Byte                       'Temp storage
'
' Variables stored as a word
Dim 3sec_cnt As Word                    '3 Sec interrupt counter
Dim Tempa As Word                       'Temp storage
Dim Tempb As Word                       'Temp storage
Dim Tempd As Word                       'Temp storage
Dim Tempx As Word                       'Temp storage
Dim Tempy As Word                       'Temp storage
Dim Chars(10) As Word                   'Character look up table
'
' Variables stored as strings
Dim Lcd_msg As String * 16              'Current message storage space
Dim Msg1 As String * 16                 'Storage for a message
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
Declare Sub Read_rtc
Declare Sub Write_rtc
'
'****************************************************************************
'* Define AVR                                                               *
'****************************************************************************
$regfile = "m168def.dat"                'Change for your AVR.
$crystal = 8000000                      '8 MHz Crystal.
Config Lcdpin = Pin , Db4 = Portc.2 , Db5 = Portc.3 , Db6 = Portc.4 , Db7 = Portc.5 , E = Portc.1 , Rs = Portc.0
Config Lcd = 16 * 1
Config I2cdelay = 1
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
'****************************************************************************
'* LED Inversion                                                            *
'****************************************************************************
Led_inv = 0                             'change to 1 for inverted LED display
'****************************************************************************
'* Init Variables                                                           *
'****************************************************************************
Rtc_sec = Makebcd(55)                   'default start up time
Rtc_min = Makebcd(59)
Rtc_hr = Makebcd(17)
Rtc_mode = &B0000_0000                  'Set midnight (24hr, bit 6 reset)
'
Msg1 = "Welcome   "
Cnt2 = 2
'
Seed0 = 125                             'Use 125 max!
Seed1 = 133                             '133 for 60Hz free-running osc.
'****************************************************************************
'* Power On Reset                                                           *
'****************************************************************************
'Gosub Clock_init                        'set up RTC
Gosub Read_rtc                          'Get current data from RTC
If Led_inv = 0 Then
   Restore Charz1
      For Tempa = 1 To 10
      Read Chars(tempa)
      Next Tempa
   End If
'
If Led_inv = 1 Then
   Restore Charz2
      For Tempa = 1 To 10
      Read Chars(tempa)
      Next Tempa
   End If
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
'
Config Timer0 = Timer , Prescale = 8    'Timer0 generates 64*120Hz clk
Load Timer0 , Seed0
'
Config Timer1 = Timer , Prescale = 1024 'Timer1 lost sync watchdog
Load Timer1 , Seed1
'
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
   If R_rtc = 1 And W_rtc = 0 Then
      Gosub Read_rtc                    'Do a read of the RTC
      R_rtc = 0                         'Clear flags
      Rtc_en = 0
      End If
'
   If W_rtc = 1 And R_rtc = 0 Then
      Gosub Write_rtc                   'Do a write of the RTC
      W_rtc = 0                         'Clear flags
      Rtc_en = 0
      End If
   End If

' Display RTC time
If Int_sec = 1 Then                     'Update the LCD display once every sec
'
   If Switch_data = 0 Then              'RTC display mode. Read the RTC
     R_rtc = 1                          'Request RTC read next AC half-cycle
     Data_hr = Rtc_hr
     Data_min = Rtc_min
     Data_sec = Rtc_sec
     End If
'
   Lcd_msg = "Time:   :  :    "         'Write Time template to LCD
   Tempc = Data_sec And &H0F
   Tempc = Tempc + 48
   Mid(lcd_msg , 14 , 1) = Tempc        'Write seconds data to LCD
   Tempc = Data_sec And &HF0
   Shift Tempc , Right , 4
   Tempc = Tempc + 48
   Mid(lcd_msg , 13 , 1) = Tempc
'
   Tempc = Data_min And &H0F
   Tempc = Tempc + 48
   Mid(lcd_msg , 11 , 1) = Tempc        'Write mins data to LCD
   Tempc = Data_min And &HF0
   Shift Tempc , Right , 4
   Tempc = Tempc + 48
   Mid(lcd_msg , 10 , 1) = Tempc
'
   Tempc = Data_hr And &H0F
   Tempc = Tempc + 48
   Mid(lcd_msg , 8 , 1) = Tempc         'Write mins data to LCD
   Tempc = Data_hr And &HF0
   Shift Tempc , Right , 4
   Tempc = Tempc + 48
   Mid(lcd_msg , 7 , 1) = Tempc
'
   Tempc = Switch_data + 48             'Place the current switch data on LCD
   Mid(lcd_msg , 16 , 1) = Tempc
'
   If Switch_data <> 0 Then             'Use brackets to indicate which
                                         'data set is controlled by the encoder
      Mid(lcd_msg , Pos_data , 1) = "<"
      Tempc = Pos_data + 3
      Mid(lcd_msg , Tempc , 1) = ">"
      End If
'
   If Switch_data = 3 Then              'New RTC data read to be written back
      Rtc_hr = Data_hr
      Rtc_min = Data_min
      Rtc_sec = Data_sec
      W_rtc = 1                         'Request RTC write next AC half-cycle
      Switch_data = 0
      End If
'
   Int_sec = 0
End If
'
If Read_sw = 1 Then                     'Poll the switch every AC half-cycle
  If Switch <> New_switch Then Toggle New_switch       'debounce switch
  If New_switch = 0 And Old_switch = 1 Then
     Incr Switch_data
     If Switch_data = 5 Then Switch_data = 0
     End If
  Old_switch = New_switch
  '#####################################################################
  '#     Switch Data:                                                  #
  '#     0 LCD displays RTC data                                       #
  '#     1 RTC hours data brackets and encoder changes                 #
  '#     2 RTC mins data brackets and encoder changes                  #
  '#     3 New data saved to RTC                                       #
  '#####################################################################
  '
' Check Encoder for new data
     Tempe = Tempe And &B0000_0011
     Tempf = Tempe
     Shift Tempe , Left , 2             'save last setting
     Tempg.0 = Enca                     'Tempe = old & Tempd = new
     Tempg.1 = Encb
     Tempe = Tempe Or Tempg
' Test the new data to determine encoder direction
     Select Case Tempe
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
' Use the new encoder data to change the enc_data by one
' Roll over the enc_data at 99 -> 0, and 0 -> 99
        Tempy = Makedec(enc_data)
        If Tempf <> Tempg And Direction = 1 Then Decr Tempy
        If Tempy = 100 Then Tempy = 0
        If Tempf <> Tempg And Direction = 0 Then Incr Tempy
        If Tempy = 255 Then Tempy = 99
        Enc_data = Makebcd(tempy)
        New_data = Enc_data             'Save the encoder data for LED display
        End If
  '
     If Switch_data = 1 Then
' Use the new encoder data to change the RTC hour data by one
' Roll over the data_hr at 23 -> 00, and 00 -> 23
        Pos_data = 6                    'Point to hours data on the LCD
        Data_hr = Makedec(data_hr)
        If Tempf <> Tempg And Direction = 1 Then Decr Data_hr
        If Data_hr > 24 Then Data_hr = 23
        If Tempf <> Tempg And Direction = 0 Then Incr Data_hr
        If Data_hr = 24 Then Data_hr = 0
        Data_hr = Makebcd(data_hr)
        New_data = Data_hr              'Copy the new hours data to the LED
        W_data = 1                      'Request RTC write next AC half-cycle
        End If
  '
     If Switch_data = 2 Then
' Use the new encoder data to change the RTC mins data by one
' Roll over the data_min at 59 -> 00, and 00 -> 59
        Pos_data = 9                    'Point to mins data on the LCD
        Data_min = Makedec(data_min)
        If Tempf <> Tempg And Direction = 1 Then Decr Data_min
        If Data_min > 60 Then Data_min = 59
        If Tempf <> Tempg And Direction = 0 Then Incr Data_min
        If Data_min = 60 Then Data_min = 0
        Data_min = Makebcd(data_min)
        New_data = Data_min             'Copy the new mins data to the LED
        W_data = 1                      'Request RTC write next AC half-cycle
        End If
'
   Read_sw = 0                          'Clear the Read_sw flag
   End If
'
If Lcd_en = 1 Then
' Write message data to LCD module. Note that writing a string takes too
' much time, so only write one character at each AC half-cycle
   Tempc = Switch_data + 48             'Convert switch_data to ASCII
   Mid(lcd_msg , 16 , 1) = Tempc        'Write switch_data to LCD
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
'
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
' and to inhibit any other I2C bus activity when the triacs have priority
Load Timer0 , Seed0                     'reset timer0
Incr Cnt1                               'Advance AC half-cycle timer
'
Select Case Cnt1                        'Decode one 1/2 cycle
   Case 1
      Rtc_en = 1                        'Enable RTC traffic on I2C bus
      Portb.2 = 1                       'Trigger 'scope
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
   Case 48
      Enable Int0                       'Hunt for next AC crossing
   Case 60
      Portb = Portb Or &B1100_0000      'Turn off both LED digits
   Case 63
      Cnt1 = 0                          'Reset AC Half-cycle counter
      Disable Timer0                    'Stop counter1
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
Write_rtc:                              'Send time data to RTC
Tempx = Rtc_sec And &B0111_1111         'Oscillator Enable (Bit7 = 0)
I2cstart
I2cwbyte Ds1307w
I2cwbyte Ds1307_add_sec                 'Write seconds
I2cwbyte Tempx
I2cstop
'
Tempx = Rtc_min And &B0111_1111         'Set Minutes
I2cstart
I2cwbyte Ds1307w
I2cwbyte Ds1307_add_min                 'Write minutes
I2cwbyte Tempx
I2cstop
'
Tempx = Rtc_hr And &B0011_1111          'Set 24 Hour Mode (Bit6 = 0)
I2cstart
I2cwbyte Ds1307w
I2cwbyte Ds1307_add_hour                'Write hours
I2cwbyte Tempx
I2cstop
'
Tempx = &B1001_0000                     'Enable SQW Output (Bit4 = 1)
I2cstart                                'Select 1Hz (Bit1, Bit0 = 0)
I2cwbyte Ds1307w
I2cwbyte Ds1307_add_control             'Write Control byte
I2cwbyte Tempx
I2cstop
'
Return
'****************************************************************************
Read_rtc:                               ' Read Data over I2C
I2cstart                                ' RTC device address and data address
I2cwbyte Ds1307w                        ' Slave adsress
I2cwbyte Ds1307_add_sec                 ' Address of seconds register
I2cstop                                 ' Generate Stop
'
I2cstart                                ' Repeated start
I2cwbyte Ds1307r                        ' Slave address (read)
I2crbyte Rtc_sec , Ack                  ' Generate ACK
I2crbyte Rtc_min , Ack
I2crbyte Rtc_hr , Nack                  ' Generate NACK
I2cstop
'
Return
'****************************************************************************
Clock_init:
'
Tempb = Rtc_sec And &B01111111          ' Oscillator Enable
I2cstart                                ' Generate start
I2cwbyte Ds1307w                        ' Slave adsress
I2cwbyte Ds1307_add_sec                 ' Address of Seconds REG
I2cwbyte Tempb                          '
I2cstop                                 ' Generate Stop
'
Tempb = Rtc_mode And &B0110_0000        ' Set 24 hr mode (b6 = 0)
Tempb = Tempb Or Rtc_hr                 ' Set Hours
I2cstart                                ' Generate start
I2cwbyte Ds1307w                        ' Slave adsress
I2cwbyte Ds1307_add_hour                ' Address of Hour REG
I2cwbyte Tempb                          '
I2cstop                                 ' Generate Stop
'
Tempb = Rtc_min And &B0111_1111         ' Set Minutes
I2cstart                                ' Generate start
I2cwbyte Ds1307w                        ' Slave adsress
I2cwbyte Ds1307_add_min                 ' Address of Min REG
I2cwbyte Tempb                          '
I2cstop                                 ' Generate Stop
'
Tempb = &B10010000                      ' 1 Hz SQW Output
I2cstart                                ' Generate start
I2cwbyte Ds1307w                        ' Slave adsress
I2cwbyte Ds1307_add_control             ' Address of Control REG
I2cwbyte Tempb                          '
I2cstop                                 ' Generate Stop
'
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