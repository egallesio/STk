/*
 *
 * n u m b e r . c				-- Numbers management
 *
 *
 * Numbers recognized by the interpreter are:
 *		- integer (which fit in a C long)
 *		- bignum  (arbitrary precision integer)
 *		- flonum  (represented as a C double)
 *
 * Bignum use the GNU gmp API. However to avoid to fall under the GPL terms
 * you can use the FGMP package (FGMP is a public domain implementation of
 * a subset of the GNU gmp library with the same API, written by Mark 
 * Henderson <markh@wimsey.bc.ca>). If your concern is speed, and if the
 * GPL is not a problem for you, use the gmp package. 
 *
 *
 *****
 *
 * Copyright © 1993-1999 Erick Gallesio - I3S-CNRS/ESSI <eg@unice.fr>
 * 
 *
 * Permission to use, copy, and/or distribute this software and its
 * documentation for any purpose and without fee is hereby granted, provided
 * that both the above copyright notice and this permission notice appear in
 * all copies and derived works.  Fees for distribution or use of this
 * software or derived works may only be charged with express written
 * permission of the copyright holder.  
 * This software is provided ``as is'' without express or implied warranty.
 *
 * This software is a derivative work of other copyrighted softwares; the
 * copyright notices of these softwares are placed in the file COPYRIGHTS
 *
 *
 *           Author: Erick Gallesio [eg@kaolin.unice.fr]
 *    Creation date: 12-May-1993 10:34
 * Last file update:  7-Jan-1999 09:33
 */

#include "stk.h"


#define SMALLNUMP(x)	(fabs(x) <= SMALLINT_MAX)
#define ISINT(x)	(floor(x) == (x))

/*  
 * This interface has been added for the Frank Schmuelling complex-number
 * package (Frank email is * <frank@zeus.ph1.Uni-Koeln.DE>). Don't rely on
 * it, since rationnal and complex numbers will be present in a future
 * release. As soon as the full tower will be implemented in STk, 
 * this door will be closed.
 */
   
#define do_addition 	_STk_do_addition
#define do_substract 	_STk_do_substract
#define do_multiply 	_STk_do_multiply
#define do_divide 	_STk_do_divide
#define bignum2double 	_STk_bignum2double


void _STk_do_multiply(SCM *x, SCM y);
void _STk_do_addition(SCM *x, SCM y);
static double do_compare(SCM x, SCM y);




static SCM makesmallint(long x)
{
  SCM z;

#ifndef COMPACT_SMALL_CST
  NEWCELL(z, tc_integer);
#endif
  SET_INTEGER(z, x); 
  return z;
}

static SCM makebignum(char *s)
{
  SCM z;
  NEWCELL(z, tc_bignum); 
  BIGNUM(z) = must_malloc(sizeof(MP_INT));
  mpz_init_set_str(BIGNUM(z), s, 10);
  return z;
}


/******************************************************************************
 * 
 * Conversion Functions 
 *
 ******************************************************************************/

static int digitp(char c, long base)
{
  c = ('0' <= c && c <= '9') ? c - '0':
      ('a' <= c && c <= 'f') ? c - 'a' + 10:
      ('A' <= c && c <= 'F') ? c - 'A' + 10:
      (c == '#')             ? 0      :
      100;
  return (c < base);
}

static SCM int2bignum(long n)
{
  SCM z;
  NEWCELL(z, tc_bignum);
  BIGNUM(z) = must_malloc(sizeof(MP_INT));
  mpz_init_set_si(BIGNUM(z), n);
  return z;
}

static SCM uint2bignum(unsigned long n)
{
  SCM z;
  NEWCELL(z, tc_bignum);
  BIGNUM(z) = must_malloc(sizeof(MP_INT));
  mpz_init_set_ui(BIGNUM(z), n);
  return z;
}

static double bignum2double(MP_INT *bn)
{
  char   *s= mpz_get_str(NULL, 10, bn);
  double d = (double) atof(s);
  
  free(s);
  return d;
}

static SCM double2integer(double n)	/* small or big depending of n's size */
{
  int i, j;
  size_t size = 20;
  char *tmp = NULL;
  SCM z;

  if (!ISINT(n)) Err("cannot convert this number to an integer", STk_makenumber(n));
  
  /* Try first to convert n to a long */
  if (((double) SMALLINT_MIN <= n) && (n <= (double) SMALLINT_MAX))
    return STk_makeinteger((long) n);

  /* n doesn't fit in a long => build a bignum. THIS IS VERY INEFFICIENT */  
  tmp = must_malloc(size);
  i = 0;
  if (n < 0.0) { tmp[i++] = '-'; n = -n; }
  do {
    if (i >= size) tmp = must_realloc(tmp, size *= 2);
    tmp[i++] = (int) fmod(n, (double) 10) + '0';
    n = floor(n / 10.0);
  }
  while (n > 0.0);
  tmp[i] = 0;

  /* Reverse the content of string tmp */
  for (i=i-1, j=(tmp[0]=='-'); i > j; i--, j++) {
    char c = tmp[i];
    tmp[i] = tmp[j];
    tmp[j] = c;
  }

  /* tmp contains a textual representation of n. Convert it to a bignum */
  z = makebignum(tmp);
  if (tmp) free(tmp);
  return z;
}



