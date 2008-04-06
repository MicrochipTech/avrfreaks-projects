


typedef struct
{
	int Ta;											// Abtastzeit in ms
	int I;											// Integralanteil
	int Kp;											// Verstärkung
	int D;											// Differenzieller Anteil
	int e;											// Regelabweichung
	int esum;										// Summe der Regelabweichungen
	int ealt;										// Regelabweichung zum ZP z-1
	
}PID_Einstellung;									// Struktur PID_Einstellungen erzeugen


void PID_Init(void);
int PID_Cyclic(int, int, PID_Einstellung*);

PID_Einstellung Regler1;								// Variable Regler1, je nach Bedarf erweitern


void main(void)
{
	PID_Init();
	
	int Ausgang, w,x;	
	
	while(1)
	{
		Ausgang = PID_Cyclic(x,w,&Regler1);
	}
}


void PID_Init(void)									// In der Init müssen die Reglereinstellungen gemacht werden
{
	Regler1.Ta=10;
	Regler1.I=100;
	Regler1.D=0;
	Regler1.Kp=1;
	Regler1.esum=0;	
	Regler1.e=0;									// Für weiteren Regler einfach Neue Variable erstellen
	Regler1.ealt=0;									// und hier Werte mit Regler2.xx=0 einstellen
}


int PID_Cyclic (int x, int w, PID_Einstellung* PID)
{    
	int y;
	PID->e = w - x;               										// aktuelle Regelabweichung bestimmen
	if ((y < 1023)&&(y > 0))  										// bei Übersteuertem stellglied Integration einfrieren
 	{                       										// (Anti-Windup)
  		  PID->esum = PID->esum + PID->e;     					      			// Summe der Regelabweichung aktualisieren  
  	}
	y = (PID->Kp*PID->e)+(PID->I*PID->Ta*PID->esum)+(PID->D*((PID->e-PID->ealt))/PID->Ta);  		// Reglergleichung
	PID->ealt = PID->e;                 										// Regelabweichung für nächste Abtastung merken
	if (y > 1023)             										// Stellgröße auf 0..1023 begrenzen (10 bit PWM)
 	{
    	y = 1023;
  	}
	if (y < 1)
  	{
    	y = 0;
  	}
	return y;                 										// Stellgröße zurückgeben
}


