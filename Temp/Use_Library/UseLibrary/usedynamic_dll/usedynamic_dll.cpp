// usedynamic_dll.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <windows.h>

int main()
{
	HINSTANCE hInst;
	double dReceive = 0;

	hInst = LoadLibrary(L"Dynamic_a.dll");					    //加载dll库
	if (hInst == NULL) {
		std::cout << "Load " << "Dynamic_a.dll" << " failed." << std::endl;
		exit(1);
	}
	else {
		std::cout << "Load " << "Dynamic_a.dll" << " successfully." << std::endl;
	}

	typedef void(*Sub)();										//函数指针
	Sub PrintHello = (Sub )GetProcAddress( hInst, "PrintHello" );	//加载库函数

	//Sub add = (Sub)GetProcAddress(hInst, "add");	//加载库函数
	//Sub sub = (Sub)GetProcAddress(hInst, "sub");	//加载库函数
	//Sub mul = (Sub)GetProcAddress(hInst, "mul");	//加载库函数
	//Sub div = (Sub)GetProcAddress(hInst, "div");	//加载库函数
	//Sub Mathprint = (Sub)GetProcAddress(hInst, "MathA::Mathprint");	//加载库函数


    std::cout << "Use dynamic!\n";

	PrintHello();//运行函数

	//dReceive = add(5, 6);
	//std::cout << dReceive << std::endl;

	//dReceive = DLL_TEST::MathA::sub(5, 6);
	//std::cout << dReceive << std::endl;

	//dReceive = DLL_TEST::MathA::mul(5, 6);
	//std::cout << dReceive << std::endl;

	//dReceive = DLL_TEST::MathA::div(5, 6);
	//std::cout << dReceive << std::endl;

	//Mathprint();

	FreeLibrary( hInst );       //释放库

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
