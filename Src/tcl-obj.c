/*
 *
 * t c l - o b j . c 		- Implementation of the (crazy) Tcl_obj functions
 *				  in the Scheme interpreeter
 *
 * Copyright © 1997-1999 Erick Gallesio - I3S-CNRS/ESSI <eg@unice.fr>
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
 *           Author: Erick Gallesio [eg@unice.fr]
 *    Creation date:  8-Jul-1997 10:33
 * Last file update:  3-Sep-1999 20:59 (eg)
 *
 */

#include "stk.h"
#ifdef USE_TK
#  include "tk-glue.h"
#  include "module.h"
#else
#  include "tcl-glue.h"
#endif


void Tcl_ResetObjResult(Interp *interp)
{
  interp->objResultPtr = STk_create_tcl_object(NULL);
}


#ifdef USE_TK
/*
 *----------------------------------------------------------------------
 *
 * Tcl_CreateObjCommand --
 *
 *	Define a new object-based command in a command table.
 *
 *----------------------------------------------------------------------
 */

Tcl_Command
Tcl_CreateObjCommand(interp, cmdName, proc, clientData, deleteProc)
    Tcl_Interp *interp;		/* Token for command interpreter (returned
				 * by previous call to Tcl_CreateInterp). */
    char *cmdName;		/* Name of command. If it contains namespace
				 * qualifiers, the new command is put in the
				 * specified namespace; otherwise it is put
				 * in the global namespace. */
    Tcl_ObjCmdProc *proc;	/* Object-based procedure to associate with
				 * name. */
    ClientData clientData;	/* Arbitrary value to pass to object
    				 * procedure. */
    Tcl_CmdDeleteProc *deleteProc;
				/* If not NULL, gives a procedure to call
				 * when this command is deleted. */
{
  struct Tk_command *res;

  res = (struct Tk_command *) Tcl_CreateCommand(interp, 
						cmdName, 
						(Tcl_CmdProc *) proc, 
						clientData, 
						deleteProc);
  res->objproc = 1;
  return (Tcl_Command) res;
}
#endif

/*****************************************************************************/
/* Lists */


Tcl_Obj *
Tcl_NewListObj(objc, objv)
    int objc;			/* Count of objects referenced by objv. */
    Tcl_Obj *CONST objv[];	/* An array of pointers to Tcl objects. */
{
  register SCM res;
  int i;
  
  res = NIL;
  if (objc > 0) {
    for (i = 0;  i < objc;  i++) 
      res = Cons((SCM) objv[i], res);
    res = Reverse(res);
  }
  
  return STk_create_tcl_object(res);
}

int
Tcl_ListObjLength(interp, listPtr, intPtr)
     Tcl_Interp *interp;	/* Used to report errors if not NULL. */
     register Tcl_Obj *listPtr;	/* List object whose #elements to return. */
     register int *intPtr;	/* The resulting int is stored here. */
{
  SCM data = TCLOBJDATA((SCM) listPtr);
  
  if (!data) 
    TCLOBJDATA((SCM) listPtr) = NIL;
  else
    if (NCONSP(data) && NNULLP(data))
      TCLOBJDATA((SCM) listPtr) = LIST1(data);
  *intPtr = STk_llength(data);
  return TCL_OK;
}



int
Tcl_ListObjAppendElement(interp, listPtr, objPtr)
    Tcl_Interp *interp;		/* Used to report errors if not NULL. */
    Tcl_Obj *listPtr;		/* List object to append objPtr to. */
    Tcl_Obj *objPtr;		/* Object to append to listPtr's list. */
{
  SCM data;
  SCM list = (SCM) listPtr;
  SCM obj  = (SCM) objPtr;


  data = TCLOBJDATA(list);

  if (!data || NULLP(data))
    data = LIST1(obj);
  else
    if (CONSP(data))
      data = STk_append2(data, LIST1(obj));
    else
      data = Cons(data, LIST1(obj));
  
  TCLOBJDATA(list) = data;

  return TCL_OK;
}



int
Tcl_ListObjReplace(interp, listPtr, first, count, objc, objv)
    Tcl_Interp *interp;		/* Used for error reporting if not NULL. */
    Tcl_Obj *listPtr;		/* List object whose elements to replace. */
    int first;			/* Index of first element to replace. */
    int count;			/* Number of elements to replace. */
    int objc;			/* Number of objects to insert. */
    Tcl_Obj *CONST objv[];	/* An array of objc pointers to Tcl objects
				 * to insert. */
{
  SCM data = TCLOBJDATA((SCM) listPtr);
  int len;
  Tcl_Obj *objs;
     
  if (!data) 
    data = NIL;
  else
    if (NCONSP(data) && NNULLP(data))
      data = LIST1(data);

  len  = STk_llength(data);
  objs = Tcl_NewListObj(objc, objv);

  if (first < 0) first = 0;
  if (first >= len) {
    data = STk_append2(data, TCLOBJDATA((SCM) objs));
    /* Tcl_DecrRefCount(objs); */
  }		       
  else {
    panic("Problem in Tcl_ListObjReplace");
  }
  return 0; /* never reached */
}

