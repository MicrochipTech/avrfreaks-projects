//avr_regset.cpp
//by outer_space

#include <iostream>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <string>
#include <cassert>
#include <cmath>
#include <cstdlib>
#include <stdexcept>

using namespace std;

template<class T> bool inArray(T [], int, T); 
template<class T> void arrayFill(T [], int, T);
template<class T> void listArray(T [], int);
string num2hertz (const double& x);

void t26();
void timer0setup();
void timer1setup();
void timer1ocrc();
void adcsetup();
void powerdown();
void watchdog();

double freq = 0;

ofstream fout("out"); // open output file
int main(int argc, char *argv[])
{
	cout << "avr_regset" << endl << endl;

	string processor = argv[1];
	freq = atof(argv[2])*1000000;
	if(argc != 3)
	{
		cout << "2 required parameters ex. 'avr_regset.bat t26 1' for tiny26 at 1MHz" << endl;
		cout << "number seen " << argc << endl;
		int i;
		for(i = 0; i<argc; i++) {cout << argv[i] << endl;}
		
	}
	else if(processor == "t26")
	{
		cout << "tiny26, " << num2hertz(freq) << endl;
		t26();
	}
	else
	{
		cout << "avr_regset does not know this processor" << endl;
		cout << processor;
	}
	fout.close();	
}

template<class T> void arrayFill(T x[], int size, T item)
{
	int i;
	for(i=0; i<size; i++) {x[i]=item;}
}

template<class T> bool inArray(T x[], int size, T item)
{
	int i;
	for(i = 0; i<size; i++)
	{
		if(x[i] == item) {return 1;}
	}
	return 0;
}

template<class T> void listArray(T x[], int size)
{
	cout << endl;
	int i;
	for(i = 0; i<size; i++) {cout << x[i] << " ";}
}

string num2hertz(const double& x)
{
	std::ostringstream o;
	if(x < 1000) {o << fixed << setprecision(3) << x << " Hz ";}
	else if(x < 1000000) {o << fixed << setprecision(3) << x/1000 << " kHz";}
	else {o << fixed << setprecision(3) << x/1000000 << " MHz";}
	return o.str ();
}


void t26()
{
	cout << endl
		<< "0. Timer0 setup" << endl
		<< "1. Timer1 setup (pll or system clock)" << endl
		<< "2. Timer1 setup with OCR1C (specify frequency)" << endl
		<< "3. ADC setup" << endl
		<< "4. Power down setup" << endl
		<< "5. Watchdog setup" << endl << ": ";

	int task;
	cin >> task;
	
	if(task == 0) {timer0setup();}
	else if(task == 1) {timer1setup();}
	else if(task == 2) {timer1ocrc();}
	else if(task == 3) {adcsetup();}
	else if(task == 4) {powerdown();}
	else if(task == 5) {watchdog();}
	else {cout << endl << "choice not available" << endl;}
 
}

void timer0setup()
{
	cout << "Timer0 setup, ";
	cout << "Select overflow frequency:" << endl;
	int i, j = 0;
	for(i = 1; i <= 1024; i *= 4)
	{
		j++;
		if(i == 4 || i == 32) {i*=2;} //prescalers dont consistently multiply by 4
		cout << right << setw(2) << j << left << ". clk/" 
			 << setw(5) << i 
			 << setw(13) << right << num2hertz(freq/256/i) << endl;
		
	}
	cout << ": ";
	int tmr0select;
	cin >> tmr0select;
	assert(tmr0select >0 && tmr0select <= 5);
	fout << endl << endl << "//Timer0 init, system clk " << num2hertz(freq) << endl;
	fout << "TIMSK |= (1<<TOIE0); // enable timer0 overflow irq" << endl;
	fout << "TCCR0 = (" << tmr0select << "<<CS00); // ";
	j = 0;
	for(i = 1; i <= 1024; i *= 4)
	{
		j++;
		if(i == 4 || i == 32) {i*=2;} //prescalers dont consistently multiply by 4
		if(j == tmr0select)
		{
			fout << " clk/256/" << i << " = " << num2hertz(freq/256/i) << endl;
		}
		
	}
	fout << endl << "//timer0 irq block" << endl << "SIGNAL(SIG_OVERFLOW0)" << endl
		<< "{" << endl << endl << "}" << endl;
}