/* Convert a number to a C-string. Result must be freed if != from buffer */
char *STk_number2Cstr(SCM n, long base, char buffer[])
{
  char *s = buffer;
  
  switch (TYPE(n)) {
    case tc_flonum:
         if (base != 10) Err("base must be 10 for this number", n);
         sprintf(buffer, "%.15g", FLONM(n));
         if (strchr(buffer, '.') == NULL && strchr(buffer, 'e') == NULL) 
	   strcat(buffer, ".0");
	 return buffer;
    case tc_integer:
	 {
	   long tmp, val = INTEGER(n);
	   int u;

	   if (val < 0) {
	     val  = -val;
	     *s++ = '-';
	   }
	   for (s++, tmp=val; tmp >= base; tmp /= base) s++;
	   *s = '\000'; tmp = val;
	   do {
	     u = tmp % base;
	     *(--s) = u + ((u < 10) ? '0' : 'a'-10);
	     tmp   /= base;
	   }
	   while (tmp);
	   return buffer;
	 }
    case tc_bignum:
           s = must_malloc(mpz_sizeinbase(BIGNUM(n), base) + 2);
	   s = mpz_get_str(s, base, BIGNUM(n));
	   return s;
  }
  return NULL; /* never reached */
}

/* Return #f if the C string doesn't contain a valid number. */
/* UGLY! must be rewritten */
SCM STk_Cstr2number(char *str, long base)
{
  int i, adigit=0, isint=1, exact=' ', radix = 0;
  register char *p = str;

  for (i = 0; i < 2; i++) {
    if (*p == '#') {
      p += 1;
      switch (*p++) {
        case 'e': if (exact == ' ') { exact = 'e'; break; }  else return Ntruth;
	case 'i': if (exact == ' ') { exact = 'i'; break; }  else return Ntruth;
	case 'b': if (!radix) {base = 2;  radix = 1; break;} else return Ntruth;
	case 'o': if (!radix) {base = 8;  radix = 1; break;} else return Ntruth;
	case 'd': if (!radix) {base = 10; radix = 1; break;} else return Ntruth;
	case 'x': if (!radix) {base = 16; radix = 1; break;} else return Ntruth;
	default:  return Ntruth;
      }
      str += 2;
    }
    if (*p != '#') break;
  }

  if (*p == '-' || *p == '+') p+=1;
  if (*p == '#') goto End;
  while(digitp(*p, base)) { p+=1; adigit=1; if (*p == '#') isint = 0; }

  if (*p=='.'){
    isint = 0; p += 1;
    while(digitp(*p, base)) { p+=1; adigit=1; }
  }

  if (!adigit) goto End;

  if (*p && strchr("eEsSfFdDlL", *p)) {
    isint = 0;
    p += 1;
    if (*p == '-' || *p == '+') p+=1;
    if (!digitp(*p, base)) goto End; 
    p+=1;
    while (digitp(*p, base)) p+=1;
  }
  if (*p) goto End;

  /* Now we are sure it is a number. Find the more adequate type */
  if (isint) {
    MP_INT n;

    if (*str == '+') str+=1; /* mpz_init_set_str doesn't recognize +xyz !!! */
    if (mpz_init_set_str(&n, str, base) < 0) {
      mpz_clear(&n);
      return Ntruth;
    }
    if (mpz_cmp_si(&n, SMALLINT_MIN) >=0 && mpz_cmp_si(&n, SMALLINT_MAX) <= 0) {
      long num = mpz_get_si(&n);
      mpz_clear(&n);
      return (exact == 'i') ? STk_makenumber((double) num) : makesmallint(num);
    }
    /* It's a bignum */
    if (exact == 'i') return STk_makenumber(bignum2double(&n));
    else {
      SCM z;

      NEWCELL(z, tc_bignum);
      BIGNUM(z) = must_malloc(sizeof(MP_INT));
      mpz_init_set(BIGNUM(z), &n);
      mpz_clear(&n);
      return z;
    }
  }
  
  /* It's a float */
  if (exact == 'e') return Ntruth; /* e cannot be specified on e float */
  if (base == 10) {
    /* Replace sharp signs by 0 */
    for(p=str; *p; p++) 
      switch (*p) {
	case '#': *p = '0'; break;
	case 's': case 'S': case 'f': case 'F':
	case 'd': case 'D': case 'l': case 'L': *p = 'e';
      }
    return STk_makenumber((double) atof(str));
  }
End:
  return Ntruth;
}

long STk_integer_value(SCM x) /* Returns LONG_MIN if not representable as int */
{
  if (INTEGERP(x)) return INTEGER(x);
  if (BIGNUMP(x)) {
    if (mpz_cmp_si(BIGNUM(x), SMALLINT_MIN)>=0 && 
	mpz_cmp_si(BIGNUM(x), SMALLINT_MAX)<=0)
      return mpz_get_si(BIGNUM(x));
  }
  return LONG_MIN;
}

long STk_integer_value_no_overflow(SCM x) /* Returns LONG_MIN if not an integer */
{
  if (INTEGERP(x)) return INTEGER(x);
  if (BIGNUMP(x))  return mpz_get_si(BIGNUM(x));
  return LONG_MIN;
}

int STk_equal_numbers(SCM number1, SCM number2) /* number1 = number2 */
{
  return do_compare(number1, number2) == 0;
}

long STk_integer2long(SCM x)
{
  if (BIGNUMP(x))  return mpz_get_si(BIGNUM(x));
  return INTEGER(x);
}

unsigned long STk_integer2ulong(SCM x)
{
  if (BIGNUMP(x)) return mpz_get_ui(BIGNUM(x));
  return (unsigned long) INTEGER(x);
}

