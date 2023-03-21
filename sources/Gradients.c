#include "main.h"
#include "extern.h"

Real GradX(Matrix *Concentration, Point2D *P) {
  Real **C = Concentration->M;
  int grid = Concentration->cols;
  Real dx = In.l/grid;
  Real dy = In.l/grid;
  int i = P->x / dx;
  int j = P->y / dy;
  int ip, im, jp, jm, ip2;
  Real Gsw, Gnw, Gne, Gse;
  Real ux, uy;

  /* periodic boundary conditions */
  if( (im=i-1) <  0    ) im += grid;
  if( (ip=i+1) >= grid ) ip -= grid;
  if( (ip2=i+2)>= grid ) ip2-= grid;
  if( (jm=j-1) <  0    ) jm += grid;
  if( (jp=j+1) >= grid ) jp -= grid;
  /********************************/

/*   printf("### i,j,ip,jp,im,jm,ip2 %d,%d,%d,%d,%d,%d,%d\n",i,j,ip,jp,im,jm,ip2); */

  Gsw = (C[ip ][j ]-C[im][j ])*.5/dx; /* gradx at South West point */
  Gnw = (C[ip ][jp]-C[im][jp])*.5/dx; /* gradx at North West point */
  Gne = (C[ip2][jp]-C[i ][jp])*.5/dx; /* gradx at North East point */
  Gse = (C[ip2][j ]-C[i ][j ])*.5/dx; /* gradx at South East point */
  
  ux = P->x /dx -(double)i; 
  uy = P->y /dy -(double)j;

  /* bilinear interpolation */
  return Gsw + (Gse-Gsw)*ux + (Gnw-Gsw)*uy + (Gsw+Gne-Gse-Gnw)*ux*uy;
}

Real GradY(Matrix *Concentration, Point2D *P) {
  Real **C = Concentration->M;
  int grid = Concentration->rows;
  Real dx = In.l/grid;
  Real dy = In.l/grid;
  int i = P->x / dx;
  int j = P->y / dy;
  int ip, im, jp, jm, jp2;
  Real Gsw, Gnw, Gne, Gse;
  Real ux, uy;

  /* periodic boundary conditions */
  if( (im=i-1) <  0    ) im += grid;
  if( (ip=i+1) >= grid ) ip -= grid;
  if( (jp2=j+2)>= grid ) jp2-= grid;
  if( (jm=j-1) <  0    ) jm += grid;
  if( (jp=j+1) >= grid ) jp -= grid;
  /********************************/

  Gsw = (C[i ][jp ]-C[i ][jm])*.5/dy; /* grady at South West point */
  Gnw = (C[i ][jp2]-C[i ][j ])*.5/dy; /* grady at North West point */
  Gne = (C[ip][jp2]-C[ip][j ])*.5/dy; /* grady at North East point */
  Gse = (C[ip][jp ]-C[ip][jm])*.5/dy; /* grady at South East point */

  ux = P->x /dx -(double)i; 
  uy = P->y /dy -(double)j;

  return Gsw + (Gse-Gsw)*ux + (Gnw-Gsw)*uy + (Gsw+Gne-Gse-Gnw)*ux*uy;
}
