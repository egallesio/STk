#include <stdio.h>
#include "libstack.h"

#define MAX_DOUBLE   1.79769313486231470e+308
#define MAX_FLOAT    ((float)3.40282346638528860e+38)
#define BIG_FLOAT    ((float)3.4e+30)


void use_stack_void(a1,a2,a3,a4,a5,a6,a7,a8,a9,a10,a11,a12,a13,a14,a15,a16,
		    b1,b2,b3,b4,b5,b6,b7,b8,b9,b10,b11,b12,b13,b14,b15,b16)
{
  return;
}

float use_stack_float(a1,a2,a3,a4,a5,a6,a7,a8,a9,a10,a11,a12,a13,a14,a15,a16,
		      b1,b2,b3,b4,b5,b6,b7,b8,b9,b10,b11,b12,b13,b14,b15,b16)
{
  return 0.0;
}

double use_stack_double(a1,a2,a3,a4,a5,a6,a7,a8,a9,a10,a11,a12,a13,a14,a15,a16,
			b1,b2,b3,b4,b5,b6,b7,b8,b9,b10,b11,b12,b13,b14,b15,b16)
{
  return 0.0;
}

void use_stack()
{
  use_stack_void(0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0);
  use_stack_float(0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0);
  use_stack_double(0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0);
}


void f_void()
{
  printf("void f(void):\n");
}

/******************************************************************************/

int f_int0()
{
  int r=100;
  printf("int f(void):");
  return r;
}

int f_int1(int a)
{
  int r=a+1;
  printf("int f(int):(%d)", a);
  return r;
}

int f_int2(int a, int b)
{
  int r=a+b;
  printf("int f(2*int):(%d,%d)",a,b);
  return r;
}

int f_int4(int a, int b, int c, int d)
{
  int r=a+b+c+d;
  printf("int f(4*int):(%d,%d,%d,%d)",a,b,c,d);
  return r;
}

int f_int8(int a, int b, int c, int d, int e, int f, int g, int h)
{
  int r=a+b+c+d+e+f+g+h;
  printf("int f(8*int):(%d,%d,%d,%d,%d,%d,%d,%d)",a,b,c,d,e,f,g,h);
  return r;
}

int f_int16(int a, int b, int c, int d, int e, int f, int g, int h,
	    int i, int j, int k, int l, int m, int n, int o, int p)
{
  int r=a+b+c+d+e+f+g+h+i+j+k+l+m+n+o+p;
  printf("int f(16*int):(%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d)",
	 a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p);
  return r;
}

/******************************************************************************/
long f_long0()
{
  long r=100;
  printf("long f(void):");
  return r;
}

long f_long1(long a)
{
  long r=a+1;
  printf("long f(long):(%d)", a);
  return r;
}

long f_long2(long a, long b)
{
  long r=a+b;
  printf("long f(2*long):(%d,%d)",a,b);
  return r;
}

long f_long4(long a, long b, long c, long d)
{
  long r=a+b+c+d;
  printf("long f(4*long):(%d,%d,%d,%d)",a,b,c,d);
  return r;
}

long f_long8(long a, long b, long c, long d, long e, long f, long g, long h)
{
  long r=a+b+c+d+e+f+g+h;
  printf("long f(8*long):(%d,%d,%d,%d,%d,%d,%d,%d)",a,b,c,d,e,f,g,h);
  return r;
}

long f_long16(long a, long b, long c, long d, long e, long f, long g, long h,
	    long i, long j, long k, long l, long m, long n, long o, long p)
{
  long r=a+b+c+d+e+f+g+h+i+j+k+l+m+n+o+p;
  printf("long f(16*long):(%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d)",
	 a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p);
  return r;
}

/******************************************************************************/

float f_float0()
{
  float r=100;
  printf("float f(void):");
  return r;
}

float f_float1(float a)
{
  float r=a+1;
  printf("float f(float):(%5.2f)", a);
  return r;
}

float f_float2(float a, float b)
{
  float r=a+b;
  printf("float f(2*float):(%5.2f,%5.2f)",a,b);
  return r;
}

float f_float4(float a, float b, float c, float d)
{
  float r=a+b+c+d;
  printf("float f(4*float):(%5.2f,%5.2f,%5.2f,%5.2f)",a,b,c,d);
  return r;
}

float f_float8(float a,float b,float c,float d,float e,float f,float g,
		 float h)
{
  float r=a+b+c+d+e+f+g+h;
  printf("float f(8*float):(%5.2f,%5.2f,%5.2f,%5.2f,%5.2f,%5.2f,%5.2f,%5.2f)",a,b,c,d,e,f,g,h);
  return r;
}