int
Tcl_ListObjIndex(interp, listPtr, index, objPtrPtr)
    Tcl_Interp *interp;		/* Used to report errors if not NULL. */
    register Tcl_Obj *listPtr;	/* List object to index into. */
    register int index;		/* Index of element to return. */
    Tcl_Obj **objPtrPtr;	/* The resulting Tcl_Obj* is stored here. */
{
  SCM data = TCLOBJDATA((SCM) listPtr);
  int len;

  if (!data) 
    data = NIL;
  else
    if (NCONSP(data) && NNULLP(data))
      data = LIST1(data);
  
  len = STk_llength(data);
  if ((index < 0) || (index >= len)) {
    /* FIXME: Est ce intelligent de retourner NULL? pourquoi pas NIL?  */
    *objPtrPtr = NULL;
  } else {
    *objPtrPtr = STk_create_tcl_object(STk_list_ref(data, STk_makeinteger(index)));
  } 
  return TCL_OK;
}

/* This function uses a malloced array for returning the Tcl_Obj pointers
 * This is not fully compatible with Tcl, but This function is used only
 * for parsing the new font name scheme.
 * Should be rewritten if used elsewhere (in particular with long lists)
 */
int
Tcl_ListObjGetElements(interp, listPtr, objcPtr, objvPtr)
    Tcl_Interp *interp;		/* Used to report errors if not NULL. */
    register Tcl_Obj *listPtr;	/* List object for which an element array
				 * is to be returned. */
    int *objcPtr;		/* Where to store the count of objects
				 * referenced by objv. */
    Tcl_Obj ***objvPtr;		/* Where to store the pointer to an array
				 * of pointers to the list's objects. */
{
  static Tcl_Obj *obj[100];
  int i, len;
  SCM data = TCLOBJDATA((SCM) listPtr);

  if (!data) 
    data = NIL;
  else 
    if (STRINGP(data))
      /* We have a string and we want to look at it as a list */
      data = STk_convert_Tcl_string2list(CHARS(data));
    else
      if (NCONSP(data) && NNULLP(data))
	data = LIST1(data);
  
  len = STk_llength(data);
  if (len > 100)
    panic("Too much data in Tcl_ListObjGetElements\n");

  for (i = 0; i < len; i++, data=CDR(data))
    obj[i] = STk_create_tcl_object(CAR(data));

  *objcPtr = len;
  *objvPtr = obj;

  return TCL_OK;
}

/*****************************************************************************/
/* Strings */

void Tcl_InitStringRep(Tcl_Obj *objPtr, char *bytePtr, int len)
{
  TCLOBJDATA((SCM) objPtr) = STk_makestrg(len, bytePtr);
}

Tcl_Obj *
Tcl_NewStringObj(bytes, length)
    register char *bytes;	/* Points to the first of the length bytes
				 * used to initialize the new object. */
    register int length;	/* The number of bytes to copy from "bytes"
				 * when initializing the new object. If 
				 * negative, use bytes up to the first
				 * NULL byte. */
{
  if (length < 0) length = strlen(bytes);
  return STk_create_tcl_object(STk_makestrg(length, bytes));
}

void
Tcl_AppendToObj(objPtr, bytes, length)
    register Tcl_Obj *objPtr;	/* Points to the object to append to. */
    char *bytes;		/* Points to the bytes to append to the
				 * object. */
    register int length;	/* The number of bytes to append from
				 * "bytes". If < 0, then append all bytes
				 * up to NULL byte. */
{
  SCM str = TCLOBJDATA((SCM) objPtr);
  char *s;
  int len;

  if (!str || NULLP(str))
    str = STk_makestring("");
  else
    if (NSTRINGP(str))
      Err("internal error (Tcl_AppendToObj only accepts strings). It was", str);

  if (length < 0) length = strlen(bytes);
  len = STRSIZE(str) + length;	
  s = STk_must_malloc((size_t) len + 1);		/* +1 for the null  */

  sprintf(s, "%s%s", CHARS(str), bytes);
  TCLOBJDATA((SCM) objPtr) = STk_makestrg(len, s);
  free(s);
}

  
char *
Tcl_GetStringFromObj(objPtr, lengthPtr)
    register Tcl_Obj *objPtr;	/* Object whose string rep byte pointer
				 * should be returned. */
    register int *lengthPtr;	/* If non-NULL, the location where the
				 * string rep's byte array length should be
				 * stored. If NULL, no length is stored. */
{
  SCM s = TCLOBJDATA((SCM) objPtr);

  if (!s)
    s = STk_makestring("");
  else
    if (NSTRINGP(s)) {
      SCM dumb;
      char *string;
      
      string = STk_convert_for_Tcl(s, &dumb);
      s = STk_makestring(string);
    }

  if (lengthPtr) *lengthPtr = STRSIZE(s);
  return CHARS(s);
}

