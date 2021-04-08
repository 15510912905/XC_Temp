// dynamic_b.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "dynamic_a.h"

#pragma comment(lib,"Dynamic_a.lib")

//using namespace DLL_TEST;

int main()
{
    double dReceive = 0;
    std::cout << "Dynamic According A !\n"<<std::endl;;

    DLL_TEST::PrintHello();

    //dReceive = DLL_TEST::DynamicMath::add(5, 6);
    //std::cout << dReceive << std::endl;

    //dReceive = DLL_TEST::DynamicMath::sub(5, 6);
    //std::cout << dReceive << std::endl;

    //dReceive = DLL_TEST::DynamicMath::mul(5, 6);
    //std::cout << dReceive << std::endl;

    //dReceive = DLL_TEST::DynamicMath::div(5, 6);
    //std::cout << dReceive << std::endl;

    dReceive = DLL_TEST::MathA::add(5,6);        
    std::cout << dReceive<<std::endl;

    dReceive = DLL_TEST::MathA::sub(5, 6);
    std::cout << dReceive << std::endl;

    dReceive = DLL_TEST::MathA::mul(5, 6);
    std::cout << dReceive << std::endl;

    dReceive = DLL_TEST::MathA::div(5, 6);
    std::cout << dReceive << std::endl;

    DLL_TEST::MathA::Mathprint();

    system("pause");
    return 0;
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
