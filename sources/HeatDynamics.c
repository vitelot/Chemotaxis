#include "main.h"
#include "extern.h"

extern void Heat( Matrix *C, Matrix *oldC, Body *body, Real dt);
extern void Euler2Dynamics( Matrix *Field, Body *B0, Body *B1, Real t, Real dt);
extern void VerletDynamics( Matrix *Field, Body *B0, Body *B1, Real t, Real dt);

static void TrackCells( Body *B, FILE *fp, double t);
static void KineticEnergy( Body *B, FILE *fp, double t);

void HeatDynamics( Matrix *C, Body *body) {
  register int i=0, j=0, k=0;
  char file[128];
  Real t=0.0;
  Real dt = In.dt;
  Real tmax = In.tmax;
  Body *body0 = body; /* body at time t-1 */
  Body body1;         /* body at time t   */
  Matrix oldC;

  FILE *fpT;
  FILE *fpE;

  if(In.track) {
    char filename[128];
    sprintf(filename, "=%s.%s.dat", In.projname, "TrackCells");
    fpT = fopen(filename, "w");
  }
  if(In.kinE) {
    char filename[128];
    sprintf(filename, "=%s.%s.dat", In.projname, "KineticEnergy");
    fpE = fopen(filename, "w");
  }

  MatrixAlloc( &oldC, C->rows, C->cols);
  BodyAlloc( &body1, body0->n);
  
  /* print initial configuration */
  sprintf(file, "%s_%06d", In.projname, i);
  BodyPrint( body, file);

  printf("Doing calculations (saving data each %.2f%%)\n", 
	 In.savestep/In.tmax*In.dt*100);

  /* one step dynamics: important when C!=0 */
  Euler2Dynamics( C, body0, &body1, t, dt);

  for(; t<tmax; t+=dt) {
    Heat( C, &oldC, &body1, dt);
    VerletDynamics( C, body0, &body1, t, dt);
    
    if(In.track>0 && (++j % In.track)==0)
      TrackCells( &body1, fpT, t);
    if(In.kinE>0 && (++k % In.kinE)==0)
      KineticEnergy( &body1, fpE, t);

    if(In.savestep>0 && (++i % In.savestep)==0) {
      sprintf(file, "%s_%06d", In.projname, i);
      BodyPrint( &body1, file);
      MatrixPrint( C, file, "Concentration");
    }
    ShowProgress( t, tmax);

  }
  printf("Done.                 \n\n");

  MatrixFree( &oldC);
  BodyFree(&body1);

  if(In.track) fclose(fpT);
  if(In.kinE)  fclose(fpE);
}

static void TrackCells( Body *B, FILE *fp, double t) {
  register int i;

  Loop(i,In.nc) {
    fprintf(fp, "%d %f %f %f %f %f\n", i, t,
	    B->cell[i].p.x, B->cell[i].p.y,
	    B->cell[i].v.x, B->cell[i].v.y);
  }
}

static void KineticEnergy( Body *B, FILE *fp, double t) {
  register int i;
  Real Ek = 0.0;

  Loop(i,In.nc) {
    Ek +=  B->cell[i].v.x * B->cell[i].v.x + B->cell[i].v.y * B->cell[i].v.y;
  }

  Ek *= .5/In.nc;

  fprintf( fp, "%f %g\n", t, Ek);
}
