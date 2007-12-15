'Cntrl_test01
'Holiday Lights Controller
'Peter J. Stonard
'12 December 06
'
'  ####################################################################
'  #  Initialize RTC, set to 1pps output (Red Led blinks)             #
'  #  Display "Welcome" on optional LCD                               #
'  #  Scroll through LED segments                                     #
'  #  Scroll through digits 00 to 99 (Normal)                         #
'  #  Scroll through digits 00 to 99 (Inverted)                       #
'  #  Repeat forever                                                  #
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
'
Dim Data1 As Byte
Dim Data2 As Byte
Dim Dgtptr As Byte
Dim Rtc_sec As Byte                     ' Second
Dim Rtc_min As Byte                     ' Minute
Dim Rtc_hr As Byte                      ' Hour
Dim Rtc_mode As Byte                    '12/24 hour mode
Dim Tempb As Byte
Dim Tempc As Byte
Dim Tempd As Byte
'
Dim Chars1(14) As Byte                  'LED segment look up table
Dim Chars2(20) As Byte                  'Normal Character look up table
Dim Chars3(20) As Byte                  'Inverted Character look up table
'
'****************************************************************************
'* Define Subroutines                                                       *
'****************************************************************************
Declare Sub Clock_init
'Declare Sub Read_ds1307
'
'****************************************************************************
'* Define AVR                                                               *
'****************************************************************************
$regfile = "m168def.dat"                'Change for your AVR.
$crystal = 8000000                      '4 MHz Crystal.
$baud = 9600                            'Output at 9600 baud.
Config Lcdpin = Pin , Db4 = Portc.2 , Db5 = Portc.3 , Db6 = Portc.4 , Db7 = Portc.5 , E = Portc.1 , Rs = Portc.0
Config Lcd = 16 * 1
Config I2cdelay = 2
'
'****************************************************************************
'* Define Ports & Data Direction Registers  (In = 0, Out = 1)               *
'****************************************************************************
' Port B
Portb = &B0000_0000                     'Set All Port Pins Low
Ddrb = &B1111_0011                      'Set encoder pins as inputs (tri-state)
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
'*******************************************************************************
'* Define I2C pins                                                             *
'*******************************************************************************
Config Sda = Portb.0                    ' I2C Data.
Config Scl = Portb.1                    ' I2C Clock
'
'*******************************************************************************
Const Ds1307w = &HD0                    'Address Of DS1307 Write
Const Ds1307r = &HD1                    'Address Of DS1307 Read
Const Ds1307_add_sec = &H00             'Address Of DS1307 Seconds
Const Ds1307_add_min = &H01             'Address Of DS1307 Min
Const Ds1307_add_hour = &H02            'Address Of DS1307 Hour
Const Ds1307_add_control = &H07         'Address Of DS1307 Control
'*******************************************************************************
'* Init Variables                                                              *
'*******************************************************************************
Rtc_sec = Makebcd(55)                   'default start up time
Rtc_min = Makebcd(59)
Rtc_hr = Makebcd(17)
Rtc_mode = &B0000_0000                  'Set midnight (24hr mode, bit 6 reset)
'*******************************************************************************
'* Power On Reset                                                              *
'*******************************************************************************
Gosub Clock_init                        'set up RTC
Restore Charz1
        For Dgtptr = 1 To 14
        Read Chars1(dgtptr)
        Next Dgtptr
'
Restore Charz2
        For Dgtptr = 1 To 20
        Read Chars2(dgtptr)
        Next Dgtptr
'
Restore Charz3
        For Dgtptr = 1 To 20
        Read Chars3(dgtptr)
        Next Dgtptr
'
'*******************************************************************************
'* Main Program                                                                *
'*******************************************************************************
Main:
Portb = &B1100_0000                     'Turn off both LED digits
Cls                                     'Init LCD
Cursor Off
Locate 1 , 1
Lcd "                "                  'Clear all text
Locate 1 , 1
Lcd "Welcome"                           'Display Welcome
'
Do
'Display LED segments one at a time
'LSD (Right) LED
   For Data1 = 1 To 13 Step 2           'Step units
      For Tempb = 0 To 255              'delay
         Portb = &B0111_1111            'Turn on LSD LED
         Tempd = Chars1(data1)          'Read character
         Portd = Tempd And &HF0
         Tempc = Chars1(data1 + 1)
         Portc = Tempc And &H3C
         Waitms 2
         Next Tempb
   Next Data1
