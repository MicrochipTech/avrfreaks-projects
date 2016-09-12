# Sam3x Arduino Due HC-SR04 with interrupts, reduce ASF layers

Uploaded by jstampfl on 2016-09-12 10:17:00 (rating 0 out of 5)

## Summary

AS7  1006.  Began with the "Getting-Started" ASF example for the Arduino Due/Sam3x board.


 


NOTE:  The HC-SR04 is a 5 volt device.  YOU MUST USE A LEVEL SHIFTER ON THE ECHO LINE.


 


The 3.3 volt trigger works fine as is.


 


Hardware connection is:  Trigger pin on Arduino pin 09 and the echo on Arduino pin 12 via the level shifter.  Connect the Arduino 5v to VCC and GND to GND.


 


Here is a Arduino sketch that will test the operation.


 



```

/* Ping))) Sensor

   This sketch reads a PING))) ultrasonic rangefinder and returns the
   distance to the closest object in range. To do this, it sends a pulse
   to the sensor to initiate a reading, then listens for a pulse
   to return.  The length of the returning pulse is proportional to
   the distance of the object from the sensor.

   The circuit:
	* +V connection of the PING))) attached to +5V
	* GND connection of the PING))) attached to ground
	* SIG connection of the PING))) attached to digital pin 7

   http://www.arduino.cc/en/Tutorial/Ping

   created 3 Nov 2008
   by David A. Mellis
   modified 30 Aug 2011
   by Tom Igoe

   This example code is in the public domain.

 */

// this constant won't change.  It's the pin number
// of the sensor's output:
const int trigPin = 9;
const int echoPin = 12;
void setup() {
  // initialize serial communication:
  Serial.begin(9600);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
}

void loop() {
  // establish variables for duration of the ping,
  // and the distance result in inches and centimeters:
  long duration, inches, cm;

  // The PING))) is triggered by a HIGH pulse of 2 or more microseconds.
  // Give a short LOW pulse beforehand to ensure a clean HIGH pulse:
  //pinMode(pingPin, OUTPUT);
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(5);
  digitalWrite(trigPin, LOW);

  // The same pin is used to read the signal from the PING))): a HIGH
  // pulse whose duration is the time (in microseconds) from the sending
  // of the ping to the reception of its echo off of an object.
  //pinMode(pingPin, INPUT);
  duration = pulseIn(echoPin, HIGH);

  // convert the time into a distance
  inches = microsecondsToInches(duration);
  cm = microsecondsToCentimeters(duration);

  Serial.print(inches);
  Serial.print("in, ");
  Serial.print(cm);
  Serial.print("cm");
  Serial.println();

  delay(1000);
}

long microsecondsToInches(long microseconds) {
  // According to Parallax's datasheet for the PING))), there are
  // 73.746 microseconds per inch (i.e. sound travels at 1130 feet per
  // second).  This gives the distance travelled by the ping, outbound
  // and return, so we divide by 2 to get the distance of the obstacle.
  // See: http://www.parallax.com/dl/docs/prod/acc/28015-PING-v1.3.pdf
  return microseconds / 74 / 2;
}

long microsecondsToCentimeters(long microseconds) {
  // The speed of sound is 340 m/s or 29 microseconds per centimeter.
  // The ping travels out and back, so to find the distance of the
  // object we take half of the distance travelled.
  return microseconds / 29 / 2;
}

```
This will run on An Arduino UNO, but the level shifter isn't needed.


 


I used an Atmel ICE as the debugger/programmer.  Connect to the JTAG port on the Arduino Due.


 


Here is the ASF modified code.  After starting an example of "Getting-Started" in ASF, replace the main.c with the following main.c.


 


Note:  The PIOD\_Hander in pio\_handler.c must be commented out.  The PIOD-Handler in main.c will be used.


 



