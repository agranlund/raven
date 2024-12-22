#include <osbind.h>
#include <linea.h>

int pts[4][2] = { 
	320, 050,
	120, 150,
	520, 150,
	320, 050
};

main()
{
	lineaport *theport;
	int fillpat[4], y;

	stuffbits(&fillpat[0], "1100110011001100");
	stuffbits(&fillpat[1], "0110011001100110");
	stuffbits(&fillpat[2], "0011001100110011");
	stuffbits(&fillpat[3], "1001100110011001");

	theport = a_init();

	theport -> plane0 = 1;
	theport -> plane1 = 0;
	theport -> plane2 = 0;
	theport -> plane3 = 0;
	
	theport -> writemode = 2;
	theport -> patptr    = fillpat;
	theport -> planefill = 0;
	theport -> clipflag  = 0;

	for(y=50; y<150; y++)
		a_fillpoly(y, pts, 3);

	Cconin();
}