float f_float8max(float a,float b,float c,float d,float e,float f,float g,
		  float h)
{
  float r=a+b+c+d+e+f+g+h;
  printf("float f(8*float -- max):(%5.2f,%5.2f,%5.2f,%5.2f,%5.2f,%5.2f,%5.2f,%5.2f)",a,b,c,d,e,f,g,h);
  return r;
}

float f_float16(float a,float b,float c,float d,float e,float f,
		  float g,float h, float i,float j,float k,float l,
		  float m,float n,float o,float p)
{
  float r=a+b+c+d+e+f+g+h+i+j+k+l+m+n+o+p;
  printf("float f(16*float):(%5.2f,%5.2f,%5.2f,%5.2f,%5.2f,%5.2f,%5.2f,%5.2f,%5.2f,%5.2f,%5.2f,%5.2f,%5.2f,%5.2f,%5.2f,%5.2f)",
	 a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p);
  return r;
}

/******************************************************************************/
double f_double0()
{
  double r=100;
  printf("double f(void):");
  return r;
}

double f_double1(double a)
{
  double r=a+1;
  printf("double f(double):(%5.2f)", a);
  return r;
}

double f_double2(double a, double b)
{
  double r=a+b;
  printf("double f(2*double):(%5.2f,%5.2f)",a,b);
  return r;
}

double f_double4(double a, double b, double c, double d)
{
  double r=a+b+c+d;
  printf("double f(4*double):(%5.2f,%5.2f,%5.2f,%5.2f)",a,b,c,d);
  return r;
}

double f_double8(double a,double b,double c,double d,double e,double f,double g,
		 double h)
{
  double r=a+b+c+d+e+f+g+h;
  printf("double f(8*double):(%5.2f,%5.2f,%5.2f,%5.2f,%5.2f,%5.2f,%5.2f,%5.2f)",a,b,c,d,e,f,g,h);
  return r;
}

double f_double8max(double a,double b,double c,double d,double e,double f,double g,
		    double h)
{
  double r=a+b+c+d+e+f+g+h;
  printf("double f(8*double -- max):(%5.2f,%5.2f,%5.2f,%5.2f,%5.2f,%5.2f,%5.2f,%5.2f)",a,b,c,d,e,f,g,h);
  return r;
}


/******************************************************************************/
int f_int_mixed(char a, short b, int c, long d, float e, double f, char *g, int *h)
{
  int r=a+b+c+d+(int)e+(int)f+*h;
  printf("int f(mixed types):('%c',%d,%d,%ld,%f,%lf,%s)",a,b,c,d,e,f,g);
  *h = 1234;
  return r;
}

float f_float_mixed(int a, float b, int c, float d)
{
  float r=a+b+c+d;
  printf("float f(mixed types):(%d,%f,%d,%f)",a,b,c,d);
  return r;
}

float f_float_mixed2(int a, int b, float c, float d, int e)
{
  float r=a+b+c+d;
  printf("float f(mixed types):(%d,%d,%f,%f,%d)",a,b,c,d,e);
  return r;
}

double f_double_mixed(char a, short b, int c, long d, float e, double f, char *g, 
		   int *h)
{
  double r=a+b+c+d+e+f+*h;
  printf("int f(mixed types):('%c',%d,%d,%ld,%f,%lf,%s",a,b,c,d,e,f,g);
  *h = 5678;
  return r;
}

  
/******************************************************************************/

void void_test()
{
  f_void();
  use_stack();
  init_ext_call();
  call_ext_void(f_void);
}

void int_test()
{
  int r;

  r = f_int0(); printf("->%d\n",r);
  r = 0; use_stack();  init_ext_call(); 
  r = call_ext_int(f_int0); printf("->%d\n",r);
  

  r = f_int1(1);  printf("->%d\n",r);
  r = 0; use_stack(); init_ext_call(); 
  push_int(1);
  r = call_ext_int(f_int1); printf("->%d\n",r);



  r = f_int2(1,2);  printf("->%d\n",r);
  r = 0; use_stack();  init_ext_call(); 
  push_int(1); 
  push_int(2);
  r = call_ext_int(f_int2); printf("->%d\n",r);

  r = f_int4(1,2,3,4);  printf("->%d\n",r);
  r = 0; use_stack();  init_ext_call(); 
  push_int(1); 
  push_int(2);
  push_int(3);
  push_int(4);
  r = call_ext_int(f_int4); printf("->%d\n",r);

  r = f_int8(1,2,3,4,5,6,7,8);  printf("->%d\n",r);
  r = 0; use_stack();  init_ext_call(); 
  push_int(1); 
  push_int(2);
  push_int(3);
  push_int(4);
  push_int(5);
  push_int(6);
  push_int(7);
  push_int(8);
  r = call_ext_int(f_int8); printf("->%d\n",r);

  r = f_int16(1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16);  printf("->%d\n",r);
  r = 0; use_stack();  init_ext_call(); 
  push_int(1); 
  push_int(2);
  push_int(3);
  push_int(4);
  push_int(5);
  push_int(6);
  push_int(7);
  push_int(8);
  push_int(9);
  push_int(10);
  push_int(11);
  push_int(12);
  push_int(13);
  push_int(14);
  push_int(15);
  push_int(16);
  r = call_ext_int(f_int16); printf("->%d\n",r);
}

