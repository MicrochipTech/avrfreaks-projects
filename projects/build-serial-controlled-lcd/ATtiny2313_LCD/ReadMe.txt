This project creates an LCD module that accepts TTL level serial data
or TWI (iC2) data as input and displays it on the LCD.

It uses LCD code contributed by Peter Fleury and TWI code by Donald R. Blake.

Useful for microcontroller development projects as the display needs only
power and a single serial data input line to use as a display.

Since all the ISP signals and a number of extra connectiona are avaialble, 
this device can also be used as a mini development system by itself.
Just connect you programmer and modify the source code provided or add your own.
NOTE: Be sure you have a copy of the original .HEX file if you plan to restore 
the default (original) functionally. 

Here's the details:
Serial Input (TTL level):
  Connect input data to RXD (J1-7) and optionally Output data to TXD (J1-8)
  BAUD RATE is fixed at 9600.
  Input data is echoed back through the TXD line but can be ignored.
  
TWI Input (I2C):  
  Connect SDA signal to J1-5 and SCL signal to J1-4.
  Device functions as a TWI slave with ADDRESS = 0x45
  TWI input data is NOT echoed back as is serial data.
  
Both Input Modes:
  Connect +5VDC to J1-2 and ground (-) to J1-1.
  On power-up or reset:
     1. Display will clear for about 1.2 sec. (led will blink 3 times)
     2. "LCD Ready..." will be sent to the serial port and displayed on the LCD.
     3. When the first character is recieved, the startup message is cleared.
        
  Escape commands:
     When the device recieves an escape character (ASCII 27/0x1B) it will 
     respond to the following command characters when they are the next byte 
     recieved.

     'c': Clear Display: - The LCD wil be cleared and next character recieved 
          will display in the top-leftmost position. 
               
     'h': Home:  - The next character recieved will display in the 
          top-leftmost position. The display will not be cleared.
          
     'K': Curson ON: - Turn the visible cursor ON  (default is OFF)
            
     'k': Curson OFF: - Turn the visible cursor OFF 
     
     'p': Position Cursor: The device will expect 2 more input characters
          indicating row and column position. the first byte following he 'p' 
          command should be '0' or '1' indicating the top or bottom row.
          the next byte should be '0'-'9' or 'A'-'F' to indicate the column
          position (starting at zero) where 'A'-'F' represents columns 10-15.
          The display is not cleared by this command.
          
     'u': Dev Port Test: This command will cause the LED and the 4 pins on the
          Dev port (J2) to toggle 5 times at a rate of about .5 seconds/toggle.
          (nice to see if things are working)
          See the schematic for dev pin and dev LED connections.
                  
                          
      All escape commands will respond with a LF & CR to the serial port.
      
   The Dev Button:  Pressing this button for more that 50ms will send the '@'
      character (0x40) to the LCD as well as the serial port (no TWI sorry)
      Usefull when using this device as a mini terminal for debugging to 
      indicate a response from the user.   
      
      
      Notes on using as a mini development system:
      The existing code uses about 90% of the ATtiny2313 available flash menory.
      You can modify the makefile and source files provided in the package to 
      make more roome avaialble for your development.
      For example, the TWI routines use a lot of memory, if you remove 
      references to the usiTWIslave files from the makefile and modify the
      input_data_ready() and get_input_byte() routines, then remove the call to 
      usiTwiSlaveInit() (optionally the call to sei() can be removed as well), 
      you will free up memory but of course only have serial communications.
      If you don't really need the LCD display that code can be removed but it's
      a lot more involved.
      
      Send any suggestions for improvements to emklaus@gmail.com
      
        
      