```

/**
 *
 *  Sam3x - Arduino Due HC-SR04 with interrupts
 *
 *  Copyright (c) 2016  John Stampfl
 *
 *  This project is to remove as many ASF layers as I can. Using the HC-SR04 an an
 *  example.
 *  NOTE:  You need to comment out the PIOD\_Handler in:
 * "src/ASF/sam/drivers/pio/pio\_handler.c"
 *
 *
 * \brief Getting Started Application.
 *
 * Copyright (c) 2011-2015 Atmel Corporation. All rights reserved.
 *
 * \asf\_license\_start
  *
  * \page License
  *
  * Redistribution and use in source and binary forms, with or without
  * modification, are permitted provided that the following conditions are met:
  *
  * 1. Redistributions of source code must retain the above copyright notice,
  *    this list of conditions and the following disclaimer.
  *
  * 2. Redistributions in binary form must reproduce the above copyright notice,
  *    this list of conditions and the following disclaimer in the documentation
  *    and/or other materials provided with the distribution.
  *
  * 3. The name of Atmel may not be used to endorse or promote products derived
  *    from this software without specific prior written permission.
  *
  * 4. This software may only be redistributed and used in connection with an
  *    Atmel microcontroller product.
  *
  * THIS SOFTWARE IS PROVIDED BY ATMEL "AS IS" AND ANY EXPRESS OR IMPLIED
  * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
  * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT ARE
  * EXPRESSLY AND SPECIFICALLY DISCLAIMED. IN NO EVENT SHALL ATMEL BE LIABLE FOR
  * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
  * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
  * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
  * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
  * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
  * POSSIBILITY OF SUCH DAMAGE.
  *
  * \asf\_license\_stop
  *
  */
#include "asf.h"
#include "stdio\_serial.h"
#include "conf\_board.h"
#include "conf\_clock.h"
#define STRING\_HEADER "-- Getting Started Example --\r\n" \
		"-- "BOARD\_NAME" --\r\n" \
		"-- Compiled: "\_\_DATE\_\_" "\_\_TIME\_\_" --\r\n"

volatile uint32\_t g\_ul\_ms\_ticks = 0; // [main\_var\_ticks]

volatile uint32\_t state,time,cnt;
volatile uint32\_t status;

void SysTick\_Handler(void)
{
	g\_ul\_ms\_ticks++;
}

void PIOD\_Handler()					//[main\_Echo interrupt\_handler]
{
	tc\_stop(TC0, 0);
	status = pio\_get\_interrupt\_status(PIOD);
	status &= pio\_get\_interrupt\_mask(PIOD);
	if (state == 3)
	{
		state = 5;
		time = tc\_read\_cv(TC0,0);
	} else
	if (state == 2)
	{
		state = 3;
		tc\_write\_rc(TC0,0,2000000U);
		tc\_start(TC0,0);
	} else
	{
		state = 0;
	}
}

static void configure\_trig(void)	//Trigger pin
{
	PIOC->PIO\_PUDR = PIO\_PC21;
	PIOC->PIO\_CODR = PIO\_PC21;
	PIOC->PIO\_OER  = PIO\_PC21;
}

static void configure\_Echo(void)	//Echo pin
{
	uint32\_t bitmask;
	pmc\_enable\_periph\_clk(ID\_PIOD);
	bitmask = PIOD->PIO\_IMR | PIO\_PD8;
	PIOD->PIO\_IDR = 0XFFFFFFFF;
	PIOD->PIO\_ISR;
	NVIC\_DisableIRQ((IRQn\_Type) ID\_PIOD);
	NVIC\_ClearPendingIRQ((IRQn\_Type) ID\_PIOD);
	NVIC\_EnableIRQ((IRQn\_Type) ID\_PIOD);
	PIOD->PIO\_ISR;
	PIOD->PIO\_IER = bitmask;
	PIOD->PIO\_MDER = PIO\_PD8;
}

void TC0\_Handler(void)
{
	tc\_stop(TC0,0);
	tc\_get\_status(TC0, 0);
	if (state == 1)
	{
		state = 2;
		PIOC->PIO\_CODR = PIO\_PC21;
	} else
	{
		state = 0;
	}
}

static void configure\_tc(void)
{
	pmc\_enable\_periph\_clk(ID\_TC0);
	tc\_init(TC0, 0, TC\_CMR\_CPCTRG);
	NVIC\_EnableIRQ((IRQn\_Type) ID\_TC0);
	tc\_enable\_interrupt(TC0, 0, TC\_IER\_CPCS);
}

static void configure\_console(void)
{
	const usart\_serial\_options\_t uart\_serial\_options = {
		.baudrate = 250000,
#ifdef CONF\_UART\_CHAR\_LENGTH
		.charlength = CONF\_UART\_CHAR\_LENGTH,
#endif
		.paritytype = CONF\_UART\_PARITY,
#ifdef CONF\_UART\_STOP\_BITS
		.stopbits = CONF\_UART\_STOP\_BITS,
#endif
	};
	sysclk\_enable\_peripheral\_clock(CONSOLE\_UART\_ID);
	stdio\_serial\_init(CONF\_UART, &uart\_serial\_options);
}

static void mdelay(uint32\_t ul\_dly\_ticks)
{
	uint32\_t ul\_cur\_ticks;

	ul\_cur\_ticks = g\_ul\_ms\_ticks;
	while ((g\_ul\_ms\_ticks - ul\_cur\_ticks) < ul\_dly\_ticks);
}

int main(void)
{
	cnt = 0;
	uint32\_t sum = 0;
	int i = 0;
	int mm = 0;
	int cm = 0;
	sysclk\_init();
	board\_init();
	configure\_trig();
	configure\_Echo();
	configure\_console();
	puts(STRING\_HEADER);
										//! [main\_step\_systick\_init]
	if (SysTick\_Config(84000000U / 1000U)) {
		puts("-F- Systick configuration error\r");
		while (1);
	}
										//! [main\_step\_systick\_init]
	configure\_tc();

	PIOC->PIO\_CODR = PIO\_PC21;			//Low level on trigger pin
	mdelay(2);							//for a short time.
	printf("start\r\n");

	while (1)
	{
		if (state == 0)
		{
		PIOC->PIO\_SODR = PIO\_PC21;		//High level on trigger &
		tc\_write\_rc(TC0,0,880U);
		tc\_start(TC0,0);				//start timer
		state = 1;
		}
		if (state == 2)
		{
			cnt++;						//no reason now.
		}

		if (state == 5)
		{
			i++;
			sum = sum + time;
			if (i == 8)
			{
				sum = sum/8;
				i = 1;
				mm = (sum +8083)/252;
				cm = (mm + 5)/10;
				printf("time = %u, mm = %d, cm = %d\r\n",(unsigned int)sum,mm,cm);
			}
			time = 0;
			state = 0;
			mdelay(500);
		}
	}
}

```

## Compilers

- Atmel Studio 6

## Tags

- Complete code
- Atmel Studio 6