void
Tcl_SetStringObj(objPtr, bytes, length)
    register Tcl_Obj *objPtr;	/* Object whose internal rep to init. */
    char *bytes;		/* Points to the first of the length bytes
				 * used to initialize the object. */
    register int length;	/* The number of bytes to copy from "bytes"
				 * when initializing the object. If 
				 * negative, use bytes up to the first
				 * NULL byte.*/
{
  if (length < 0) length = strlen(bytes);
  TCLOBJDATA((SCM) objPtr) = STk_makestrg(length, bytes);
  /* old string will be automatically garbaged */
}

void
Tcl_AppendStringsToObj TCL_VARARGS_DEF(Tcl_Obj *,arg1)
{
    va_list argList;
    register Tcl_Obj *objPtr;
    int newLength, oldLength;
    register char *string, *dst;
#ifdef STk_CODE
    SCM data;
#endif

    objPtr = (Tcl_Obj *) TCL_VARARGS_START(Tcl_Obj *,arg1,argList);

#ifdef STk_CODE
    data = TCLOBJDATA((SCM) objPtr);

    if (!data) 
      TCLOBJDATA((SCM) objPtr) = data = STk_makestring("");
    else
      if (NSTRINGP(data)) {
	SCM newdata;
	
	STk_convert_for_Tcl(data, &newdata);
	TCLOBJDATA((SCM) objPtr) = data = newdata;
      }
#else
    if (objPtr->typePtr != &tclStringType) {
	ConvertToStringType(objPtr);
    }
#endif

    /*
     * Figure out how much space is needed for all the strings, and
     * expand the string representation if it isn't big enough. If no
     * bytes would be appended, just return.
     */

#ifdef STk_CODE
    newLength = oldLength = STRSIZE(data);
#else
    newLength = oldLength = objPtr->length;
#endif
    while (1) {
	string = va_arg(argList, char *);
	if (string == NULL) {
	    break;
	}
	newLength += strlen(string);
    }
    if (newLength == oldLength) {
	return;
    }

#ifdef STk_CODE
    CHARS(data) = (char *) STk_must_realloc(CHARS(data),(size_t)newLength+1);
#else
    if ((long)newLength > objPtr->internalRep.longValue) {
	/*
	 * There isn't currently enough space in the string
	 * representation so allocate additional space.  If the current
	 * string representation isn't empty (i.e. it looks like we're
	 * doing a series of appends) then overallocate the space so
	 * that we won't have to do as much reallocation in the future.
	 */

	Tcl_SetObjLength(objPtr,
		(objPtr->length == 0) ? newLength : 2*newLength);
    }
#endif

    /*
     * Make a second pass through the arguments, appending all the
     * strings to the object.
     */

    TCL_VARARGS_START(Tcl_Obj *,arg1,argList);
#ifdef STk_CODE
    dst = CHARS(data) + oldLength;
#else
    dst = objPtr->bytes + oldLength;
#endif
    while (1) {
	string = va_arg(argList, char *);
	if (string == NULL) {
	    break;
	}
	while (*string != 0) {
	    *dst = *string;
	    dst++;
	    string++;
	}
    }

    /*
     * Add a null byte to terminate the string.  However, be careful:
     * it's possible that the object is totally empty (if it was empty
     * originally and there was nothing to append).  In this case dst is
     * NULL; just leave everything alone.
     */

    if (dst != NULL) {
	*dst = 0;
    }
#ifdef STk_CODE
    STRSIZE(data) = newLength;
#else
    objPtr->length = newLength;
#endif
    va_end(argList);
}


/*****************************************************************************/
/* Doubles */

void
Tcl_SetDoubleObj(objPtr, dblValue)
    register Tcl_Obj *objPtr;	/* Object whose internal rep to init. */
    register double dblValue;	/* Double used to set the object's value. */
{
  TCLOBJDATA((SCM) objPtr) = STk_makenumber(dblValue);
}

