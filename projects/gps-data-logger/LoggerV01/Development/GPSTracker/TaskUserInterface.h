#ifndef INCLUDE_TaskUserInterface
#define INCLUDE_TaskUserInterface


#include "ReceiverIO.h"

extern GPSLocation UILoc;

#define ScreenRefreshPeriod     500  //ms

void UIDisplayUpdate(const GPSLocation* loc);

#endif //INCLUDE_TaskUserInterface