void timer1setup()
{
	cout << "Timer1 setup" << endl;
	cout << "Select clock source:" << endl;
	cout << "1. system clock " << num2hertz(freq) << endl;
	cout << "2. PLL 64 MHz" << endl;
	int choice1;
	cout << "Enter choice:";
	cin >> choice1;
	assert(choice1 > 0 && choice1 <= 2);
	double t1freq = freq;
	if(choice1 == 2) 
	{
		t1freq = 64000000;
		fout << "//Timer1 init for 64 MHz pll" << endl;
		fout << "PLLCSR = (1<<PLLE); //enable pll" << endl;
		fout << "while (!(PLLCSR & PLOCK)); //wait for phase lock" << endl;
		fout << "PLLCSR |= (1<<PCKE); //enable for timer1 clk" << endl;
	}
	else
	{
		fout << "//Timer1 init for system clk " << num2hertz(freq) << endl;
	}
	int i, j = 0;
	for(i = 1; i <= 16384; i*=2)
	{
		j++;
		cout << right << setw(2) << j << left << ". clk/256/"
			<< setw(5) << i << " = " << setw(13) << right
			<< num2hertz(t1freq/256/i) << endl;
	}
	int tmr1;
	cin >> tmr1;
	assert(tmr1>0 && tmr1 <=15);
	fout << "TIMSK |= (1<<TOIE1); // enable timer1 overflow irq" << endl;
	fout << "TCCR1B = (1<<PSR1)|(";
	j = 0;
	for(i = 1; i<=16384; i*=2)
	{
		j++;
		if(tmr1 == j) 
		{
			fout << j << "<<CS10); // clk/256/" << i << " = " << num2hertz(t1freq/256/i) << endl << endl;
			break;
		}
	}
	
	fout << "//timer1 overflow irq block" << endl << "SIGNAL(SIG_OVERFLOW1)" << endl << "{"
		<< endl << endl << "}" << endl;
}

void timer1ocrc()
{
	cout << "Choose a frequency from " << num2hertz(freq/16384/256)
		<< " to " << num2hertz(freq/256) << endl << endl << ": ";
	double desired;
	cin >> desired;
	assert(desired >= freq/16384/256 && desired <= freq/256);
	double basefreq;
	int ocr;
	int i;
	int j = 0;
	for(i = 1; i<=16384; i*=2)
	{
		j++;
		if(desired >= (basefreq=freq/i/256)) 
		{
			ocr = int (256*basefreq/desired);
			break;
		}
	}
	
	cout << endl << "base frequency " << fixed << setprecision(3) << basefreq << endl;
	cout << "ocr calculated to " << ocr << endl;
	double actual = basefreq*256/ocr;
	double error = abs(actual/desired - 1)*100;		
	cout << "ocr frequency = " << fixed << setprecision(3) << actual << endl;
	cout << "error of " << error << "%" << endl;
	
	fout << endl << endl << "//Timer1 init for specified frequency, system clk " << num2hertz(freq) << endl;
	fout << "OCR1C = " << ocr << ";" << endl;
	fout << "TIMSK |= (1<<TOIE1); // enable timer1 overflow irq" << endl;
	fout << "TCCR1B = (1<<CTC1)|(1<<PSR1)|(" << j << "<<CS10); // " << num2hertz(actual) << endl;
	fout << endl << "//timer1 overflow irq block" << endl << "SIGNAL(SIG_OVERFLOW1)" << endl
		<< "{" << endl << endl << "}" << endl;
}

