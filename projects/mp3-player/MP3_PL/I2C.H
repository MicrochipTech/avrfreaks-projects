

void i2c_init ();
byte i2c_transmit (byte adress, byte subadress, byte data);
byte i2c_receive (byte adress, byte subadress, byte* data);
