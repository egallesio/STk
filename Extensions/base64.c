/*
 *
 * b a s e 6 4 . c			-- Base64 support for STk
 *
 * Copyright © 1998-1999 Erick Gallesio - I3S-CNRS/ESSI <eg@unice.fr>
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
 *    Creation date: 20-Jul-1998 12:19
 * Last file update:  3-Sep-1999 20:18 (eg)
 */

#include <stk.h>
#include <ctype.h>

static char table[] =  
	"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
static char rev_table[128];

#define OutChar(c, f) {Putc((c), (f)); if (++count>=72) {Putc('\n', (f)); count=0;}}

static void initialize_rev_table(void)
{
  char *p;
  int count = 0;

  for (p = table; *p; p++) rev_table[(int) *p] = count++;
}

static void encode(SCM f, SCM g)
{
  int c, state, count, old;

  state = old = count = 0;
  while ((c = Getc(f)) != EOF) {
    switch (++state) {
      case 1: OutChar(table[(c>>2) & 0x3f], g); 
	      break;
      case 2: OutChar(table[((old<<4) & 0x30) | ((c>>4) & 0x0f)], g); 
	      break;
      case 3: OutChar(table[((old<<2) & 0x3c) | ((c>>6) & 0x03)], g);
	      OutChar(table[c & 0x3f], g);
	      state = 0;
	      break;
    }
    old = c;
  }
  switch (state) {
    case 0: /* nothing */;
    case 1: OutChar(table[(old<<4) & 0x30], g);
            OutChar('=', g);
            OutChar('=', g);
	    break;
    case 2: OutChar(table[(old<<2) & 0x3c], g);
            OutChar('=', g);
	    break;
  }
}

static void decode(SCM f, SCM g)
{
  static int initialized = 0;
  int c, bits, group, j;

  if (!initialized) {
    initialize_rev_table();    
    initialized = 1;
  }

  group = 0;  j = 18;
  while ((c = Getc(f)) != EOF) {
    if (c != '\n') {
      if (c != '=') {
	bits = rev_table[c];
	group |= bits << j;
      }
      
      j -= 6;
      
      if (j < 0) {
	c = (group&0xff0000) >> 16; if (c) Putc(c, g);
	c = (group&0x00ff00) >> 8;  if (c) Putc(c, g);
	c = (group&0x0000ff);       if (c) Putc(c, g);
	group = 0;
	j = 18;
      }
    }
  }
}

static PRIMITIVE base64_encode(SCM f, SCM g)
{
  ENTER_PRIMITIVE("base64-encode");

  if (!INP(f))  Serror("bad input port", f);
  if (g == UNBOUND)
    g = STk_curr_oport;
  else
    if (!OUTP(g)) Serror("bad output port", g);
  
  encode(f, g);
  return UNDEFINED;
}

static PRIMITIVE base64_decode(SCM f, SCM g)
{
  ENTER_PRIMITIVE("base64-decode");

  if (!INP(f))  Serror("bad input port", f);
  if (g == UNBOUND)
    g = STk_curr_oport;
  else
    if (!OUTP(g)) Serror("bad output port", g);
  
  decode(f, g);
  return UNDEFINED;
}

PRIMITIVE STk_init_base64(void)
{
  STk_add_new_primitive("base64-decode",  tc_subr_1_or_2,  base64_decode);
  STk_add_new_primitive("base64-encode",  tc_subr_1_or_2,  base64_encode);
  return UNDEFINED;
}
