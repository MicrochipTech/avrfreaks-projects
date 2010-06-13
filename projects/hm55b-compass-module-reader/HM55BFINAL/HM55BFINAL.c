/*
   READ COMPASS ANGLE FROM THE PARALLAX HM55B DIGITAL COMPASS WITH THE POLOLU BABY ORANGUTAN 328P
   MAJORITY OF PROGRAM LOGIC: kiilo@kiilo.org WHO WROTE IT FOR THE ARDUINIO
   MODIFIED TO RUN ON THE BABY O BY ME: scottfromscott@bellsouth.net
   SEE THE flash_angle() FUNCTION FOR AN EXPLANATION OF HOW THE RED USER LED FLASHES THE CURRENT ANGLE 

   REFERENCES:
   http://www.parallax.com/Portals/0/Downloads/docs/prod/compshop/HM55BModDocs.pdf -- compass documentation
   http://www.parallax.com/Store/Sensors/CompassGPS/tabid/173/ProductID/98/List/0/Default.aspx?SortField=ProductName,ProductName -- product page
   http://arduino.cc/playground/HM55B -- killo's Arduino version
   http://forum.pololu.com -- general orangutan help
   http://www.pololu.com/docs/0J14 -- Baby O user's guide
   http://www.pololu.com/docs/pdf/0J20/pololu_avr_library.pdf
   http://www.pololu.com/docs/pdf/0J18/avr_library_commands.pdf
*/

#define F_CPU 20000000UL	// Baby Orangutan frequency (20MHz)

#include <math.h> 
#include <avr/io.h>
#include <util/delay.h> // uses F_CPU to achieve us and ms delays
#include <pololu/orangutan.h>
#include <stdlib.h>

// data connections:
//
// BABY O PINS <==> HM55B PINS
//  
//   PB0:IO_B0 <==> #4:CLK
//   PB1:IO_B1 <==> #5/Enable
//   PB2:IO_B2 <==> #1:Dout,#2:Din

// orangutan library defines used:
//
// for set_digital_output(), LOW, HIGH
// for set_digital_input() -- PULL_UP_ENABLED - enable pull-up resistor

float compass_angle = 999; // global variable available for other modules use

// for calibration: print out the compass 16-segment compass template at
// http://www.parallax.com/Portals/0/Downloads/docs/prod/compshop/HM55BModDocs.pdf
// line up north with a pocket compass and tape the template to a table
// this compass outputs negative angles for all westerly directions (180..359 -> -180..-1)
// so label the template as follows:
// 180(-180), 202.5(-157.5), 225(-135), 247.5(-112.5), 270(-90), 292.5(-67.5), 315(-45), 337.5(-22.5)
// set the Baby O powered HM55B in position facing south on compass template
// set calibrate_mode below to 1, then build and run this program
// you will now determine the compass reading and offset for each of 16 angles:
// starting with 180 degrees, read and write down the template angle, the compass reading
// and the offset. Repeat for all 16 angles. The offset for the reading will be a + or -
// number added to the reading to match the template angle-- eg. for a template angle of -90,
// where the reading is -115, the offset would be 25 -- -90 = -115 + 25
// now set calibrate_mode to 1 and complete the procedure above.
//
// Next, enter these values in the raw[] and offsets[] arrays in the corrected() function below
// the raw[] array is filled left to right, in negative to positive angles order
// the arrays are linked by their indices -- eg. offsets[2] goes with raw[2]
// in addition to the 16 values you just entered in each array above, you must do the following:
// add -180 at the beginning and 180 at the end of the raw[] array declaration;
// eg.    =>   int raw[18] = {-180, ...(your 16 values)..., 180}; 
// you must also add corresponding offset values for these to the offsets[] array
// which can easily be calculated with any spreadsheet program as follows:
// the offset for for -180 degrees...
// col A: template angle; col B: reading; col c: offset
// A1: 157.5, B1: (reading for 157.5), C1: (offset)  
// B2: -180, C2: =(B2-B1)*(C3-C1)/(B3-B1)+C1
// A3: -180, B3: (reading for -180), C3: (offset)
// 
// the offset for for 180 degrees...same as above but change B2 to 180
//
// so if offset for -180 is -4.2 and -23 for 180, then your offsets array declaration would look as follows
// eg. => int offsets[18] = {-4.2, ...(your 16 offsets)..., -23};
// 
// once your arrays are complete you may reset calibrate_mode to 0 below
// and rebuild and test your completed program which should more accurately
// display the compass angles
   
int calibrate_mode = 0; //0 = use linear interpolation tables; 1 = flash raw angle values for calibration

//// FUNCTIONS

void delayms( uint16_t millis )
{
   while(millis)
   {
      _delay_ms(1);
      millis--;
   }
}

void ShiftOut(int Value, int BitsCount)
{
   for(int i = BitsCount; i >= 0; i--)
   {
      set_digital_output(IO_B0, HIGH);
      if ((Value & 1 << i) == ( 1 << i)) set_digital_output(IO_B2, HIGH); // 1
      else set_digital_output(IO_B2, LOW); // 0 
      set_digital_output(IO_B0, LOW);
      delayMicroseconds(1);
   }
}

