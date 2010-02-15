
#include <ioavr.h>
#include <inavr.h>
#include <stdlib.h>
#include "global.h"
#include "uart.h"
#include "sm_driver.h"
#include "speed_cntr.h"

//! Global status flags
struct GLOBAL_FLAGS status = {FALSE, FALSE, 0};

void ShowHelp(void);
void ShowData(int position, int acceleration, int deceleration, int speed, int steps);

/*! \brief Init of peripheral devices.
 *
 *  Setup IO, uart, stepper, timer and interrupt.
 */
void Init(void)
{
  // Init of IO pins
  sm_driver_Init_IO();
  // Init of uart
  InitUART();

  // Set stepper motor driver output
  sm_driver_StepOutput(0);

  // Init of Timer/Counter1
  speed_cntr_Init_Timer1();

  __enable_interrupt();
}

/*! \brief Demo of linear speed controller.
 *
 *  Serial interface frontend to test linear speed controller.
 */
void main(void)
{
  // Number of steps to move.
  int steps = 1000;
  // Accelration to use.
  int acceleration = 100;
  // Deceleration to use.
  int deceleration = 100;
  // Speed to use.
  int speed = 800;
  // Tells if the received string was a valid command.
  char okCmd = FALSE;

  Init();

  // Outputs help screen.
  uart_SendString("\n\r");
  ShowHelp();
  ShowData(stepPosition, acceleration, deceleration, speed, steps);

  while(1) {
    // If a command is received, check the command and act on it.
    if(status.cmd == TRUE){
      if(UART_RxBuffer[0] == 'm'){
        // Move with...
        if(UART_RxBuffer[1] == ' '){
          // ...number of steps given.
          steps = atoi((char const *)UART_RxBuffer+2);
          speed_cntr_Move(steps, acceleration, deceleration, speed);
          okCmd = TRUE;
          uart_SendString("\n\r  ");
        }
        else if(UART_RxBuffer[1] == 'o'){
          if(UART_RxBuffer[2] == 'v'){
            if(UART_RxBuffer[3] == 'e'){
              // ...all parameters given
              if(UART_RxBuffer[4] == ' '){
                int i = 6;
                steps = atoi((char const *)UART_RxBuffer+5);
                while((UART_RxBuffer[i] != ' ') && (UART_RxBuffer[i] != 13)) i++;
                i++;
                acceleration = atoi((char const *)UART_RxBuffer+i);
                while((UART_RxBuffer[i] != ' ') && (UART_RxBuffer[i] != 13)) i++;
                i++;
                deceleration = atoi((char const *)UART_RxBuffer+i);
                while((UART_RxBuffer[i] != ' ') && (UART_RxBuffer[i] != 13)) i++;
                i++;
                speed = atoi((char const *)UART_RxBuffer+i);
                speed_cntr_Move(steps, acceleration, deceleration, speed);
                okCmd = TRUE;
                uart_SendString("\n\r  ");
              }
            }
          }
        }
      }
      else if(UART_RxBuffer[0] == 'a'){
        // Set acceleration.
        if(UART_RxBuffer[1] == ' '){
          acceleration = atoi((char const *)UART_RxBuffer+2);
          okCmd = TRUE;
        }
      }
      else if(UART_RxBuffer[0] == 'd'){
        // Set deceleration.
        if(UART_RxBuffer[1] == ' '){
          deceleration = atoi((char const *)UART_RxBuffer+2);
          okCmd = TRUE;
        }
      }
      else if(UART_RxBuffer[0] == 's'){
        if(UART_RxBuffer[1] == ' '){
          speed = atoi((char const *)UART_RxBuffer+2);
          okCmd = TRUE;
        }
      }
      else if(UART_RxBuffer[0] == 13){
        speed_cntr_Move(steps, acceleration, deceleration, speed);
        okCmd = TRUE;
      }
      else if(UART_RxBuffer[0] == '?'){
        ShowHelp();
        okCmd = TRUE;
      }

      // Send help if invalid command is received.
      if(okCmd != TRUE)
        ShowHelp();

      // Clear RXbuffer.
      status.cmd = FALSE;
      uart_FlushRxBuffer();

      if(status.running == TRUE){
        uart_SendString("Running...");
        while(status.running == TRUE);
        uart_SendString("OK\n\r");
      }

      ShowData(stepPosition, acceleration, deceleration, speed, steps);
    }//end if(cmd)
  }//end while(1)
}

//! Help message
__flash char Help[] = {"\n\r--------------------------------------------------------------\n\rAtmel AVR446 - Linear speed control of stepper motor\n\r\n\r?        - Show help\n\ra [data] - Set acceleration (range: 71 - 32000)\n\rd [data] - Set deceleration (range: 71 - 32000)\n\rs [data] - Set speed (range: 12 - motor limit)\n\rm [data] - Move [data] steps (range: -64000 - 64000)\n\rmove [steps] [accel] [decel] [speed]\n\r         - Move with all parameters given\n\r<enter>  - Repeat last move\n\r\n\r    acc/dec data given in 0.01*rad/sec^2 (100 = 1 rad/sec^2)\n\r    speed data given in 0.01*rad/sec (100 = 1 rad/sec)\n\r--------------------------------------------------------------\n\r"};

/*! \brief Sends help message.
 *
 *  Outputs help message.
 */
void ShowHelp(void)
{
  unsigned int i = 0;
  while(Help[i] != 0)
    uart_SendByte(Help[i++]);
}

/*! \brief Sends out data.
 *
 *  Outputs the values of the data you can control by serial interface
 *  and the current position of the stepper motor.
 *
 *  \param acceleration Accelration setting.
 *  \param deceleration Deceleration setting.
 *  \param speed Speed setting.
 *  \param steps Position of the stepper motor.
 */
void ShowData(int position, int acceleration, int deceleration, int speed, int steps)
{
  uart_SendString("\n\r  Motor pos: ");
  uart_SendInt(position);
  uart_SendString("    a:");
  uart_SendInt(acceleration);
  uart_SendString("  d:");
  uart_SendInt(deceleration);
  uart_SendString("  s:");
  uart_SendInt(speed);
  uart_SendString("  m:");
  uart_SendInt(steps);
  uart_SendString("\n\r> ");
}


