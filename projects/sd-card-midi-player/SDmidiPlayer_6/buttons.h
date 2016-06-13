#ifndef _buttons_h_
#define _buttons_h_

#define buttonMask (_BV(PINC0)|_BV(PINC1)|_BV(PINC2))
#define noButton   ( ( PINC & buttonMask ) == buttonMask )
#define button0    ( ( PINC & _BV(PINC0) ) == 0 )
#define button1    ( ( PINC & _BV(PINC1) ) == 0 )
#define button2    ( ( PINC & _BV(PINC2) ) == 0 )

#endif