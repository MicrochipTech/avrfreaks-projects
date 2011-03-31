//**************************************************************************
//! @file $RCSfile: dali_code.h,v $
//!
//! Copyright (c) 2005 Atmel.
//!
//! Please read file license.txt for copyright notice.
//!
//! @brief This file contains all DALI command code definitions.
//!
//! @version $Revision: 1.1 $ $Name: megaballast-refd-0_2_9 $
//!
//! @todo
//! @bug
//**************************************************************************

//_____ I N C L U D E S ____________________________________________________

//_____ D E F I N I T I O N S ______________________________________________

#ifndef _DALI_CODE_
#define _DALI_CODE_

//----- Pre-defined response on query
#define DALI_YES                                    0xFF
#define DALI_MASK                                   0xFF
#define DALI_DONT_CHANGE                            0xFF

//----- No command, setting or query
#define NO_DALI_COMMAND_CODE                        0x00

//----- Arc power control commands
#define IMMEDIATE_OFF                               0x00
#define UP_200MS                                    0x01
#define DOWN_200MS                                  0x02
#define STEP_UP                                     0x03
#define STEP_DOWN                                   0x04
#define RECALL_MAX_LEVEL                            0x05
#define RECALL_MIN_LEVEL                            0x06
#define STEP_DOWN_AND_OFF                           0x07
#define ON_AND_STEP_UP                              0x08
#define GO_TO_SCENE                                 0x10  // 'd_data' => "scene"

//----- General configuration commands
    //- Need to be received twice
#define RESET                                       0x20
#define STORE_ACTUAL_DIM_LEVEL_IN_DTR               0x21  // "actual_dim_level" => 'dtr'

//----- Arc power parameters settings
    //- Need to be received twice
#define STORE_THE_DTR_AS_MAX_LEVEL                  0x2A  // 'dtr' => "max_level"
#define STORE_THE_DTR_AS_MIN_LEVEL                  0x2B  // 'dtr' => "min_level"
#define STORE_THE_DTR_AS_SYSTEM_FAILURE_LEVEL       0x2C  // 'dtr' => "system_failure_level"
#define STORE_THE_DTR_AS_POWER_ON_LEVEL             0x2D  // 'dtr' => "power_on_level"
#define STORE_THE_DTR_AS_FADE_TIME                  0x2E  // 'dtr' => "fade_time"
#define STORE_THE_DTR_AS_FADE_RATE                  0x2F  // 'dtr' => "fade_rate"
#define STORE_THE_DTR_AS_SCENE                      0x40  // 'd_data' => "scene", 'dtr' => new "scene_level"

//----- System parameters settings
    //- Need to be received twice
#define REMOVE_FROM_SCENE                           0x50  // 'd_data' => "scene"
#define ADD_TO_GROUP                                0x60  // 'd_data' => "group"
#define REMOVE_FROM_GROUP                           0x70  // 'd_data' => "group"
#define STORE_DTR_AS_SHORT_ADDRESS                  0x80  // 'dtr' => "short_address"

//----- Queries related to status information
#define QUERY_STATUS                                0x90  // "DALI_YES" => 'd_data' else nothing
#define QUERY_BALLAST                               0x91  // "DALI_YES" => 'd_data' else nothing
#define QUERY_LAMP_FAILURE                          0x92  // "DALI_YES" => 'd_data' else nothing
#define QUERY_LAMP_POWER_ON                         0x93  // "DALI_YES" => 'd_data' else nothing
#define QUERY_LIMIT_ERROR                           0x94  // "DALI_YES" => 'd_data' else nothing
#define QUERY_RESET_STATE                           0x95  // "DALI_YES" => 'd_data' else nothing
#define QUERY_MISSING_SHORT_ADDRESS                 0x96  // "DALI_YES" => 'd_data' else nothing
#define QUERY_VERSION_NUMBER                        0x97  // "version_number" => 'd_data'
#define QUERY_CONTENT_DTR                           0x98  // 'dtr' => 'd_data'
#define QUERY_DEVICE_TYPE                           0x99  // "device_type" => 'd_data'
#define QUERY_PHYSICAL_MINIMUM_LEVEL                0x9A  // "physical_minimum_level" => 'd_data'
#define QUERY_POWER_FAILURE                         0x9B  // ... response (c.f. DALI standard) => 'd_data'

//----- Queries related to arc power parameters settings
#define QUERY_ACTUAL_LEVEL                          0xA0  // "actual_dim_level" or "DALI_MASK" => 'd_data'
#define QUERY_MAX_LEVEL                             0xA1  // "max_level" => 'd_data'
#define QUERY_MIN_LEVEL                             0xA2  // "min_level" => 'd_data'
#define QUERY_POWER_ON_LEVEL                        0xA3  // "power_on_level" => 'd_data' 
#define QUERY_SYSTEM_FAILURE_LEVEL                  0xA4  // "system_failure_level" => 'd_data' 
#define QUERY_FADE                                  0xA5  // "fade_time,fade_rate" => 'd_data'

//----- Queries related to system parameters settings
#define QUERY_SCENE_LEVEL                           0xB0  // 'd_data' => "scene", "scene_level" => 'd_data' 
#define QUERY_GROUPS_0_7                            0xC0  // "group_0_7" => 'd_data'
#define QUERY_GROUPS_8_15                           0xC1  // "group_8_15" => 'd_data'
#define QUERY_RANDOM_ADDRESS_H                      0xC2  // "random_address.h" => 'd_data'
#define QUERY_RANDOM_ADDRESS_M                      0xC3  // "random_address.m" => 'd_data'
#define QUERY_RANDOM_ADDRESS_L                      0xC4  // "random_address.l" => 'd_data'

//----- Use of extended commands
#define QUERY_APPLICATION_EXTENTED_COMMAND          0xE0  // for "device_type" = 0, extended commands are not used

//----- Extended commands - Terminate special processes
#define TERMINATE                                   0xA1

//----- Extended commands - Download information to the dtr
#define DATA_TRANSFER_REGISTER                      0xA3  // received direct data => 'dtr'

//----- Extended commands - Addressing commands
    //- Need to be received twice
#define INITIALISE                                  0xA5    // 'd_data' => "reaction_of_ballasts"
#define RANDOMISE                                   0xA7
#define COMPARE                                     0xA9
#define WITHDRAW                                    0xAB
#define SEARCHADDRH                                 0xB1    // 'd_data' => "search_address.h"
#define SEARCHADDRM                                 0xB3    // 'd_data' => "search_address.m"
#define SEARCHADDRL                                 0xB5    // 'd_data' => "search_address.l"
#define PROGRAM_SHORT_ADDRESS                       0xB7    // 'd_data' => "short_address"
#define VERIFY_SHORT_ADDRESS                        0xB9    // 'd_data' => "short_address", "DALI_YES" => 'd_data' if equal
#define QUERY_SHORT_ADDRESS                         0xBB    // "short_address" or "DALI_MASK" => 'd_data'
#define PHYSICAL_SELECTION                          0xBD

//----- Extended commands - Special extended command
#define ENABLE_DEVICE_TYPE_X                        0xC1    // for "device_type" = 0, extended commands are not used
                                                            // 'd_data' => "device_type"

//_____ D E C L A R A T I O N S ____________________________________________


#endif  // _DALI_CODE_
