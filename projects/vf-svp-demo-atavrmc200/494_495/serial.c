/**
* @file serial.c
*
* Copyright (c) 2004 Atmel.
*
* @brief This module provides function for UART communication
*
* @version 1.0 (CVS revision : 1.1)
* @date 2006/03/09 16:33:39
* @author raubree
*****************************************************************************/
#include "config.h"
#include "serial.h"

/**
* @brief UART initialization
* @param void
* @pre void
* @post UART initialized
*/
void init_uart(void)
{
  BAUD_RATE_LOW_REG = BRREG_VALUE;
  UART_CONTROL_REG = (1 << ENABLE_RECEIVER_BIT) |
                     (1 << ENABLE_TRANSMITTER_BIT); // enable receive and transmit
}

/**
* @brief send a character on the UART
* @param char c
* @pre UART initialization
* @post Char send on the UART
*/
void sendchar(char c)
{
  UART_DATA_REG = c;                                   // prepare transmission
  while (!(UART_STATUS_REG & (1 << TRANSMIT_COMPLETE_BIT)));// wait until byte sendt
  UART_STATUS_REG |= (1 << TRANSMIT_COMPLETE_BIT);          // delete TXCflag
}

/**
* @brief receive a character from the UART
* @param void
* @pre UART initialization
* @post Char in the UART_DATA_REG register
*/
char recchar(void)
{
  while(!(UART_STATUS_REG & (1 << RECEIVE_COMPLETE_BIT)));  // wait for data
  return UART_DATA_REG;
}

/**
* @brief send a character on the UART with feedback
* @param ch
* @pre UART initialization
* @post Char send on the UART with a confirmation code
*/
int putchar(int ch)
{
  UART_DATA_REG = ch;                                   // prepare transmission
  while (!(UART_STATUS_REG & (1 << TRANSMIT_COMPLETE_BIT)));// wait until byte sendt
  UART_STATUS_REG |= (1 << TRANSMIT_COMPLETE_BIT);          // delete TXCflag
  return ch;
}

/**
* @brief Test if a character has been received
* @param void
* @pre UART initialization
* @post FALSE no character receive TRUE a character receive
*/
char tstrx(void)
{
  if (UART_STATUS_REG & (1 << RECEIVE_COMPLETE_BIT))
  {
    return TRUE;
  }
  else
  {
    return FALSE;
  }
}

/**
* @brief send a string on the UART
* @param string str
* @pre UART initialization
* @post String send to the UART
*/
void putstring(char *str)
{
  char ch;

  while((ch=*str)!= '\0')
  {
    sendchar(ch);
    str++;
  }
}

/**
* @brief send an integer on the UART
* @param integer number
* @pre UART initialization
* @post Integer send to the UART
*/
void putint(int number)
{
  char value[6]={0,0,0,0,0,0};

   while((number - 10000)>=0)
  {
    number -= 10000;
    value[5]++;
  }
  value[5] += '0';

  while((number - 1000)>=0)
  {
    number -= 1000;
    value[4]++;
  }
  value[4] += '0';

  while((number - 100)>=0)
  {
    number -= 100;
    value[3]++;
  }
  value[3] += '0';

  while((number - 10)>=0)
  {
    number -= 10;
    value[2]++;
  }
  value[2] += '0';

  value[1] = number + '0';
  value[0] = '\0';

  sendchar(value[5]);
  sendchar(value[4]);
  sendchar(value[3]);
  sendchar(value[2]);
  sendchar(value[1]);
}

/**
* @brief send an unsigned char on the UART
* @param Unsigned char (U8) number
* @pre UART initialization
* @post Unsigned char send to the UART
*/
void putU8(U8 number)
{
  char value[3]={0,0,0};

  while((number - 100)>=0)
  {
    number -= 100;
    value[2]++;
  }
  value[2] += '0';

  while((number - 10)>=0)
  {
    number -= 10;
    value[1]++;
  }
  value[1] += '0';

  value[0] = number + '0';

  sendchar(value[2]);
  sendchar(value[1]);
  sendchar(value[0]);
}

/**
* @brief send a integer signed to the UART
* @param Signed Integer number
* @pre UART initialization
* @post Signed integer send to the UART
*/
void putS16(S16 number)
{
  char value[6]={0,0,0,0};

  if(number >= 0)
  {
    value[5]='+';
  }
  else
  {
    value[5]='-';
    number *= -1;
  }

  while((number - 10000)>=0)
  {
    number -= 10000;
    value[4]++;
  }
  value[4] += '0';

  while((number - 1000)>=0)
  {
    number -= 1000;
    value[3]++;
  }
  value[3] += '0';

  while((number - 100)>=0)
  {
    number -= 100;
    value[2]++;
  }
  value[2] += '0';

  while((number - 10)>=0)
  {
    number -= 10;
    value[1]++;
  }
  value[1] += '0';

  value[0] = number + '0';

  sendchar(value[5]);
  sendchar(value[4]);
  sendchar(value[3]);
  sendchar(value[2]);
  sendchar(value[1]);
  sendchar(value[0]);
}
