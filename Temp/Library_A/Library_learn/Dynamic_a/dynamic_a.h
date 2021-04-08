#pragma once

#ifndef _DLL_TEST_H_
#define _DLL_TEST_H_

#ifdef DYNAMIC_H_			//ͨ���ú��Ƿ��壬�Զ��ж�Ҫ�������ǵ���dll�ĺ������������
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

        double add(double a, double b);//�ӷ�
        double sub(double a, double b);//����
        double mul(double a, double b);//�˷�
        double div(double a, double b);//����

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

        static __declspec(dllexport) double add(double a, double b);//�ӷ�
        static __declspec(dllexport) double sub(double a, double b);//����
        static __declspec(dllexport) double mul(double a, double b);//�˷�
        static __declspec(dllexport) double div(double a, double b);//����

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
