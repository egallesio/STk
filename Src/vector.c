/*
 *
 * v e c t o r . c 			-- vectors management
 *
 * Copyright © 1993-1999 Erick Gallesio - I3S-CNRS/ESSI <eg@unice.fr>
 * 
 *
 * Permission to use, copy, modify, distribute,and license this
 * software and its documentation for any purpose is hereby granted,
 * provided that existing copyright notices are retained in all
 * copies and that this notice is included verbatim in any
 * distributions.  No written agreement, license, or royalty fee is
 * required for any of the authorized uses.
 * This software is provided ``AS IS'' without express or implied
 * warranty.
 *           Author: Erick Gallesio [eg@unice.fr]
 *    Creation date: ???
 * Last file update:  3-Sep-1999 21:02 (eg)
 */

#include <string.h>
#include "stk.h"


SCM STk_makevect(int len, SCM init)
{
  long j;
  SCM  z;

  NEWCELL(z, tc_vector);
  z->storage_as.vector.dim  = len;
  z->storage_as.vector.data = (SCM *) must_malloc(len * sizeof(SCM));

  if (init)
    for(j=0 ;j<len; j++) z->storage_as.vector.data[j] = init;
  return z;
}


/**** Section 6.8  ****/

PRIMITIVE STk_vectorp(SCM obj)
{
  return VECTORP(obj) ? Truth: Ntruth;
}

PRIMITIVE STk_make_vector(SCM len, SCM init)
{
  long k;

  if ((k=STk_integer_value(len))<0) Err("make-vector: bad vector length", len);
  return STk_makevect(k, init);
}

PRIMITIVE STk_vector(SCM l, int len)
{
  int j;
  SCM z = STk_makevect(len, NULL);
  
  for (j = 0; j < len; j++, l=CDR(l)) {
    VECT(z)[j] = CAR(l);
  }
  return z;
}

PRIMITIVE STk_vector_length(SCM v)
{
  if NTYPEP(v, tc_vector) Err("vector-length: not a vector", v);
  return STk_makeinteger((long) v->storage_as.vector.dim);
}

PRIMITIVE STk_vector_ref(SCM v, SCM index)
{
  long k;

  if (NVECTORP(v))      	        Err("vector-ref: not a vector", v);
  if ((k=STk_integer_value(index)) < 0) Err("vector-ref: bad index", index);
  if (k >= v->storage_as.vector.dim) 
    Err("vector-ref: index out of bounds", index);
  return VECT(v)[k];
}

PRIMITIVE STk_vector_set(SCM v, SCM index, SCM value)
{
  long k;
  
  if (NVECTORP(v)) 			Err("vector-set!: not a vector", v);
  if ((k=STk_integer_value(index)) < 0) Err("vector-set!: bad index", index);
  if (k >= v->storage_as.vector.dim) 
    Err("vector-set!: index out of bounds", index);
  
  VECT(v)[k] = value;
  return UNDEFINED;
}

PRIMITIVE STk_vector2list(SCM v)
{
  int j, len;
  SCM z, tmp;

  if (NVECTORP(v)) Err("vector->list: not a vector", v);
    
  len = v->storage_as.vector.dim;
  tmp = z = NIL;

  for (j=0; j<len; j++) {
    if (j == 0)
      tmp = z = Cons(VECT(v)[j], NIL);
    else 
      tmp = CDR(tmp) = Cons(VECT(v)[j], NIL);
  }
  return z;
}

PRIMITIVE STk_list2vector(SCM l)
{
  if (NCONSP(l) && NNULLP(l)) Err("list->vector: not a list", l);
  return STk_vector(l, STk_llength(l));
}


PRIMITIVE STk_vector_fill(SCM v, SCM fill)
{
  int j, len;

  if (NVECTORP(v)) Err("vector-fill!: not a vector", v);
  
  for (j=0, len= v->storage_as.vector.dim; j < len; j++) 
    VECT(v)[j] = fill;
 
  return UNDEFINED;
}

/*
 * 
 * STk bonus
 *
 */

PRIMITIVE STk_vector_copy(SCM vect)
{
  SCM z;
  int n;

  if (NVECTORP(vect)) Err("vector-copy: bad vector", vect);
  n = vect->storage_as.vector.dim;
  z = STk_makevect(n, NULL);
  memcpy(VECT(z), VECT(vect), n * sizeof(struct obj*));
  return z;
}

PRIMITIVE STk_vector_resize(SCM vect, SCM size)
{
  long old_size, new_size;

  if (NVECTORP(vect))  			    Err("vector-resize: bad vector", vect);
  if ((new_size=STk_integer_value(size))<0) Err("vector-resize: bad new size",size);
  
  old_size 		       = vect->storage_as.vector.dim;
  vect->storage_as.vector.dim  = new_size;
  vect->storage_as.vector.data = must_realloc(vect->storage_as.vector.data, 
					      new_size*sizeof(SCM));

  if (old_size < new_size) { /* Fill in new cells with an unbound value */
    long i;
    
    for (i = old_size; i < new_size; i++) 
      VECT(vect)[i] = UNBOUND;
  }
  return vect;
}


PRIMITIVE STk_sort(SCM obj, SCM test)
{
  SCM *v;
  register int i, j, incr, n;
  int list = 0;

  if (NULLP(obj))        { return NIL; } 
  else if (CONSP(obj))   { obj  = STk_list2vector(obj); list = 1; } 
  else if (VECTORP(obj)) { obj  = STk_vector_copy(obj); 	  } 
  else 			 { Err("sort: bad object to sort", obj);  }
  
  /* 
   * Use a shell sort. It has good performance on small arrays 
   * This sort should have better performances than a cleverer one 
   * for the sorts we'll have to do in STklos (which are always small
   * arrays).
   */

  v    = VECT(obj);
  n    = obj->storage_as.vector.dim;

  for (incr = n / 2; incr; incr /= 2) {
    for (i = incr; i < n; i++) {
      for (j = i-incr; j >= 0; j -= incr) {
	if (Apply2(test, v[j], v[j+incr]) != Ntruth)
	  break;
	else {
	  SCM tmp   = v[j+incr];
	  v[j+incr] = v[j];
	  v[j]	    = tmp;
	}
      }
    }
  }
  return list ? STk_vector2list(obj) : obj;
}
