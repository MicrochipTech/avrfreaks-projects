/************************************************************
Salvo tiny salvoliccatmega-y  OSL  OSY   tiny(no timeout, unlimited task/event, no priority)
Salvo free salvoficcatmega-t  OSF  OST   freeware (3 task / 7 event max)
************************************************************/

#define OSUSE_LIBRARY       TRUE
#define OSLIBRARY_TYPE      OSL
#define OSLIBRARY_CONFIG    OSY

#define OSEVENTS            2
#define OSEVENT_FLAGS       0
#define OSMESSAGE_QUEUES    0
#define OSTASKS             8