/******************************************************************************/

SCM STk_makenumber(double x)
{
  /* Floats are not stored in a struct obj since this leads to memory consumption
   * This  memory consumption due to alignment problems. 
   * For instance on a Sun 4, where double are 8 bytes, a struct obj with a double
   * in line will occupy 16 bytes whereas it occupies only 12 bytes if the double
   * is mallocated.
   * This change (94/08/29) will give worst performances when crunching numbers,
   * but use Fortran if this is your job :->
   */

  SCM z;
  NEWCELL(z,tc_flonum); 
  FLONM(z) = x; 
  return z;
}


SCM STk_makeinteger(long x)
{
  return (SMALLINT_MIN <= x && x <= SMALLINT_MAX) ? makesmallint(x): int2bignum(x);
}

SCM STk_makeunsigned(unsigned long x)
{
  return (x <= SMALLINT_MAX) ? makesmallint(x): uint2bignum(x);
}


/******************************************************************************
 *
 * Simple operations 
 *
 ******************************************************************************/

static SCM clone(SCM number)
{
  /* clone a number */
  switch (TYPE(number)) {
    case tc_integer: return makesmallint(INTEGER(number));
    case tc_bignum:  {
      		       SCM z;
		       
		       NEWCELL(z, tc_bignum); 
		       BIGNUM(z) = must_malloc(sizeof(MP_INT));
		       mpz_init(BIGNUM(z));
		       mpz_set(BIGNUM(z), BIGNUM(number));
		       return z;
		     }
    case tc_flonum:  return STk_makenumber(FLONM(number));
    default:	     /* This is not a number. Return the parameter unmodified. */
                     return number; /* Error will be signaled later */
  }
}

void _STk_do_multiply(SCM *x, SCM y)
{
  switch (TYPE(*x)) {
    case tc_integer:
         switch (TYPE(y)) {
	   case tc_integer:
	        {
		  double prod;
		  if (SMALLNUMP(prod=(double) INTEGER(*x) * INTEGER(y)))
		    SET_INTEGER(*x, (long) prod);
		  else {
		    MP_INT tmp;
		    
		    *x = int2bignum(INTEGER(*x));
		    mpz_init_set_si(&tmp, INTEGER(y));
		    mpz_mul(BIGNUM(*x), BIGNUM(*x), &tmp);
		    mpz_clear(&tmp);
		  }
		}
		break;
	   case tc_bignum:
		*x = int2bignum(INTEGER(*x));
		mpz_mul(BIGNUM(*x), BIGNUM(*x), BIGNUM(y));
		break;
	   case tc_flonum:
		*x = STk_makenumber(INTEGER(*x) * FLONM(y));
		break;
	 }
	 break;
    case tc_bignum:
	 switch (TYPE(y)) {
	   case tc_integer: 
	        {
		  SCM tmp = int2bignum(INTEGER(y));
		  mpz_mul(BIGNUM(*x), BIGNUM(*x), BIGNUM(tmp));
		}
		break;
	   case tc_bignum:
		mpz_mul(BIGNUM(*x), BIGNUM(*x),  BIGNUM(y));
		break;
	   case tc_flonum:
		*x = STk_makenumber(bignum2double(BIGNUM(*x)) * FLONM(y));
		break;
	 }
	 break;
    case tc_flonum:
	 switch (TYPE(y)) {
	   case tc_integer:
	        FLONM(*x) *= INTEGER(y);
		break;
	   case tc_bignum:
		FLONM(*x) *= bignum2double(BIGNUM(y));
		break;
	   case tc_flonum:
		FLONM(*x) *= FLONM(y);
		break;
	 }
	 break;
  }
}

void _STk_do_addition(SCM *x, SCM y)
{
  switch (TYPE(*x)) {
    case tc_integer:
         switch (TYPE(y)) {
	   case tc_integer:
	        {
		  double add =(double) INTEGER(*x) + INTEGER(y);

		  if (SMALLNUMP(add))
		    SET_INTEGER(*x, (long) add);
		  else {
		    *x = int2bignum(INTEGER(*x));
		    mpz_add(BIGNUM(*x), BIGNUM(*x), BIGNUM(int2bignum(INTEGER(y))));
		  }
		}
		break;
	   case tc_bignum:
		*x = int2bignum(INTEGER(*x));
		mpz_add(BIGNUM(*x), BIGNUM(*x), BIGNUM(y));
		break;
	   case tc_flonum:
		*x = STk_makenumber(INTEGER(*x) + FLONM(y));
		break;
	 }
	 break;
    case tc_bignum:
	 switch (TYPE(y)) {
	   case tc_integer:
	      {
	        SCM tmp = int2bignum(INTEGER(y));
		mpz_add(BIGNUM(*x), BIGNUM(*x), BIGNUM(tmp));
	      }
	      break;
	   case tc_bignum:
		mpz_add(BIGNUM(*x), BIGNUM(*x),  BIGNUM(y));
		break;
	   case tc_flonum:
		*x = STk_makenumber(bignum2double(BIGNUM(*x)) + FLONM(y));
		break;
	 }
	 break;
    case tc_flonum:
	 switch (TYPE(y)) {
	   case tc_integer:
	        FLONM(*x) += INTEGER(y);
		break;
	   case tc_bignum:
		FLONM(*x) += bignum2double(BIGNUM(y));
		break;
	   case tc_flonum:
		FLONM(*x) += FLONM(y);
		break;
	 }
	 break;
  }
}

