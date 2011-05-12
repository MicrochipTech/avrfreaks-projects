$regfile "m8535.dat"
$crystal = 6000000
'$lib "i2c_twi.lbx"

Config Twi = 100000

Config Scl = Portc.0
Config Sda = Portc.1
'Config Lcd = 20 * 2

'Config Serialin = Buffered , Size = 20
'Config Serialout = Buffered , Size = 20

Enable Interrupts

Dim A As Byte
Dim B As Byte
'Dim Read_data As Byte
'Dim Temprature(9) As Byte


Enable Twi
Twbr = &H16
Twsr = Twsr And &HFC

'Cls
'Lcd "AIR UNIVERSITY"
'Wait 1
'Lowerline
'Lcd "FIRE DETECTION"
'Wait 1

Do
For B = 0 To 31 Step 1

   I2cstart                                                 'start condition
   I2cwbyte &HD0                                            'slave address
   I2cwbyte 0                                               'asdress of EEPROM
   I2cwbyte B                                               'value to write
   I2cstop                                                  'stop condition

   Wait 1


Next B

Loop
End