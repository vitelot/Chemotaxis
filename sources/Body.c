#include "main.h"
#include "extern.h"
#include <time.h>

#define MAXFAIL 1000000 /* max nr of failed cell placement, in case of core */

void BodyAlloc(Body *B, int NC) {

  B->n = NC;
  
  B->cell = (Cell *) malloc( (size_t) NC * sizeof(Cell));
}

void BodyRealloc(Body *B, int NC) {

  B->n = NC;
  
  B->cell = (Cell *) realloc( B->cell, NC * sizeof(Cell));
}

void BodyFree( Body *B) {
  B->n = 0;
  free(B->cell);
  B->cell = NULL;
}

void BodyCopy( Body *dest, Body *source) {
  register int i;
  int NC = dest->n;

  if(dest->n != source->n) {
    perr("Cannot copy two bodies of different size (%d != %d).\n",
	 dest->n, source->n);
    exit(1);
  }

  Loop( i, NC) {
    dest->cell[i].p.x = source->cell[i].p.x;
    dest->cell[i].v.x = source->cell[i].v.x;
    dest->cell[i].p.y = source->cell[i].p.y;
    dest->cell[i].v.y = source->cell[i].v.y;
  }
}

void BodySwap( Body *A, Body *B) {
  register int i;
  int NC = A->n;
  Real tmp;

  if(A->n != B->n) {
    perr("Cannot swap two bodies of different size (%d != %d).\n",
	 A->n, B->n);
    exit(1);
  }

  Loop( i, NC) {
    tmp = A->cell[i].p.x;
    A->cell[i].p.x = B->cell[i].p.x;
    B->cell[i].p.x = tmp;
    tmp = A->cell[i].v.x;
    A->cell[i].v.x = B->cell[i].v.x;
    B->cell[i].v.x = tmp;
    
    tmp = A->cell[i].p.y;
    A->cell[i].p.y = B->cell[i].p.y;
    B->cell[i].p.y = tmp;
    tmp = A->cell[i].v.y;
    A->cell[i].v.y = B->cell[i].v.y;
    B->cell[i].v.y = tmp;
  }
}

void BodyInit( Body *B) {
  /* load starting positions and velocities if file "+positions" exists */
  register int i;
  FILE *fp;

  if( (fp=fopen("+positions", "r")) ) {
    char line[128];
 
    printf("File \"+positions\" with starting cell positions found. Loading it.\n\n");

    /* count how many cells there are */
    i=0;
    while(1) {
      fgets( line, 126, fp);
      if(feof(fp)) break;
      if( line[0] == '#') continue; /* lines starting with # are comments */
      ++i;
/*       printf("### %d %s", i, line); */
    }
    if( i != B->n ) {
      printf("Found %d cells instead of %d. Using %d from now on.\n\n",
	     i, B->n, i);
      BodyRealloc( B, i);
      In.nc = i;
    }
    rewind(fp);
    /* finally load cell positions */
    i=0;
    while(1) {
      fgets( line, 126, fp);
      if(feof(fp)) break;
      if( line[0] == '#') continue; /* lines starting with # are comments */
      sscanf(line, "%*d %lg %lg %lg %lg", 
	     &B->cell[i].p.x, &B->cell[i].p.y,
	     &B->cell[i].v.x, &B->cell[i].v.y);
      ++i;
    }
    fclose(fp);

  } else {
  /* assign cells random positions inside the square with edge In.l
     and zero velocities */

    srand(time(NULL));
    if(!In.twocf) { /* no problms with overlap */
      Loop(i,B->n) {
	B->cell[i].p.x = rand()/(RAND_MAX+1.0)*In.l;
	B->cell[i].p.y = rand()/(RAND_MAX+1.0)*In.l;
	B->cell[i].v.x = 0.0;
	B->cell[i].v.y = 0.0;
      }
    } else {
      Bool failed;
      register int j;
      Int countfail;
      Real L = In.l;
      Real xtmp, ytmp, dx, dy;
      if( In.l/In.radius*In.l/3.14/In.radius <= In.nc) {
	printf("Not enough space for cells without overlap. reduce number of cells (<%d).\n",
	       (int)(In.l/In.radius*In.l/3.14/In.radius));
	exit(1);
      }
      Loop(i,B->n) {
	B->cell[i].v.x = 0.0;
	B->cell[i].v.y = 0.0;
      }
      B->cell[0].p.x = rand()/(RAND_MAX+1.0)*L;
      B->cell[0].p.y = rand()/(RAND_MAX+1.0)*L;
      for(i=1; i<(B->n); i++) {
	countfail = 0;
	do {
	  failed = FALSE;
	  xtmp = rand()/(RAND_MAX+1.0)*L;
	  ytmp = rand()/(RAND_MAX+1.0)*L;
	  Loop(j,i) {
	    dx = xtmp - B->cell[j].p.x;
	    dy = ytmp - B->cell[j].p.y;
/* 	    printf("### i:%d xi:%g yi:%g j:%d xj:%g yj:%g dx:%g dy:%g\n", */
/* 		   i,B->cell[j].p.x,B->cell[j].p.y, */
/* 		   j,xtmp,ytmp, */
/* 		   dx,dy); */
	    /* PBC */
	    if(dx>0) while(fabs(dx)>L/2.) dx -= L;
	    if(dx<0) while(fabs(dx)>L/2.) dx += L;
	    if(dy>0) while(fabs(dy)>L/2.) dy -= L;
	    if(dy<0) while(fabs(dy)>L/2.) dy += L;
/* 	    printf("*** i:%d j:%d dx:%g dy:%g\n",i,j,dx,dy); */

	    if( dx*dx + dy*dy <= 4.*In.radius*In.radius) {
	      ++countfail;
	      failed = TRUE;
/* 	      printf("??? d/2r:%g\n", sqrt(dx*dx + dy*dy)/2./In.radius); */
	      break;
	    }
	  } 
	  if(countfail > MAXFAIL) {
	    printf("Cannot find an empty space for cell #%d after %d trials. Set a lower density.\n",
		   i, MAXFAIL);
	    exit(1);
	  }
	} while(failed);
	B->cell[i].p.x = xtmp;
	B->cell[i].p.y = ytmp;
      }
    }
  }
}

