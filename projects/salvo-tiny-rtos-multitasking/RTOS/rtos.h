/************************************************************
// Multitasking functions
************************************************************/
#include <salvo.h>
#include "board.h"

#define TASK_1_P           OSTCBP(1)   // task #1
#define TASK_2_P           OSTCBP(2)   //  ""  #2
#define TASK_3_P           OSTCBP(3)   //  ""  #3
#define TASK_4_P           OSTCBP(4)   //  ""  #4
#define TASK_5_P           OSTCBP(5)   //  ""  #5
#define TASK_6_P           OSTCBP(6)   //  ""  #6
#define TASK_7_P           OSTCBP(7)   //  ""  #7
#define TASK_8_P           OSTCBP(8)   //  ""  #8

#define PRIO_8            10           // task priorities (tiny has no priority)
#define PRIO_7             8           //
#define PRIO_6             7           //
#define PRIO_5             6           //
#define PRIO_4             4           //
#define PRIO_3             3           //
#define PRIO_2             2           //
#define PRIO_1             1           // "" (highest)
