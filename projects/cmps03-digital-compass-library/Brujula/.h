#include "WProgram.h"
#include "Wire.h"

class Brujula: private TwoWire
{
	public:
		Brujula();
		//	Devuelve la lectura entre 0000 y 3600
		int lectura();
		//	Devuelve la lectura entre 0.0 y 360.0
		float lecturaf();
		//	Devuelve la version del detector.
		int version();
};
