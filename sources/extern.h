#ifndef __EXTERN_H
#  define __EXTERN_H

#include "main.h"

/************************ Global variables ************************/
extern FILE *fperr; /* file pointer for error messages */
extern struct Input In; /* Input parameters */
/******************************************************************/


/* Body.c */
extern void BodyAlloc( Body *B, int nc);
extern void BodyRealloc(Body *B, int NC);
extern void BodyFree ( Body *B);
extern void BodyCopy( Body *dest, Body *source);
extern void BodySwap( Body *A, Body *B);
extern void BodyInit ( Body *B);
extern void BodyPrint( Body *B, const char *projname);

/*  Matrix.c */
extern void MatrixAlloc( Matrix *A, int rows, int cols);
extern void MatrixFree ( Matrix *A);
extern void MatrixInit ( Matrix *A, double val);
extern void MatrixCopy ( Matrix *dest, Matrix *source);
extern void MatrixPrint( Matrix *A, const char *projname, const char *name);

/*  IntVector.c */
/* extern int *IntVectorAlloc( int dim); */
/* extern int *IntVectorRealloc( int *v, int newdim); */
/* extern void IntVectorFree( int *v); */
/* extern void IntVectorInit( int *v, int dim, int value); */
/* extern int  IntVectorIntersect( int *a, int dima, int *b, int dimb); */
/* extern void IntVectorCopy( int *a, int *b, int dim); */

/*  RealVector.c */
/* extern Real *RealVectorAlloc( int dim); */
/* extern Real *RealVectorRealloc( Real *v, int newdim); */
/* extern void  RealVectorFree( Real *v); */
/* extern void  RealVectorInit( Real *v, int dim, Real value); */
/* extern void  RealVectorCopy( Real *a, Real *b, int dim); */

/*  Error.c */
extern void perr(const char *fmt, ...);
extern void OpenError(char *errfilename);
extern void CloseError(void);
extern void AllocError( char *s);
extern void FileOpenError( char *s);

/*  Input.c */
extern void InputParameters( char *ini_file);

/*  ShowProgress.c */
extern void ShowProgress(float fraction, float total);

#endif
