void get_sun_pos(float udtLocationdLatitude,float udtLocationdLongitude,float *altitude,float *azimuth){

/* get sun position */


 /*
	a simplified, yet accurate sun position calculator
	based on Blanco-Muriel et al.'s SPA algorithm
	closely matches altitude, azimuth values returned by NOAA calculator
	This variation was converted from Javascript  
*/

	float zenith;
	float pi =3.14159265358979323846;
	float twopi=(2*pi);
	float rad=(pi/180);
	float dEarthMeanRadius=6371.01;	// In km
	float dAstronomicalUnit=149597890;	// In km
	float dDecimalHours = udtTimedHours + (udtTimedMinutes + udtTimedSeconds / 60.0 ) / 60.0;
	long liAux1 =(udtTimeiMonth-14)/12;
	long liAux2=(1461*(udtTimeiYear + 4800 + liAux1))/4 + (367*(udtTimeiMonth - 2-12*liAux1))/12- (3*(udtTimeiYear + 4900 + liAux1)/100)/4+udtTimeiDay-32075;
	float dJulianDate=(liAux2)-0.5+dDecimalHours/24.0;
	float dElapsedJulianDays = dJulianDate-2451545.0;
	float dOmega=2.1429-0.0010394594*dElapsedJulianDays;
	float dMeanLongitude = 4.8950630+ 0.017202791698*dElapsedJulianDays; // Radians
	float dMeanAnomaly = 6.2400600+ 0.0172019699*dElapsedJulianDays;
	float dEclipticLongitude = dMeanLongitude + 0.03341607*sin( dMeanAnomaly ) + 0.00034894*sin( 2*dMeanAnomaly )-0.0001134 -0.0000203*sin(dOmega);
	float dEclipticObliquity = 0.4090928 - 6.2140e-9*dElapsedJulianDays +0.0000396*cos(dOmega);
	float dSin_EclipticLongitude= sin( dEclipticLongitude );
	float dY = cos( dEclipticObliquity ) * dSin_EclipticLongitude;
	float dX = cos( dEclipticLongitude );
	float dRightAscension = atan2( dY,dX );
	if( dRightAscension < 0.0 ) dRightAscension = dRightAscension + twopi;
	float dDeclination = asin( sin( dEclipticObliquity )*dSin_EclipticLongitude );
	float dGreenwichMeanSiderealTime = 6.6974243242 + 0.0657098283*dElapsedJulianDays + dDecimalHours;
	float dLocalMeanSiderealTime = (dGreenwichMeanSiderealTime*15 + *udtLocationdLongitude)*rad;
	float dHourAngle = dLocalMeanSiderealTime - dRightAscension;
	float dLatitudeInRadians = *udtLocationdLatitude*rad;
	float dCos_Latitude = cos( dLatitudeInRadians );
	float dSin_Latitude = sin( dLatitudeInRadians );
	float dCos_HourAngle= cos( dHourAngle );
	float udtSunCoordinatesdZenithAngle = (acos( dCos_Latitude*dCos_HourAngle*cos(dDeclination) + sin( dDeclination )*dSin_Latitude));
	dY = -sin( dHourAngle );
	dX = tan( dDeclination )*dCos_Latitude - dSin_Latitude*dCos_HourAngle;
	float udtSunCoordinatesdAzimuth = atan2( dY, dX );
	if ( udtSunCoordinatesdAzimuth < 0.0 ) 
		udtSunCoordinatesdAzimuth = udtSunCoordinatesdAzimuth + twopi;
	udtSunCoordinatesdAzimuth = udtSunCoordinatesdAzimuth/rad;
	float dParallax=(dEarthMeanRadius/dAstronomicalUnit)*sin(udtSunCoordinatesdZenithAngle);
	udtSunCoordinatesdZenithAngle=(udtSunCoordinatesdZenithAngle + dParallax)/rad;
	*azimuth=udtSunCoordinatesdAzimuth;
	zenith=udtSunCoordinatesdZenithAngle;
	*altitude=90-udtSunCoordinatesdZenithAngle;
}
