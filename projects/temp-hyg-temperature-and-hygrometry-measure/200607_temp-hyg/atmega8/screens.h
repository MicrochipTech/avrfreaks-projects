#ifndef SCREENS_H
#define SCREENS_H 1

#include "translations.h"

#define SCREEN_MENU 10
#define SCREEN_HOME 0
#define SCREEN_TIME_AND_DATE 1

#define SCREEN_MENU_LINES 2

static char PROGMEM screenMenu_0[] = TRANSLATION_HOME;
static char PROGMEM screenMenu_1[] = TRANSLATION_TIME_AND_DATE;

static PGM_P screenMenu[SCREEN_MENU_LINES] PROGMEM = {
    screenMenu_0,
    screenMenu_1,
};

#define SCREEN_GRAPH_DAY 0
#define SCREEN_GRAPH_MONTH 1
#define SCREEN_GRAPH_YEAR 1
#define SCREEN_GRAPH_ALL 1

extern uint8_t screen;

void screenTimeDisplay(void);
void screenSensorsDisplay(void);
void screenHomeDraw(void);
void screenMenuHandleButtons(void);
void screenMenuDraw(void);
void screenDateAndTimeDraw(void);
void screenDateAndTimeHandleButtons(void);
void screenGraphDraw(void);

#endif
