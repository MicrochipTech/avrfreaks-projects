#include "stdafx.h"

#include <stdio.h>
#include <math.h>
#include <iostream.h>


 #define scalechange  (20.4894 / 1000000.0)
 #define rz  (-0.8421)
 #define ry  (-0.247)
 #define rx  (-0.1502)

 #define r0c0   (1 + scalechange) 
 #define X0c1   (-1 * rz * 0.000004848136811) 
 #define r0c2   (ry * 0.000004848136811) 

 #define r1c0   (rz * 0.000004848136811) 
 #define r1c1   r0c0  
 #define r1c2   (-1 * rx * 0.000004848136811) 

 #define r2c0   (-1 * ry * 0.000004848136811) 
 #define r2c1   (rx * 0.000004848136811) 
 #define r2c2  r0c0

 #define pi   3.1415926535 
 #define NGa   6377563.396 
 #define NGb   6356256.910 
 #define WGS84a   6378137.0 
 #define WGS84b	  6356752.3141 

 #define NGFo   0.9996012717 
 #define NGphiOrigin   (49.0 * pi/180) 
 #define NGlamdaOrigin   (-2.0 * pi/180) 
 #define NGeOrigin   400000.0 
 #define NGnOrigin   -100000.0 

 #define m_aSquared   (NGa * NGa) 	
 #define m_bSquared   (NGb * NGb) 
 #define m_eSquared   ((m_aSquared - m_bSquared)/ m_aSquared) 
 #define m_n   			((NGa - NGb)/ (NGa + NGb)) 
 #define m_nToTheTwo   (m_n * m_n) 
 #define m_nToTheThree   (m_nToTheTwo * m_n) 
 #define m_NGaNGFo   (NGa * NGFo) 
 #define m_NGbNGFo   (NGb * NGFo) 
 #define m_WGS84aSquared   (WGS84a * WGS84a) 
 #define m_WGS84bSquared   (WGS84b * WGS84b) 
 #define m_WGS84eSquared   ((m_WGS84aSquared - m_WGS84bSquared)/ m_WGS84aSquared) 

#define TooFarSouth  1
#define TooFarNorth 2
#define TooFarWest  3
#define TooFarEast  4

double phi;
double lamda;

double m_Northing;
double m_Easting;

double lati_;
double longi_;
double ElipsoidHeight;		

		
int InputValidation(void)
{
	// some validation
	if (lati_ < 30.0)   return TooFarSouth;
	if (lati_ > 70.0)   return TooFarNorth;
	if (longi_ < -10.0)	return TooFarWest;
	if (longi_ > 3.0)   return TooFarEast;
	
	return 0;
}

int ValidateOSGB(void)
{
	// some validation
	if (m_Northing < 0.0)		return TooFarSouth;
	if (m_Northing > 1300000.0)	return TooFarNorth;
	if (m_Easting < 0.0)		return TooFarWest;
	if (m_Easting > 700000.0)	return TooFarEast;
	
	return 0;
}

void HelTrans( void)
{
	// Helmert Transformation

	// lat long height

	//This lat long to 3-d uses WGS84 m_eSquaredIN, NGaIN

	double v = WGS84a/ sqrt(1 - m_WGS84eSquared *  pow( sin(phi), 2) );
	double inCoOrdX = (v + ElipsoidHeight)*  cos(phi) *  cos(lamda);
	double inCoOrdY = (v + ElipsoidHeight)*  cos(phi) *  sin(lamda);
	double inCoOrdZ = ((1 - m_WGS84eSquared)* v + ElipsoidHeight) *  sin(phi);

	double prod1 = (1 + scalechange) * inCoOrdX +
		(-1 * rz * 0.000004848136811) * inCoOrdY + 
		(ry * 0.000004848136811) * inCoOrdZ +
		-446.448;

	double prod2 = (rz * 0.000004848136811) * inCoOrdX +
		(1 + scalechange) * inCoOrdY +
		(-1 * rx * 0.000004848136811) * inCoOrdZ +
		125.157;

	double prod3 = (-1 * ry * 0.000004848136811) * inCoOrdX +
		(rx * 0.000004848136811) * inCoOrdY +
		(1 + scalechange) * inCoOrdZ +
		-542.06;

	// This 3-d to lat long uses OSGB m_eSquared, NGa

	double p =  sqrt(prod1 * prod1 + prod2 * prod2);
	double lat1 =  atan2(prod3, p * (1 - m_eSquared));
	double v1 = NGa /  sqrt(1 - m_eSquared *  pow( sin(lat1), 2));
	double lat2 =  atan2(prod3 + m_eSquared * v1 *  sin(lat1), p);
	double v2 = NGa /  sqrt(1 - m_eSquared*  pow( sin(lat2), 2));

	phi =  atan2(prod3 + m_eSquared * v2 *  sin(lat2), p);
	lamda =  atan2(prod2, prod1);
	ElipsoidHeight = p / cos(phi) - v2;
}

