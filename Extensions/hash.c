/*
 *
 * h a s h  . c			-- Hash Tables 
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
 *
 *           Author: Erick Gallesio [eg@kaolin.unice.fr]
 *    Creation date: 17-Jan-1994 17:49
 * Last file update: 14-Sep-1999 14:15 (eg)
 */

#include <stk.h>

static void 	 free_hash_table(SCM ht);
static void 	 mark_hash_table(SCM ht);
static PRIMITIVE hash_table_hash(SCM obj);


/**** Definitions for new type tc_hash ****/
static int tc_hash;
static STk_extended_scheme_type hash_table_type = {
  "hash-table",		/* name */
  0,			/* is_procp */
  mark_hash_table,	/* gc_mark_fct */
  free_hash_table,	/* gc_sweep_fct */
  NULL,			/* apply_fct */
  NULL,			/* display_fct */
  NULL,			/* compare_fct */
};

typedef enum {hash_eq, hash_string, hash_comp} hash_type;

typedef struct {
  struct Tcl_HashTable *h;
  hash_type type;
  SCM comparison;	/* unused if not a comparison hash table */
  SCM sxhash_fct;
} Scheme_hash_table;

#define HASH(x)		   ((Scheme_hash_table *) ((x)->storage_as.extension.data))
#define LHASH(x)	   ((x)->storage_as.extension.data)
#define HASHP(x)	   (TYPEP(x, tc_hash))
#define HASH_COMP(x)	   (HASH(x)->comparison)
#define HASH_SXHASH(x)	   (HASH(x)->sxhash_fct)
#define HASH_H(x)	   (HASH(x)->h)
#define HASH_TYPE(x)	   (HASH(x)->type)
#define HASH_WORD(h1, h2)  ((((h1) << 4) + (h1)) ^ (h2))  /* Good repartition ? */

/* This function is duplicated from tclHash.c
 * It would be possible to export this function form tchHash.c, but I prefer 
 * to avoid modifications, as far as possible, from files in the Tcl directory
 * Given the size of this function, a duplication should not be a problem 
 */
static unsigned long HashString(register char *string)
{
  register unsigned long result;
  register int c;

  result = 0;
  for ( ; ; ) {
    c = *string++;
    if (c == 0) break;
    result += (result<<3) + c;
  }
  return result;
}

/*
 * sxhash permits to calculate a "universal" hash value  a` la CL sxhash 
 * function
 *
 */
unsigned long sxhash(SCM obj)
{
  register unsigned long h;
  register SCM tmp;
  register int i;

  switch (TYPE(obj)) {
    case tc_cons:       h = sxhash(CAR(obj));
      			for(tmp=CDR(obj); CONSP(tmp); tmp=CDR(tmp))
			  h = HASH_WORD(h, sxhash(CAR(tmp)));
			h = HASH_WORD(h, sxhash(tmp));
			return h;
    case tc_integer:	
    case tc_bignum:	return (unsigned long) STk_integer_value_no_overflow(obj);
    case tc_flonum:	return (unsigned long) FLONM(obj);
    case tc_symbol:     if (CELLINFO(obj) & CELL_INFO_UNINTERNED)
			  /* Interned symbol. Work on the interned one
			   * to have the same hash value
			   */
			  obj = Intern(PNAME(obj));
			
    			/* For some reasons, returning just obj as an unsigned 
			 * long, which is correct, yields worse results than
			 * the following code.
			 */
 			return HASH_WORD(0, (unsigned long) obj);
    case tc_keyword:	return HashString(KEYVAL(obj));
    case tc_string:	return HashString(CHARS(obj));
    case tc_vector:	h = 0;
			for (i=VECTSIZE(obj)-1; i >= 0; i--) 
			  h = HASH_WORD(h, sxhash(VECT(obj)[i]));
			return h;
    default:	        /* Either a small constant or a complex type (STklos
			 * object, user defined type, hashtable...). In this 
			 * case we return the type of the object. This is very 
			 * inneficient but it should be rare to use a structured
			 * object as a key. Note that returning the type 
			 * works even if we have not COMPACT_SMALL_CST (as far as 
			 * I know, nobody undefine it). In this case  SMALL_CSTP
			 * always return FALSE.  */
      			 return (SMALL_CSTP(obj)) ? (unsigned long) obj:
			   			    (unsigned long) TYPE(obj);
  }
}

/* 
 * find_key: equivalent to the assoc function except that it works
 * with any comparison. If no association is found, find_key returns
 * NULL 
 */
