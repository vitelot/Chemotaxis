#include "main.h"
#include "extern.h"

static void create_ini( char *);
static void printinfo(Bool);

void InputParameters( char *ini_file)
{
FILE *fp;
char line[128], *p;
int totlin=22;
double version;

    fp = fopen( ini_file, "r");
    if( !fp) {
      printf("Error opening file %s\n", ini_file);
      printf("Creating a default one.\n\n");
      create_ini( ini_file);
    }

    while( !feof(fp)) {
      fgets( line, 126, fp);
      if( line[0] == '#') continue; /* lines starting with # are comments */
      /* read up to # or line end */
      for( p=line; *p!='#' && *p!='\0'; p++) ;
      *p = '\0'; /* skip inline comments */

      if(sscanf(line, " VERSION %lf", &version)           ==1) {
	totlin--;
	if(version != VERSION) {
	  perr("The initialization file \"%s\" belongs to an old release %.2lf.\n", ini_file, version);
	  perr("Please delete it and rerun the program to create its new version.\n");
	  exit(1);
	}
	continue;
      }
      /*###################################################################*/
      if(sscanf(line, " NC         =   %d", &In.nc)      ==1) {totlin--; continue;}
      if(sscanf(line, " L          =   %lg",&In.l)       ==1) {totlin--; continue;}
      if(sscanf(line, " GRID       =   %d", &In.grid)    ==1) {totlin--; continue;}
      if(sscanf(line, " DT         =   %lg",&In.dt)      ==1) {totlin--; continue;}
      if(sscanf(line, " TMAX       =   %lg",&In.tmax)    ==1) {totlin--; continue;}
      if(sscanf(line, " RADIUS     =   %lg",&In.radius)  ==1) {totlin--; continue;}
      if(sscanf(line, " ALPHA      =   %lg",&In.alpha)   ==1) {totlin--; continue;}
      if(sscanf(line, " INELASTIC  =   %lg",&In.inelastic)==1) {totlin--; continue;}
      if(sscanf(line, " FRICTION   =   %lg",&In.friction)==1) {totlin--; continue;}
      if(sscanf(line, " FRICTTIME  =   %lg",&In.frctime) ==1) {totlin--; continue;}
      if(sscanf(line, " TRAINFRICT =   %lg",&In.trainfrict)==1) {totlin--; continue;}
      if(sscanf(line, " TRAINTIME  =   %lg",&In.traintime) ==1) {totlin--; continue;}
      if(sscanf(line, " TWOCF      =   %d", &In.twocf  ) ==1) {totlin--; continue;}

      if(sscanf(line, " PROJNAME   =   %s", In.projname) ==1) {totlin--; continue;}

      if(sscanf(line, " SAVESTEP   =   %d", &In.savestep)==1) {totlin--; continue;}
      if(sscanf(line, " TRACK      =   %d", &In.track   )==1) {totlin--; continue;}
      if(sscanf(line, " KINETIC_E  =   %d", &In.kinE    )==1) {totlin--; continue;}
      if(sscanf(line, " PRINT      =   %d", &In.printdat)==1) {totlin--; continue;}
      if(sscanf(line, " XMGRACE    =   %d", &In.rungrace)==1) {totlin--; continue;}
      if(sscanf(line, " PNGSHOW    =   %s", &In.pngshow )==1) {totlin--; continue;}
      if(sscanf(line, " GNUPLOT    =   %d", &In.rungnuplot)==1) {totlin--; continue;}
      
      /* if you reach here, the line is not recognized */
      perr("The following line in %s was not recognized:\n%s\n"
	   "Exiting.\n", ini_file, line);
      exit(1);
    }
     fclose(fp);

     if(totlin > 0) {
       printf("There are %d lines less in your %s file. Check for missing ones. Exiting.\n",
	      totlin, ini_file);
       exit(1);
     }
     if(totlin < 0) {
       printf("There are %d lines more in your %s file. Check for duplicates. Exiting.\n",
	      -totlin, ini_file);
       exit(1);
     }

     if(In.dt*In.grid/In.l*In.grid/In.l >0.21) {
       printf("ERROR: Time interval DT looks too large. [r=%g should be < 0.2]\n",
	      In.dt*In.grid/In.l*In.grid/In.l);
       printf("Suggested value: %g\n", 1./5./In.grid/In.grid*In.l*In.l);
       exit(1);
     }


     printinfo(FALSE);
     printinfo( TRUE);
}

#define  prbool(x) ((x)? "TRUE": "FALSE")

