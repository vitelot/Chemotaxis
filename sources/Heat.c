#include "main.h"
#include "extern.h"

extern Real J(Body *, Point2D *);

void Heat( Matrix *Concentration, Matrix *oldConcentration, 
	   Body *body, 
	   Real dt) {

  register int i,j;
  int grid = Concentration->rows;
  Real dx = In.l/grid;
  Real dy = In.l/grid;
  int im, ip, jm, jp; /* i-1, i+1, j-1, j+1 according to PBC */
  Real r = dt/dx/dx, Jij;
  Point2D P;

  double **newC = Concentration->M; /* avoid nasty '->' op all the time */
  double **C = oldConcentration->M; /* idem                             */

  MatrixCopy( oldConcentration, Concentration);

  Loop(i, grid) {

    P.x = i * dx;

    /* periodic boundary conditions */
    if( (im=i-1) <  0    ) im += grid;
    if( (ip=i+1) >= grid ) ip -= grid;
    /********************************/

    Loop(j, grid) {

      P.y = j  * dy;
      Jij = J( body, &P);

      /* periodic boundary conditions */
      if( (jm=j-1) <  0    ) jm += grid;
      if( (jp=j+1) >= grid ) jp -= grid;
      /********************************/

      newC[i][j] = 
	C[i][j] +
	r*(C[im][j] + C[ip][j] + C[i][jp] + C[i][jm] - 4.0*C[i][j]) +
	dt * (Jij - C[i][j]);
    }
  }
  
}
