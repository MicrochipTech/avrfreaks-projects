/*===========================================================================
     _____        _____        _____        _____
 ___|    _|__  __|_    |__  __|__   |__  __| __  |__  ______
|    \  /  | ||    \      ||     |     ||  |/ /     ||___   |
|     \/   | ||     \     ||     \     ||     \     ||___   |
|__/\__/|__|_||__|\__\  __||__|\__\  __||__|\__\  __||______|
    |_____|      |_____|      |_____|      |_____|

--[Mark3 Realtime Platform]--------------------------------------------------

Copyright (c) 2012-2015 Funkenstein Software Consulting, all rights reserved.
See license.txt for more information
===========================================================================*/
/*!
    \file joystick.h
    
    \brief Joystick driver interface
    
    This module provides the basic interface on which any sort of joystick 
    driver can be implemented.  By using this abstraction, the underlying 
    hardware can change without affecting applications that are written to
    use the common APIs.

    At the heart of this API is the Scan() API.  This function should be 
    called at regular intervals to update the state of the joystick.  This
    is done in order to minimize the overhead from reading the joystick
    on-demand, but also to ensure timing consistency in response - which
    is helpful when implementing games and UI's for which display updates
    and joystick scanning should be synchronized.
        
    The Scan() method calls the Driver's Control() method using a special 
    JOYSTICK_SCAN command, which is implemented by the actual hardware driver.
    
    Accessing the data is a matter of calling the ReadCurrent() and 
    ReadPrevious() methods, which return the current and previous state
    of the joystick.  Note that ReadCurrent() returns the state of the joystick
    as of the last time the Scan() method was run - the value returned from
    ReadCurrent() can only change with another call to Scan().  To determine 
    changes in state from frame-to-frame, compare the results from calls to 
    ReadCurrent() and ReadPrevious().  In this way, it is possible to implement
    more complex, application-specific input state machines.
*/

#include "kerneltypes.h"
#include "driver.h"

#ifndef __JOYSTICK_H__
#define __JOYSTICK_H__

//---------------------------------------------------------------------------
/*!
    Structure defining a generic joystick report structure.  In this case, 
    composed of the following items:
    - Digital input, including a D-pad and up to 12 additional buttons.
    - 16-bit analog input for two sets of analog x/y axis.
    .
*/
typedef struct
{
    union
    {
        K_USHORT usDigital;
        struct
        {
            unsigned int bUp         : 1;
            unsigned int bDown         : 1;
            unsigned int bLeft        : 1;
            unsigned int bRight        : 1;
            unsigned int bButton1    : 1;
            unsigned int bButton2    : 1;
            unsigned int bButton3    : 1;
            unsigned int bButton4    : 1;
            unsigned int bButton5    : 1;
            unsigned int bButton6    : 1;
            unsigned int bButton7    : 1;
            unsigned int bButton8    : 1;
            unsigned int bButton9    : 1;
            unsigned int bButton10    : 1;
            unsigned int bSelect    : 1;
            unsigned int bStart        : 1;
        };
    };
    K_SHORT usAnalogX1;
    K_SHORT usAnalogY1;
    K_SHORT usAnalogX2;
    K_SHORT usAnalogY2;
} JoystickReport;

//---------------------------------------------------------------------------
/*!
    Generic joystick driver events/commands
*/
typedef enum
{
    JOYSTICK_SCAN = 0x80    //!< Scan event/command.  Must be implemented in
                            //!< The inheriting class.
} JoystickCommands;

//---------------------------------------------------------------------------
/*!
    Joystick driver base class.  This class implements the API methods required
    for a generic joystick driver - including calls to scan the joystick
    hardware, read the current joystick value, and read the previously-read
    joystick value.  
*/
class JoystickDriver : public Driver
{
public:
    /*!
        \fn void Scan()
        
        Read the current joystick values from hardware.  This calls the
        underlying driver's Control() function with the JOYSTICK_SCAN event,
        which is implemented in the inheriting class.  This copies the 
        newly-read data into the CurrentReport member, while moving the 
        previously-read report data into m_stLastReport.
    */
    void Scan();
    
    /*!
        \fn void ReadCurrent( JoystickReport &pstReport_ )
        
        Copy out the current joystick report data.  This does not trigger a 
        hardware re-scan of the joystick hardware.
        
        \param pstReport_ Pointer to the JoystickReport object that will 
               hold the read data.
    */
    void ReadCurrent( JoystickReport *pstReport_ );
    
    /*!
        \fn void ReadPrevious( JoystickReport &pstReport_ )
        
        Copy out the previous joystick report data.  This does not trigger a
        hardware re-scan of the joystick hardware.
        
        \param pstReport_ Pointer to the JoystickReport object that will 
               hold the read data.
    */
    void ReadPrevious( JoystickReport *pstReport_ );

protected:
    JoystickReport m_stCurrentReport;    //!< This holds the current joystick data
    JoystickReport m_stLastReport;        //!< This holds the previous joystick data
};

//---------------------------------------------------------------------------

#endif //__JOYSTICK_H__
