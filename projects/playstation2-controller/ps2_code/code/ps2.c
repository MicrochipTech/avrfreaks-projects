/* 
 * PS2 controller on Atmel MCU
 * 
 * V1.0  
 * 
 * By: Lars Ivar Miljeteig, September 2010
 * Platform: AVR ATmega168 (avr-gcc)
 * Dependencies: avr/io avr/interrupt delay.h stdint.h larslib.h
 *
 * Reading data from a PlayStation 2 controller
 */

#include "larslib.h"
#include "spi.h"
#include "ps2.h"

// Commands for the controller
const uint8_t PS2_CONFIGMODE[5] = {0x01, 0x43, 0x00, 0x01, 0x00};
const uint8_t PS2_ANALOGMODE[9] = {0x01, 0x44, 0x00, 0x01, 0x03, 0x00, 0x00, 0x00, 0x00};
const uint8_t PS2_SETUPMOTOR[9] = {0x01, 0x4D, 0x00, 0x00, 0x01, 0xff, 0xff, 0xff, 0xff};
const uint8_t PS2_RETURNPRES[9] = {0x01, 0x4f, 0x00, 0xFF, 0xFF, 0x03, 0x00, 0x00, 0x00};
const uint8_t PS2_EXITCONFIG[9] = {0x01, 0x43, 0x00, 0x00, 0x5a, 0x5a, 0x5a, 0x5a, 0x5a};

// The poll command is sent to get the status of each button,
//  and to set the speed of the motors. Can also turn on the
//  small motor
void ps2_poll(uint8_t speed, uint8_t smallmotor)
{
    uint8_t i = 0;


    PORTB &= ~(1<<PB2); // Attention

    // Send header
    rx_buffer[0] = SPI_MTx(0x01);
    rx_buffer[1] = SPI_MTx(0x42);
    rx_buffer[2] = SPI_MTx(0x00);

    // Motors on/off
    rx_buffer[3] = SPI_MTx(smallmotor); // 0x01 => small motor on
    rx_buffer[4] = SPI_MTx(speed);      // Big motor speed


    // Joystick:
    ps2.rx = SPI_MTx(0x00);
    ps2.ry = SPI_MTx(0x00);
    ps2.lx = SPI_MTx(0x00);
    ps2.ly = SPI_MTx(0x00);

    // Pressure buttons:
    for(i=0; i<12; i++) ps2.pressure[i] = SPI_MTx(0x00);


    PORTB |= (1<<PB2); // Attention off

}

// A general function for sending commands to the ps2 controller via SPI
void ps2_send(const uint8_t *cmd, uint8_t length)
{
    uint8_t i = 0;
    PORTB &= ~(1<<PB2);
    for(i=0; i<length; i++) rx_buffer[i] = SPI_MTx(cmd[i]);
    PORTB |= (1<<PB2);    
}

// Go into configuration mode to adjust the settings
void ps2_configmode()
{
    ps2_send(PS2_CONFIGMODE, 5);
}

// Force analog mode to enable pressure values
void ps2_analogmode()
{
    ps2_send(PS2_ANALOGMODE, 9);
}

// Enable the internal vibration motors
void ps2_setupmotor()
{
    ps2_send(PS2_SETUPMOTOR, 9);
}

// Ask to get the pressure values as well
void ps2_returnpres()
{
    ps2_send(PS2_RETURNPRES, 9);
}

// Exit configuration mode
void ps2_exitconfig()
{
    ps2_send(PS2_EXITCONFIG, 9);
}

// Initialize the ps2 controller
void ps2_init()
{
    // Attention pin idle high
    PORTB |= (1<<PB2);

    // Configure controller to send everything
    ps2_configmode();
    _delay_ms(250);
    ps2_analogmode();
    _delay_ms(250);
    ps2_setupmotor();
    _delay_ms(250);
    ps2_returnpres();
    _delay_ms(250);
    ps2_exitconfig();

}

