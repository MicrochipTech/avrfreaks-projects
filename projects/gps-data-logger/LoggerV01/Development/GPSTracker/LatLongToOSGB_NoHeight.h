#ifndef LATOSGB_INCLUDE
#define LATOSGB_INCLUDE

#define TooFarSouth  1
#define TooFarNorth 2
#define TooFarWest  3
#define TooFarEast  4

//void LLtoOSGBPrep(void);

uint8_t LLtoOSGB(const double Lat, const double Long);

extern long OSGBNorthing;
extern long OSGBEasting;
extern char OSGBGridSquare[];


#endif //LATOSGB_INCLUDE