static void _STk_do_substract(SCM *x, SCM y)
{
  switch (TYPE(*x)) {
    case tc_integer:
         switch (TYPE(y)) {
	   case tc_integer:
	        {
		  double add = (double) INTEGER(*x) - INTEGER(y);
		  if (SMALLNUMP(add))
		    SET_INTEGER(*x,(long) add);
		  else {
		    *x = int2bignum(INTEGER(*x));
		    mpz_sub(BIGNUM(*x), BIGNUM(*x), BIGNUM(int2bignum(INTEGER(y))));
		  }
		}
		break;
	   case tc_bignum:
		*x = int2bignum(INTEGER(*x));
		mpz_sub(BIGNUM(*x), BIGNUM(*x), BIGNUM(y));
		break;
	   case tc_flonum:
		*x = STk_makenumber(INTEGER(*x) - FLONM(y));
		break;
	 }
	 break;
    case tc_bignum:
	 switch (TYPE(y)) {
	   case tc_integer:
	      {
	        SCM tmp = int2bignum(INTEGER(y));
		mpz_sub(BIGNUM(*x), BIGNUM(*x), BIGNUM(tmp));
	      }
	      break;
	   case tc_bignum:
		mpz_sub(BIGNUM(*x), BIGNUM(*x),  BIGNUM(y));
		break;
	   case tc_flonum:
		*x = STk_makenumber(bignum2double(BIGNUM(*x)) - FLONM(y));
		break;
	 }
	 break;
    case tc_flonum:
	 switch (TYPE(y)) {
	   case tc_integer:
	        FLONM(*x) -= INTEGER(y);
		break;
	   case tc_bignum:
		FLONM(*x) -= bignum2double(BIGNUM(y));
		break;
	   case tc_flonum:
		FLONM(*x) -= FLONM(y);
		break;
	 }
	 break;
  }
}

static void _STk_do_divide(SCM *x, SCM y)
{
  switch (TYPE(*x)) {
    case tc_integer:
         switch (TYPE(y)) {
	   case tc_integer:
	        {
		  double div = (double) INTEGER(*x) / INTEGER(y);
		 
		  if (ISINT(div))
		    SET_INTEGER(*x, (double) div);
		  else
		    *x = STk_makenumber(div);
		}
		break;
	   case tc_bignum:
		{
		  MP_INT q,  r;

		  mpz_init(&q); mpz_init(&r);
		  *x = int2bignum(INTEGER(*x));
		  mpz_mdivmod(&q, &r, BIGNUM(*x), BIGNUM(y));
		  if (mpz_cmp_ui(&r, 0L) == 0)
		    mpz_set(BIGNUM(*x), &q);
		  else
		    *x=STk_makenumber(bignum2double(BIGNUM(*x)) /
				      bignum2double(BIGNUM(y)));
		  mpz_clear(&q); mpz_clear(&r);
		}
		break;
	   case tc_flonum:
		*x = STk_makenumber(INTEGER(*x) / FLONM(y));
		break;
	 }
	 break;
    case tc_bignum:
	 switch (TYPE(y)) {
	   case tc_integer:
	   case tc_bignum:
	      {
		MP_INT q,  r;	

		mpz_init(&q); mpz_init(&r);
		if (INTEGERP(y)) 
		  y = int2bignum(INTEGER(y));

		mpz_mdivmod(&q, &r, BIGNUM(*x), BIGNUM(y));
		if (mpz_cmp_ui(&r, 0L) == 0)
		  mpz_set(BIGNUM(*x), &q);
		else
		  *x=STk_makenumber(bignum2double(BIGNUM(*x)) /
				    bignum2double(BIGNUM(y)));
		mpz_clear(&q); mpz_clear(&r);
	      }
	      break;
	   case tc_flonum:
		*x = STk_makenumber(bignum2double(BIGNUM(*x)) / FLONM(y));
		break;
	 }
	 break;
    case tc_flonum:
	 switch (TYPE(y)) {
	   case tc_integer:
	        FLONM(*x) /= INTEGER(y);
		break;
	   case tc_bignum:
		FLONM(*x) /= bignum2double(BIGNUM(y));
		break;
	   case tc_flonum:
		FLONM(*x) /= FLONM(y);
		break;
	 }
	 break;
  }
}

static SCM do_integer_division(SCM x, SCM y, int quotientp)
{
  MP_INT q, r;
  SCM res;
  int exact = 1;

  /* Divide x by y and return its quotient or remainder. y is not 0 */
  if (FLONUMP(x))
    if (ISINT(FLONM(x))) { x = double2integer(FLONM(x)); exact = 0; }
    else Err("bad number in an integer division", x);
  
  if (FLONUMP(y))
    if (ISINT(FLONM(y))) { y = double2integer(FLONM(y)); exact = 0; }
    else Err("bad number in an integer division", y);
  
  /* Here, x and y can only be integer or bignum (not float) */
  if (INTEGERP(x))
    if (INTEGERP(y)) {
      res = makesmallint(quotientp? INTEGER(x)/INTEGER(y):
			  	     INTEGER(x)%INTEGER(y));
      return exact? res : STk_exact2inexact(res);
    }
    else
      x = int2bignum(INTEGER(x));
  else {
    /* x is a bignum */
    if (INTEGERP(y))
      y = int2bignum(INTEGER(y));
  }
  
  /* x and y are both bignums */
  res = makebignum("0");

  mpz_init(&q); mpz_init(&r);
  mpz_divmod(&q, &r, BIGNUM(x), BIGNUM(y)); /* use divmod instead of mdivmod */
  mpz_set(BIGNUM(res), quotientp? &q : &r); /* Thanks to kerch@parc.xerox.com*/
  mpz_clear(&q); mpz_clear(&r);
  return exact? res : STk_exact2inexact(res);
}

