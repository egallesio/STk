/*
 *
 * h t m l . c			-- Html support for STk
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
 *    Creation date:  1-Sep-1995 23:10
 * Last file update:  3-Sep-1999 20:20 (eg)
 */

#include <ctype.h>
#include "stk.h"

#define MAXTOKEN	40

struct char_type {
  char *name;
  unsigned char c;
};

static struct char_type table [] = {
  {"lt",    	'<'},  	 {"gt",    	'>'},    {"amp",	'&'},  
  {"quot",	'"'},    
  {"nbsp",	'\xa0'}, {"iexcl",	'\xa1'}, {"cent",	'\xa2'}, 
  {"pound",	'\xa3'}, {"curren",	'\xa4'}, {"yen",	'\xa5'},
  {"brvbar",	'\xa6'}, {"sect",	'\xa7'}, {"uml",	'\xa8'},
  {"copy",	'\xa9'}, {"ordf",	'\xaa'}, {"laquo",	'\xab'},
  {"not",	'\xac'}, {"shy",	'\xad'}, {"reg",	'\xae'},
  {"hibar",	'\xaf'}, {"deg",	'\xb0'}, {"plusmn",	'\xb1'},
  {"sup2",	'\xb2'}, {"sup3",	'\xb3'}, {"acute",	'\xb4'},
  {"micro",	'\xb5'}, {"para",	'\xb6'}, {"middot",	'\xb7'},
  {"cedil",	'\xb8'}, {"sup1",	'\xb9'}, {"ordm",	'\xba'},
  {"raquo",	'\xbb'}, {"frac14",	'\xbc'}, {"frac12",	'\xbd'},
  {"frac34",	'\xbe'}, {"iquest",	'\xbf'}, {"Agrave",	'\xc0'},
  {"Aacute",	'\xc1'}, {"Acirc",	'\xc2'}, {"Atilde",	'\xc3'},
  {"Auml",	'\xc4'}, {"Aring",	'\xc5'}, {"AElig",	'\xc6'},
  {"Ccedil",	'\xc7'}, {"Egrave",	'\xc8'}, {"Eacute",	'\xc9'},
  {"Ecirc",	'\xca'}, {"Euml",	'\xcb'}, {"Igrave",	'\xcc'},
  {"Iacute",	'\xcd'}, {"Icirc",	'\xce'}, {"Iuml",	'\xcf'},
  {"ETH",	'\xd0'}, {"Ntilde",	'\xd1'}, {"Ograve",	'\xd2'},
  {"Oacute",	'\xd3'}, {"Ocirc",	'\xd4'}, {"Otilde",	'\xd5'},
  {"Ouml",	'\xd6'}, {"times",	'\xd7'}, {"Oslash",	'\xd8'},
  {"Ugrave",	'\xd9'}, {"Uacute",	'\xda'}, {"Ucirc",	'\xdb'},
  {"Uuml",	'\xdc'}, {"Yacute",	'\xdd'}, {"THORN",	'\xde'},
  {"szlig",	'\xdf'}, {"agrave",	'\xe0'}, {"aacute",	'\xe1'},
  {"acirc",	'\xe2'}, {"atilde",	'\xe3'}, {"auml",	'\xe4'},
  {"aring",	'\xe5'}, {"aelig",	'\xe6'}, {"ccedil",	'\xe7'},
  {"egrave",	'\xe8'}, {"eacute",	'\xe9'}, {"ecirc",	'\xea'},
  {"euml",	'\xeb'}, {"igrave",	'\xec'}, {"iacute",	'\xed'},
  {"icirc",	'\xee'}, {"iuml",	'\xef'}, {"eth",	'\xf0'},
  {"ntilde",	'\xf1'}, {"ograve",	'\xf2'}, {"oacute",	'\xf3'},
  {"ocirc",	'\xf4'}, {"otilde",	'\xf5'}, {"ouml",	'\xf6'},
  {"divide",	'\xf7'}, {"oslash",	'\xf8'}, {"ugrave",	'\xf9'},
  {"uacute",	'\xfa'}, {"ucirc",	'\xfb'}, {"uuml",	'\xfc'},
  {"yacute",	'\xfd'}, {"thorn",	'\xfe'}, {"yuml",	'\xff'},
  {"", 0}};

static void skip_spaces(SCM f)
{
    int	c;

    for (;;) {
      c = Getc(f);
      if (c == EOF) break;
      if (!isspace(c)) {
	Ungetc(c, f);
	break;
      }
    }
}