static SCM find_key(SCM obj, SCM alist, SCM comparison)
{
  register SCM l, tmp;

  for(l=alist; !NULLP(l); l=CDR(l)) {
    tmp = CAR(l);
    if (Apply2(comparison, obj, CAR(tmp)) != Ntruth) return tmp;
  }
  return NULL;
}

/* 
 * remove_key: remove the given key from the association list.
 * The key is compared with the ``comparison'' function
 */
static SCM remove_key(SCM obj, SCM alist, SCM comparison)
{
  register SCM l;

  for(l=NIL; !NULLP(alist); alist=CDR(alist)) {
    if (Apply2(comparison, obj, CAR(CAR(alist))) == Ntruth) 
      l = Cons(CAR(alist), l);
  }
  return l;
}

/* 
 * The_func returns a cell which represent the ORIGINAL subr given as
 * a string. Don't use intern to avoid problems if "eq?" or
 * "hash-table-hash" have been redefined by the user. This is probably
 * not useful, but ...
 */
static SCM the_func(char *s)
{
  SCM z;
  
  if (strcmp(s, "eq?") == 0) {
    NEWCELL(z, tc_subr_2);
    z->storage_as.subr0.f = (SCM (*)()) STk_eq;
  }
  else { /* s is "hash-table-hash" */
    NEWCELL(z, tc_subr_1);
    z->storage_as.subr0.f = (SCM (*)()) sxhash;
  }
  z->storage_as.subr0.name = s;
  
  return z;
}


    
/******************************************************************************/

/*
 * STk_sxhash: the Scheme version of sxhash
 */
static PRIMITIVE hash_table_hash(SCM obj)
{
  long int x = sxhash(obj);
  
  return STk_makeinteger((x < 0) ? -x : x);
}


static PRIMITIVE make_hash_table(SCM l, int len)
{
  SCM sxhash, compar, z;
  hash_type type=hash_comp;

  switch (len) {
    case 0: compar = the_func("eq?");
            sxhash = the_func("hash-table-hash");
	    break;
    case 1: compar = CAR(l);
            sxhash = the_func("hash-table-hash");
	    break;
    case 2: compar = CAR(l);
      	    sxhash = CAR(CDR(l));
	    break;
    default: STk_err("make-hash-table: bad list of parameters", l);
  }

  if (STk_procedurep(compar) == Ntruth) 
    STk_err("make-hash-table: bad comparison function", compar);

  if (STk_procedurep(sxhash) == Ntruth) 
    STk_err("make-hash-table: bad hash function", sxhash);  

  if (TYPEP(compar, tc_subr_2))
    /* 
     * We have a procedure. See if it is 'eq?' or 'string?'.
     * If so, we implement the hash table in the most efficient 
     * way. Otherwise, we will use another method (i.e. find a key
     * code for each object with the sxhash  function and strore
     * each element  with this key in a A-list, the A-list for a
     * given key is is found  using the Tcl Hash functions 
     */
    if ((void *) SUBRF(compar) == (void *) STk_eq)    type = hash_eq;    else
    if ((void *) SUBRF(compar) == (void *) STk_streq) type = hash_string;

  /* Make a new hash table object */
  NEWCELL(z, tc_hash);
  
  LHASH(z) 	      = (Scheme_hash_table *)must_malloc(sizeof(Scheme_hash_table));
  HASH(z)->h	      = must_malloc(sizeof(Tcl_HashTable));
  HASH(z)->type	      = type;
  HASH(z)->sxhash_fct = sxhash;
  HASH(z)->comparison = compar;
  Tcl_InitHashTable(HASH_H(z), (type == hash_string)? TCL_STRING_KEYS :
		    				      TCL_ONE_WORD_KEYS);
  return z;
}

static PRIMITIVE hash_table_p(SCM obj)
{
  return HASHP(obj) ? Truth: Ntruth;
}


static PRIMITIVE hash_table_put(SCM ht, SCM key, SCM val)
{
  Tcl_HashEntry *entry;
  SCM index;
  int new_entry;

  if (!HASHP(ht)) Err("hash-table-put!: bad hash table", ht);
  
  switch (HASH_TYPE(ht)) {
    case hash_eq: 
      entry = Tcl_CreateHashEntry(HASH_H(ht), (char *) key, &new_entry);
      Tcl_SetHashValue(entry, val);
      break;
    case hash_string:
      if (!STRINGP(key)) Err("hash-table-put!: bad string", key);
      entry = Tcl_CreateHashEntry(HASH_H(ht), CHARS(key), &new_entry);
      Tcl_SetHashValue(entry, val);
      break;
    case hash_comp:
      index = Apply1(HASH_SXHASH(ht), key);
      if ((entry=Tcl_FindHashEntry(HASH_H(ht), (char *) index)) != NULL) {
	SCM old = (SCM) Tcl_GetHashValue(entry);	/* waz here */
	SCM tmp = find_key(key, old, HASH_COMP(ht));

	if (tmp) {
	  CAR(tmp) = key; /* Generally useless. But we don't master the hash fct */
	  CDR(tmp) = val;		      /* (i.e. it can have side-effects) */
	}
	else
	  Tcl_SetHashValue(entry, Cons(Cons(key, val), old));
      }
      else {						/* new bucket */
	SCM tmp =  LIST1(Cons(key, val)); /* place it in tmp to avoid GC problems */
	entry = Tcl_CreateHashEntry(HASH_H(ht), (char *) index, &new_entry);
	Tcl_SetHashValue(entry, tmp);
      }
      break;
  }
  return UNDEFINED;
}