static double do_compare(SCM x, SCM y)
{
  switch (TYPE(x)) {
    case tc_integer:
         switch (TYPE(y)) {
	   case tc_integer: return (double) (INTEGER(x)-INTEGER(y));
	   case tc_bignum:  return (double) -(mpz_cmp_si(BIGNUM(y), INTEGER(x)));
	   case tc_flonum:  return (double) INTEGER(x) - FLONM(y);
	 }
    case tc_bignum:
	 switch (TYPE(y)) {
	   case tc_integer: return (double) mpz_cmp_si(BIGNUM(x), INTEGER(y));
	   case tc_bignum:  return (double) mpz_cmp(BIGNUM(x), BIGNUM(y));
	   case tc_flonum:  return bignum2double(BIGNUM(x))-FLONM(y);
	 }
    case tc_flonum:
	 switch (TYPE(y)) {
	   case tc_integer: return (FLONM(x) - (double) INTEGER(y));
	   case tc_bignum:  return (FLONM(x) - bignum2double(BIGNUM(y)));
	   case tc_flonum:  return (FLONM(x) - FLONM(y));
	 }
    default: return 0.0; /* never reached */
  }
}


/******************************************************************************
 * 
 * Scheme primitives and utilities 
 *
 ******************************************************************************/


/**** Section 6.5 ****/

PRIMITIVE STk_numberp(SCM x)
{
  return NUMBERP(x)? Truth : Ntruth;
}

PRIMITIVE STk_integerp(SCM x)
{
  switch (TYPE(x)) {
    case tc_integer:
    case tc_bignum: return Truth;
    case tc_flonum: {
      		      double val = FLONM(x);
		      return (floor(val) == val)? Truth : Ntruth;
		    }
    default: return Ntruth;
  }
}

PRIMITIVE STk_exactp(SCM x)
{
  if (NNUMBERP(x)) Err("exact?: Bad number", x);
  return EXACTP(x) ? Truth: Ntruth;
}

PRIMITIVE STk_inexactp(SCM x)
{
  if (NNUMBERP(x)) Err("inexact?: Bad number", x);
  return NEXACTP(x) ? Truth: Ntruth;
}


#define Compare(name, operator)					\
PRIMITIVE name(SCM l, SCM env, int from_eval)			\
{								\
  register SCM tmp1, tmp2;					\
								\
  if (NCONSP(l)) Err("too few parameters", l);			\
  tmp1 = from_eval? EVALCAR(l): CAR(l);				\
  if (NNUMBERP(tmp1)) goto Error;				\
								\
  for (l=CDR(l); NNULLP(l); l=CDR(l),tmp1=tmp2) {		\
    tmp2 = from_eval? EVALCAR(l): CAR(l);			\
    if (NNUMBERP(tmp2)) goto Error;				\
    if (do_compare(tmp1, tmp2) operator 0) return Ntruth;	\
  }								\
  return Truth;							\
Error:								\
  Err("Bad number in a comparison", l);				\
  return UNDEFINED; /* never reached */				\
}

Compare(STk_numequal,  !=)
Compare(STk_lessp,     >=)
Compare(STk_greaterp,  <=)
Compare(STk_lessep,    >)
Compare(STk_greaterep, <)


PRIMITIVE STk_zerop(SCM n)
{
  switch (TYPE(n)) {
    case tc_integer: return (INTEGER(n) == 0) 		     ? Truth: Ntruth;
    case tc_flonum:  return (FLONM(n) == 0.0) 		     ? Truth: Ntruth;
    case tc_bignum:  return (mpz_cmp_ui(BIGNUM(n), 0L) == 0) ? Truth: Ntruth;
    default:         Err("zero?: bad number", n); 
		     return UNDEFINED; /* never reached */
  }
}

PRIMITIVE STk_positivep(SCM n)
{
  switch (TYPE(n)) {
    case tc_integer: return (INTEGER(n) > 0) 		     ? Truth: Ntruth;
    case tc_flonum:  return (FLONM(n) > 0.0) 		     ? Truth: Ntruth;
    case tc_bignum:  return (mpz_cmp_ui(BIGNUM(n), 0L) > 0)  ? Truth: Ntruth;
    default:         Err("positive?: bad number", n);
		     return UNDEFINED; /* never reached */
  }
}  

PRIMITIVE STk_negativep(SCM n)
{
  switch (TYPE(n)) {
    case tc_integer: return (INTEGER(n) < 0) 		     ? Truth: Ntruth;
    case tc_flonum:  return (FLONM(n) < 0.0) 		     ? Truth: Ntruth;
    case tc_bignum:  return (mpz_cmp_ui(BIGNUM(n), 0L) < 0)  ? Truth: Ntruth;
    default:         Err("negative?: bad number", n);
		     return UNDEFINED; /* never reached */
  }
}

