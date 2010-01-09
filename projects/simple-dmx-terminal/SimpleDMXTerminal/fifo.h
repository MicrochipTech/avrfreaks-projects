/*
 *	FiFo
 *
 *	
 *
 *
 */

/*
 *	Definitionen und Deklarationen
 */

	#define FIFO_SIZE	256	// Größe des FiFos in Bytes
 	#define FIFO_EMPTY	0

	// Macro: Gibt die Anzahl der im FiFo vorhandenen Zeichen zurück
	#define fifo_get_count(p)		((p)->count)
	#define fifo_is_not_empty(p)	((p)->count)

	// FiFo Aufbau
	struct fifo
	{
 		char buffer[FIFO_SIZE];
		char *in_ptr;
		char *out_ptr;
		int  count;
	};

	typedef struct fifo fifo;

/*
 *	Sichtbare Funktionen
 */

	void	fifo_init(fifo *fpp);
	int 	fifo_push(fifo *fpp,char c);
	int		fifo_pop(fifo *fpp);
