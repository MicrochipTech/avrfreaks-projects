$regfile "m8535.dat"
$crystal = 6000000
'$lib "i2c_twi.lbx"

Config Twi = 100000

Config Scl = Portc.0
Config Sda = Portc.1
Config Lcd = 20 * 2

Config Serialin = Buffered , Size = 20
Config Serialout = Buffered , Size = 20

Enable Interrupts

Dim A As Byte
Dim B As Byte
'Dim Read_data As Byte
'Dim Temprature(9) As Byte


Enable Twi
Twbr = &H16
Twsr = Twsr And &HFC

Cls
Lcd "AIR UNIVERSITY"
Wait 1
Lowerline
Lcd "FIRE DETECTION"
Wait 1


Do

'I2cstart();
' I2cwrite(0xd0);
' I2cwrite(reg);
' I2cstart();
' I2cwrite(0xd1);
' data = i2c_read (0);
' I2cstop();
 I2cstart                                                   'generate start

  I2cwbyte &HD0                                             'slave adsress

  I2cwbyte 1                                                'address of EEPROM

  I2cstart                                                 'repeated start

  I2cwbyte &HD1                                             'slave address (read)

  I2crbyte A , Nack                                         'read byte

  I2cstop




 Waitms 40
 Cls
 Lcd A

 Print A
Loop

End