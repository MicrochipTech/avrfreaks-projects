//ds1620 routinerne af kim madsen 25-04-01
//

 

//level 1 routinerne
#define start_convert 0xee
#define read_temp 0xaa
#define write_config 0x0c
#define count_remain 0xa0
#define read_config 0xac
#define load_counter 0x41
#define stop_convert 0x22
#define rst  4 //porta pin2
#define clk  2 //porta pin1
#define dq   1 //porta pin0

//eeprom unsigned int gemds1620timeout;
volatile unsigned int ds1620timeout;
volatile char DS1620_config = 0,DS1620_retur_config = 0,DS1620_fremloeb_config = 0;
signed int ConverteretTemp = 0,ConvFremloebTemp = 0,ConvReturTemp = 0;

#pragma warn- 
void SendProtocolTilDS1620(char protocol)
{
#asm 
	.EQU PORT = 0x1b;
	.EQU DDR = 0x1a;
	.EQU PIN = 0x19;
 	.EQU dq = 0;
	.EQU clk = 1;
	.EQU rst = 2;

	ld  r30,y
	ldi r26,8
	
_sendProtocol_igen:
	sbrs r30,0 ; drop næste cmd hvis bit 0 i r30 er sat dq er høj fra starten
	cbi  PORT,dq ; slet dq bit
	cbi  PORT,clk ;sæt clk lav
	nop
	nop
	nop
	nop
	nop	
	nop ; max frekvens på clk ben på 1.75 mhz
	sbi PORT,clk ; sæt clk høj
	sbi PORT,dq ; sæt dq høj
	clc ; clear carry
	lsr r30;roter en plads til højre
	dec r26 ; tælleren en ned
	brne _sendProtocol_igen ;kør gennem denne routine 8 gange (r26)
#endasm
}
void VaelgKreds(char KredsNummer)
{
#asm 
	ld r30,y ;kreds nummer i r30
	ldi r26,rst*2; sæt start bit i r26
	clr r27; clear r27
	cp r30,r27; er r26 og r 27 ens ?
	breq afslut_saet_rst; afslut og sæt rst
	ldi r27,3;overflow ?
	cp r30,r27; sammenlign
	brge afslut; afslut
vaelg_kreds_igen:
	lsl r26;roter r26 mod venstre
	dec r30;tael r30 ned	
	brne vaelg_kreds_igen; er r30 > 0 gentag
afslut_saet_rst:
	in r27,PORT;hent vaerdi en af port
	or r27,r26; sæt bit værdi fra r26
	out PORT,r27;sæt bittene på porten
afslut:
#endasm
}
void SkrivIDS1620(char KredsNummer,char protocol,int vaerdi,char antalbit)
{
#asm
	sbi PORT,dq ; dq = høj
	sbi PORT,clk ; clk = høj
#endasm	
	VaelgKreds(KredsNummer);
        SendProtocolTilDS1620(protocol);
#asm   
	ld r26,y
	ldd r30,y+1
	ldd r31,y+2
	tst r26
	breq SkrivDS1620Slut; der er ingen bit i param 2
_sendbit_igen:	
	sbrs r30,0 ; drop næste cmd hvis bit 0 i reg 30 er sat dq er høj fra starten
	cbi  PORT,dq ; slet dq bit
	cbi  PORT,clk ;sæt clk lav
	nop
	nop
	nop
	nop
	nop	
	nop ; max frekvens på clk ben på 1.75 mhz
	sbi PORT,clk ; sæt clk høj
	sbi PORT,dq ; sæt dq høj
	clc ; clear carry
	lsr r31;roter en plads til højre
	ror r30
	dec r26 ; tælleren en ned
	brne _sendbit_igen ;kør gennem denne routine 8 gange (r26)
		
SkrivDS1620Slut:
	in r27,PORT;hent vaerdi en af port
	andi r27,0xe3; slet bit 2 til 4
	out PORT,r27;sæt bittene på porten

#endasm
}

