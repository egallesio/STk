#include <stk.h>
#include <sys/types.h>
#include <sys/time.h>
#include <time.h>

static PRIMITIVE get_time(void)
{
  return STk_makeinteger((long) time(NULL));
}

static PRIMITIVE time_string(void)
{
  time_t t= time(NULL);
  char *s;

  s = ctime(&t); s[24] = '\0';
  return STk_makestring(s);
}

PRIMITIVE STk_init_time(void)
{
  STk_add_new_primitive("get-time",	   tc_subr_0, get_time);
  STk_add_new_primitive("time-string", tc_subr_0, time_string);
}