int ShiftIn(int BitsCount)
{
   int ShiftIn_result, i;
   ShiftIn_result = 0;
   set_digital_input(IO_B2, PULL_UP_ENABLED);
   for(i = BitsCount; i >= 0; i--)
   {
      set_digital_output(IO_B0, HIGH);
      delayMicroseconds(1);
      if (is_digital_input_high(IO_B2)) ShiftIn_result = (ShiftIn_result << 1) + 1;
      else ShiftIn_result = (ShiftIn_result << 1) + 0;
      set_digital_output(IO_B0, LOW);
      delayMicroseconds(1);
   }
   if ((ShiftIn_result & 1 << 11) == 1 << 11) ShiftIn_result = (0B11111000 << 8) | ShiftIn_result;
   return ShiftIn_result;
}

void setup()
{
   set_digital_output(IO_B1, HIGH); // not enabled initially
   set_digital_output(IO_B0, LOW); // clock is low initially
}

void HM55B_Reset()
{
   set_digital_output(IO_B1, LOW);
   ShiftOut(0B0000, 3); // send nibble of zeros
   set_digital_output(IO_B1, HIGH);
}

void HM55B_StartMeasurementCommand()
{
   set_digital_output(IO_B1, LOW);
   ShiftOut(0B1000, 3);
}

int HM55B_ReadCommand()
{
   int result = 0;
   set_digital_output(IO_B1, HIGH);
   set_digital_output(IO_B1, LOW);
   ShiftOut(0B1100, 3);
   result = ShiftIn(3);
   return result;
}

float get_compass_angle()
{
   float X_Data = 0, Y_Data = 0, angle = 0; // initialize to avoid compiler warning
   int HM55B_result; // status flag

   HM55B_StartMeasurementCommand(); // necessary!!
   delayms(40); // the data is 40ms later ready
   HM55B_result = HM55B_ReadCommand();
   while(HM55B_result != 0B1100) HM55B_result = HM55B_ReadCommand(); // poll until ready status
   X_Data = ShiftIn(11); // Field strength in X
   Y_Data = ShiftIn(11); // and Y direction
   set_digital_output(IO_B1, HIGH); // ok deselect chip
   angle = 180 * (atan2(-1 * Y_Data , X_Data) / M_PI);
   return(angle);
}
   
// I added the next 2 modules to use the red LED to aid reading and calibrating the compass
// If you don't need them, delete them and flash_angle() and delayms() references from main()

void flash_digit_value(int digit)
{
   // flash LED 'digit value' times for each digit in angle
   int counter;

   if(digit==0) digit = 10; // special case for 0
   
   for(counter=1;counter<=digit;counter++)
   {
      set_digital_output(IO_D1, HIGH); // LED on
	  delayms(200);	
      set_digital_output(IO_D1, LOW); // LED off
	  delayms(200);	
   }
   delayms(1000); // indicates end of digit
}

void flash_angle(float angle)
{
   // LED flashes indicate angle in degrees 0..359 by default
   // 1 flash -> 1, 2 flashes -> 2, .. 9 flashes -> 9, BUT 10 flashes -> 0 (special case)
   // ex. angle = 037 degrees -> 10 flashes + pause + 3 flashes + pause + 7 flashes + pause
   // ex. angle = -037 degrees (same as above but followed by 1 flash for - sign)
   // (readings repeat every 6 seconds)

   int ones, tens, hundreds;
   int uangle = abs(angle);
   hundreds = (int)(uangle/100);
   tens = (int)((uangle - hundreds*100)/10);
   ones = uangle - tens*10 - hundreds*100;
   flash_digit_value(hundreds);
   flash_digit_value(tens);
   flash_digit_value(ones);
   if(angle<0) flash_digit_value(1); // indicate negative angle
}

float adjusted(float myangle)
{
   // adjust raw angle reading by adding an offset 
   // these are values from my testing, yours will be different
   // 16 offsets are determined by comparing template angles with HM55B readings; the first and last offsets
   // are determined by linear interpolation
   //                 { -180, ... your 16 readings here ..., 180); 
   float raw[18] =    {  -180,-175, -152, -137,-126,-115,-106,-99,  -89, 9,   68, 90,  100,111,  125,141, 164,  180};
   float offsets[18] ={ -4.98,  -5, -5.5,    2,13.5,  25,38.5, 54, 66.5,-9,-45.5,-45,-32.5,-21,-12.5, -6,-6.5,-5.83};
   //                  {-4.98, ..., -5.83} these two values computed with spreadsheet   
   int index = 0;
   float offset;

   for(index=17;index>0;index--) if(myangle>=raw[index-1]) break;

   // do linear interpolation...

   offset = (myangle-raw[index-1])*(offsets[index]-offsets[index-1])/(raw[index]-raw[index-1])+offsets[index-1];

   if(calibrate_mode) return(myangle); // return raw angle for calibration
   else return(myangle+offset<0?myangle+offset+360:myangle+offset); // return calibrated angle 000..359
}

int main()
{
   int reading;
   while(1)
   {
      compass_angle = 0;
      for(reading=1;reading<=100;reading++) //get 100 readings, then average...
	  {
	     setup();
         HM55B_Reset();
         compass_angle += get_compass_angle();
	  }
	  compass_angle /= 100;
      // now lookup the angle in the lookup table...
      compass_angle = adjusted(compass_angle);
      flash_angle(compass_angle);
      delayms(2000); // pause before next reading
   }
   return 0;
}