void adcsetup()
{
	int ps; //calculate prescaler based on freq
	double psfreq = 0;
	int i, j = 0;
	for(i = 2; i <= 128; i*=2)
	{
		j++;
		if(freq/i <= 200000)
		{
			ps = j;
			psfreq = freq/i;
			break;
		}
	}
	
	string ref[4];
	ref[0] = "Vcc";
	ref[1] = "AREF, pA3";
	ref[2] = "2.56v, AREF nc";
	ref[3] = "2.56v, cap on AREF";
	
	cout << "ADC setup, prescaler = " << ps << ", clk/" << i << " = " << num2hertz(psfreq) << endl << endl;
	cout << "Select reference" << endl;
	for(i = 0; i <=3; i++)
	{
		cout << i << ". " << ref[i] << endl; 
	}
	cout << ": ";
	int refsel;
	cin >> refsel;
	cout << endl << "enter options, separate by space" << endl;
	#define adcoptions_size  5
	#define in_adcopt_size 5
	string adcoptions[adcoptions_size];
	adcoptions[0] = "free running mode";
	adcoptions[1] = "interrupt enable";
	adcoptions[2] = "left justify";
	adcoptions[3] = "differential channel";
	adcoptions[4] = "20x gain stage";
	for(i = 0; i<adcoptions_size; i++)
	{
		cout << i << ". " << adcoptions[i] << endl;
	}
	cout << i << ". done" << endl;
	int in_adcopt[adcoptions_size];
	arrayFill(in_adcopt, adcoptions_size, -1);
	i = 0;
	while(i < adcoptions_size)
	{
		cin >> in_adcopt[i];
		if(in_adcopt[i] == adcoptions_size) {break;} //break if index limit is reached
		i++;
	}
	cout << "received - ";
	listArray(in_adcopt, adcoptions_size);
	cout << endl;
	string adc[11] = //map adc inputs to pins
	{"ADC0 (pA0)", "ADC1 (pA1)", "ADC2 (pA2)", "ADC3 (pA4)", "ADC4 (pA5)", "ADC5 (pA6)",
	"ADC6 (pA7)", "ADC7 (pB4)", "ADC8 (pB5)", "ADC9 (pB6)", "ADC10 (pB7)"};
	#define muxlist_size 32
	string muxlist[muxlist_size];
	for(i = 0; i <= 10; i++) {	muxlist[i] = "single ended " + adc[i];}
	muxlist[11] = "differential 20x " + adc[0] + "+ " + adc[1] + "- ";
	muxlist[14] = "differential 20x " + adc[2] + "+ " + adc[1] + "- ";
	muxlist[18] = "differential 20x " + adc[4] + "+ " + adc[3] + "- ";
	muxlist[20] = "differential 20x " + adc[4] + "+ " + adc[5] + "- ";
	muxlist[23] = "differential 20x " + adc[6] + "+ " + adc[5] + "- ";
	muxlist[25] = "differential 20x " + adc[8] + "+ " + adc[9] + "- ";
	muxlist[28] = "differential 20x " + adc[10] + "+ " + adc[9] + "- ";
	muxlist[12] = "differential 1x " + adc[0] + "+ " + adc[1] + "- ";
	muxlist[15] = "differential 1x " + adc[2] + "+ " + adc[1] + "- ";
	muxlist[16] = "differential 1x " + adc[2] + "+ " + adc[3] + "- ";
	muxlist[19] = "differential 1x " + adc[4] + "+ " + adc[3] + "- ";
	muxlist[21] = "differential 1x " + adc[4] + "+ " + adc[5] + "- ";
	muxlist[24] = "differential 1x " + adc[6] + "+ " + adc[5] + "- ";
	muxlist[26] = "differential 1x " + adc[8] + "+ " + adc[9] + "- ";
	muxlist[29] = "differential 1x " + adc[10] + "+ " + adc[9] + "- ";
	muxlist[13] = "differential 20x gain offset " + adc[1];
	muxlist[17] = "differential 20x gain offset " + adc[3];
	muxlist[22] = "differential 20x gain offset " + adc[5];
	muxlist[27] = "differential 20x gain offset " + adc[9];
	muxlist[30] = "1.18V (Vbg)";
	muxlist[31] = "0V (GND)";
	
	if(!inArray(in_adcopt, in_adcopt_size, 3)) // check for single ended mode
	{
		for(i = 0; i<=10;i++) {cout << i << ". " << muxlist[i] << endl;}
		cout << "30. " << muxlist[30] << endl;
		cout << "31. " << muxlist[31] << endl;
	}
	else if(inArray(in_adcopt, in_adcopt_size, 3) && inArray(in_adcopt, in_adcopt_size, 4))
	{
		int diff20x[11] = {11, 14, 18, 20, 23, 25, 28, 13, 17, 22, 27};
		for(i = 0; i<11; i++) {cout << diff20x[i] << ". " << muxlist[diff20x[i]] << endl;}
	}
	else if(inArray(in_adcopt, in_adcopt_size, 3) && !inArray(in_adcopt, in_adcopt_size, 4))
	{
		int diff1x[8] = {12, 15, 16, 19, 21, 24, 26, 29};
		for(i = 0; i<8; i++) {cout << diff1x[i] << ". " << muxlist[diff1x[i]] << endl;}
	}
	else {cout << "error picking adc options";}
	cout << ": ";
	int muxpick;
	cin >> muxpick;
	assert(muxpick >=0 && muxpick <32);	
	
	fout << endl << "//ADC setup, system clk " << num2hertz(freq) << endl;
	fout << "ADMUX = (" << refsel << "<<REFS0)";
	if(inArray(in_adcopt, in_adcopt_size, 2)) {fout << "|(1<<ADLAR)";}
	fout << "|(" << muxpick << "<<MUX0); // " << muxlist[muxpick] << endl;
	fout << "ADCSR = (1<<ADEN)|(" << ps << "<<ADPS0)";
	if(inArray(in_adcopt, in_adcopt_size, 0)) {fout << "|(1<<ADFR)";}
	if(inArray(in_adcopt, in_adcopt_size, 1)) {fout << "|(1<<ADIF)|(1<<ADIE)";}
	fout << "; // max sample rate of " << num2hertz(psfreq/14) << endl;
	
}



