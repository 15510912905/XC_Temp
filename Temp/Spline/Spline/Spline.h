#ifndef _FUNC_SPLINE_H_
#define _FUNC_SPLINE_H_

typedef double RealNum;
#define	ABSZERO (0)
#define ZERO (1e-12)
#define ISZERO(Value) (((-1.0*ZERO) < (Value)) && ((Value) < (1.0*ZERO)))
#define LESSTHANZERO(Value) ((Value) < ZERO)
#define GREATERTHANZERO(Value) (ZERO < (Value))
#define GREATERTHAN(Value,Value1) ((Value)>(Value1))       
#define LESSTHAN(Value,Value1) ((Value)<(Value1))
#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))
#define ISBETWEEN(Value,Value1,Value2) (GREATERTHAN(Value, MIN(Value1, Value2)) && LESSTHAN(Value, MAX(Value1, Value2)))

typedef struct tagComplex
{
	RealNum real;
	RealNum image;
}Complex;

#endif