static PRIMITIVE hash_table_get(SCM ht, SCM key, SCM default_value)
{
  Tcl_HashEntry *entry;
  SCM index;

  if (!HASHP(ht)) Err("hash-table-get: bad hash table", ht);

  switch (HASH_TYPE(ht)) {
    case hash_eq: 
      if (entry=Tcl_FindHashEntry(HASH_H(ht), (char *) key))
	/* Key already in hash table */
	return (SCM) Tcl_GetHashValue(entry);
      break;
    case hash_string:
      if (!STRINGP(key)) Err("hash-table-get: bad string", key);
      if (entry=Tcl_FindHashEntry(HASH_H(ht), CHARS(key)))
	/* Key already in hash table */
	return (SCM) Tcl_GetHashValue(entry);
      break;
    case hash_comp:
      index = Apply1(HASH_SXHASH(ht), key);
      if (entry=Tcl_FindHashEntry(HASH_H(ht), (char *) index)) {
	SCM tmp, val = (SCM) Tcl_GetHashValue(entry);

	if (tmp = find_key(key, val, HASH_COMP(ht))) 
	  return CDR(tmp);
      }
      break;
  }
  /* If we are here, key was not present in table */
  if (default_value == UNBOUND)  
    Err("hash-table-get: entry not defined for this key", key);
  return default_value;
}

static PRIMITIVE hash_table_remove(SCM ht, SCM key)
{
  Tcl_HashEntry *entry;
  SCM index;

  if (!HASHP(ht)) Err("hash-table-remove!: bad hash table", ht);

  switch (HASH_TYPE(ht)) {
    case hash_eq:
      if (entry=Tcl_FindHashEntry(HASH_H(ht), (char *) key)) 
	/* Key alrady in hash table */
	Tcl_DeleteHashEntry(entry);
      break;
    case hash_string:
      if (!STRINGP(key)) Err("hash-table-remove: bad string", key);
      if (entry=Tcl_FindHashEntry(HASH_H(ht), CHARS(key))) 
	/* Key alrady in hash table */
	Tcl_DeleteHashEntry(entry);
      break;
    case hash_comp:
      index = Apply1(HASH_SXHASH(ht), key);
      if (entry=Tcl_FindHashEntry(HASH_H(ht), (char *) index)) {
	SCM tmp, val = (SCM) Tcl_GetHashValue(entry);
	
	tmp = remove_key(key, val, HASH_COMP(ht));

	if (NULLP(tmp)) 
	  /* This was the only entry for this key. We can delete the entry */
	  Tcl_DeleteHashEntry(entry);
	else
	  Tcl_SetHashValue(entry, tmp);
      }
      break;
  }
  return UNDEFINED;
}

static PRIMITIVE hash_table_for_each(SCM ht, SCM proc)
{
  Tcl_HashEntry *entry;
  Tcl_HashSearch search;

  if (!HASHP(ht)) Err("hash-table-for-each: bad hash table", ht);
  if (STk_procedurep(proc)==Ntruth) Err("hash-table-for-each: bad procedure", proc);
  
  for (entry = Tcl_FirstHashEntry(HASH_H(ht), &search);
       entry;
       entry = Tcl_NextHashEntry(&search)) {

    switch (HASH_TYPE(ht)) {
      case hash_eq:
	Apply2(proc, (SCM) Tcl_GetHashKey(HASH_H(ht), entry),
	             (SCM) Tcl_GetHashValue(entry));
	break;
      case hash_string:
	{ 
	  char *s = Tcl_GetHashKey(HASH_H(ht), entry);
	
	  Apply2(proc, STk_makestring(s), (SCM) Tcl_GetHashValue(entry));
	}
	break;
      case hash_comp: 
	{
	  SCM val;  
	  
	  for (val=(SCM) Tcl_GetHashValue(entry); !NULLP(val); val = CDR(val))
	    Apply2(proc, CAR(CAR(val)), CDR(CAR(val)));
	}
    }
  }
  return UNDEFINED;
}

