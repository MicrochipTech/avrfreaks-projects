/*
 * PositionSensors.h
 *
 * Created: 5/20/2017 4:39:34 PM
 *  Author: Big Daddy
 */ 


#ifndef POSITIONSENSORS_H_
#define POSITIONSENSORS_H_

// local or extern?
#ifdef _POSITIONSENSORS_
#define GLOBAL
#define _I(x) x
#else // end #ifdef _POSITIONSENSORS_
#define GLOBAL extern
#define _I(x)
#endif // end #ifndef _POSITIONSENSORS_

GLOBAL void PositionSensorInit(void);
GLOBAL U8 gn_PosDir[ _I(6) ];
GLOBAL S16 gn_PosCnt[ _I(6) ];

// clean up
#undef _I
#undef GLOBAL

#endif /* POSITIONSENSORS_H_ */