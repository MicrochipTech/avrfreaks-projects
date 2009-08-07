
#include "string.h"
#include "board.h"

#include "conf_player.h"
#include "mp3dec.h"
#include "pwm_dac.h"

#include "sd_mmc.h"

#include "file.h"

unsigned short outBuf[OUTBUF_SIZE];
unsigned short *outPtr;
unsigned char inBuf[INBUF_SIZE];
unsigned char *readPtr;
volatile unsigned char *writePtr;
Bool dacStarted;

volatile int bytesLeft;
volatile Bool playback_abort;
long nFrames;

Fs_file_segment _MEM_TYPE_SLOW_ segment;

extern void sd_reader_get_sector(U32 addr);
extern volatile Bool in_transfer;

void CheckAndRead()
// Called from sd_reader.c after each transfer and when filling the buffer.
{
	// If in transfer, we just wait for the interrupt routine to call this function again.
	if(!in_transfer && (INBUF_SIZE-bytesLeft) >= 512) {
		// Read another sector.
		if(!fs_g_seg.u32_size_or_pos) {
			/* If there are no more sectors in the current cluster, find the next.
			 * I don't really understand why file_read() needs a "Fs_file_segment" and not
			 * just an integer, but it's probably best to not make modifications to the fat
			 * driver as it's in the software framework. */
			if(!file_read(&segment)) {
				// Probably EOF.
				return;
			}
		}
		sd_reader_get_sector(fs_g_seg.u32_addr++); // Start reading the sector.
		fs_g_seg.u32_size_or_pos--; // This should hold the number of sectors left in the file or cluster.
	}
}

static void RewindInBuffer()
// Called before starting decoding of each frame.
{
	/* Wait for end of transfer. We need to do this to avoid writing over the location the
	 * PDCA is accessing. Here it's usually a long time since we last started a transfer, so
	 * it shouldn't really be any stagnation here at all during playback. */
	while(in_transfer);

	memmove(inBuf, readPtr, bytesLeft); // Move last chunk to start of buffer.
	readPtr = inBuf;
	writePtr = inBuf + bytesLeft;

	CheckAndRead(); // Starts reading sectors as long as there's room in the buffer.

	/* Simply wait for the end of the transfer before continuing.
	 * It seems that it's quite hard to optimize the player to read while playing, at
	 * least with the marginal memory available, so this is probably the best solution for now. */
	while(in_transfer);
}

void abort_playback()
// This function is called from a joystick interrupt to abort the playback.
{
	playback_abort = TRUE;
}

int play_selected_file()
/* Play the selected file in the navigator.
 * Returns number of frames decoded, which is 0 if the format is wrong. */
{
	int err, offset;
	HMP3Decoder hMP3Decoder;

	outPtr = outBuf;
	readPtr = inBuf;

	playback_abort = FALSE;

	if ((hMP3Decoder = MP3InitDecoder()) == 0)
	{
		//printf("** Cannot initialize MP3 decoder.\r\n");
		return FALSE;
	}

	bytesLeft = 0;
	readPtr = inBuf;
	nFrames = 0;
	dacStarted = FALSE;

	file_open(FOPEN_MODE_R);
	segment.u16_size = 0;
	segment.u32_addr = 0;
	fs_g_seg.u32_size_or_pos = 0;

	/* Skip any ID3-tags at the beginning of the file.
	 * According to Wikipedia, ID3v2-tags can be as large as 256MB,
	 * so we just scan through the whole file, even though it's possible
	 * that this isn't an MP3-file at all. */
	while(1) {
		RewindInBuffer();
		if(bytesLeft<=0) return 0; // EOF.
		offset = MP3FindSyncWord(readPtr, bytesLeft);
		if (offset < 0) {
			bytesLeft = 0; // Look in the next chunk.
		} else {
			readPtr += offset;
			bytesLeft -= offset;
			break;
		}
	}

	while(1) {
		/* Find start of next MP3 frame. Assume EOF if no sync found. */
		offset = MP3FindSyncWord(readPtr, bytesLeft);
		if (offset < 0) break;

		readPtr += offset;
		bytesLeft -= offset;

		RewindInBuffer(); // Rewind the buffer and fill it.

		/* Decode one MP3 frame. Note that the outBuf parameter isn't used
		 * due to modifications in subband.c. These modifications also
		 * start and maintain the dac. */
		err = MP3Decode(hMP3Decoder, &readPtr, (int*) &bytesLeft, NULL, 0);

		if (err || playback_abort) break;

		nFrames++;

		/* Immediately start decoding another frame.
		 * Code in subband.c will prevent the decoder from overwriting samples the dac
		 * hasn't consumed yet. */
	}

	MP3FreeDecoder(hMP3Decoder);
	pwm_dac_stop(); // Stop the dac.
	// TODO: Flush the dac instead of stopping it.

	file_close(); // Close the file.

	return nFrames; // Return number of played frames.
}
