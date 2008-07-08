//**************************************************************************
//! @file dali_code.h,v
//!
//! Copyright (c) 2005 Atmel.
//!
//! Please read file license.txt for copyright notice.
//!
//! @brief This file contains all DALI command code definitions.
//!
//! @version 1.1 pwm3-ac-ctrl-motor-0_0_6
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
#define NO_DALI_COMMAND_CODE                        0x0000

//----- Arc power control commands
#define DIRECT_ARC_POWER_CONTROL                    0xFE00  // 'd_data' => "arc_power_level"
#define IMMEDIATE_OFF                               0x0100
#define UP_200MS                                    0x0101
#define DOWN_200MS                                  0x0102
#define STEP_UP                                     0x0103
#define STEP_DOWN                                   0x0104
#define RECALL_MAX_LEVEL                            0x0105
#define RECALL_MIN_LEVEL                            0x0106
#define STEP_DOWN_AND_OFF                           0x0107
#define ON_AND_STEP_UP                              0x0108
#define GO_TO_SCENE                                 0x0110  // 'd_data' => "scene"

//----- General configuration commands
    //- Need to be received twice
#define RESET                                       0x0120
#define STORE_ACTUAL_DIM_LEVEL_IN_DTR               0x0121  // "actual_dim_level" => 'dtr'

//----- Arc power parameters settings
    //- Need to be received twice
#define STORE_THE_DTR_AS_MAX_LEVEL                  0x012A  // 'dtr' => "max_level"
#define STORE_THE_DTR_AS_MIN_LEVEL                  0x012B  // 'dtr' => "min_level"
#define STORE_THE_DTR_AS_SYSTEM_FAILURE_LEVEL       0x012C  // 'dtr' => "system_failure_level"
#define STORE_THE_DTR_AS_POWER_ON_LEVEL             0x012D  // 'dtr' => "power_on_level"
#define STORE_THE_DTR_AS_FADE_TIME                  0x012E  // 'dtr' => "fade_time"
#define STORE_THE_DTR_AS_FADE_RATE                  0x012F  // 'dtr' => "fade_rate"
#define STORE_THE_DTR_AS_SCENE                      0x0140  // 'd_data' => "scene", 'dtr' => new "scene_level"

//----- System parameters settings
    //- Need to be received twice
#define REMOVE_FROM_SCENE                           0x0150  // 'd_data' => "scene"
#define ADD_TO_GROUP                                0x0160  // 'd_data' => "group"
#define REMOVE_FROM_GROUP                           0x0170  // 'd_data' => "group"
#define STORE_DTR_AS_SHORT_ADDRESS                  0x0180  // 'dtr' => "short_address"

//----- Queries related to status information
#define QUERY_STATUS                                0x0190  // "DALI_YES" => 'd_data' else nothing
#define QUERY_BALLAST                               0x0191  // "DALI_YES" => 'd_data' else nothing
#define QUERY_LAMP_FAILURE                          0x0192  // "DALI_YES" => 'd_data' else nothing
#define QUERY_LAMP_POWER_ON                         0x0193  // "DALI_YES" => 'd_data' else nothing
#define QUERY_LIMIT_ERROR                           0x0194  // "DALI_YES" => 'd_data' else nothing
#define QUERY_RESET_STATE                           0x0195  // "DALI_YES" => 'd_data' else nothing
#define QUERY_MISSING_SHORT_ADDRESS                 0x0196  // "DALI_YES" => 'd_data' else nothing
#define QUERY_VERSION_NUMBER                        0x0197  // "version_number" => 'd_data'
#define QUERY_CONTENT_DTR                           0x0198  // 'dtr' => 'd_data'
#define QUERY_DEVICE_TYPE                           0x0199  // "device_type" => 'd_data'
#define QUERY_PHYSICAL_MINIMUM_LEVEL                0x019A  // "physical_minimum_level" => 'd_data'
#define QUERY_POWER_FAILURE                         0x019B  // ... response (c.f. DALI standard) => 'd_data'

//----- Queries related to arc power parameters settings
#define QUERY_ACTUAL_LEVEL                          0x01A0  // "actual_dim_level" or "DALI_MASK" => 'd_data'
#define QUERY_MAX_LEVEL                             0x01A1  // "max_level" => 'd_data'
#define QUERY_MIN_LEVEL                             0x01A2  // "min_level" => 'd_data'
#define QUERY_POWER_ON_LEVEL                        0x01A3  // "power_on_level" => 'd_data' 
#define QUERY_SYSTEM_FAILURE_LEVEL                  0x01A4  // "system_failure_level" => 'd_data' 
#define QUERY_FADE                                  0x01A5  // "fade_time,fade_rate" => 'd_data'

//----- Queries related to system parameters settings
#define QUERY_SCENE_LEVEL                           0x01B0  // 'd_data' => "scene", "scene_level" => 'd_data' 
#define QUERY_GROUPS_0_7                            0x01C0  // "group_0_7" => 'd_data'
#define QUERY_GROUPS_8_15                           0x01C1  // "group_8_15" => 'd_data'
#define QUERY_RANDOM_ADDRESS_H                      0x01C2  // "random_address.h" => 'd_data'
#define QUERY_RANDOM_ADDRESS_M                      0x01C3  // "random_address.m" => 'd_data'
#define QUERY_RANDOM_ADDRESS_L                      0x01C4  // "random_address.l" => 'd_data'

//----- Use of extended commands
#define QUERY_APPLICATION_EXTENTED_COMMAND          0x01E0  // for "device_type" = 0, extended commands are not used

//----- Extended commands - Terminate special processes
#define TERMINATE                                   0xA100

//----- Extended commands - Download information to the dtr
#define DATA_TRANSFER_REGISTER                      0xA300  // received direct data => 'dtr'

//----- Extended commands - Addressing commands
    //- Need to be received twice
#define INITIALISE                                  0xA500  // 'd_data' => "reaction_of_ballasts"
#define RANDOMISE                                   0xA700
#define COMPARE                                     0xA900
#define WITHDRAW                                    0xAB00
#define SEARCHADDRH                                 0xB100  // 'd_data' => "search_address.h"
#define SEARCHADDRM                                 0xB300  // 'd_data' => "search_address.m"
#define SEARCHADDRL                                 0xB500  // 'd_data' => "search_address.l"
#define PROGRAM_SHORT_ADDRESS                       0xB700  // 'd_data' => "short_address"
#define VERIFY_SHORT_ADDRESS                        0xB900  // 'd_data' => "short_address", "DALI_YES" => 'd_data' if equal
#define QUERY_SHORT_ADDRESS                         0xBB00  // "short_address" or "DALI_MASK" => 'd_data'
#define PHYSICAL_SELECTION                          0xBD00

//----- Extended commands - Special extended command
#define ENABLE_DEVICE_TYPE_X                        0xC100  // for "device_type" = 0, extended commands are not used
                                                            // 'd_data' => "device_type"

//_____ D E C L A R A T I O N S ____________________________________________


#endif  // _DALI_CODE_
