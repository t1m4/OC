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
int n, t;
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
	int k;
	cin >> k;
	n = pow(2, k);
	
	A = new int[n];
	srand(time(NULL));
	int s = 0;
	for (int i = 0; i < n; i++) {
		A[i] = rand() % 10;
		//cout << A[i] << ' ';
		s += A[i];
	}
	cout << s << endl;

	PrintArray(A, n);

	for (int i = 0; i < k; i++) {
		t = n / pow(2, i + 1);
		
		cout << "this is t and kol: " <<t << endl;
		HANDLE* hThreadArray = new HANDLE[t];//дескриптор потоков -  число, с помощью которого можно идентифицировать ресурс.
		for (int j = 0; j < t; j++) {
			hThreadArray[j] = CreateThread(
				NULL,						// default security attributes
				0,							// use default stack size.  Размер стека
				MyThreadFunction,			// thread function name. Имя функции потока !важно
				LPVOID(j),					// argument to thread function. Аргументы для  функции потока !важно
				0,							// use default creation flags 
				0);
			//cout << "this is t: " << t << endl;
			WaitForMultipleObjects(t, hThreadArray, TRUE, INFINITE);//описание в docs отслеживает состояние функций, блокирует исплнение потока до тех пор пока обьекты не станут signaled(завершился)  или не пройдет время
			//число, массив дескрипторов, true(ждать пока все потоки придут в signaled), задержка(ждем пока все не завершаться)

			//may be error
			CloseHandle(hThreadArray[j]);//очистка и возвращение памяти в систему		
			delete []hThreadArray;
		}
		PrintArray(A, n);
	}
	//PrintArray(A, n);

	/*

	delete[] A;
	delete [] hThreadArray;*/
	delete [] A;
	return 0;
}

DWORD WINAPI MyThreadFunction(LPVOID lpParam) {
	int i = int(lpParam);
	A[i] = A[i] + A[i + t];
	return 0;
}