#include "main.h"
#include "extern.h"

Real *RealVectorAlloc( int dim)
{
  Real *out;

  out = (Real *) calloc( (size_t) (dim+1), sizeof(Real)); /* better one location more */

  if(!out) {
    perr("Allocation error in IntVectorAlloc. Exit.\n");
    exit(1);
  }
  return out;
}

Real *RealVectorRealloc( Real *v, int newdim)
{
  Real *out;

  out = (Real *) realloc( (void *) v, (size_t) newdim*sizeof(Real));

  if(!out) {
    perr("Reallocation error in RealVectorRealloc. Exit.\n");
    exit(1);
  }
  return out;
}

void RealVectorFree( Real *v)
{ free( (Real *) v); }

void RealVectorInit( Real *v, Int dim, Real value)
{
  register Int i;
  for( i=0; i<dim; i++)
    v[i] = value;
}

void RealVectorCopy( Real *a, Real *b, int dim)
{
  /* copy b onto a */
  register Int i;

  for( i=0; i<dim; i++)
    a[i] = b[i];
}