/* next_entity: Read an entity such as <A HREF=x.html> */
static SCM next_entity(SCM f)
{
  Tcl_DString dStr1, dStr2;
  int c;
  char ch;
  SCM z;

  Tcl_DStringInit(&dStr1);   Tcl_DStringInit(&dStr2);
  
  skip_spaces(f);
  while ((c = Getc(f)) != EOF && (c != '>') && (c != ' ') && (c != '\t')) {
    ch = tolower(c);
    Tcl_DStringAppend(&dStr1, &ch, 1);
  } 
  if (c == ' ' || c == '\t') {
    /* Read the argument */
    skip_spaces(f);
    while ((c = Getc(f)) != EOF && (c != '>')) {
      ch = c;
      Tcl_DStringAppend(&dStr2, &ch, 1);
    }
  }
    
  if (Tcl_DStringValue(&dStr1)[0] == '\0') 
    z = STk_makestring("<>");
  else 
    if (Tcl_DStringValue(&dStr1)[0]=='/' && Tcl_DStringValue(&dStr1)[1] == '\0')
      z = STk_makestring("</>"); 
    else
      z = Cons(STk_makestring(Tcl_DStringValue(&dStr1)),
	       STk_makestring(Tcl_DStringValue(&dStr2)));

  Tcl_DStringFree(&dStr1); Tcl_DStringFree(&dStr2);
  return z;
}


/* Read an entity such as &amp;  */
static void next_character(Tcl_DString *dStr, SCM f)  
{
  char *t, ch, token[MAXTOKEN];
  int c, i;
  
  token[0] = '&';
  if ((c=Getc(f)) == '#') {		/* Read a &#000; entity */
    token[1] = '#';
    t = token + 2;
    
    while ((c=Getc(f)) != EOF && isdigit(c) && t < &token[MAXTOKEN-1]) {
      *t++ = c;
    }
    *t = '\0';
    if (c != ';')
      /* Unget the terminator character for next reading */
      Ungetc(c, f);
    ch = (char) atoi(token+2);
    if (ch > 10) {
      Tcl_DStringAppend(dStr, &ch, 1);
      return;
    }
  }
  else {				/* Read a &aaaaa; entity */
    t = token + 1;
    while (c != EOF && isalpha(c) && t < &token[MAXTOKEN-1]) {
      *t++ = c;
      c = Getc(f);
    }
    *t = '\0';
    if (c != ';') 
      /* Unget the terminator character for next reading */
      Ungetc(c, f);
    
    /* Search the given token in the translation table */
    for (i = 0; table[i].c; i++)
      if (strcmp(token+1, table[i].name) == 0) {
	Tcl_DStringAppend(dStr, &table[i].c, 1);
	return;
      }
  }

  /* If we are here, we have not found the item in table or the number was 
   * ill formed => bad syntax. */
  Tcl_DStringAppend(dStr, token, -1);
  if (c == ';') Tcl_DStringAppend(dStr, ";", 1); /* as netscape */
}


static PRIMITIVE html_next_token(SCM f)   /* Return next HTML token */
{
  int c;

  ENTER_PRIMITIVE("%html:next-token");

  if (!INP(f)) Serror("bad port", f);

  if (Eof(f) || ((c = Getc(f)) == EOF)) return STk_eof_object;

  if (c == '<') 
    return next_entity(f);
  else {
    SCM z;
    Tcl_DString dStr;
    char ch;

    Tcl_DStringInit(&dStr);
    do {
      if (c == '<') { 
	Ungetc(c, f); 
	break; 
      }
      else {
	if (c == '&') 
	  next_character(&dStr, f);
	else {
	  ch = c;
	  Tcl_DStringAppend(&dStr, &ch, 1);
	}
      }
    }
    while ((c = Getc(f)) != EOF);
    z = STk_makestring(Tcl_DStringValue(&dStr));
    return z;
  }
}


static PRIMITIVE html_clean_spaces(SCM str, SCM ignore_spaces)
{
  Tcl_DString dString;
  char c, *s;
  int only_spaces = TRUE;
  SCM z;

  ENTER_PRIMITIVE("%html:clean-spaces");

  if (!STRINGP(str)) Serror("bad string", str);

  Tcl_DStringInit(&dString);
  for (s = CHARS(str); c = *s; s++) {
    if (c == ' ' || c == '\n' || c == '\t' || c == '\r') {
      if (ignore_spaces == Ntruth) {
	Tcl_DStringAppend(&dString, " ", 1);
	ignore_spaces = Truth;
      }
    }
    else {
      Tcl_DStringAppend(&dString, &c, 1);
      only_spaces = FALSE;
      ignore_spaces = Ntruth;
    }
  }
  z = Cons(STk_makestring(Tcl_DStringValue(&dString)),
	   only_spaces ? Truth: Ntruth);
  Tcl_DStringFree(&dString);

  return z;
}

PRIMITIVE STk_init_html(void)
{
  STk_add_new_primitive("%html:clean-spaces",  tc_subr_2,  html_clean_spaces);
  STk_add_new_primitive("%html:next-token",    tc_subr_1,  html_next_token);
  return UNDEFINED;
}
