#include "projectCommon.h"
#include "LatLongToOSGB_NoHeight.h"
#include <avr\pgmspace.h>

#include <math.h>


#define FOURTHPI (M_PI / 4)
#define deg2rad  (M_PI / 180)
#define rad2deg  (180.0 / M_PI)


#define k0 			   0.9996012717
#define aC 			   6377563.396 //Airy
#define LongOrigin     -2
#define LongOriginRad  (LongOrigin * deg2rad)


long OSGBNorthing=0;
long OSGBEasting=0;
char OSGBGridSquare[3]="  ";


#define eccSquared       0.0066705397615973
#define M0 	     		 5429228.5969543
#define eccPrimeSquared  0.0067153346685161

static char PROGMEM GridSquare[] ="VWXYZQRSTULMNOPFGHJKABCDE";





//-----------------------------------------------------------------------------------
	
static uint8_t InputValidation(const double Lat, const double Long)
{
	// some validation
	if (Lat < 30.0)   return TooFarSouth;
	if (Lat > 70.0)   return TooFarNorth;
	if (Long < -10.0) return TooFarWest;
	if (Long > 3.0)   return TooFarEast;
	
	return 0;
}

//-----------------------------------------------------------------------------------


static uint8_t ValidateOSGB(void)
{
	// some validation
	if (OSGBNorthing < 0.0)			return TooFarSouth;
	if (OSGBNorthing > 1300000.0)	return TooFarNorth;
	if (OSGBEasting  < 0.0)			return TooFarWest;
	if (OSGBEasting  > 700000.0)	return TooFarEast;
	
	return 0;
}

//-----------------------------------------------------------------------------------


static void CoordsToOSGBSquare(const double easting, const double northing)
{
	
	int32_t posx, posy; //positions in grid


	OSGBEasting = (long)(easting  + 0.5); //round to nearest int
	OSGBNorthing =(long)(northing + 0.5); //round to nearest int

	//find correct 500km square
	
	posx = OSGBEasting  / 500000L;
	posy = OSGBNorthing / 500000L;
	
	OSGBGridSquare[0] = pgm_read_byte_near(&GridSquare[posx + posy * 5 + 7]);

	//find correct 100km square
	
	posx = OSGBEasting  % 500000L;	//remove 500 km square
	posy = OSGBNorthing % 500000L;	//remove 500 km square
	posx = posx / 100000L;			//find 100 km square
	posy = posy / 100000L;			//find 100 km square
	
	OSGBGridSquare[1] = pgm_read_byte_near(&GridSquare[posx + posy * 5]);

	OSGBGridSquare[2] = '\0';		//terminate grid ref string with null

	//remainder is northing and easting
	
	OSGBNorthing = OSGBNorthing % 500000L; 
	OSGBNorthing = OSGBNorthing % 100000L;

	OSGBEasting = OSGBEasting % 500000L;
	OSGBEasting = OSGBEasting % 100000L;
}

//------------------------------------------------------------------------------------------
// the following code generates the constants. This only needs to be run to get the 
// constants. Once they are found, the code can be altered to contain the values
// explicitly. This could be done on a spreadsheet, the code is here for completeness
/*
void LLtoOSGBPrep(void)
{


	const double LatOrigin = 49;
	const double LatOriginRad = LatOrigin * deg2rad;


	const double majoraxis = 6377563.396;//Airy
	const double minoraxis = 6356256.91;//Airy
	const double a = 6377563.396;		//Airy

	eccSquared = (majoraxis * majoraxis - minoraxis * minoraxis) / (majoraxis * majoraxis);

	//only calculate M0 once since it is based on the origin 
	//of the OSGB projection, which is fixed
	
	M0 = a*((1		- eccSquared/4 - 3*eccSquared*eccSquared/64	- 5*eccSquared*eccSquared*eccSquared/256)*LatOriginRad 
				- (3*eccSquared/8	+ 3*eccSquared*eccSquared/32	+ 45*eccSquared*eccSquared*eccSquared/1024)*sin(2*LatOriginRad)
									+ (15*eccSquared*eccSquared/256 + 45*eccSquared*eccSquared*eccSquared/1024)*sin(4*LatOriginRad) 
									- (35*eccSquared*eccSquared*eccSquared/3072)*sin(6*LatOriginRad));

	eccPrimeSquared = (eccSquared)/(1-eccSquared);


//these values are const and are...

//eccSquared =0.0066705397615973 //ecc
//M0=5429228.5969543 	//M0
//eccPrimeSquared =0.0067153346685161 //eccPrime
}
*/

//------------------------------------------------------------------------------------------

uint8_t LLtoOSGB(const double Lat, const double Long)
{
	//converts lat/long to OSGB coords.  Equations from USGS Bulletin 1532 
	//East Longitudes are positive, West longitudes are negative. 
	//North latitudes are positive, South latitudes are negative
	//Lat and Long are in decimal degrees
	//Written by Chuck Gantz- chuck.gantz@globalstar.com

	uint8_t error =	InputValidation(Lat,Long);
	if (error)
		return error;
		
		
	double N, T, C, A, M;
	
	double LatRad = Lat*deg2rad;
	double LongRad = Long*deg2rad;
	double easting, northing;
		


	N = aC/sqrt(1-eccSquared*sin(LatRad)*sin(LatRad));
	T = tan(LatRad)*tan(LatRad);
	C = eccPrimeSquared*cos(LatRad)*cos(LatRad);
	A = cos(LatRad)*(LongRad-LongOriginRad);


	
	M = aC*((1	- eccSquared/4		- 3*eccSquared*eccSquared/64	- 5*eccSquared*eccSquared*eccSquared/256)*LatRad 
				- (3*eccSquared/8	+ 3*eccSquared*eccSquared/32	+ 45*eccSquared*eccSquared*eccSquared/1024)*sin(2*LatRad)
									+ (15*eccSquared*eccSquared/256 + 45*eccSquared*eccSquared*eccSquared/1024)*sin(4*LatRad) 
									- (35*eccSquared*eccSquared*eccSquared/3072)*sin(6*LatRad));
	
	easting = (double)(k0*N*(A+(1-T+C)*A*A*A/6
					+ (5-18*T+T*T+72*C-58*eccPrimeSquared)*A*A*A*A*A/120));
/*
	easting = (double)(k0*N*(A+(1-T+C)*pow(A,3)/6
					+ (5-18*pow(T,3)+72*C-58*eccPrimeSquared)*pow(A,5)/120));
*/
	easting += 400000.0; //false easting

	northing = (double)(k0*(M-M0+N*tan(LatRad)*(A*A/2+(5-T+9*C+4*C*C)*A*A*A*A/24
				 + (61-58*T+T*T+600*C-330*eccPrimeSquared)*A*A*A*A*A*A/720)));
/*
	northing = (double)(k0*(M-M0+N*tan(LatRad)*(A*A/2+(5-T+9*C+4*C*C)*pow(A,4)/24
				 + (61-58*pow(T,3)+600*C-330*eccPrimeSquared)*pow(A,6)/720)));
*/
	northing -= 100000.0;//false northing

	error =	ValidateOSGB();
	if (error)
		return error;
	
	CoordsToOSGBSquare(easting, northing);
	
	return 0;		//ok

}