static PRIMITIVE hash_table_map(SCM ht, SCM proc)
{
  Tcl_HashEntry *entry;
  Tcl_HashSearch search;
  SCM result = NIL;

  if (!HASHP(ht)) Err("hash-table-map: bad hash table", ht);
  if (STk_procedurep(proc)==Ntruth) Err("hash-table-map: bad procedure", proc);
  
  for (entry = Tcl_FirstHashEntry(HASH_H(ht), &search);
       entry;
       entry = Tcl_NextHashEntry(&search)) {
    
    switch (HASH_TYPE(ht)) {
      case hash_eq:
	result = Cons(Apply2(proc, (SCM)Tcl_GetHashKey(HASH_H(ht), entry),
			     	   (SCM) Tcl_GetHashValue(entry)),
		      result);
	break;
      case hash_string:
	{ 
	  char *s = Tcl_GetHashKey(HASH_H(ht), entry);
	
	  result = Cons(Apply2(proc, STk_makestring(s), 
			             (SCM) Tcl_GetHashValue(entry)),
			result);
	}
	break;
      case hash_comp: 
	{
	  SCM val;  
	  
	  for (val=(SCM) Tcl_GetHashValue(entry); !NULLP(val); val = CDR(val))
	    result = Cons(Apply2(proc, CAR(CAR(val)), CDR(CAR(val))), 
			  result);
	}
    }
  }
  return result;
}

static PRIMITIVE hash_table_stats(SCM ht)
{
  Tcl_HashSearch search;
  char *s;

  if (!HASHP(ht)) Err("hash-table-stats: bad hash table", ht);

  /* 
   * There is a bug in the Tcl/hash module. Tcl_HashStats makes a division by 0 
   * if the hash table is empty.
   */
  if (Tcl_FirstHashEntry(HASH_H(ht), &search)) {
    s = Tcl_HashStats(HASH_H(ht));
    Puts(s,    STk_curr_eport);
    Putc('\n', STk_curr_eport);
    free(s);
  }
  else 
    Puts("Empty hash table\n", STk_curr_eport);
  return UNDEFINED;
}


static void free_hash_table(SCM ht)
{
  Tcl_DeleteHashTable(HASH_H(ht));
  free(HASH_H(ht));
  free(HASH(ht));
}


static void mark_hash_table(SCM ht)
{
  Tcl_HashEntry *entry;
  Tcl_HashSearch search;

  /* Mark information stored in the hash structure */
  STk_gc_mark(HASH_COMP(ht));
  STk_gc_mark(HASH_SXHASH(ht));

  /* Mark the content of the Tcl hash table */
  for (entry = Tcl_FirstHashEntry(HASH_H(ht), &search);
       entry;
       entry = Tcl_NextHashEntry(&search)) {

    /* The only cas where the must be marked is if the hash table is
     * an eq? one. In effect,
     *    hash_eq table: the key is in the Tcl key field and must be marked
     *    hash_string table: Tcl hashtable has made a copy in the entry
     *    hash_comp: the key is in the value field which will be always marked
     */
    if (HASH_TYPE(ht) == hash_eq)
      STk_gc_mark((SCM) Tcl_GetHashKey(HASH_H(ht), entry));
    
    /* and mark the value in all cases */
    STk_gc_mark((SCM) Tcl_GetHashValue(entry));
  }
}


/******************************************************************************/

PRIMITIVE STk_init_hash(void)
{
  tc_hash = STk_add_new_type(&hash_table_type);

  STk_add_new_primitive("make-hash-table",     tc_lsubr,       make_hash_table);
  STk_add_new_primitive("hash-table?",	       tc_subr_1,      hash_table_p);
  STk_add_new_primitive("hash-table-hash",     tc_subr_1,      hash_table_hash);
  STk_add_new_primitive("hash-table-put!",     tc_subr_3,      hash_table_put);
  STk_add_new_primitive("hash-table-get",      tc_subr_2_or_3, hash_table_get);
  STk_add_new_primitive("hash-table-remove!",  tc_subr_2,      hash_table_remove);
  STk_add_new_primitive("hash-table-for-each", tc_subr_2,      hash_table_for_each);
  STk_add_new_primitive("hash-table-map",      tc_subr_2,      hash_table_map);
  STk_add_new_primitive("hash-table-stats",    tc_subr_1,      hash_table_stats);
  return UNDEFINED;
}
