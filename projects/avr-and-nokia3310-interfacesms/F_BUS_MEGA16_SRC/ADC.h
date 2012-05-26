#ifndef _ADC_H
	#define _ADC_H

	/* Initialize ADC */
	void InitADC(void);

	/* ADC Start Convertion */
	unsigned int ReadADC(unsigned char Channel);
	void ReadTemperature(void);
#endif