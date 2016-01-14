# Soft timers

Uploaded by demiurg1978 on 2016-01-13 19:44:00 (rating 5 out of 5)

## Summary

      soft\_timers.h:


 



```

//========================================================================
#ifndef SOFT\_TIMERS\_H

#define SOFT\_TIMERS\_H

#include "soft\_timers.h"

#include "main\_def\_func.h"
//========================================================================

//========================================================================
// Выбор: с прерыванием или с опросом флага OCFx.
#define \_\_ST\_NO\_INTERRUPT\_\_ // \_\_ST\_NO\_INTERRUPT\_\_ // \_\_ST\_INTERRUPT\_\_

// Состав структуры: bool status, u08 sys\_tick\_prev, u16 cnt, u16 period.
// Итого 6 байтов на один программный таймер. Если убрать элемент period,
// то теперь будет 4 байта на таймер.
#define \_\_ST\_NO\_PERIOD\_\_ // \_\_ST\_PERIOD\_\_ // \_\_ST\_NO\_PERIOD\_\_
//========================================================================

//========================================================================
#define ST\_TCNT         TCNT0
#define ST\_TIMSK        TIMSK0
#define ST\_OCIE         OCIE0A
#define ST\_OCR          OCR0A
#define ST\_TCCR         TCCR0B
#define ST\_CS0          CS00
#define ST\_CS1          CS01
#define ST\_CS2          CS02

#define ST\_TIFR         TIFR0
#define ST\_OCF          OCF0A
//========================================================================

//========================================================================
#define SYS\_TICK        (F\_CPU/64/1000)
//========================================================================

//========================================================================
typedef struct soft\_timer
{
   bool status;
   u08 sys\_tick\_prev;
   u16 cnt;       // Counter.

#ifdef \_\_ST\_PERIOD\_\_
   u16 period;    // Period.
#endif
} soft\_timer;
//========================================================================

//========================================================================
void init\_soft\_timers (void);

#define handle\_soft\_timer(a) \_handle\_soft\_timer(&a)
bool \_handle\_soft\_timer(soft\_timer *ptr\_timer);

#ifdef \_\_ST\_PERIOD\_\_
#define set\_soft\_timer(a,b,c) \_set\_soft\_timer (&a, b, c)
void \_set\_soft\_timer (soft\_timer *ptr\_timer, u16 time, u16 period);
#endif

#ifdef \_\_ST\_NO\_PERIOD\_\_
#define set\_soft\_timer(a,b) \_set\_soft\_timer (&a, b)
void \_set\_soft\_timer (soft\_timer *ptr\_timer, u16 time);
#endif

#define reset\_soft\_timer(a) \_reset\_soft\_timer (&a)
void \_reset\_soft\_timer (soft\_timer *ptr\_timer);

void proc\_sys\_tick (void);
//========================================================================

#endif

```
 


soft\_timer.c:


 



