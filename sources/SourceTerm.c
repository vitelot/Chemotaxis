#include "main.h"
#include "extern.h"

#define X(i) (body->cell[(i)].p.x)
#define Y(i) (body->cell[(i)].p.y)

/* single cell contribution */
static Real Ji( Real xi, Real yi, Real x, Real y) {

  Real dx = fabs(xi-x);
  Real dy = fabs(yi-y);
  Real L = In.l; /* box edge */

  /* periodic boundary conditions */
  if( dx>.5*L ) dx -= L;
  if( dy>.5*L ) dy -= L;

  if( dx*dx + dy*dy < In.radius*In.radius )
    return In.alpha;

  return 0.0;

}

/* total contribution */
Real J(Body *body, Point2D *pt) {
  register int i;
  Real sum=0.0;

  Loop(i, body->n)
    sum += Ji( X(i), Y(i), pt->x, pt->y);
  return sum;
}
