
    //---------------------------------------------------------------------------------------------------------//
    //     __ __  ___             ____  _____                                                                  //
    //    / // / /   |_   _______/ __ \/ ___/                                                                  //
    //   / // /_/ /| | | / / ___/ / / /\__ \                                                                   //
    //  /__  __/ ___ | |/ / /  / /_/ /___/ /                                                                   //
    //    /_/ /_/  |_|___/_/   \____//____/                                                                    //
    //    ____                    ____       _              _       _                                          //
    //   / ___|___   ___  _ __   / ___|  ___| |__   ___  __| |_   _| | ___ _ __                                //
    //  | |   / _ \ / _ \| '_ \  \___ \ / __| '_ \ / _ \/ _` | | | | |/ _ \ '__|                               //
    //  | |__| (_) | (_) | |_) |  ___) | (__| | | |  __/ (_| | |_| | |  __/ |                                  //
    //   \____\___/ \___/| .__/  |____/ \___|_| |_|\___|\__,_|\__,_|_|\___|_|                                  //
    //                   |_|                                                                                   //
    //     __                       __                                                                         //
    //    / /  __ __  ______ ______/ /__  ____ _                                                               //
    //   / _ \/ // / / __/ // / __/ __/ |/ /  ' \                                                              //
    //  /_.__/\_, /  \__/\_,_/_/  \__/|___/_/_/_/                                                              //
    //       /___/                                                                                             //
    //                                                                                                         //
    // Copyright (c) 2007,2008, Curt Van Maanen                                                                //
    //                                                                                                         //
    // Permission to use, copy, modify, and/or distribute this software for any                                //
    // purpose with or without fee is hereby granted, provided that the above                                  //
    // copyright notice and this permission notice appear in all copies.                                       //
    //                                                                                                         //
    // THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES                                //
    // WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF                                        //
    // MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR                                 //
    // ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES                                  //
    // WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN                                   //
    // ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF                                 //
    // OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.                                          //
    //                                                                                                         //
    // Version 2007.10.06                                                                                      //
    //---------------------------------------------------------------------------------------------------------//
    //  4AvrOS.h                                                                                               //
    //---------------------------------------------------------------------------------------------------------//



    #ifndef _4AVROS_H
    #define _4AVROS_H



    /*-----------------------------------------------------------------------------------------------------------
         ___  ___ ___ _      _   ___    _ _____ ___ ___  _  _ ___
        |   \| __/ __| |    /_\ | _ \  /_\_   _|_ _/ _ \| \| / __|
        | |) | _| (__| |__ / _ \|   / / _ \| |  | | (_) | .` \__ \
        |___/|___\___|____/_/ \_\_|_\/_/ \_\_| |___\___/|_|\_|___/

    -----------------------------------------------------------------------------------------------------------*/

    /*-----------------------------------------------------------------------------------------------------------
     semaOwner[] keeps track of sema task owner
    -----------------------------------------------------------------------------------------------------------*/

    #if (SEMAPHORES_ON)
    extern uint8_t semaOwner[];
    #endif

    /*-----------------------------------------------------------------------------------------------------------
     thisTaskN is the the current task number
    -----------------------------------------------------------------------------------------------------------*/

    extern uint8_t thisTaskN;



    /*-----------------------------------------------------------------------------------------------------------
         ___ _   _ _  _  ___ _____ ___ ___  _  _     ___ ___  ___ _____ ___ _______   _____ ___ ___
        | __| | | | \| |/ __|_   _|_ _/ _ \| \| |   | _ \ _ \/ _ \_   _/ _ \_   _\ \ / / _ \ __/ __|
        | _|| |_| | .` | (__  | |  | | (_) | .` |   |  _/   / (_) || || (_) || |  \ V /|  _/ _|\__ \
        |_|  \___/|_|\_|\___| |_| |___\___/|_|\_|   |_| |_|_\\___/ |_| \___/ |_|   |_| |_| |___|___/

    -----------------------------------------------------------------------------------------------------------*/

    /*-----------------------------------------------------------------------------------------------------------
     all os calls needed by tasks (used in os call define macros) (these are 'public' functions)
    -----------------------------------------------------------------------------------------------------------*/

    void    __save_goto     (uint8_t *gl);
    uint8_t __get_priority  (uint8_t tn);
    void    __set_priority  (uint8_t tn, uint8_t pri);
    void    __set_rtr       (uint8_t tn, uint8_t i);
    void    __clear_rtr     (uint8_t tn);
    void    __wait          (uint8_t nTicks);
    #if (SEMAPHORES_ON)
    uint8_t __getSema       (uint8_t n);
    void    __releaseSema   (uint8_t n);
    #endif



    /*-----------------------------------------------------------------------------------------------------------
         ___  ___ ___ ___ _  _ ___ ___
        |   \| __| __|_ _| \| | __/ __|
        | |) | _|| _| | || .` | _|\__ \
        |___/|___|_| |___|_|\_|___|___/

    -----------------------------------------------------------------------------------------------------------*/

    /*-----------------------------------------------------------------------------------------------------------
     generate goto label, need 2 'layers' because __LINE__ needs expanding first to line number
    -----------------------------------------------------------------------------------------------------------*/

    #define MY_CONCAT2(s,L)                 __CONCAT(s,L)
    #define MY_CONCAT(s,L)                   MY_CONCAT2(s,L)

    /*-----------------------------------------------------------------------------------------------------------
     macros for running tasks, put at beginning and end of tasks (functions)
     sets up function name and task loop, goto will cause a jump to appropriate resume position in task
    -----------------------------------------------------------------------------------------------------------*/

    #define BEGIN_TASK(name)                void (name)(uint8_t *gl){\
                                            if (gl) goto *gl;\
                                            for(;;) {

    #define END_TASK                        } }



    /*-----------------------------------------------------------------------------------------------------------
          ___  ___    ___   _   _    _      ___  ___ ___ ___ _  _ ___ ___
         / _ \/ __|  / __| /_\ | |  | |    |   \| __| __|_ _| \| | __/ __|
        | (_) \__ \ | (__ / _ \| |__| |__  | |) | _|| _| | || .` | _|\__ \
         \___/|___/  \___/_/ \_\____|____| |___/|___|_| |___|_|\_|___|___/

    -----------------------------------------------------------------------------------------------------------*/

    /*-----------------------------------------------------------------------------------------------------------
     suspend the task for ticks * ~10ms
     ready to run bit will be cleared, timer0 will set it ready to run when ticks gets to 0
    -----------------------------------------------------------------------------------------------------------*/

    #define osWAIT(ticks)                   { __save_goto(&&MY_CONCAT(L,__LINE__));\
                                            __wait(ticks);\
                                            return; }\
                                            MY_CONCAT(L,__LINE__):

    /*-----------------------------------------------------------------------------------------------------------
     wait while 'statement' is true, if true, wait ~10ms, then try again
    -----------------------------------------------------------------------------------------------------------*/

    #define osWAIT_WHILE(statement)         { __save_goto(&&MY_CONCAT(L,__LINE__));\
                                            MY_CONCAT(L,__LINE__):\
                                            if(statement) { __wait(1); return; } }

    /*-----------------------------------------------------------------------------------------------------------
     wait until 'statement' is true, if not true, wait ~10ms, then try again
    -----------------------------------------------------------------------------------------------------------*/

    #define osWAIT_UNTIL(statement)         { __save_goto(&&MY_CONCAT(L,__LINE__));\
                                            MY_CONCAT(L,__LINE__):\
                                            if(!(statement)) { __wait(1); return; } }

    /*-----------------------------------------------------------------------------------------------------------
     clear rtr of task, do statement, then yield, waiting for irq to set task rtr again
    -----------------------------------------------------------------------------------------------------------*/

    #define osWAIT_IRQ(statement)           { __clear_rtr(thisTaskN);\
                                            {statement}\
                                            __save_goto(&&MY_CONCAT(L,__LINE__));\
                                            return; }\
                                            MY_CONCAT(L,__LINE__):

    /*-----------------------------------------------------------------------------------------------------------
     check if any other higher priority tasks are ready to run, yield to them if so
     does not clear the ready to run bit, will return right away if it is the highest priority task
    -----------------------------------------------------------------------------------------------------------*/

    #define osYIELD                         { __save_goto(&&MY_CONCAT(L,__LINE__));\
                                            return; }\
                                            MY_CONCAT(L,__LINE__):

    /*-----------------------------------------------------------------------------------------------------------
     suspend the task, will only start again when another task or irq sets this task ready to run
    -----------------------------------------------------------------------------------------------------------*/

    #define osSUSPEND                       { __save_goto(&&MY_CONCAT(L,__LINE__));\
                                            __wait(0);\
                                            return; }\
                                            MY_CONCAT(L,__LINE__):

    /*-----------------------------------------------------------------------------------------------------------
     get priority number of a task
    -----------------------------------------------------------------------------------------------------------*/

    #define osGET_PRIORITY(taskname)        __get_priority(taskname ## NUMBER)

    /*-----------------------------------------------------------------------------------------------------------
     set priority of a task
    -----------------------------------------------------------------------------------------------------------*/

    #define osSET_PRIORITY(taskname,pri)    __set_priority(taskname ## NUMBER,pri)

    /*-----------------------------------------------------------------------------------------------------------
     clear the ready to run status of a task, scheduler does not run (ticks is also cleared)
     taskname is the name of the task, the macro will expand to get the number
    -----------------------------------------------------------------------------------------------------------*/

    #define osCLEAR_RTR(taskname)           __clear_rtr(taskname ## NUMBER)
    #define osDISABLE_TASK(taskname)        __clear_rtr(taskname ## NUMBER)

    /*-----------------------------------------------------------------------------------------------------------
     get ownership of a semaphore, if available, otherwise suspend until it becomes available
    -----------------------------------------------------------------------------------------------------------*/

    #if (SEMAPHORES_ON)
    #define osGET_SEMA(semaNum)             { __save_goto(&&MY_CONCAT(L,__LINE__));\
                                            if(!(__getSema(semaNum))) return;}\
                                            MY_CONCAT(L,__LINE__):
    #endif

    /*-----------------------------------------------------------------------------------------------------------
     release ownership of a semaphore, check if other tasks want it
    -----------------------------------------------------------------------------------------------------------*/

    #if (SEMAPHORES_ON)
    #define osRELEASE_SEMA(semaNum)         __releaseSema(semaNum);
    #endif

    /*-----------------------------------------------------------------------------------------------------------
     if a task owns a semaphore, only the these two commands will enable the task
    -----------------------------------------------------------------------------------------------------------*/

    #if (SEMAPHORES_ON)
    #define osSET_SEMA_OWNER_RTR(semaNum)   __set_rtr(semaOwner[semaNum],1)
    #define osENABLE_SEMA_OWNER(semaNum)    __set_rtr(semaOwner[semaNum],1)
    #endif

    /*-----------------------------------------------------------------------------------------------------------
     set the ready to run status of a task, scheduler does not run (ticks is also cleared)
     taskname is the name of the task, the macro will expand to get the number
     if task owns a semaphore, it will not be enabled (use osENABLE_SEMA_OWNER instead)
    -----------------------------------------------------------------------------------------------------------*/

    #if (SEMAPHORES_ON)
    #define osSET_RTR(taskname)             __set_rtr(taskname ## NUMBER,nSemas)
    #define osENABLE_TASK(taskname)         __set_rtr(taskname ## NUMBER,nSemas)
    #else
    #define osSET_RTR(taskname)             __set_rtr(taskname ## NUMBER,1)
    #define osENABLE_TASK(taskname)         __set_rtr(taskname ## NUMBER,1)
    #endif

    /*---------------------------------------------------------------------------------------------------------*/

    #endif // _4AVROS_H


