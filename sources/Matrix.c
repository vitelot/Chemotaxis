#include "main.h"
#include "extern.h"

void MatrixAlloc(Matrix *A, int rows, int cols)
{
  double **t;
  register int i;

  A->rows = rows;
  A->cols = cols;

	t = (double **) malloc( (size_t) rows * sizeof(double *) );
	if (!t) perr("Allocation ERROR 1 in  funct MATRIX_ALLOC\n\n");
	for (i=0; i<rows; i++)
	{
	  t[i] = (double *) malloc( (size_t) cols * sizeof(double) );
	  if (!t[i]) perr("Allocation ERROR 2 in funct MATRIX_ALLOC\n\n");
	}

  A->M = t;
}

void MatrixFree(Matrix *A) {
  register int i;
  double **t = A->M;

  for (i=0; i<(A->rows); i++)
    free( (double *) t[i]);

  free( (double **) t);

  A->rows = 0;
  A->cols = 0;
  A->M = NULL;
}

void MatrixInit(Matrix *A, double val) {
  /* load concentration matrix if file "+field" exists */
  register int i,j;
  FILE *fp;

  if( (fp=fopen("+field", "r")) ) {
    char line[128];
    double field;
    int cols, rows;
 
    printf("File \"+field\" with starting concentrations found.\nScanning it.\n\n");

    cols=0;
    while(1) {
      fgets( line, 126, fp);

      if( line[0] == '#') continue; /* lines starting with # are comments */
      if(sscanf(line, "%lg", &field)== -1) break;
      ++cols;
    }
    printf("Found %d columns in matrix\n", cols);
    rewind(fp);

    i=0;
    while(1) {
      fgets( line, 126, fp);
      if(feof(fp)) break;
      if( line[0] == '#') continue; /* lines starting with # are comments */
      if(sscanf(line, "%lg", &field)== -1) continue;
      ++i;
    }
    printf("Found %d data in matrix\n", i);
    if(i%cols != 0) {
      printf("It seems that the loaded matrix is not rectangular [rest:%d]. Exit.\n", i%cols);
      exit(1);
    }
    rows = i/cols;
    printf("Matrix found to be (%d, %d).\n", rows, cols);

    if(A->rows != rows || A->cols != cols) {
      printf("Matrix dimensions do not fit grid dimensions. Exit.\n");
      exit(1);
/*       printf("Reallocating memory to fit new matrix dimensions.\n"); */
/*       MatrixFree(A); */
/*       MatrixAlloc( A, rows, cols); */
    }
    rewind(fp);
    printf("Loading it, finally.\n");

    i = j = 0;
    while(1) {
      fgets( line, 126, fp);
      if(feof(fp)) break;
      if( line[0] == '#') continue; /* lines starting with # are comments */
      if(sscanf(line, "%lg", &field)== -1) {++i; j=0; continue;}
      A->M[i][j] = field;
      ++j;
    }

    fclose(fp);

/*     MatrixPrint(A, "init", "test"); */

    printf("\n");

  } else {
    /* initialize to a given constant value */
    for (i=0; i<(A->rows); i++)
      for (j=0; j<(A->cols); j++)
	A->M[i][j] = val;
  }

}

void MatrixPrint(Matrix *A, const char *projname, const char *name) {
  register int i,j;
  char filename[128];
  FILE *fp;

  if(! In.printdat) return;

  sprintf(filename, "=%s.%s.dat", projname, name);
  printf("Saving %s matrix onto file \"%s\".\n",
         name, filename);

  fp = fopen( filename, "w");
  if( !fp)  FileOpenError(filename);

/*   fprintf(fp, "#%5s\t%5s\t%10s\n", "row", "col", "C"); */

  Loop( i, A->rows) {
    Loop( j, A->cols) {
/*       fprintf(fp, "%5d\t%5d\t%10g\n", i,j, A->M[i][j]); */
      fprintf(fp, "%g\n", A->M[i][j]);
    }
    fprintf(fp, " \n");
  }

  fclose(fp);

  if(In.rungnuplot) {
    char gnuplotfile[128] = "plot.g";
    char cmd[128];

    sprintf(gnuplotfile, "=%s.%s.g", projname, name);
    fp = fopen( gnuplotfile, "w");
      if( !fp)  FileOpenError(gnuplotfile);

      fprintf(fp, "set contour both\n");
      fprintf(fp, "set cntrparam level 10\n");
      fprintf(fp, "set hidden3d\n");
      fprintf(fp, "splot \"%s\" w l\n", filename);
      fprintf(fp, "pause -1\n");

    fclose(fp);

    printf("\nGNUPLOT: press ENTER to exit.\n");

    sprintf(cmd, "gnuplot %s", gnuplotfile);
    system(cmd);
  }
}

void MatrixCopy( Matrix *dest, Matrix *source)
{
  register int i, j;

  if( (dest->rows != source->rows) || (dest->cols != source->cols)) {
    perr("Cannot copy two matrices with different dimensions.\n");
    exit(1);
  }

  for (i=0; i<(dest->rows); i++)
    for (j=0; j<(dest->cols); j++)
      dest->M[i][j] = source->M[i][j];
}