PRIMITIVE STk_oddp(SCM n)
{
  MP_INT q, r;
  long res;

  switch (TYPE(n)) {
    case tc_integer: return (INTEGER(n)%2) 		     ? Truth: Ntruth;
    case tc_bignum:  mpz_init(&q), mpz_init(&r);
      		     mpz_divmod_ui(&q, &r, BIGNUM(n), 2L);
      		     res = mpz_cmp_ui(&r, 0L);
      		     mpz_clear(&q); mpz_clear(&r);
      		     return (res != 0) ? Truth: Ntruth;
    default:         Err("odd?: bad number", n);
		     return UNDEFINED; /* never reached */
  }
}  

PRIMITIVE STk_evenp(SCM n)
{
  MP_INT q, r;
  long res;

  switch (TYPE(n)) {
    case tc_integer: return (INTEGER(n)%2) 		     ? Ntruth: Truth;
    case tc_bignum:  mpz_init(&q), mpz_init(&r);
      		     mpz_divmod_ui(&q, &r, BIGNUM(n), 2L);
      		     res = mpz_cmp_ui(&r, 0L);
      		     mpz_clear(&q); mpz_clear(&r);
      		     return (res == 0) ? Truth: Ntruth;
    default:         Err("even?: bad number", n);
		     return UNDEFINED; /* never reached */
  }
}

PRIMITIVE STk_max(SCM l, SCM env, int from_eval)
{
  register SCM tmp, max;
  int inexact;

  if (NULLP(l)) Err("max: bad number of arguments", NIL);

  tmp = from_eval? EVALCAR(l): CAR(l);
  if (NNUMBERP(tmp)) goto Error;
  inexact = FLONUMP(tmp);
  max = tmp;
  
  for (l=CDR(l); NNULLP(l); l=CDR(l)) {
    tmp = from_eval? EVALCAR(l): CAR(l);
    if (NNUMBERP(tmp)) goto Error;
    inexact |= FLONUMP(tmp);
    if (do_compare(tmp, max) > 0) max = tmp;
  }
  return (inexact && EXACTP(max)) ? STk_exact2inexact(max) : max;
Error:
  Err("max: bad number", tmp);
  return UNDEFINED; /* never reached */
}

PRIMITIVE STk_min(SCM l, SCM env, int from_eval)
{
  register SCM tmp, min;
  int inexact;

  if (NULLP(l)) Err("min: bad number of arguments", NIL);

  tmp = from_eval? EVALCAR(l): CAR(l);
  if (NNUMBERP(tmp)) goto Error;
  inexact = FLONUMP(tmp);
  min = tmp;
  
  for (l=CDR(l); NNULLP(l); l=CDR(l)) {
    tmp = from_eval? EVALCAR(l): CAR(l);
    if (NNUMBERP(tmp)) goto Error;
    inexact |= FLONUMP(tmp);
    if (do_compare(tmp, min) < 0) min = tmp;
  }
  return (inexact && EXACTP(min)) ? STk_exact2inexact(min) : min;
Error:
  Err("min: bad number", tmp);
  return UNDEFINED; /* never reached */
}

PRIMITIVE STk_plus(SCM l, SCM env, int from_eval)
{
  SCM tmp, res = makesmallint(0);
  
  for ( ; CONSP(l); l=CDR(l)) {
    tmp = from_eval? EVALCAR(l): CAR(l);
    if (NNUMBERP(tmp)) Err("+: not a number", tmp);
    do_addition(&res, tmp);
  }
  return res;
}

PRIMITIVE STk_difference(SCM l, SCM env, int from_eval)
{
  SCM tmp, res;

  if (NULLP(l)) Err("-: no argument given", NIL);
  if (NULLP(CDR(l))) l = Cons(makesmallint(0), l);	/* (- x) --> (- 0 x) */

  tmp = res = clone(EVALCAR(l));
  if (NNUMBERP(res)) goto Error;
  for (l=CDR(l) ; CONSP(l); l=CDR(l)) {
    tmp = from_eval? EVALCAR(l): CAR(l);
    if (NNUMBERP(tmp)) goto Error;
    do_substract(&res, tmp);
  }
  return res;
Error:
  Err("-: not a number", tmp);
  return UNDEFINED; /* never reached */
}

PRIMITIVE STk_times(SCM l, SCM env, int from_eval)
{
  SCM tmp, res = makesmallint(1);

  for ( ; CONSP(l); l=CDR(l)) {
    tmp = from_eval? EVALCAR(l): CAR(l);
    if (NNUMBERP(tmp)) Err("*: not a number", tmp);
    do_multiply(&res, tmp);
  }
  return res;
}

PRIMITIVE STk_division(SCM l, SCM env, int from_eval)
{
  SCM tmp, res;

  if (NULLP(l)) Err("/: no argumenent given", NIL);
  if (NULLP(CDR(l))) l = Cons(makesmallint(1), l);	/* (/ x) --> (/ 1 x) */
  
  tmp = res = clone(EVALCAR(l));
  if (NNUMBERP(res)) goto Error;
  for (l=CDR(l) ; CONSP(l); l=CDR(l)) {
    tmp = from_eval? EVALCAR(l): CAR(l);
    if (NNUMBERP(tmp) || STk_zerop(tmp) == Truth) goto Error;
    do_divide(&res, tmp);
  }
  return res;
Error:
  Err("/: not a valid number", tmp);
  return UNDEFINED; /* never reached */
}