static void printinfo(Bool tofile)
{
FILE *fp=stdout;

  if(tofile) {
    char file[128];
    sprintf( file, "+%s.init", In.projname);
    fp = fopen( file, "w");
    if(!fp) FileOpenError( file);
  }

  fprintf(fp, "IIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIII\n");
  fprintf(fp, "IIIIIIII Main computational parameters IIIIIIIIIII\n");
  fprintf(fp, " NC        = %d\n",                           In.nc);
  fprintf(fp, " L         = %g\n",                           In.l);
  fprintf(fp, " GRID      = %d\n",                           In.grid);
  fprintf(fp, " DT        = %g\n",                           In.dt);
  fprintf(fp, " TMAX      = %g\n",                           In.tmax);
  fprintf(fp, " RADIUS    = %g\n",                           In.radius);
  fprintf(fp, " ALPHA     = %g\n",                           In.alpha);
  fprintf(fp, " INELASTIC = %g\n",                           In.inelastic);
  fprintf(fp, " FRICTION  = %g\n",                           In.friction);
  fprintf(fp, " FRICTTIME = %g\n",                           In.frctime);
  fprintf(fp, " TRAINFRICT= %g\n",                           In.trainfrict);
  fprintf(fp, " TRAINTIME = %g\n",                           In.traintime);
  fprintf(fp, " TWOCF     = %s\n",                  prbool(In.twocf));
  fprintf(fp, "IIIIIIII Basic control  IIIIIIIIIIIIIIIIIIIIIIIIIII\n");
  fprintf(fp, " PROJNAME  = \"%s\"\n",                       In.projname);
  fprintf(fp, "IIIIIIII Output control IIIIIIIIIIIIIIIIIIIIIIIIIII\n");
  fprintf(fp, " SAVESTEP  = %d\n",                           In.savestep);
  fprintf(fp, " TRACK     = %d\n",                           In.track);
  fprintf(fp, " KINETIC_E = %d\n",                           In.kinE);
  fprintf(fp, " PRINT     = %s\n",                  prbool(In.printdat));
  fprintf(fp, " XMGRACE   = %s\n",                  prbool(In.rungrace));
  fprintf(fp, " PNGSHOW   = %s\n",                           In.pngshow);
  fprintf(fp, " GNUPLOT   = %s\n",                  prbool(In.rungnuplot));
  fprintf(fp, "IIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIII\n\n");

  if(tofile) {
    fclose( fp);
    return;
  }

  fflush(stdout);
} 

static void create_ini( char *file)
{
  FILE *fel;

    fel = fopen( file, "w");
    if( !fel) {
      perr("Cannot create default %s ini-file. Aborting.\n", file);
      exit(1);
    }
    fprintf( fel, 
     "############################################################################\n"
     "### CELLS' default initialization file ######################################\n"
     "############################################################################\n");
    fprintf( fel, 
     " VERSION %4.2f                         ######################################\n", VERSION);
    fprintf( fel, 
     "############################################################################\n"
     "# Use the flag -h to have a list of command line options\n"
     "################### Master parameters ######################################\n"
     "#\n"
     " NC         =    3		# Number of cells\n"
     " L          =    1.0		# Edge of the box\n"
     " GRID       =    20		# Number of edge grid points\n");
    fprintf( fel, 
     " DT         =    2e-4		# time integration step [keep it < 1/(4*GRID^2)]\n"
     " TMAX       =    20.0		# final integration time\n"
     " RADIUS     =    0.15 	        # source term particle effective radius\n"
     " ALPHA      =    1.0		# intensity of source term\n");
    fprintf( fel, 
     " INELASTIC  =    0.0		# intensity of friction during collision\n"
     " FRICTION   =    0.0		# intensity of friction\n"
     " FRICTTIME  =    100.0		# time to switch on friction\n"
     " TRAINFRICT =    0.0		# intensity of friction perp to rails\n"
     " TRAINTIME  =    0.0		# time to switch on train-friction\n"
     " TWOCF      =    0		# include 2 center force\n"
     "#\n");
    fprintf( fel, 
     "################### Basic control  #########################################\n"
     "#\n"
     " PROJNAME   =   %s		# Project name\n"
     "#\n", In.projname);
    fprintf( fel,
     "################### Output control #########################################\n"
     "#\n"
     " SAVESTEP       =   10000         # Save cell positions on file each SAVESTEP time steps\n"
     " TRACK          =   0             # Save cell trajectories in only one HUGE file each TRACK steps\n"
     " KINETIC_E      =   0             # Save total kinetic energy each given steps\n"
     " PRINT          =   1             # Save cell positions on file\n"
     " XMGRACE        =   0             # Run xmgrace to show cells in box; 2 for gnuplot\n"
     " PNGSHOW        =   \"display\"     # Program to show png pictures\n"
     " GNUPLOT        =   0             # Run gnuplot to show concentration field\n");
    fprintf( fel, 
     "######################## END ###############################################\n"
     );
    fclose( fel);
    printf("Default \"%s\" initialization file created. You might edit it.\n", file);
    printf("Exiting...\n");
    exit(1);
}
