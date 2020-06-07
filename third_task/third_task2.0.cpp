// 12.02.2020.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

//#include "pch.h"
//#include "pch.h"
#include <windows.h>
#include <tchar.h>
#include <strsafe.h>
#include <iostream>
#include <stdlib.h>
#include <algorithm>
#include <math.h>
#include <time.h>

using namespace std;
int n, t, half;
int* A;
#define MAX_THREADS n

//беззнаковое целое
DWORD WINAPI MyThreadFunction(LPVOID lpParam);//параметр - указатель на тип, может быть любой тип данных
DWORD WINAPI MyThreadFunctionArrayB(LPVOID lpParam);
void PrintArray(int* A, int n) {
	for (int i = 0; i < n; i++)
	{
		cout << A[i] << ' ';
	}
	cout << endl;
}
int _tmain()
{

	cin >> n;
	A = new int[n];
	srand(time(NULL));
	int s = 0;
	for (int i = 0; i < n; i++) {
		A[i] = rand() % 10;
		s += A[i];
	}
	cout << s << endl;

	PrintArray(A, n);
	t = n;
	HANDLE* hThreadArray = new HANDLE[t / 2];//дескриптор потоков -  число, с помощью которого можно идентифицировать ресурс.	
	int index;
	while (t > 1) {		
		index = t % 2;
		half = t / 2;		
	
		cout << "this is t - " << half << endl;
		for (int j = 0; j < half; j++) {
			hThreadArray[j] = CreateThread(
				NULL,						// default security attributes
				0,							// use default stack size.  Размер стека
				MyThreadFunction,			// thread function name. Имя функции потока !важно
				LPVOID(j + index),			// argument to thread function. Аргументы для  функции потока !важно
				0,							// use default creation flags 
				0);
		}
		WaitForMultipleObjects(half, hThreadArray, TRUE, INFINITE);

		for (int j = 0; j < half; j++)
			CloseHandle(hThreadArray[j]);//очистка и возвращение памяти в систему		
				
		t -= half;
		PrintArray(A, n);
	}
	cout << "Sum - " << A[0] << endl;
	delete[] hThreadArray;
	delete[] A;
	return 0;
}

DWORD WINAPI MyThreadFunction(LPVOID lpParam) {
	int i = int(lpParam);
	A[i] = A[i] + A[i + half];
	return 0;
}