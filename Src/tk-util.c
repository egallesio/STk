/*
 *
 * t k - u t i l . c 		- Some Tk utilities 
 *
 *
 * Copyright © 1993-1996 Erick Gallesio - I3S-CNRS/ESSI <eg@unice.fr>
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
 *            Author: Erick Gallesio [eg@unice.fr]
 *    Creation date: 19-Feb-1993 22:15
 * Last file update: 26-Apr-1996 23:17
 *
 */

#ifdef USE_TK
#include "stk.h"
#include "tk-glue.h"

PRIMITIVE STk_string2widget(SCM str)
{
  SCM tmp, w;
  char *s;

  if (NSTRINGP(str)) Err("string->widget: bad string", str);

  s = CHARS(str);
  if (strcmp(s, ".") == 0) s = ROOT_WINDOW;

  tmp = Intern(s);
  if (STk_symbol_boundp(tmp, STk_globenv) == Truth && TKCOMMP(w=STk_eval(tmp, NIL)))
    return w;
  return Ntruth;
}

PRIMITIVE STk_widget2string(SCM widget)
{
  char *tmp;

  if (NTKCOMMP(widget)) Err("widget->string: bad widget", widget);
  tmp = (widget == STk_root_window) ? ROOT_WINDOW
    				: widget->storage_as.tk.data->Id;
  return STk_makestring(tmp);
}

PRIMITIVE STk_tk_commandp(SCM obj)
{
  return TKCOMMP(obj) ? Truth : Ntruth;
}

PRIMITIVE STk_widget_name(SCM widget)
{
  char *tmp;
  
  if (NTKCOMMP(widget)) Err("widget-name: bad widget", widget);
  tmp = (widget == STk_root_window) ? ROOT_WINDOW
    				    : widget->storage_as.tk.data->Id;
  return Intern(tmp);
}

PRIMITIVE STk_get_widget_data(SCM widget)
{
  if (NTKCOMMP(widget)) Err("get-widget-data: bad widget", widget);
  return widget->storage_as.tk.l_data;
}

PRIMITIVE STk_set_widget_data(SCM widget, SCM value)
{
  if (NTKCOMMP(widget)) Err("set-widget-data!: bad widget", widget);
  widget->storage_as.tk.l_data = value;
  return UNDEFINED;
}
#endif /* USE_TK */