Portb = &B1100_0000                     'Turn off both LED digits
'MSD (Left) LED
   For Data1 = 1 To 13 Step 2           'Step units
      For Tempb = 0 To 255              'delay
         Portb = &B1011_1111            'Turn on MSD LED
         Tempd = Chars1(data1)          'Read character
         Portd = Tempd And &HF0
         Tempc = Chars1(data1 + 1)
         Portc = Tempc And &H3C
         Waitms 2
         Next Tempb
   Next Data1
'
'Display Digits to match PCB silkscreen
For Data2 = 1 To 19 Step 2              'Step tens
   For Data1 = 1 To 19 Step 2           'Step units
      For Tempb = 0 To 63               'delay
         Portb = &B0111_1111            'Turn on MSD LED
         Tempd = Chars2(data1)          'Read character
         Portd = Tempd And &HF0
         Tempc = Chars2(data1 + 1)
         Portc = Tempc And &H3C
         Waitms 2
         Portb = &B1011_1111            'Turn on LSD LED
         Tempd = Chars2(data2)          'Read character
         Portd = Tempd And &HF0
         Tempc = Chars2(data2 + 1)
         Portc = Tempc And &H3C
         Waitms 2
         Next Tempb
   Next Data1
Next Data2
'
Portb = &B1100_0000                     'Turn off both LED digits
'Display Digits upside down to PCB silkscreen
For Data2 = 1 To 19 Step 2              'Step tens
   For Data1 = 1 To 19 Step 2           'Step units
      For Tempb = 0 To 63              'delay
         Portb = &B1011_1111            'Turn on MSD LED
         Tempd = Chars3(data1)          'Read character
         Portd = Tempd And &HF0
         Tempc = Chars3(data1 + 1)
         Portc = Tempc And &H3C
         Waitms 2
         Portb = &B0111_1111            'Turn on LSD LED
         Tempd = Chars3(data2)          'Read character
         Portd = Tempd And &HF0
         Tempc = Chars3(data2 + 1)
         Portc = Tempc And &H3C
         Waitms 2
         Next Tempb
   Next Data1
Next Data2
'
Loop                                    'Repeat forever
'*******************************************************************************
'* Subroutines                                                                 *
'*******************************************************************************
Clock_init:
'
Tempb = RTC_sec And &B01111111          ' Oscillator Enable
I2cstart                                ' Generate start
I2cwbyte Ds1307w                        ' Slave adsress
I2cwbyte Ds1307_add_sec                 ' Address of Seconds RTC
I2cwbyte Tempb                          '
I2cstop                                 ' Generate Stop
'
Tempb = RTC_mode Or &B0100_0000         ' Set 12/24 hr mode
Tempb = Tempb Or RTC_hr                 ' Set Hours
I2cstart                                ' Generate start
I2cwbyte Ds1307w                        ' Slave adsress
I2cwbyte Ds1307_add_hour                ' Address of Hour RTC
I2cwbyte Tempb                          '
I2cstop                                 ' Generate Stop
'
Tempb = RTC_min And &B0111_1111         ' Set Minutes
I2cstart                                ' Generate start
I2cwbyte Ds1307w                        ' Slave adsress
I2cwbyte Ds1307_add_min                 ' Address of Min RTC
I2cwbyte Tempb                          '
I2cstop                                 ' Generate Stop
'
Tempb = &B10010000                      ' 1 Hz SQW Output
I2cstart                                ' Generate start
I2cwbyte Ds1307w                        ' Slave adsress
I2cwbyte Ds1307_add_control             ' Address of Control RTC
I2cwbyte Tempb                          '
I2cstop                                 ' Generate Stop
'
Return
'*******************************************************************************
End                                     'end program
'*******************************************************************************
Charz1:                                 'Segment map
Data &H7F , &HFF                        'A
Data &HBF , &HFF                        'B
Data &HDF , &HFF                        'C
Data &HEF , &HFF                        'D
Data &HFF , &HFB                        'E
Data &HFF , &HF7                        'F
Data &HFF , &HEF                        'G
'
Charz2:                                 'Normal character map
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
'
Charz3:                                 'Inverted character map
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
