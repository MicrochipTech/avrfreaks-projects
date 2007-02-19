#ifndef __cfifo_h__
#define __cfifo_h__

class Cfifo {
public:
	int fill ;
	int empty ;
	int maxfifo ;
	uint8_t *pmem ;
	int error ;

	Cfifo( int size )
		{
		error = 0 ;
		fill = 0 ;
		empty = 0 ;
		maxfifo = size ;
		pmem = (uint8_t *)malloc(size);
		if ( pmem == NULL  ) error = 1 ;
		}
	~Cfifo()
		{
		if ( pmem != 0 ) { free (pmem) ; pmem = NULL ; }
		}
	int filled()
		{
		int diff ;

		diff = fill - empty ;

		if ( diff < 0 ) diff += maxfifo ;
		return diff ;
		}
	int spacefree()
		{
		return maxfifo - filled() - 1 ;
		}
	void put(uint8_t b) ;
	uint8_t get(void) ;
	int read(tfh fd);	// read as much as possible directly to fifo
	void flush(void)
		{
		empty = fill = 0 ;
		}

	} ;



int getpairs( tfh fdi , Cfifo &fifo , uint32_t &nlineno , uint nmaxval, double *pvector1, double *pvector2 );
// reads pairs x= y= from an open ASCII file


int ansiread( tfh fh, char *p, int n, class Cfifo &buffer );
// reads one ascii / ansi line from file through fifo into p[0]..p[n-1]


#endif