```

//========================================================================
// Реализация программных таймеров.
// Примечание. Важно!!!!!!!!!!!!!!!
// При смене состояния конечного автомата нужно сбросить
// все таймеры, которые используются в этом состоянии. Иначе может быть
// следующая ситуация: если таймер уже использовался в
// текущем состоянии и флаг EN установлен, то по окончании времени
// выполнится соответствующий код.
//--------------------- Пример использования -----------------------------
// set\_soft\_timer (ST\_LED\_2\_BLINK, 50, 50); // Установка таймера.
// if (handle\_soft\_timer (ST\_LED\_2\_BLINK))  // Проверка таймера.
//========================================================================

//========================================================================
#include "soft\_timers.h"
//========================================================================

//========================================================================
static u08 sys\_tick;
//========================================================================

//========================================================================
#ifdef \_\_ST\_INTERRUPT\_\_
#pragma vector = TIMER2\_COMP\_vect
\_\_interrupt void SysTimerComp (void)
{
   ST\_OCR += SYS\_TICK;
   sys\_tick++;
}
#endif
//========================================================================

//========================================================================
void init\_soft\_timers (void)
{
   sys\_tick = 0;
   ST\_TCNT = 0;
   ST\_OCR = SYS\_TICK;
   ST\_TCCR = ((1<<ST\_CS1) | (1<<ST\_CS0));

#ifdef \_\_ST\_INTERRUPT\_\_
   set\_bit (ST\_TIMSK, ST\_OCIE);
#endif
}
//------------------------------------------------------------------------
#ifdef \_\_ST\_PERIOD\_\_
void \_set\_soft\_timer (soft\_timer *ptr\_timer, u16 time, u16 period)
#else
void \_set\_soft\_timer (soft\_timer *ptr\_timer, u16 time)
#endif
{
   ptr\_timer -> status = true;

   if (time == 0)
      ptr\_timer -> sys\_tick\_prev = ~sys\_tick;
   else
      ptr\_timer -> sys\_tick\_prev = sys\_tick;

   ptr\_timer -> cnt = time;

#ifdef \_\_ST\_PERIOD\_\_
   ptr\_timer -> period = period;
#endif
}
//------------------------------------------------------------------------
bool \_handle\_soft\_timer (soft\_timer *ptr\_timer)
{
   if (ptr\_timer -> status)
   {
      if (ptr\_timer -> sys\_tick\_prev != sys\_tick)
      {
         ptr\_timer -> sys\_tick\_prev = sys\_tick;

         if (ptr\_timer -> cnt == 0)
         {
            #ifdef \_\_ST\_PERIOD\_\_
            if (ptr\_timer -> period != 0)
               ptr\_timer -> cnt = ptr\_timer -> period;
            #endif

            return true;
         }
         else
         {
            ptr\_timer -> cnt--;

            if (ptr\_timer -> cnt == 0)
            {
               #ifdef \_\_ST\_PERIOD\_\_
               if (ptr\_timer -> period == 0)
                  ptr\_timer -> status = false;
               else
                  ptr\_timer -> cnt = ptr\_timer -> period;
               #else
               ptr\_timer -> status = false;
               #endif

               return true;
            }
         }
      }
   }

   return false;
}
//------------------------------------------------------------------------
void \_reset\_soft\_timer (soft\_timer *ptr\_timer)
{
   ptr\_timer -> status = false;
   ptr\_timer -> sys\_tick\_prev = 0;
   ptr\_timer -> cnt = 0;

#ifdef \_\_ST\_PERIOD\_\_
   ptr\_timer -> period = 0;
#endif
}
//------------------------------------------------------------------------
#ifdef \_\_ST\_NO\_INTERRUPT\_\_
void proc\_sys\_tick (void)
{
   static u08 \_proc\_sys\_tick;

   switch (\_proc\_sys\_tick)
   {
      case 0:
         init\_soft\_timers ();
         \_proc\_sys\_tick = 1;
         break;

      case 1:
         if (ST\_TIFR & (1<<ST\_OCF))
         {
            ST\_TIFR = (1<<ST\_OCF);
            ST\_OCR += SYS\_TICK;
            sys\_tick++;
         }
         break;
   }
}
#endif
//========================================================================

```
 



```

//************************************************************************
// Example of the using
//************************************************************************

static u08 \_proc\_led\_2;

soft\_timer ST\_LED\_2\_BLINK;

// Immediate execution
case 0:
   set\_soft\_timer (ST\_LED\_2\_BLINK, 0, 50);
   \_proc\_led\_2 = 1;
   break;

case 1:
   if (handle\_soft\_timer (ST\_LED\_2\_BLINK))
   {

   }
   break;

// Execution after the specified time
case 0:
   set\_soft\_timer (ST\_LED\_2\_BLINK, 50, 50);
  \_proc\_led\_2 = 1;
   break;

case 1:
   if (handle\_soft\_timer (ST\_LED\_2\_BLINK))
   {

   }
   break;

// Other
case 0:
   set\_soft\_timer (ST\_LED\_2\_BLINK, 1000, 50);
  \_proc\_led\_2 = 1;
   break;

case 1:
   if (handle\_soft\_timer (ST\_LED\_2\_BLINK))
   {

   }
   break;
```
 



