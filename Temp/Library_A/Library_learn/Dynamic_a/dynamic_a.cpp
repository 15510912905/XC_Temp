// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

DLL_EXPORT_IMPORT void DLL_TEST::PrintHello()
{
    std::cout << "First Dynamic A!" << std::endl;
    std::cout << "According Dynamic !" << std::endl;
    std::cout << "Add Class " << std::endl;
}

DLL_TEST::DynamicMath::DynamicMath(void):m_dAddValue(0), m_dSubValue(0), m_dMulValue(0), m_dDivValue(0)
{

}

DLL_TEST::DynamicMath::~DynamicMath(void)
{

}

double DLL_TEST::DynamicMath::add(double a, double b)
{
    m_dAddValue = a + b;
    return m_dAddValue;
}

double DLL_TEST::DynamicMath::sub(double a, double b)
{
    m_dSubValue = a - b;
    return m_dSubValue;
}

double DLL_TEST::DynamicMath::mul(double a, double b)
{
    m_dMulValue = a * b;
    return m_dMulValue;
}

double DLL_TEST::DynamicMath::div(double a, double b)
{
    m_dDivValue = a / b;
    return m_dDivValue;
}

void DLL_TEST::DynamicMath::Mathprint(void)
{

    std::cout << m_dAddValue << m_dSubValue << m_dMulValue << m_dDivValue << std::endl;    
}

double DLL_TEST::MathA::m_dAddVal = 0;
double DLL_TEST::MathA::m_dSubVal = 0;
double DLL_TEST::MathA::m_dMulVal = 0;
double DLL_TEST::MathA::m_dDivVal = 0;

DLL_TEST::MathA::MathA(void)
{

}

DLL_TEST::MathA::~MathA(void)
{

}

__declspec(dllexport) double DLL_TEST::MathA::add(double a, double b)
{
    m_dAddVal = a + b;
    return a + b;
}

__declspec(dllexport) double DLL_TEST::MathA::sub(double a, double b)
{
    m_dSubVal = a - b;
    return  a - b;
}

__declspec(dllexport) double DLL_TEST::MathA::mul(double a, double b)
{
    m_dMulVal = a * b;
    return a * b;
}

__declspec(dllexport) double DLL_TEST::MathA::div(double a, double b)
{
    m_dDivVal = a / b;
    return a / b;
}

__declspec(dllexport) void DLL_TEST::MathA::Mathprint(void)
{

    std::cout << "MathA Output" << '+' << m_dAddVal << '-' << m_dSubVal << '*' << m_dMulVal << '/' << m_dDivVal << std::endl;
}