void long_test()
{
  long r;
  long lg = 1000;

  r = f_long0(); printf("->%d\n",r);
  r = 0; use_stack();  init_ext_call(); 
  r = call_ext_long(f_long0); printf("->%d\n",r);
  

  r = f_long1(lg);  printf("->%d\n",r);
  r = 0; use_stack(); init_ext_call(); 
  push_long(lg);
  r = call_ext_long(f_long1); printf("->%d\n",r);



  r = f_long2(lg,200);  printf("->%d\n",r);
  r = 0; use_stack();  init_ext_call(); 
  push_long(lg); 
  push_long(200);
  r = call_ext_long(f_long2); printf("->%d\n",r);

  r = f_long4(lg,2,3,4);  printf("->%d\n",r);
  r = 0; use_stack();  init_ext_call(); 
  push_long(lg); 
  push_long(2);
  push_long(3);
  push_long(4);
  r = call_ext_long(f_long4); printf("->%d\n",r);

  r = f_long8(lg,2,3,4,5,6,7,8);  printf("->%d\n",r);
  r = 0; use_stack();  init_ext_call(); 
  push_long(lg); 
  push_long(2);
  push_long(3);
  push_long(4);
  push_long(5);
  push_long(6);
  push_long(7);
  push_long(8);
  r = call_ext_long(f_long8); printf("->%d\n",r);

  r = f_long16(lg,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16);  printf("->%d\n",r);
  r = 0; use_stack();  init_ext_call(); 
  push_long(lg); 
  push_long(2);
  push_long(3);
  push_long(4);
  push_long(5);
  push_long(6);
  push_long(7);
  push_long(8);
  push_long(9);
  push_long(10);
  push_long(11);
  push_long(12);
  push_long(13);
  push_long(14);
  push_long(15);
  push_long(16);
  r = call_ext_long(f_long16); printf("->%d\n",r);
}


void float_test()
{
  float r;

  r = f_float0(); printf("->%5.2f\n",r);
  r = 0; use_stack();  init_ext_call(); 
  r = call_ext_float(f_float0); printf("->%5.2f\n",r);
  

  r = f_float1(1.0);  printf("->%5.2f\n",r);
  r = 0; use_stack(); init_ext_call(); 
  push_float(1.0);
  r = call_ext_float(f_float1); printf("->%5.2f\n",r);

  r = f_float2(1,2);  printf("->%5.2f\n",r);
  r = 0; use_stack();  init_ext_call(); 
  push_float(1); 
  push_float(2);
  r = call_ext_float(f_float2); printf("->%5.2f\n",r);

  r = f_float4(1,2,3,4);  printf("->%5.2f\n",r);
  r = 0; use_stack();  init_ext_call(); 
  push_float(1); 
  push_float(2);
  push_float(3);
  push_float(4);
  r = call_ext_float(f_float4); printf("->%5.2f\n",r);

  r = f_float8(1,2,3,4,5,6,7,8);  printf("->%5.2f\n",r);
  r = 0; use_stack();  init_ext_call(); 
  push_float(1); 
  push_float(2);
  push_float(3);
  push_float(4);
  push_float(5);
  push_float(6);
  push_float(7);
  push_float(8);
  r = call_ext_float(f_float8); printf("->%5.2f\n",r);

  r = f_float8max(BIG_FLOAT, BIG_FLOAT, BIG_FLOAT, BIG_FLOAT, 
		  BIG_FLOAT, BIG_FLOAT, BIG_FLOAT, BIG_FLOAT);  
  printf("->%5.2f\n",r);
  r = 0; use_stack();  init_ext_call(); 
  push_float(BIG_FLOAT); 
  push_float(BIG_FLOAT); 
  push_float(BIG_FLOAT); 
  push_float(BIG_FLOAT); 
  push_float(BIG_FLOAT); 
  push_float(BIG_FLOAT); 
  push_float(BIG_FLOAT); 
  push_float(BIG_FLOAT); 
  r = call_ext_float(f_float8max); printf("->%5.2f\n",r);

  r = f_float16(1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16);  printf("->%5.2f\n",r);
  r = 0; use_stack();  init_ext_call(); 
  push_float(1); 
  push_float(2);
  push_float(3);
  push_float(4);
  push_float(5);
  push_float(6);
  push_float(7);
  push_float(8);
  push_float(9);
  push_float(10);
  push_float(11);
  push_float(12);
  push_float(13);
  push_float(14);
  push_float(15);
  push_float(16);
  r = call_ext_float(f_float16); printf("->%5.2f\n",r);
}


