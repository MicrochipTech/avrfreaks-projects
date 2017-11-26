#define abs(x) ((x) > 0 ? (x) : (-x))
//Calibration constants are higly influenced by used device (one attiny45 was 155, attiny85 was 135) has to be checked for each MCU
//Using full range scan cause problems with some OS (timeout -> device not detected)
#define CALIB_START             101     //Middle of calibration interval range (see datasheet for Attiny45/85) - chip dependent
#define CALIB_INTERVAL_SIZE     32      //Actually it will be 31 as -8-4-2-1==-15 and +8+4+2+1==+15
//Called by V-USB after device reset
//Calibrate oscillator to usable value for USB comunication
void hadUsbReset()
{
	uint_fast16_t frameLength = 0;
	const uint_fast16_t targetLength = (uint_fast16_t)(1499 * (double)F_CPU / 10.5e6 + 0.5);
	uint_fast16_t bestDeviation = 0xFFFF;     //Some init value that is high enough

	uint8_t testCalibration = CALIB_START;
	uint8_t bestCalibration = CALIB_START;
	uint8_t step;           //OSCCAL modification step

	//Do a binary search to get optimum OSCCAL
	//Focus on only 1 range and only small portion from it -> reach F_CPU 16.5 MHz using PLL and prescalers
	//Starting value is in the middle of this interval defined by CALIB_START
	OSCCAL = testCalibration;
	frameLength = usbMeasureFrameLength();

	//Halving interval appoach
	for(step = (CALIB_INTERVAL_SIZE / 4); step > 0; step >>= 1) {
		if(frameLength < targetLength) {
			testCalibration += step; // frequency too low
		} else {
			testCalibration -= step; // frequency too high
		}

		OSCCAL = testCalibration;
		frameLength = usbMeasureFrameLength();

		if(abs(frameLength - targetLength) < bestDeviation) {
			bestCalibration = testCalibration; // new optimum found
			bestDeviation = abs(frameLength - targetLength);
		}
	}

	OSCCAL = bestCalibration;
}