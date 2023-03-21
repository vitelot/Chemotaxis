#include "main.h"
#include "extern.h"

extern void ScanCommandLineParams( int argc, char *argv[], char *name);
extern void HeatDynamics( Matrix *, Body *);

FILE *fperr;
struct Input In;

int main( int argc, char *argv[])
{

Body body;/* set of cells */
Matrix C; /* concentration */
char ProjName[128];
char inifile[128]="CELLS.INI";


 printf("\nProgram Cells Version %4.2f is running.\n\n",VERSION);
 fperr = stderr;

 ScanCommandLineParams( argc, argv, inifile);
 InputParameters(inifile);

 sprintf(ProjName, "%s", In.projname);

 BodyAlloc( &body, In.nc);
 BodyInit ( &body);

 MatrixAlloc( &C, In.grid, In.grid);
 MatrixInit ( &C, 0.0);

 HeatDynamics( &C, &body);

 BodyPrint( &body, In.projname);
 MatrixPrint( &C, In.projname, "Concentration");

 BodyFree ( &body);
 MatrixFree( &C);

 printf("\n------------------ END -----------------\n\n");
 return 0;
}
