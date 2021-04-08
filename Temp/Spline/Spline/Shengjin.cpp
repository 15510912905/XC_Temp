//#include <cmath>
//using namespace std;
//
//#define ABSZERO                                          0
//#define ZERO                                             1e-6
//#define ISZERO(Value)                                    ((-1.0f * ZERO < (Value)) && ((Value)  < 1.0f * ZERO))
//#define LESSTHANZERO(Value)                              ((Value) < ZERO)
//#define GREATERTHANZERO(Value)                           (ZERO < (Value))
//
//#define ISBETWEEN(Value, Value1, Value2)                 (GREATERTHAN(Value, MIN(Value1, Value2)) && LESSTHAN(Value, MAX(Value1, Value2)))
//typedef double RealNum;
//
//typedef struct tagComplex
//{
//	RealNum  real;
//	RealNum  image;
//}Complex;
//
////rA * (X ^3) + rB * (X ^ 2) + rC * (X) + rD = 0
//int Shengjin(RealNum rA, RealNum rB, RealNum rC, RealNum rD, RealNum* rX1, RealNum* rX2, RealNum* rX3)
//{
//	RealNum fDA = rB * rB - 3.0f * rA * rC;
//	RealNum fDB = rB * rC - 9.0f * rA * rD;
//	RealNum fDC = rC * rC - 3.0f * rB * rD;
//
//	RealNum rTempA = ABSZERO;
//	RealNum rTempB = ABSZERO;
//	RealNum fDelta = fDB * fDB - 4.0f * fDA * fDC;
//	if (ISZERO(fDA) && ISZERO(fDB))
//	{
//		if (!ISZERO(rA))
//		{
//			*rX1 = -1.0f * rB / (3.0f * rA);
//		}
//		else if (!ISZERO(rB))
//		{
//			*rX1 = -1.0f * rC / rB;
//		}
//		else if (!ISZERO(rC))
//		{
//			*rX1 = -1.0f * rD / rC;
//		}
//		else// if (!ISZERO(rD))
//		{
//			return 0;
//		}
//		*rX2 = *rX1;
//		*rX3 = *rX1;
//		return 1;
//	}
//
//	if (GREATERTHANZERO(fDelta))
//	{
//		RealNum rY1 = fDA * rB + 3.0f * rA * (0.5f * (-1.0f * fDB + sqrtf(fDelta)));
//		RealNum rY2 = fDA * rB + 3.0f * rA * (0.5f * (-1.0f * fDB - sqrtf(fDelta)));
//		if (GREATERTHANZERO(rY1))
//		{
//			rTempA = powf(rY1, 1.0f / 3.0f);
//		}
//		else
//		{
//			rTempA = -1.0f * powf(-1.0f * rY1, 1.0f / 3.0f);
//		}
//		if (GREATERTHANZERO(rY2))
//		{
//			rTempB = powf(rY2, 1.0f / 3.0f);
//		}
//		else
//		{
//			rTempB = -1.0f * powf(-1.0f * rY2, 1.0f / 3.0f);
//		}
//		*rX1 = -1.0f * (rB + rTempA + rTempB) * (1.0f / (3.0f * rA));
//		Complex cX2;
//		cX2.real = (1.0f / (6.0f * rA)) * (-2.0f * rB + rTempA + rTempB);
//		cX2.image = (1.0f / (6.0f * rA)) * sqrtf(3.0f) * (rTempA - rTempB);
//		Complex cX3;
//		cX3.real = cX2.real;
//		cX3.image = -1.0f * cX2.image;
//
//		return 1;
//	}
//	else if (ISZERO(fDelta))
//	{
//		if (ISZERO(fDA))
//		{
//			return 0;
//		}
//		RealNum rK = fDB / fDA;
//		*rX1 = -1.0f * rB / rA + rK;
//		*rX2 = -0.5f * rK;
//		*rX3 = *rX2;
//		return 2;
//	}
//	else
//	{
//		if (LESSTHANZERO(fDA))
//		{
//			return 0;
//		}
//		if (GREATERTHANZERO(fDA))
//		{
//			rTempA = powf(fDA, 3.0f);
//		}
//		else
//		{
//			rTempA = -1.0f * powf(-1.0f * fDA, 3.0f);
//		}
//		RealNum rT = (2.0f * fDA * rB - 3.0f * rA * fDB) / (2.0f * sqrtf(rTempA));
//		if ( !(ISBETWEEN(rT, -1.0f, 1.0f)) )
//		{
//			return 0;
//		}
//		RealNum rTheta = acosf(rT);
//		*rX1 = -1.0f * (rB + 2.0f * sqrtf(fDA) * cos(rTheta / 3.0f)) * (1.0f / (3.0f * rA));
//		*rX2 = (-1.0f * rB + sqrtf(fDA) * (cos(rTheta / 3.0f) + sqrtf(3.0f) * sin(rTheta / 3.0f))) * (1.0f / (3.0f * rA));
//		*rX3 = (-1.0f * rB + sqrtf(fDA) * (cos(rTheta / 3.0f) - sqrtf(3.0f) * sin(rTheta / 3.0f))) * (1.0f / (3.0f * rA));
//		return 3;
//	}
//
//	return 0;
//}