#include "main.h"
#include "extern.h"

extern Real GradX(Matrix *Concentration, Point2D *P);
extern Real GradY(Matrix *Concentration, Point2D *P);

static void ExternalForce( Point2D *P, Vector2D *V, Vector2D *oldV, Real t, Vector2D *F);
static void TrainFriction( Vector2D *V2, Vector2D *V1, Vector2D *Vperp);
static void TwoCenterForce( Body *B, int c, Vector2D *F);
static void Fp( Real dx, Real dy, Vector2D *V, Real r, Vector2D *F);

/* Second order Euler algorithm for the initial timestep */
void Euler2Dynamics( Matrix *Field, 
		     Body *B0, Body *B1,
		     Real t, Real dt) {

  register int i;
  int NC = B0->n;
  Real L = In.l; /* edge */
  Real dt2 = .5*dt*dt;
  Real tmp;
  Real Fx, Fy;
  Vector2D F;
  Point2D P;
  Vector2D V, oldV;

  Loop( i, NC) {
    P.x = B0->cell[i].p.x;
    P.y = B0->cell[i].p.y;
    V.x = B0->cell[i].v.x;
    V.y = B0->cell[i].v.y;
    
    oldV.x = V.x; /* useless but pro forma */
    oldV.y = V.y;

    Fx = GradX( Field, &P);
    Fy = GradY( Field, &P);

    ExternalForce(&P,&V,&oldV, t, &F);
    Fx += F.x;
    Fy += F.y;

    TwoCenterForce(B0,i, &F);
    Fx += F.x;
    Fy += F.y;

    tmp = B0->cell[i].p.x + dt * B0->cell[i].v.x + dt2 * Fx;
    /* PBC */
    if(tmp >= L)   tmp -=  (int) (tmp/L) * L;
    if(tmp <  0.0) tmp -= ((int) (tmp/L) - 1 )* L;
    /*******/
    B1->cell[i].p.x = tmp;
    B1->cell[i].v.x = B0->cell[i].v.x + dt * Fx;

    tmp = B0->cell[i].p.y + dt * B0->cell[i].v.y + dt2 * Fy;
    /* PBC */
    if(tmp >= L)   tmp -=  (int) (tmp/L) * L;
    if(tmp <  0.0) tmp -= ((int) (tmp/L) - 1 )* L;
    /*******/
    B1->cell[i].p.y = tmp;
    B1->cell[i].v.y = B0->cell[i].v.y + dt * Fy;
  }

}

void VerletDynamics( Matrix *Field, 
		     Body *B0, Body *B1,
		     Real t, Real dt) {
  /* in input:
       B0 is body at time t-1 
       B1 is body at time t 
  */
  register int i;
  int NC = B0->n;
  Real L = In.l; /* edge */
  Real dt2 = dt*dt;
  Real tmp;
  Real shift;
  Real Fx, Fy;
  Vector2D F;
  Point2D P;
  Vector2D V;
  Vector2D oldV;

  Loop( i, NC) {
    /* get the position of the cell */
    P.x = B1->cell[i].p.x;
    P.y = B1->cell[i].p.y;
    V.x = B1->cell[i].v.x;
    V.y = B1->cell[i].v.y;

    /* velocity at time t-1 */
    oldV.x = B0->cell[i].v.x;
    oldV.y = B0->cell[i].v.y;

    Fx = GradX( Field, &P);
    Fy = GradY( Field, &P);

    ExternalForce(&P,&V,&oldV, t, &F);
    Fx += F.x;
    Fy += F.y;

    TwoCenterForce(B1,i, &F);
    Fx += F.x;
    Fy += F.y;

    /* update new position in B0 */
    tmp = 2.*B1->cell[i].p.x  - B0->cell[i].p.x + dt2 * Fx;
    /* update velocity. (dt*grad) is to get speed at t+dt  */
    B0->cell[i].v.x = .5 * (tmp - B0->cell[i].p.x) /dt + dt * Fx;
    /* PBC */
    if(tmp >= L) {
      shift = (int) (tmp/L) * L;
      tmp -=  shift;
      B1->cell[i].p.x -= shift; /* discontinuity in Vx otherwise */
    }
    if(tmp <  0.0) {
      shift = ((int) (tmp/L) - 1 )* L;
      tmp -= shift;
      B1->cell[i].p.x -= shift;
    }
    /*******/
    B0->cell[i].p.x = tmp;

    tmp = 2.*B1->cell[i].p.y - B0->cell[i].p.y + dt2 * Fy;
    /* update velocity */
    B0->cell[i].v.y = .5 * (tmp - B0->cell[i].p.y) /dt + dt * Fy;
    /* PBC */
    if(tmp >= L) {
      shift = (int) (tmp/L) * L;
      tmp -=  shift;
      B1->cell[i].p.y -= shift;
    }
    if(tmp <  0.0) {
      shift = ((int) (tmp/L) - 1 )* L;
      tmp -= shift;
      B1->cell[i].p.y -= shift;
    }
    /*******/
    B0->cell[i].p.y = tmp;

    /* now B0 has body at time t+1 and B1 at time t */
  }
  /* let B0 be again the body at time t-1 and B1 at time t */
  BodySwap( B0, B1);
}