void BodyPrint( Body *B, const char *projname) {
  register int i;
  char filename[128];
  FILE *fp;

  if(! In.printdat) return;

  sprintf(filename, "=%s.%s.dat", projname, "CellPositions");
  printf("Saving cells positions and velocities onto file \"%s\".\n",
         filename);

  fp = fopen( filename, "w");
  if( !fp)  FileOpenError(filename);

  fprintf(fp, "#%5s\t%10s\t%10s\t%10s\t%10s\n",
	  "i", "x", "y", "vx", "vy");

  Loop( i, B->n) {
    fprintf(fp, "%5d\t%10g\t%10g\t%10g\t%10g\n", i,
	    B->cell[i].p.x,
	    B->cell[i].p.y,
	    B->cell[i].v.x,
	    B->cell[i].v.y);
  }

  fclose(fp);

  switch(In.rungrace) {
    char cmd[128];
    char gnufile[128];
  case 1: /* xmgrace */
    sprintf(filename, "=%s.%s.agr", projname, "CellsPositions");
    fp = fopen( filename, "w");
    if( !fp)  FileOpenError(filename);

    fprintf(fp,"@    world 0, 0, %f, %f\n",In.l,In.l);
/*     fprintf(fp,"@    xaxis  label \"x\"\n"); */
/*     fprintf(fp,"@    yaxis  label \"y\"\n"); */
/*     fprintf(fp,"@    xaxis  ticklabel format decimal\n"); */
/*     fprintf(fp,"@    xaxis  ticklabel prec 1\n"); */
/*     fprintf(fp,"@    yaxis  ticklabel format decimal\n"); */
/*     fprintf(fp,"@    yaxis  ticklabel prec 1\n"); */
/*     fprintf(fp,"@    s0 symbol 1\n"); */
/*     fprintf(fp,"@    s0 symbol fill color 2\n"); */
/*     fprintf(fp,"@    s0 symbol fill pattern 1\n"); */
/*     fprintf(fp,"@    s0 line type 0\n"); */
    fprintf(fp,"@    title \"L=%.1f N=%d\"\n", In.l, In.nc);

    fprintf(fp,"@    frame linestyle 0\n");
    fprintf(fp,"@    xaxis  label \"x\"\n");
    fprintf(fp,"@    xaxis  tick major %f\n", In.l/In.grid);
    fprintf(fp,"@    xaxis  tick minor 0\n");
    fprintf(fp,"@    xaxis  tick major grid on\n");
    fprintf(fp,"@    xaxis  ticklabel off\n");

    fprintf(fp,"@    yaxis  label \"y\"\n");
    fprintf(fp,"@    yaxis  tick major %f\n", In.l/In.grid);
    fprintf(fp,"@    yaxis  tick minor 0\n");
    fprintf(fp,"@    yaxis  tick major grid on\n");
    fprintf(fp,"@    yaxis  ticklabel off\n");

    fprintf(fp,"@    s0 fill type 1\n");
    fprintf(fp,"@    s0 fill color 2\n");

    Loop( i, B->n) {
      fprintf(fp, "%g %g %g\n",
	      B->cell[i].p.x,
	      B->cell[i].p.y,
	      In.radius);
    }

    fclose(fp);

/*     sprintf( cmd, "xmgrace -autoscale none -free %s&", filename); */
    sprintf( cmd, "xmgrace -autoscale none -free -graphtype fixed -settype xyr %s&", filename);
    system(cmd);
    break;

  case 2: /* gnuplot */
    sprintf(gnufile, "=%s.%s.g", projname, "CellsPositions");
    fp = fopen( gnufile, "w");
    if( !fp)  FileOpenError(gnufile);

    fprintf(fp,"set terminal png\n");
    fprintf(fp,"set xrange [0:%f]\n",In.l);
    fprintf(fp,"set xtics %f\n",In.l/In.grid);

    fprintf(fp,"set yrange [0:%f]\n",In.l);
    fprintf(fp,"set ytics %f\n",In.l/In.grid);

    fprintf(fp,"set grid\n");
    fprintf(fp,"set format \"\"\n");
    fprintf(fp,"set title \"L=%.1f N=%d\"\n", In.l, In.nc);

/*     fprintf(fp,"plot \"%s\" u 2:3 w p pt 3 ps 4 notitle\n",filename); */
    fprintf(fp,"plot \"%s\" u 2:3 \n",filename);
    fprintf(fp,"pause -1\n");

    fclose(fp);

/*     printf("\nGNUPLOT: press ENTER to exit.\n"); */

    sprintf(cmd, "gnuplot %s > %s.png && %s %s.png&", 
	    gnufile, filename, In.pngshow, filename);
    system(cmd);
    break;

  default:
    break;
  }

}
