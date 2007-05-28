//This code is written to test PPM_Lib.c
#include <PPM_Lib.c>

int main (void) {
	//Load PPM_Channel_n_PulseLength[0 to 7] with 125
	//This wil result in getting a 1.5ms pulse from each recevier channel
	for (int i = 0; i < 8 ; i++) {
		PPM_Channel_n_PulseLength [i] = 125;
	}
	
	//Start the PPM pulse generation
	PPM_Start();

	while (1);

	return 0;
}