void DoCalc(bool bApplyOSGB1936Transform)
{
	if (bApplyOSGB1936Transform)
		HelTrans();

	double phiSine =  sin(phi);
	double phiSineSquared = phiSine * phiSine;
	double phiCos =  cos(phi);
	double phiCosCubed = phiCos * phiCos * phiCos;
	double phiCosToTheFifth = phiCos * phiCos * phiCosCubed; // ****************************** FIXED
	double phiTan =  tan(phi);
	double phiTanSquared = phiTan * phiTan;
	double phiTanToTheFourth = phiTanSquared * phiTanSquared;
	double OneMinuseSquaredphiSineSquared = 1.0 - m_eSquared * phiSineSquared;
	double phiMinusNGphiOrigin = phi - NGphiOrigin;
	double phiPlusNGphiOrigin = phi + NGphiOrigin;

	double v = m_NGaNGFo /  sqrt(OneMinuseSquaredphiSineSquared);
	double p = m_NGaNGFo * (1.0 - m_eSquared) *  pow((OneMinuseSquaredphiSineSquared), -1.5);
	double nnSquared = v/p - 1.0;

	double frag1 = (1.0 + m_n + (5.0/4.0) * (m_nToTheTwo + m_nToTheThree)) * phiMinusNGphiOrigin;
	double frag2 = (3.0 * (m_n + m_nToTheTwo) + (21.0/8.0) * m_nToTheThree) *  sin(phiMinusNGphiOrigin) *  cos(phiPlusNGphiOrigin);
	double frag3 = ((15.0/8.0) * (m_nToTheTwo + m_nToTheThree)) *  sin(2.0 * (phiMinusNGphiOrigin)) *  cos(2.0 * (phiPlusNGphiOrigin));
	double frag4 = ((35.0/24) * m_nToTheThree) *  sin(3.0 * (phiMinusNGphiOrigin)) *  cos(3.0 * (phiPlusNGphiOrigin));

	double lamdaDiff = lamda - NGlamdaOrigin;
	double lamdaDiffSquared = lamdaDiff * lamdaDiff;
	double lamdaDiffToTheFourth = lamdaDiffSquared * lamdaDiffSquared;

	{
		double I = m_NGbNGFo * (frag1 - frag2 + frag3 - frag4) + NGnOrigin;
		double II = (v/2.0) * phiSine * phiCos;
		double III = (v/24.0) * phiSine * phiCosCubed * (5 - phiTanSquared + 9 * nnSquared);
		double IIIA = (v/720.0) * phiSine * phiCosToTheFifth * (61 - 58 * phiTanSquared + phiTanToTheFourth);

		m_Northing = I + II * lamdaDiffSquared + III * lamdaDiffToTheFourth + IIIA * lamdaDiffToTheFourth * lamdaDiffSquared;
	}
	{
		double IV = v * phiCos;
		double V = (v/6.0) * phiCosCubed * (v/p - phiTanSquared);
		double VI = (v/120.0) * phiCosToTheFifth * (5 - 18 * phiTanSquared + phiTanToTheFourth + 14 * nnSquared - (58 * (phiTanSquared * nnSquared)) );

		m_Easting = NGeOrigin + IV * lamdaDiff + V * lamdaDiffSquared * lamdaDiff + VI * lamdaDiffToTheFourth * lamdaDiff;
	}
}

void   BuildOSGBString(void)
{
	long Northing = (long)(m_Northing + 0.5);
	long Easting = (long)(m_Easting + 0.5);

	int osgbEasting =		 Easting  / 100000;
	int osgbNorthing = 14 - (Northing / 100000);

	int iNorth = ((osgbNorthing * 5) /25) * 5 + 'H';
	if (osgbEasting > 4)
		iNorth++;

	if (iNorth >= 'I')
		iNorth++;

	int iEast = (osgbEasting % 5) + 'A';
	iEast += (osgbNorthing * 5) % 25;
	if (iEast >= 'I')
		iEast++;

	
	printf("%c", (char)iNorth);
	printf("%c", (char)iEast);
	printf(" %ld",  Easting  % 100000);
	printf(" %ld\n",Northing % 100000);
	
	
}

void Convert(void)
{
	
		ati_  =52.701;
		longi_ =03.001;
		ElipsoidHeight=0;

		InputValidation();

		phi = lati_ * pi / 180.0;
		lamda = longi_ * pi / 180.0;

		DoCalc(true);
		ValidateOSGB();
		BuildOSGBString();	
	}
}

