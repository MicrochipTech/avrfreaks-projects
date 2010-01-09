/*
 *	FiFo
 *
 *	Definition der Länge: FIFO_SIZE s. fifo.h
 *
 *
 */

#include 	"fifo.h"

/*
 *	FiFo Initialisieren
 *
 *	ffp		Zeiger auf FiFo
 */

 	void fifo_init(fifo *ffp)
	{
		// Zeiger aus den Puffer-Anfang setzen
		ffp->in_ptr = ffp->out_ptr =ffp->buffer;
		// FiFo ist leer
		ffp->count = FIFO_EMPTY;
	}

/*
 *	Zeichen ins FiFo schreiben
 *
 *	ffp		Zeiger auf FiFo
 *	c		Zu schreibendes Zeichen
 *
 *	Return:	0	wenn ok
 *			-1	error, wenn FiFo voll 
 *
 */

 	int fifo_push(register fifo *ffp, char c)
	{
		if(ffp->count < FIFO_SIZE) {
			*(ffp->in_ptr) = c;
			ffp->count++;
			// Zeiger weiterschieben bzw. wieder auf den Pufferanfang setzen
			if(ffp->in_ptr < ffp->buffer + FIFO_SIZE-1)
				ffp->in_ptr++;
			else
				ffp->in_ptr = ffp->buffer;	// Wrap arround
			
			return 0;
		} else 
			return -1;	
	}

/*
 *	Zeichen aus dem FiFo holen
 *
 *	ffp		Zeiger auf FiFo
 *
 *	Return:	Zeichen		wenn ok
 *			-1			error, wenn FiFo leer 
 */

 	int fifo_pop(fifo *ffp)
	{
		int c;

		if(ffp->count > 0) {
			c = 0x00FF & *(ffp->out_ptr);
			ffp->count--;
			// Zeiger weiterschieben bzw. wieder auf den Pufferanfang setzen
			if(ffp->out_ptr < ffp->buffer + FIFO_SIZE-1)
				ffp->out_ptr++;
			else
				ffp->out_ptr = ffp->buffer;	// Wrap arround
			
			return c;
		} else
			return -1;	
	}

