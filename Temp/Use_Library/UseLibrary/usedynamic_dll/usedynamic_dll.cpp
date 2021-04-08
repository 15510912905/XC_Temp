// usedynamic_dll.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <windows.h>

int main()
{
	HINSTANCE hInst;
	double dReceive = 0;

	hInst = LoadLibrary(L"Dynamic_a.dll");					    //����dll��
	if (hInst == NULL) {
		std::cout << "Load " << "Dynamic_a.dll" << " failed." << std::endl;
		exit(1);
	}
	else {
		std::cout << "Load " << "Dynamic_a.dll" << " successfully." << std::endl;
	}

	typedef void(*Sub)();										//����ָ��
	Sub PrintHello = (Sub )GetProcAddress( hInst, "PrintHello" );	//���ؿ⺯��

	//Sub add = (Sub)GetProcAddress(hInst, "add");	//���ؿ⺯��
	//Sub sub = (Sub)GetProcAddress(hInst, "sub");	//���ؿ⺯��
	//Sub mul = (Sub)GetProcAddress(hInst, "mul");	//���ؿ⺯��
	//Sub div = (Sub)GetProcAddress(hInst, "div");	//���ؿ⺯��
	//Sub Mathprint = (Sub)GetProcAddress(hInst, "MathA::Mathprint");	//���ؿ⺯��


    std::cout << "Use dynamic!\n";

	PrintHello();//���к���

	//dReceive = add(5, 6);
	//std::cout << dReceive << std::endl;

	//dReceive = DLL_TEST::MathA::sub(5, 6);
	//std::cout << dReceive << std::endl;

	//dReceive = DLL_TEST::MathA::mul(5, 6);
	//std::cout << dReceive << std::endl;

	//dReceive = DLL_TEST::MathA::div(5, 6);
	//std::cout << dReceive << std::endl;

	//Mathprint();

	FreeLibrary( hInst );       //�ͷſ�

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