```

//========================================================================
#include "proc\_device.h"
//========================================================================

//========================================================================
void proc\_device (void)
{
   static u08 \_proc\_device;

   switch (\_proc\_device)
   {
      case 0: // Блок инициализации.
         #ifdef \_\_ST\_PERIOD\_\_
         set\_proc\_led\_1\_run ();
         set\_proc\_led\_2\_run ();
         set\_proc\_led\_3\_run ();
         set\_proc\_led\_4\_run ();
         set\_proc\_led\_5\_run ();
         set\_proc\_led\_6\_run ();
         set\_proc\_led\_7\_run ();
         set\_proc\_led\_8\_run ();
         #endif

         \_proc\_device = 1;
         break;

      case 1:
         #ifdef \_\_ST\_PERIOD\_\_
         proc\_led\_1 ();
         proc\_led\_2 ();
         proc\_led\_3 ();
         proc\_led\_4 ();
         proc\_led\_5 ();
         proc\_led\_6 ();
         proc\_led\_7 ();
         proc\_led\_8 ();
         #else
         led\_1\_blink ();
         led\_2\_blink ();
         led\_3\_blink ();
         led\_4\_blink ();
         led\_5\_blink ();
         led\_6\_blink ();
         led\_7\_blink ();
         led\_8\_blink ();
         #endif
         break;
   }
}
//========================================================================

//========================================================================
static u08 \_proc\_led\_1;

static soft\_timer ST\_PROC\_LED\_1;
static soft\_timer ST\_PROC\_LED\_2;
static soft\_timer ST\_PROC\_LED\_3;
static soft\_timer ST\_PROC\_LED\_4;
static soft\_timer ST\_PROC\_LED\_5;
static soft\_timer ST\_PROC\_LED\_6;
static soft\_timer ST\_PROC\_LED\_7;
static soft\_timer ST\_PROC\_LED\_8;

void set\_proc\_led\_1\_run (void)
{
   led\_1\_off ();
   \_proc\_led\_1 = 1;
}

void set\_proc\_led\_1\_off (void)
{
   led\_1\_off ();
   \_proc\_led\_1 = 0;
}

void proc\_led\_1 (void)
{
   switch (\_proc\_led\_1)
   {
      case 0:
         break; // Idle.

      case 1:
         #ifdef \_\_ST\_PERIOD\_\_
         set\_soft\_timer (ST\_PROC\_LED\_1, 0, 10);
         #else
         set\_soft\_timer (ST\_PROC\_LED\_1, 0); // !!!!!!!!!!!!!!!!!!!!!!!!!!!
         #endif
         \_proc\_led\_1 = 2;
         break;

      case 2:
         if (handle\_soft\_timer (ST\_PROC\_LED\_1))
         {
            if (!(LED\_1\_DDR & (1<<LED\_1)))
               led\_1\_on ();
            else
               led\_1\_off ();
         }
         break;
   }
}
//========================================================================

//========================================================================
static u08 \_proc\_led\_2;

void set\_proc\_led\_2\_run (void)
{
   led\_2\_off ();
   \_proc\_led\_2 = 1;
}

void set\_proc\_led\_2\_off (void)
{
   led\_2\_off ();
   \_proc\_led\_2 = 0;
}

void proc\_led\_2 (void)
{
   switch (\_proc\_led\_2)
   {
      case 0:
         break; // Idle.

      case 1:
//         set\_soft\_timer (ST\_PROC\_LED\_2, 0, 10);
         set\_soft\_timer (ST\_PROC\_LED\_2, 0); // !!!!!!!!!!!!!!!!!!!!!!!!!!!
         \_proc\_led\_2 = 2;
         break;

      case 2:
         if (handle\_soft\_timer (ST\_PROC\_LED\_2))
         {
            if (!(LED\_2\_DDR & (1<<LED\_2)))
               led\_2\_on ();
            else
               led\_2\_off ();
         }
         break;
   }
}
//========================================================================

//========================================================================
static u08 \_proc\_led\_3;

void set\_proc\_led\_3\_run (void)
{
   led\_3\_off ();
   \_proc\_led\_3 = 1;
}

void set\_proc\_led\_3\_off (void)
{
   led\_3\_off ();
   \_proc\_led\_3 = 0;
}

void proc\_led\_3 (void)
{
   switch (\_proc\_led\_3)
   {
      case 0:
         break; // Idle.

      case 1:
//         set\_soft\_timer (ST\_PROC\_LED\_3, 0, 10);
         set\_soft\_timer (ST\_PROC\_LED\_3, 0); // !!!!!!!!!!!!!!!!!!!!!!!!!!!
         \_proc\_led\_3 = 2;
         break;

      case 2:
         if (handle\_soft\_timer (ST\_PROC\_LED\_3))
         {
            if (!(LED\_3\_DDR & (1<<LED\_3)))
               led\_3\_on ();
            else
               led\_3\_off ();
         }
         break;
   }
}
//========================================================================

//========================================================================
static u08 \_proc\_led\_4;

void set\_proc\_led\_4\_run (void)
{
   led\_4\_off ();
   \_proc\_led\_4 = 1;
}

void set\_proc\_led\_4\_off (void)
{
   led\_4\_off ();
   \_proc\_led\_4 = 0;
}

void proc\_led\_4 (void)
{
   switch (\_proc\_led\_4)
   {
      case 0:
         break; // Idle.

      case 1:
//         set\_soft\_timer (ST\_PROC\_LED\_4, 0, 10);
         set\_soft\_timer (ST\_PROC\_LED\_4, 0); // !!!!!!!!!!!!!!!!!!!!!!!!!!!
         \_proc\_led\_4 = 2;
         break;

      case 2:
         if (handle\_soft\_timer (ST\_PROC\_LED\_4))
         {
            if (!(LED\_4\_DDR & (1<<LED\_4)))
               led\_4\_on ();
            else
               led\_4\_off ();
         }
         break;
   }
}
//========================================================================

//========================================================================
static u08 \_proc\_led\_5;

void set\_proc\_led\_5\_run (void)
{
   led\_5\_off ();
   \_proc\_led\_5 = 1;
}

void set\_proc\_led\_5\_off (void)
{
   led\_5\_off ();
   \_proc\_led\_5 = 0;
}

void proc\_led\_5 (void)
{
   switch (\_proc\_led\_5)
   {
      case 0:
         break; // Idle.

      case 1:
//         set\_soft\_timer (ST\_PROC\_LED\_5, 0, 10);
         set\_soft\_timer (ST\_PROC\_LED\_5, 0); // !!!!!!!!!!!!!!!!!!!!!!!!!!!
         \_proc\_led\_5 = 2;
         break;

      case 2:
         if (handle\_soft\_timer (ST\_PROC\_LED\_5))
         {
            if (!(LED\_5\_DDR & (1<<LED\_5)))
               led\_5\_on ();
            else
               led\_5\_off ();
         }
         break;
   }
}
//========================================================================

//========================================================================
static u08 \_proc\_led\_6;

void set\_proc\_led\_6\_run (void)
{
   led\_6\_off ();
   \_proc\_led\_6 = 1;
}

void set\_proc\_led\_6\_off (void)
{
   led\_6\_off ();
   \_proc\_led\_6 = 0;
}

void proc\_led\_6 (void)
{
   switch (\_proc\_led\_6)
   {
      case 0:
         break; // Idle.

      case 1:
//         set\_soft\_timer (ST\_PROC\_LED\_6, 0, 10);
         set\_soft\_timer (ST\_PROC\_LED\_6, 0); // !!!!!!!!!!!!!!!!!!!!!!!!!!!
         \_proc\_led\_6 = 2;
         break;

      case 2:
         if (handle\_soft\_timer (ST\_PROC\_LED\_6))
         {
            if (!(LED\_6\_DDR & (1<<LED\_6)))
               led\_6\_on ();
            else
               led\_6\_off ();
         }
         break;
   }
}
//========================================================================

//========================================================================
static u08 \_proc\_led\_7;

void set\_proc\_led\_7\_run (void)
{
   led\_7\_off ();
   \_proc\_led\_7 = 1;
}

void set\_proc\_led\_7\_off (void)
{
   led\_7\_off ();
   \_proc\_led\_7 = 0;
}

void proc\_led\_7 (void)
{
   switch (\_proc\_led\_7)
   {
      case 0:
         break; // Idle.

      case 1:
//         set\_soft\_timer (ST\_PROC\_LED\_7, 0, 10);
         set\_soft\_timer (ST\_PROC\_LED\_7, 0); // !!!!!!!!!!!!!!!!!!!!!!!!!!!
         \_proc\_led\_7 = 2;
         break;

      case 2:
         if (handle\_soft\_timer (ST\_PROC\_LED\_7))
         {
            if (!(LED\_7\_DDR & (1<<LED\_7)))
               led\_7\_on ();
            else
               led\_7\_off ();
         }
         break;
   }
}
//========================================================================

//========================================================================
static u08 \_proc\_led\_8;

void set\_proc\_led\_8\_run (void)
{
   led\_8\_off ();
   \_proc\_led\_8 = 1;
}

void set\_proc\_led\_8\_off (void)
{
   led\_8\_off ();
   \_proc\_led\_8 = 0;
}

void proc\_led\_8 (void)
{
   switch (\_proc\_led\_8)
   {
      case 0:
         break; // Idle.

      case 1:
//         set\_soft\_timer (ST\_PROC\_LED\_8, 0, 10);
         set\_soft\_timer (ST\_PROC\_LED\_8, 0); // !!!!!!!!!!!!!!!!!!!!!!!!!!!
         \_proc\_led\_8 = 2;
         break;

      case 2:
         if (handle\_soft\_timer (ST\_PROC\_LED\_8))
         {
            if (!(LED\_8\_DDR & (1<<LED\_8)))
               led\_8\_on ();
            else
               led\_8\_off ();
         }
         break;
   }
}
//========================================================================

//========================================================================
void led\_1\_blink (void)
{
   if (!(LED\_1\_DDR & (1<<LED\_1)))
   {
      set\_bit (LED\_1\_DDR, LED\_1);

      #ifdef \_\_ST\_PERIOD\_\_
      set\_soft\_timer (ST\_PROC\_LED\_1, 0, 10);
      #else
      set\_soft\_timer (ST\_PROC\_LED\_1, 0);
      #endif
   }

   if (handle\_soft\_timer (ST\_PROC\_LED\_1))
   {
      if (!(LED\_1\_PORT & (1<<LED\_1)))
         led\_1\_on ();
      else
         led\_1\_off ();

      #ifdef \_\_ST\_NO\_PERIOD\_\_
      set\_soft\_timer (ST\_PROC\_LED\_1, 10);
      #endif
   }
}
//------------------------------------------------------------------------
void led\_2\_blink (void)
{
   if (!(LED\_2\_DDR & (1<<LED\_2)))
   {
      set\_bit (LED\_2\_DDR, LED\_2);

      #ifdef \_\_ST\_PERIOD\_\_
      set\_soft\_timer (ST\_PROC\_LED\_2, 0, 10);
      #else
      set\_soft\_timer (ST\_PROC\_LED\_2, 0);
      #endif
   }

   if (handle\_soft\_timer (ST\_PROC\_LED\_2))
   {
      if (!(LED\_2\_PORT & (1<<LED\_2)))
         led\_2\_on ();
      else
         led\_2\_off ();

      #ifdef \_\_ST\_NO\_PERIOD\_\_
      set\_soft\_timer (ST\_PROC\_LED\_2, 10);
      #endif
   }
}
//------------------------------------------------------------------------
void led\_3\_blink (void)
{
   if (!(LED\_3\_DDR & (1<<LED\_3)))
   {
      set\_bit (LED\_3\_DDR, LED\_3);

      #ifdef \_\_ST\_PERIOD\_\_
      set\_soft\_timer (ST\_PROC\_LED\_3, 0, 10);
      #else
      set\_soft\_timer (ST\_PROC\_LED\_3, 0);
      #endif
   }

   if (handle\_soft\_timer (ST\_PROC\_LED\_3))
   {
      if (!(LED\_3\_PORT & (1<<LED\_3)))
         led\_3\_on ();
      else
         led\_3\_off ();

      #ifdef \_\_ST\_NO\_PERIOD\_\_
      set\_soft\_timer (ST\_PROC\_LED\_3, 10);
      #endif
   }
}
//------------------------------------------------------------------------
void led\_4\_blink (void)
{
   if (!(LED\_4\_DDR & (1<<LED\_4)))
   {
      set\_bit (LED\_4\_DDR, LED\_4);

      #ifdef \_\_ST\_PERIOD\_\_
      set\_soft\_timer (ST\_PROC\_LED\_4, 0, 10);
      #else
      set\_soft\_timer (ST\_PROC\_LED\_4, 0);
      #endif
   }

   if (handle\_soft\_timer (ST\_PROC\_LED\_4))
   {
      if (!(LED\_4\_PORT & (1<<LED\_4)))
         led\_4\_on ();
      else
         led\_4\_off ();

      #ifdef \_\_ST\_NO\_PERIOD\_\_
      set\_soft\_timer (ST\_PROC\_LED\_4, 10);
      #endif
   }
}
//------------------------------------------------------------------------
void led\_5\_blink (void)
{
   if (!(LED\_5\_DDR & (1<<LED\_5)))
   {
      set\_bit (LED\_5\_DDR, LED\_5);

      #ifdef \_\_ST\_PERIOD\_\_
      set\_soft\_timer (ST\_PROC\_LED\_5, 0, 10);
      #else
      set\_soft\_timer (ST\_PROC\_LED\_5, 0);
      #endif
   }

   if (handle\_soft\_timer (ST\_PROC\_LED\_5))
   {
      if (!(LED\_5\_PORT & (1<<LED\_5)))
         led\_5\_on ();
      else
         led\_5\_off ();

      #ifdef \_\_ST\_NO\_PERIOD\_\_
      set\_soft\_timer (ST\_PROC\_LED\_5, 10);
      #endif
   }
}
//------------------------------------------------------------------------
void led\_6\_blink (void)
{
   if (!(LED\_6\_DDR & (1<<LED\_6)))
   {
      set\_bit (LED\_6\_DDR, LED\_6);

      #ifdef \_\_ST\_PERIOD\_\_
      set\_soft\_timer (ST\_PROC\_LED\_6, 0, 10);
      #else
      set\_soft\_timer (ST\_PROC\_LED\_6, 0);
      #endif
   }

   if (handle\_soft\_timer (ST\_PROC\_LED\_6))
   {
      if (!(LED\_6\_PORT & (1<<LED\_6)))
         led\_6\_on ();
      else
         led\_6\_off ();

      #ifdef \_\_ST\_NO\_PERIOD\_\_
      set\_soft\_timer (ST\_PROC\_LED\_6, 10);
      #endif
   }
}
//------------------------------------------------------------------------
void led\_7\_blink (void)
{
   if (!(LED\_7\_DDR & (1<<LED\_7)))
   {
      set\_bit (LED\_7\_DDR, LED\_7);

      #ifdef \_\_ST\_PERIOD\_\_
      set\_soft\_timer (ST\_PROC\_LED\_7, 0, 10);
      #else
      set\_soft\_timer (ST\_PROC\_LED\_7, 0);
      #endif
   }

   if (handle\_soft\_timer (ST\_PROC\_LED\_7))
   {
      if (!(LED\_7\_PORT & (1<<LED\_7)))
         led\_7\_on ();
      else
         led\_7\_off ();

      #ifdef \_\_ST\_NO\_PERIOD\_\_
      set\_soft\_timer (ST\_PROC\_LED\_7, 10);
      #endif
   }
}
//------------------------------------------------------------------------
void led\_8\_blink (void)
{
   if (!(LED\_8\_DDR & (1<<LED\_8)))
   {
      set\_bit (LED\_8\_DDR, LED\_8);

      #ifdef \_\_ST\_PERIOD\_\_
      set\_soft\_timer (ST\_PROC\_LED\_8, 0, 10);
      #else
      set\_soft\_timer (ST\_PROC\_LED\_8, 0);
      #endif
   }

   if (handle\_soft\_timer (ST\_PROC\_LED\_8))
   {
      if (!(LED\_8\_PORT & (1<<LED\_8)))
         led\_8\_on ();
      else
         led\_8\_off ();

      #ifdef \_\_ST\_NO\_PERIOD\_\_
      set\_soft\_timer (ST\_PROC\_LED\_8, 10);
      #endif
   }
}
//========================================================================
```

## Compilers

- IAR Embedded Workbench for AVR

## Tags

- General Lib.functions
- IAR Embedded Workbench for AVR