void powerdown()
{
	cout << endl << "Select powerdown mode" << endl;
	string modelist[4];
	modelist[0] = "Idle Mode, only the CPU is stopped, automatically starts the ADC"; 
	modelist[1] = "Noise Reduction, similar to idle, but timers will not interrupt";
	modelist[2] = "Power-down, all clocks stopped. External irqs will work.";
	modelist[3] = "Standby, for external clock only.  Similar to power-down, faster recovery.";
	int i;
	for(i = 0; i<4; i++) {cout << i << ". " << modelist[i] << endl;}
	int mode;
	cin >> mode;
	assert(mode >=0 && mode <=3);
	fout << endl << "// " << modelist[mode] << endl;
	fout << "MCUCR &= ~(3<<SM0); //if another sleep was already used" << endl;
	fout << "MCUCR |= (1<<SE)|(" << mode << "<<SM0); //";
}

void watchdog()
{
	cout << "Select overflow period" << endl;
	int i;
	double fper = 1/freq;
	double cyc = 16384;
	for(i = 0; i<8; i++)
	{
		cout << i << ". " << fixed << setw(7) << setprecision(0) << cyc << " * " << num2hertz(1000000) 
			<< " = " << setw(7) << setprecision(2) << cyc/1000 << " ms" << endl;  
		cyc *= 2;
	}
	int mode;
	cin >> mode;
	assert(mode>=0 && mode <=7);
	fout << endl << "//Watchdog setup" << endl;
	cyc = 16384;
	for(i = 0; i<mode; i++) {cyc *= 2;}
	fout << "WDTCR = (1<<WDE)|(" << mode << "<<WDP0); // " << setprecision(2)
		<< fixed << setw(7) << cyc/1000 << " ms" << endl << endl;
	fout << "//Watchdog disable" << endl;
	fout << "WDTCR |= (1<<WDCE)|(1<<WDE);" << endl;
	fout << "WDTCR &= ~(1<<WDE);" << endl;
}

