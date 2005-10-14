#ifndef INCLUDE_UITripUpload
#define INCLUDE_UITripUpload

#include <inttypes.h>
#include "ReceiverIO.h"

void UIDisplayHeader(int16_t tripNo);
void UITripUpload(int16_t tripNo);
//void UIUploadRecords(int s, int e);

void _UIEraseFlash(void);		//erase all flash


#endif //INCLUDE_UITripUpload