PRIMITIVE STk_absolute(SCM x)
{
  switch (TYPE(x)) {
    case tc_integer: return (INTEGER(x) < 0) ? makesmallint(-INTEGER(x)) : x;
    case tc_flonum:  return (FLONM(x) < 0.0) ? STk_makenumber(-FLONM(x)) : x;
    case tc_bignum:  if (mpz_cmp_ui(BIGNUM(x), 0L) < 0) {
     			SCM tmp = clone(x);
			mpz_neg(BIGNUM(tmp), BIGNUM(x));
			return tmp;
		      }
      		      return x;
    default:         Err("abs: bad number", x);
		     return UNDEFINED; /* never reached */
  }
}  

PRIMITIVE STk_quotient(SCM n1, SCM n2)
{
  if (NNUMBERP(n1) || NNUMBERP(n2))
    Err("quotient: bad arguments", Cons(n1, n2));
  if (STk_zerop(n2) == Truth) Err("quotient: division by 0", NIL);
      
  return do_integer_division(n1, n2, TRUE);
}

PRIMITIVE STk_remainder(SCM n1, SCM n2)
{
  if (NNUMBERP(n1) || NNUMBERP(n2))
    Err("remainder: bad arguments", Cons(n1, n2));
  if (STk_zerop(n2) == Truth) Err("remainder: division by 0", NIL);

  return do_integer_division(n1, n2, FALSE);
}

PRIMITIVE STk_modulo(SCM n1, SCM n2)
{
  SCM z;

  if (NNUMBERP(n1) || NNUMBERP(n2))
    Err("modulo: bad arguments", Cons(n1, n2));
  if (STk_zerop(n2) == Truth) Err("modulo: division by 0", NIL);

  z = do_integer_division(n1, n2, FALSE);
  if (STk_negativep(n1) != STk_negativep(n2) && STk_zerop(z) != Truth) 
    /*kerch@parc.xerox.com*/
    do_addition(&z, n2);
  return z;
}


static SCM gcd2(SCM n1, SCM n2)
{
  return (STk_zerop(n2) == Truth) ? n1 : gcd2(n2, STk_modulo(n1, n2));
}

PRIMITIVE STk_gcd(SCM l, SCM env, int from_eval)
{
  register SCM tmp, res;

  if (NULLP(l)) return makesmallint(0L);
  
  tmp = EVALCAR(l);
  if (NNUMBERP(tmp)) goto Error;
  res = STk_absolute(tmp);
  
  for (l=CDR(l); NNULLP(l); l=CDR(l)) {
    tmp = from_eval? EVALCAR(l): CAR(l);
    if (NNUMBERP(tmp)) goto Error;
    res = STk_absolute(gcd2(res, tmp));
  }
  return res;
Error:
  Err("gcd: bad number", tmp);
  return UNDEFINED; /* never reached */
}

PRIMITIVE STk_lcm(SCM l, SCM env, int from_eval)
{
  SCM tmp, res, gcd;

  if (NULLP(l)) return makesmallint(1L);

  tmp = EVALCAR(l);
  if (NNUMBERP(tmp)) goto Error;
  res = STk_absolute(clone(tmp));
  
  for (l=CDR(l); NNULLP(l); l=CDR(l)) {
    tmp=EVALCAR(l);
    if (NNUMBERP(tmp)) goto Error;
    if (STk_zerop(tmp)==Truth) return tmp;
    gcd = gcd2(res, tmp);
    do_multiply(&res, tmp);
    do_divide(&res, gcd);
    res = STk_absolute(res);
  }
  return res;
Error:
  Err("lcm: bad number", tmp);
  return UNDEFINED; /* never reached */
}

PRIMITIVE STk_floor(SCM x)
{
  if (NNUMBERP(x)) Err("floor: bad number", x);
  if (FLONUMP(x)) return STk_makenumber(floor(FLONM(x)));
  return x;
}

PRIMITIVE STk_ceiling(SCM x)
{
  if (NNUMBERP(x)) Err("ceiling: bad number", x);
  if (FLONUMP(x)) return STk_makenumber(ceil(FLONM(x)));
  return x;
}

PRIMITIVE STk_truncate(SCM x)
{
  if (NNUMBERP(x)) Err("truncate: bad number", x);
  if (FLONUMP(x)) {
    double d = FLONM(x);
    return STk_makenumber(d < 0.0 ? ceil(d) : floor(d));
  }
  return x;
}

PRIMITIVE STk_round(SCM x)
{
  if (NNUMBERP(x)) Err("round: bad number", x);
  if (FLONUMP(x)) {
    /*  
     * R4RS states that round must returns the closest integer to x, rounding to
     * even when x is halfway between two integers (round to even is for consistency
     * with IEEE standard).
     * This explains the (too much) complicate computation below
     */
    double v 	      = FLONM(x); 
    double v_plus_0_5 = v + 0.5;
    double res        = floor(v_plus_0_5);

    return STk_makenumber(
      (v_plus_0_5 == res && v_plus_0_5/2 != floor(v_plus_0_5/2)) ? res-1: res);
  }
  return x;
}

#if !defined(WIN32) && !defined(__STDC__)
#  define concat(a,b) a/**/b
#else
#  define concat(a,b) a##b
#endif

#define transcendental(fct)                                             \
    PRIMITIVE concat(STk_,fct)(SCM z)                                   \
    {                                                                   \
      if (NUMBERP(z))                                                   \
        return STk_makenumber(fct(FLONM(STk_exact2inexact(z))));        \
      Err("transcendental function: bad number", z);                    \
      return UNDEFINED /* never reached */;                             \
    }

transcendental(exp)
transcendental(log)
transcendental(sin)
transcendental(cos)
transcendental(tan)
transcendental(asin)
transcendental(acos)

