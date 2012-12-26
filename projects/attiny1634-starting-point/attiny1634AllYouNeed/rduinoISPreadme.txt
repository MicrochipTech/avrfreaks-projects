This ArduinoISP includes a fix to the subroutine start_pmode

void start_pmode() {
  SPI.begin();
  digitalWrite(RESET, HIGH);
  pinMode(RESET, OUTPUT);
  digitalWrite(SCK, LOW);
  delay(20);
  digitalWrite(RESET, LOW);
  delay(20); //needed this for it to work for attiny1634
  spi_transaction(0xAC, 0x53, 0x00, 0x00);
  pmode = 1;
}


ArduinoISP
This is a sketch that allows your Arduino to work as a STK500v1 programmer.

See the excellent Arduino as AVR ISP tutorial on arduino.cc.

Please report issues on the mega-isp site at code.google.com.

Use a recent version!
Arduino IDE 1.00 was released 30 November 2011

Check out latest available Arduino IDE from: http://code.google.com/p/arduino/downloads/list

If your Arduino IDE version is <= Arduino-0023:

The ArduinoISP that ships as an example with the Arduino software is old. You can download the ArduinoISP.ino here and replace the one you find in ...\arduino-002x\examples\ArduinoISP. This version is known to work with the avrdude that ships with Arduino-0023. That is an older version of avrdude (5.04, latest is 5.11).