static void ExternalForce( Point2D *P, Vector2D *V, Vector2D *oldV, Real t, Vector2D *F) {
  /* external conservative + friction forces */
  /* just friction for the moment */
  Vector2D Vperp;

  F->x = F->y = 0.0;

  if( t>In.frctime) {
    F->x += -In.friction * V->x;
    F->y += -In.friction * V->y;
  }

  if( In.trainfrict >0 && t>In.traintime) {
    TrainFriction( V,oldV, &Vperp);
/*     printf("### %g %g %g %g %g %g\n", V->x,V->y, */
/* 	   oldV->x,oldV->y, */
/* 	   Vperp.x, Vperp.y);  */
    F->x += -In.trainfrict * Vperp.x;
    F->y += -In.trainfrict * Vperp.y;

  }
  
  return;
}

void TrainFriction( Vector2D *V2, Vector2D *V1, Vector2D *Vperp) {
  /* Cells would prefer to run straight */
  /* V2 == velocity at time t; V1 == velocity at time t-1 */

  const Real epsilon = 1e-6;
  Real mod2V2 = V2->x*V2->x + V2->y*V2->y;       /* |V2|^2 */
  Real modV1  = sqrt(V1->x*V1->x + V1->y*V1->y); /* |V1| */
  Real modVperp; /* perpendicular to V1 */
  Real V2V1 = V2->x*V1->x + V2->y*V1->y; /* V2*V1 */
  Real tmp;

  if(modV1>epsilon) {
    tmp = V2V1/modV1;
    modVperp = sqrt(fabs(mod2V2 - tmp*tmp));

    if( ((V2->x - V1->x)*(-V1->y) + (V2->y - V1->y)*( V1->x)) > 0 ) {
      /* Vperp must point in the same direction of V2-V1 */
      Vperp->x = modVperp * (-V1->y) / modV1;
      Vperp->y = modVperp * ( V1->x) / modV1;
    } else {
      Vperp->x = modVperp * ( V1->y) / modV1;
      Vperp->y = modVperp * (-V1->x) / modV1;
    }      
  }

/*   printf("+++ %g %g %g %g %g\n", */
/* 	 modV1, */
/* 	 modVperp, */
/* 	 tmp, */
/* 	 mod2V2 - tmp*tmp, */
/* 	 V2V1); */

  return;
}

static void TwoCenterForce( Body *B, int c, Vector2D *F) {
  /* 2-body force between cell c and the rest */
  if(In.twocf==1) {
    register int i;
    int NC = B->n;
    Real L = In.l; /* edge */
    Real r = In.radius; /* force acts inside this distance */
    Real xc = B->cell[c].p.x, yc = B->cell[c].p.y; /* coord of cell c */
    Real dx, dy; /* diff of coord of cell i and c */
    Real fy, fx;
    Vector2D fp;

    fx = fy = 0.0;
    Loop(i, NC) {
      if(i==c) continue; /* no interaction with itself */
      dx = (xc - B->cell[i].p.x);
      dy = (yc - B->cell[i].p.y);
      /* PBC */
      if(dx>0) while(fabs(dx)>L/2.) dx -= L;
      if(dx<0) while(fabs(dx)>L/2.) dx += L;
      if(dy>0) while(fabs(dy)>L/2.) dy -= L;
      if(dy<0) while(fabs(dy)>L/2.) dy += L;
    
      Fp( dx, dy, &(B->cell[c].v), r, &fp);
      fx += fp.x;
      fy += fp.y;
    }
    F->x = fx;
    F->y = fy;
  }
  return;
}

static void Fp( Real x, Real y, Vector2D *V, Real r, Vector2D *F) {
  Real d2 = x*x + y*y;
  Real r2 = 4.*r*r;
  Real d;
  Real coeff = In.inelastic;
  Real vp = (V->x*x + V->y*y)/d2;/* component along d, divided d */

  if(d2 >= r2) { F->x = F->y = 0.0; return; }

/*   if(coeff < 1.0 ) { */
/*     V->x *= coeff; */
/*     V->y *= coeff; */
/*   } */

  d = sqrt(d2);
/*   F->x = (1./(d2*d2) - 1./(r2*r2)) * x/d - coeff * vp * x; */
/*   F->y = (1./(d2*d2) - 1./(r2*r2)) * y/d - coeff * vp * y; */
  F->x = (2.*r - d) *x/d - coeff * vp * x;
  F->y = (2.*r - d) *y/d - coeff * vp * y;
  return;
}