int LaesIDS1620(char KredsNummer,char protocol,char antalbit)
{
#asm
	sbi PORT,dq ; dq = høj
	sbi PORT,clk ; clk = høj
#endasm	
	VaelgKreds(KredsNummer);
        SendProtocolTilDS1620(protocol);
#asm   
	ld r26,y;antal bit i r26
	clr r30
	clr r31
	cbi DDR,dq ;sæt bit 0 PORTA til indput
	sbi PORT,dq ;vær sikker på at bit 0 er høj
laesbit:
	lsr r31 ;rokoler int med nul i lsb
	ror r30 ;rokoler int gennem carry
	cbi PORT,clk ;clk lav
	nop
	nop
	nop
	nop
	nop
	nop ;max frekvens på clk ben = 1.75mhz
	sbic PIN,dq ; skip næste cmd hvis bit er = 0
	sbr r31,0x80   ; sæt bit 0 i r30
	sbi PORT,clk ; clk høj
	dec r26
	brne laesbit 

	;rokoler på plads
	ldi r27,LOW(16) ;antal bit i en int
	ld r26,y ;antal brugte bit
	sub r27,r26; antal manglende bit i r27
	breq laesDS1620Slut;er r27 = 0 afslut
	
laesbit_slut:
	lsr r31 ;rokoler int med nul i pos 7
	ror r30 ;rokoler int gennem carry
	dec r27	
	brne laesbit_slut ; er r27 = 0 ellers fortsæt
	
laesDS1620Slut:
	
	sbi DDR,dq ;sæt bit 0 PORTA til output
	in r27,PORT;hent vaerdi en af port
	andi r27,0xe3; slet bit 2 til 4
	out PORT,r27;sæt bittene på porten
#endasm
}
#pragma warn+

void DS1620StartConvert(char KredsNummer)
{
	SkrivIDS1620(KredsNummer,start_convert,0,0);
	SkrivIDS1620(KredsNummer,stop_convert,0,0);
}

void DS1620Init()
{          
	int taeller;
	DDRA = 0xff;
	PORTA =3;//sæt rst lav og clk samt dq høj
	for(taeller = 0;taeller < 0xffff;taeller++);//vent på power on af ds1620
	SkrivIDS1620(0,write_config,2,8);
	SkrivIDS1620(1,write_config,2,8);
	SkrivIDS1620(2,write_config,2,8);
	ds1620timeout = ds1620timeoutVaerdi;
}

double hent_temp_fra_kreds(char KredsNummer)
{//testet virker også ved - temp

int DS1620_count_pr_degree;
int DS1620_countRemain;
int DS1620_temp;
double tempD = 0.0, Countdegree = 0.0,CountRemain = 0.0;
unsigned int temp;

	DS1620_temp = LaesIDS1620(KredsNummer,read_temp,9);
	DS1620_countRemain = LaesIDS1620(KredsNummer,count_remain,9); 
	SkrivIDS1620(KredsNummer,load_counter,0,0);
	DS1620_count_pr_degree = LaesIDS1620(KredsNummer,count_remain,9);
	//beregn temp
	Countdegree = 0.0+DS1620_count_pr_degree;
	CountRemain = 0.0+DS1620_countRemain;
 	DS1620StartConvert(KredsNummer);
	if(DS1620_temp < 256)// TEMP > 127 -temp
	{ 
		temp = DS1620_temp/2;
 		tempD = 0.0 + temp;
 	}
 	else     
 	{
		temp = (512-DS1620_temp)/2;
 		tempD = 0.0 - temp;
 	 }
 	tempD -= 0.25;
 	if(DS1620_count_pr_degree != 0)
 		tempD += ((Countdegree-CountRemain)/Countdegree);
 	return tempD;// gemmes med 2 decimaler
}

void DS1620_servicer_timeout()
{
	ds1620timeout = ds1620timeoutVaerdi;
	DS1620_config =(char)LaesIDS1620(0,read_config,8);
	DS1620_fremloeb_config = (char)LaesIDS1620(1,read_config,8);
	DS1620_retur_config =(char)LaesIDS1620(2,read_config,8);

        ConverteretTemp = (signed int)(hent_temp_fra_kreds(0)*100);// main termometer
        if((DS1620_fremloeb_config & 0x0c) == 8)//er ds1620 tilstede ?
        {//tilstede
		ConvFremloebTemp = (signed int) (hent_temp_fra_kreds(1)*100);
        }
        if((DS1620_retur_config & 0x0c) == 8)//er ds1620 tilstede ?
        {//tilstede
	ConvReturTemp = (signed int) (hent_temp_fra_kreds(2)*100);
        }        
}