int
Tcl_GetDoubleFromObj(interp, objPtr, dblPtr)
    Tcl_Interp *interp; 	/* Used for error reporting if not NULL. */
    register Tcl_Obj *objPtr;	/* The object from which to get a double. */
    register double *dblPtr;	/* Place to store resulting double. */
{
  SCM data = TCLOBJDATA((SCM) objPtr);

  if (!data) 
    data = STk_makenumber((double) 0.0);

  if (INTEGERP(data) || BIGNUMP(data))
    data = STk_exact2inexact(data);
  if (FLONUMP(data)) {
    *dblPtr = FLONM(data);
    return TCL_OK;
  }
  else {
    if (interp != NULL) {
      Tcl_ResetResult(interp);
      Tcl_AppendToObj(Tcl_GetObjResult(interp), 
		      "expected floating-point number", -1);
    }
    return TCL_ERROR;
  }  
}



/*****************************************************************************/
/* Longs */

void
Tcl_SetLongObj(objPtr, longValue)
    register Tcl_Obj *objPtr;	/* Object whose internal rep to init. */
    register long longValue;	/* Long integer used to initialize the
				 * object's value. */
{
  TCLOBJDATA((SCM) objPtr) = STk_makeinteger(longValue);
}

int
Tcl_GetLongFromObj(interp, objPtr, longPtr)
    Tcl_Interp *interp; 	/* Used for error reporting if not NULL. */
    register Tcl_Obj *objPtr;	/* The object from which to get a long. */
    register long *longPtr;	/* Place to store resulting long. */
{
  SCM data = TCLOBJDATA((SCM) objPtr);

  if (!data)
    *longPtr = 0L;
  else
    *longPtr = STk_integer_value_no_overflow(data);

  if (*longPtr == LONG_MIN) {
    Tcl_ResetResult(interp);
    Tcl_AppendToObj(Tcl_GetObjResult(interp), 
		    "expected integer number", -1);
    return TCL_ERROR;
  }
  else 
    return TCL_OK;
}

/*****************************************************************************/
/* Integers (as longs in STk) */

Tcl_Obj *
Tcl_NewIntObj(intValue)
    register int intValue;	/* Int used to initialize the new object. */
{
  return STk_create_tcl_object(STk_makeinteger(intValue));
}


void
Tcl_SetIntObj(objPtr, intValue)
    register Tcl_Obj *objPtr;	/* Object whose internal rep to init. */
    register int intValue;	/* Int integer used to initialize the
				 * object's value. */
{
  TCLOBJDATA((SCM) objPtr) = STk_makeinteger(intValue);
}

int
Tcl_GetIntFromObj(interp, objPtr, intPtr)
    Tcl_Interp *interp; 	/* Used for error reporting if not NULL. */
    register Tcl_Obj *objPtr;	/* The object from which to get a int. */
    register int *intPtr;	/* Place to store resulting int. */
{
  SCM data = TCLOBJDATA((SCM) objPtr);
  
  if (!data)
    *intPtr = 0;
  else
    *intPtr = STk_integer_value_no_overflow(data);

  if (*intPtr == LONG_MIN) {
    Tcl_ResetResult(interp);
    Tcl_AppendToObj(Tcl_GetObjResult(interp), 
		    "expected integer number", -1);
    return TCL_ERROR;
  }
  else 
    return TCL_OK;
}

/*****************************************************************************/
/* Booleans */
void
Tcl_SetBooleanObj(objPtr, boolValue)
    register Tcl_Obj *objPtr;	/* Object whose internal rep to init. */
    register int boolValue;	/* Boolean used to set object's value. */
{
  TCLOBJDATA((SCM) objPtr) = boolValue? Truth: Ntruth;
}

int
Tcl_GetBooleanFromObj(interp, objPtr, boolPtr)
    Tcl_Interp *interp; 	/* Used for error reporting if not NULL. */
    register Tcl_Obj *objPtr;	/* The object from which to get boolean. */
    register int *boolPtr;	/* Place to store resulting boolean. */
{
  SCM data = TCLOBJDATA((SCM) objPtr);

  /* Accept various form of boolean "yes" 1 #t */
  if (!data) 
    *boolPtr = 0;
  else
    if (STRINGP(data))
      Tcl_GetBoolean(interp, CHARS(data), boolPtr);
    else
      if (INTEGERP(data))
	*boolPtr = STk_integer_value_no_overflow(data) != 0;
      else 
	*boolPtr = (data != Ntruth);
  return TCL_OK;
}
  
Tcl_Obj *
Tcl_NewBooleanObj(boolValue)
    register int boolValue;	/* Boolean used to initialize new object. */
{
  return STk_create_tcl_object(boolValue? Truth: Ntruth);
}