void double_test()
{
  double r;

  r = f_double0(); printf("->%5.2f\n",r);
  r = 0; use_stack();  init_ext_call(); 
  r = call_ext_double(f_double0); printf("->%5.2f\n",r);
  

  r = f_double1(1);  printf("->%5.2f\n",r);
  r = 0; use_stack(); init_ext_call(); 
  push_double(1);
  r = call_ext_double(f_double1); printf("->%5.2f\n",r);



  r = f_double2(1,2);  printf("->%5.2f\n",r);
  r = 0; use_stack();  init_ext_call(); 
  push_double(1); 
  push_double(2);
  r = call_ext_double(f_double2); printf("->%5.2f\n",r);

  r = f_double4(1,2,3,4);  printf("->%5.2f\n",r);
  r = 0; use_stack();  init_ext_call(); 
  push_double(1); 
  push_double(2);
  push_double(3);
  push_double(4);
  r = call_ext_double(f_double4); printf("->%5.2f\n",r);

  r = f_double8(1,2,3,4,5,6,7,8);  printf("->%5.2f\n",r);
  r = 0; use_stack();  init_ext_call(); 
  push_double(1); 
  push_double(2);
  push_double(3);
  push_double(4);
  push_double(5);
  push_double(6);
  push_double(7);
  push_double(8);
  r = call_ext_double(f_double8); printf("->%5.2f\n",r);
}

void mixed_test()
{
  {
    int r;
    int dumb = 1 ;

    r = f_int_mixed('?',1,2,3,4.123456,5.123456789012345,"Hello, world",&dumb);
    printf("->%d (%d)\n",r, dumb);
    r = 0; dumb = 1; use_stack(); init_ext_call(); 
    push_char('?');
    push_short(1);
    push_int(2);
    push_long(3);
    push_float(4.123456);
    push_double(5.123456789012345);
    push_string("Hello, world");
    push_ptr(&dumb);
    r = call_ext_int(f_int_mixed); printf("->%d (%d)\n",r, dumb);
  }

  {
    float r;

    r = f_float_mixed(1,2,3,4); printf("->%5.2f\n", r);
    r = 0;  use_stack(); init_ext_call(); 
    push_int(1);
    push_float(2);
    push_int(3);
    push_float(4);
    r = call_ext_float(f_float_mixed); printf("->%5.2f\n", r);

    r = f_float_mixed2(1,2,3,4,5); printf("->%5.2f\n", r);
    r = 0;  use_stack(); init_ext_call(); 
    push_int(1);
    push_int(2);
    push_float(3);
    push_float(4);
    push_int(5);
    r = call_ext_float(f_float_mixed2); printf("->%5.2f\n", r); 
  }

  {
    double r;
    int dumb = 1;

    r = f_double_mixed('?',1,2,3,4.123456,5.123456789012345,"Hello, world",&dumb);
    printf("->%5.2lf (%d)\n",r, dumb);
    r = 0; dumb = 1; use_stack(); init_ext_call(); 
    push_char('?');
    push_short(1);
    push_int(2);
    push_long(3);
    push_float(4.123456);
    push_double(5.123456789012345);
    push_string("Hello, world");
    push_ptr(&dumb);
    r = call_ext_double(f_double_mixed); printf("->%5.2lf (%d)\n",r, dumb);
  }
}


main()
{
  printf("On this machine we have:\n");
  printf("\tsizeof(char)\t= %d\n", sizeof(char));
  printf("\tsizeof(short)\t= %d\n", sizeof(short));
  printf("\tsizeof(int)\t= %d\n", sizeof(int));
  printf("\tsizeof(long)\t= %d\n", sizeof(long));
  printf("\tsizeof(float)\t= %d\n", sizeof(float));
  printf("\tsizeof(double)\t= %d\n", sizeof(double));
  printf("\tsizeof(void*)\t= %d\n", sizeof(void*));

  void_test();
  int_test();
  long_test(); 
  float_test();
  double_test(); 
  mixed_test(); 
}
