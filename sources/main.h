#ifndef __MAIN_H
# define __MAIN_H

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "Version.h"

#define Loop(i,N) for( (i)=0; (i)<(N); ++(i))
#define Vector2D Point2D

#define P(x) printf("### check point %d ###\n", (x));  /* for debugging */

typedef double Real;
typedef int Int;
typedef enum {FALSE, TRUE} Bool;

typedef struct {
  Real x;
  Real y;
} Point2D;

typedef struct {
  Point2D p; /* position */
  Vector2D v; /* velocity */
} Cell;

typedef struct {
  int n;
  Cell *cell;
} Body; /* set of n cells */

typedef struct {
  int rows;
  int cols;
  Real **M;
} Matrix;

struct Input {
  int nc;
  double l;
  int grid;
  double dt;
  double tmax;
  double radius;
  double alpha;
  double inelastic;
  double friction;
  double frctime;
  double trainfrict;
  double traintime;
  char projname[128];
  int twocf;
  int savestep;
  int track;
  int kinE;
  int printdat;
  int rungrace;
  char pngshow[128];
  int rungnuplot;
};

#endif