/*****************************************************************************/
/* Indexes */
int
Tcl_GetIndexFromObj(interp, objPtr, tablePtr, msg, flags, indexPtr)
    Tcl_Interp *interp; 	/* Used for error reporting if not NULL. */
    Tcl_Obj *objPtr;		/* Object containing the string to lookup. */
    char **tablePtr;		/* Array of strings to compare against the
				 * value of objPtr; last entry must be NULL
				 * and there must not be duplicate entries. */
    char *msg;			/* Identifying word to use in error messages. */
    int flags;			/* 0 or TCL_EXACT */
    int *indexPtr;		/* Place to store resulting integer index. */
{
    int index, length, i, numAbbrev;
    char *key, *p1, *p2, **entryPtr;
    Tcl_Obj *resultPtr;

#ifndef STk_CODE
    /*
     * See if there is a valid cached result from a previous lookup.
     */

    if ((objPtr->typePtr == &tclIndexType)
	    && (objPtr->internalRep.twoPtrValue.ptr1 == (VOID *) tablePtr)) {
	*indexPtr = (int) objPtr->internalRep.twoPtrValue.ptr2;
	return TCL_OK;
    }
#endif
    /*
     * Lookup the value of the object in the table.  Accept unique
     * abbreviations unless TCL_EXACT is set in flags.
     */

    key = Tcl_GetStringFromObj(objPtr, &length);
    index = -1;
    numAbbrev = 0;
    for (entryPtr = tablePtr, i = 0; *entryPtr != NULL; entryPtr++, i++) {
	for (p1 = key, p2 = *entryPtr; *p1 == *p2; p1++, p2++) {
	    if (*p1 == 0) {
		index = i;
		goto done;
	    }
	}
	if (*p1 == 0) {
	    /*
	     * The value is an abbreviation for this entry.  Continue
	     * checking other entries to make sure it's unique.  If we
	     * get more than one unique abbreviation, keep searching to
	     * see if there is an exact match, but remember the number
	     * of unique abbreviations and don't allow either.
	     */

	    numAbbrev++;
	    index = i;
	}
    }
    if ((flags & TCL_EXACT) || (numAbbrev != 1)) {
	goto error;
    }

    done:
#ifndef STk_CODE
    if ((objPtr->typePtr != NULL)
	    && (objPtr->typePtr->freeIntRepProc != NULL)) {
	objPtr->typePtr->freeIntRepProc(objPtr);
    }
    objPtr->internalRep.twoPtrValue.ptr1 = (VOID *) tablePtr;
    objPtr->internalRep.twoPtrValue.ptr2 = (VOID *) index;
    objPtr->typePtr = &tclIndexType;
#endif
    *indexPtr = index;
    return TCL_OK;

    error:
    if (interp != NULL) {
	resultPtr = Tcl_GetObjResult(interp);
	Tcl_AppendStringsToObj(resultPtr,
		(numAbbrev > 1) ? "ambiguous " : "bad ", msg, " \"",
		key, "\": must be ", *tablePtr, (char *) NULL);
	for (entryPtr = tablePtr+1; *entryPtr != NULL; entryPtr++) {
	    if (entryPtr[1] == NULL) {
		Tcl_AppendStringsToObj(resultPtr, ", or ", *entryPtr,
			(char *) NULL);
	    } else {
		Tcl_AppendStringsToObj(resultPtr, ", ", *entryPtr,
			(char *) NULL);
	    }
	}
    }
    return TCL_ERROR;
}  
  
#ifdef USE_TK
/*****************************************************************************/
/* Eval */

/*
 * Tcl_EvalObj --
 *
 *	Execute Tcl commands stored in a Tcl object. 
 */

int
Tcl_EvalObj(interp, objPtr)
    Tcl_Interp *interp;			/* Token for command interpreter
					 * (returned by a previous call to
					 * Tcl_CreateInterp). */
    Tcl_Obj *objPtr;			/* Pointer to object containing
					 * commands to execute. */
{
  SCM cmd =  TCLOBJDATA((SCM) objPtr);
  int res;

  PUSH_ERROR_HANDLER
    {
       SCM expr = STk_convert_tcl_list_to_scheme(cmd);
       
       /* Do as if we had a catch around the expression */
       STk_err_handler->context |= ERR_IGNORED;
       STk_eval(expr, MOD_ENV(STk_Tk_module));
       res = TCL_OK;
    }
  WHEN_ERROR 
    {
      res = TCL_ERROR;
    }
  POP_ERROR_HANDLER;
  return res;
}
#endif