PRIMITIVE STk_atan(SCM y, SCM x)
{
  if (NNUMBERP(y)) Err("atan: bad  number", y);

  if (x == UNBOUND)
    return STk_makenumber(atan(FLONM(STk_exact2inexact(y))));
  else {
    double fl_x = FLONM(STk_exact2inexact(x));
    double fl_y = FLONM(STk_exact2inexact(y));

    /* Make a special case for (atan 0 0) -- in this case return 0 */
    return STk_makenumber((fl_x == 0 && fl_y == 0) ? 0L : atan2(fl_y, fl_x));
  }
}

PRIMITIVE STk_sqrt(SCM z)
{
  switch (TYPE(z)) {
    case tc_integer: {
      	 double d;
	 
	 if (INTEGER(z) < 0) goto Error;
	 d = (double) sqrt((double) INTEGER(z));
	 return ISINT(d) ? makesmallint((long) d): STk_makenumber(d);
       }
    case tc_bignum: {
         MP_INT root, remainder;
	 SCM res;

	 if (mpz_cmp_si(BIGNUM(z), 0L) < 0) goto Error;
	 mpz_init(&root); mpz_init(&remainder);
	 mpz_sqrtrem(&root, &remainder, BIGNUM(z));
	 if (mpz_cmp_si(&remainder, 0L) == 0) {
	   /* Result is an integer */
	   res = makebignum("0");
	   mpz_set(BIGNUM(res), &root);
	 }
	 else {
	   /* Result is a flonum */
	   res = STk_makenumber(bignum2double(&root));
	 }
	 mpz_clear(&root); mpz_clear(&remainder);
	 return res;
       }
    case tc_flonum: {
         if (FLONM(z) < 0.0) goto Error;
	 return STk_makenumber((double) sqrt(FLONM(z)));
       }
    default:
         Err("sqrt: bad number", z);
  }
Error:
  Err("sqrt: number is negative", z);
  return UNDEFINED; /* never reached */
}

/*
 (define (exact_expt z1 z2)
  (define (square x) (* x x))
  (cond ((= z2 0) 1)
	((negative? z2) (/ 1 (exact_expt z1 (abs z2))))
        ((even? z2) (square (exact_expt z1 (/ z2 2))))
        (else (* z1 (exact_expt z1 (- z2 1))))))
*/

static SCM exact_expt(SCM z1, SCM z2)
{
  SCM res;
  
  if (STk_zerop(z2) == Truth) return makesmallint(1);
  if (STk_negativep(z2) == Truth) { 
    /* (/ 1 (expt z1 (abs z2) */
    res = makesmallint(1);
    do_divide(&res, exact_expt(z1, STk_absolute(z2)));
    return res;
  }
  if (STk_evenp(z2) == Truth) {
    /* (square (expt z1 (/ z2 2)) */
    res = clone(z2);
    do_divide(&res, makesmallint(2));
    res = exact_expt(z1, res);
    do_multiply(&res, res);
    return res;
  }
  /* (* (expt z1 (- z2 1)) z1) */
  res = clone(z2);
  do_substract(&res, makesmallint(1));
  res = exact_expt(z1, res);
  do_multiply(&res, z1);
  return res;
}

PRIMITIVE STk_expt(SCM z1, SCM z2)
{
  if (NNUMBERP(z1)) Err("expt: bad number", z1);
  if (NNUMBERP(z2)) Err("expt: bad number", z2);

  return (EXACTP(z1) && EXACTP(z2)) ?
    		exact_expt(z1, z2)  :
		STk_makenumber((double) pow(FLONM(STk_exact2inexact(z1)),
					    FLONM(STk_exact2inexact(z2))));
}

PRIMITIVE STk_exact2inexact(SCM z)
{
  switch (TYPE(z)) {
    case tc_integer: return STk_makenumber((double) INTEGER(z));
    case tc_bignum:  return STk_makenumber(bignum2double(BIGNUM(z)));
    case tc_flonum:  return z;
    default:         Err("exact->inexact: bad number", z);
		     return UNDEFINED; /* never reached */
  }
}

PRIMITIVE STk_inexact2exact(SCM z)
{
  switch (TYPE(z)) {
    case tc_integer: 
    case tc_bignum:  return z;
    case tc_flonum:  return double2integer(floor(FLONM(z)+0.5));
    default:         Err("inexact->exact: bad number", z);
      		     return UNDEFINED; /* never reached */
  }
}


PRIMITIVE STk_string2number(SCM str, SCM base)
{
  long b = (base==UNBOUND)? 10L : STk_integer_value(base);

  if (NSTRINGP(str)) Err("string->number: not a string", str);
  if (b != 2 && b != 8 && b != 10 && b != 16)
    Err("string->number: base must be 2, 8, 10 or 16", NIL);

  return STk_Cstr2number(CHARS(str), b);
}

PRIMITIVE STk_number2string(SCM n, SCM base)
{
  long b = (base==UNBOUND)? 10L : STk_integer_value(base);
  char *s, buffer[100];
  SCM z;

  if (NNUMBERP(n))   Err("number->string: bad number", n);
  if (b != 2 && b != 8 && b != 10 && b != 16)
    Err("number->string: base must be 2, 8, 10 or 16", NIL);

  s = STk_number2Cstr(n, b, buffer);
  z = STk_makestring(s);
  if (s != buffer) free(s);
  return z;
}

PRIMITIVE  STk_bignump(SCM n)
{
  return BIGNUMP(n) ? Truth: Ntruth;
}

