#pragma once

#ifndef _DLL_TEST_H_
#define _DLL_TEST_H_

#ifdef DYNAMIC_H_			//通过该宏是否定义，自动判断要导出还是导入dll的函数或变量符号
	#define DLL_EXPORT_IMPORT extern "C" __declspec(dllexport)
#else
	#define DLL_EXPORT_IMPORT extern "C" __declspec(dllimport)
#endif

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files
#include <windows.h>
#include <iostream>

namespace DLL_TEST
{
    DLL_EXPORT_IMPORT void PrintHello();

    class DynamicMath
    {
    public:
        DynamicMath(void);
        ~DynamicMath(void);

        double add(double a, double b);//加法
        double sub(double a, double b);//减法
        double mul(double a, double b);//乘法
        double div(double a, double b);//除法

        void Mathprint(void);

    protected:
    private:
        double m_dAddValue;
        double m_dSubValue;
        double m_dMulValue;
        double m_dDivValue;

    };

    class MathA
    {
    public:
        MathA(void);
        ~MathA(void);

        static __declspec(dllexport) double add(double a, double b);//加法
        static __declspec(dllexport) double sub(double a, double b);//减法
        static __declspec(dllexport) double mul(double a, double b);//乘法
        static __declspec(dllexport) double div(double a, double b);//除法

        static __declspec(dllexport) void Mathprint(void);

    protected:
    private:
        static __declspec(dllexport) double m_dAddVal;
        static __declspec(dllexport) double m_dSubVal;
        static __declspec(dllexport) double m_dMulVal;
        static __declspec(dllexport) double m_dDivVal;
    };
}

#endif
