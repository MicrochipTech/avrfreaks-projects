/*===========================================================================
     _____        _____        _____        _____
 ___|    _|__  __|_    |__  __|__   |__  __| __  |__  ______
|    \  /  | ||    \      ||     |     ||  |/ /     ||___   |
|     \/   | ||     \     ||     \     ||     \     ||___   |
|__/\__/|__|_||__|\__\  __||__|\__\  __||__|\__\  __||______|
    |_____|      |_____|      |_____|      |_____|

--[Mark3 Realtime Platform]--------------------------------------------------

Copyright (c) 2013 Funkenstein Software Consulting, all rights reserved.
See license.txt for more information
===========================================================================*/
/*!
    \file nlfs_config.h
    \brief NLFS configuration parameters
*/

#ifndef __NLFS_CONFIG_H
#define __NLFS_CONFIG_H

#define DEBUG       0

#if DEBUG
 #include <stdio.h>
 #include <stdlib.h>
 #define DEBUG_PRINT     printf
#else
 #define DEBUG_PRINT(...)
#endif


#endif // NLFS_CONFIG_H
