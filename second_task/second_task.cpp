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

using namespace std;
int n, m;
int	* A;
int* B;
int* C;
#define MAX_THREADS n+m

//беззнаковое целое
DWORD WINAPI MyThreadFunctionArrayA(LPVOID lpParam);//параметр - указатель на тип, может быть любой тип данных
DWORD WINAPI MyThreadFunctionArrayB(LPVOID lpParam);
void PrintArray(int *A, int n) {
	for (int i = 0; i < n; i++)
	{
		cout << A[i] << ' ';
	}
	cout << endl;
}

int _tmain()
{
	
	cin >> n >> m;
	int* T = new int[n + m];
	for (int i = 0; i < n + m; i++)
		T[i] = i + 1;
	srand(GetTickCount64()); //время в миллисек для того чтобы рандом был всегда разный    
	random_shuffle(T, T + n + m);//следующий за последним //может рандомно выбрать одну из перестановок
	A = T; 
	B = T + n;
	sort(A, A + n);
	sort(B, B + m);

	PrintArray(A, n);
	PrintArray(B, m);
	C = new int[n + m];

	HANDLE* hThreadArray = new HANDLE[MAX_THREADS];//дескриптор потоков -  число, с помощью которого можно идентифицировать ресурс.

	// Create MAX_THREADS worker threads.
	//создает максимальное колво потоков
	for (int i = 0; i < n; i++)
	{
		hThreadArray[i] = CreateThread(
			NULL,						// default security attributes
			0,							// use default stack size.  Размер стека
			MyThreadFunctionArrayA,			// thread function name. Имя функции потока !важно
			LPVOID(i),					// argument to thread function. Аргументы для  функции потока !важно
			0,							// use default creation flags 
			0);		// returns the thread identifier.  Возвращяет идефикатор потока
		//возвращяет дескриптор нового потока		
	}

	for (int i = 0; i < m; i++)
	{
		hThreadArray[i + n] = CreateThread(
			NULL,						// default security attributes
			0,							// use default stack size.  Размер стека
			MyThreadFunctionArrayB,			// thread function name. Имя функции потока !важно
			LPVOID(i),					// argument to thread function. Аргументы для  функции потока !важно
			0,							// use default creation flags 
			0);		// returns the thread identifier.  Возвращяет идефикатор потока
	}

	//ждать завершения потоков
	WaitForMultipleObjects(MAX_THREADS, hThreadArray, TRUE, INFINITE);//описание в docs отслеживает состояние функций, блокирует исплнение потока до тех пор пока обьекты не станут signaled(завершился)  или не пройдет время
	//число, массив дескрипторов, true(ждать пока все потоки придут в signaled), задержка(ждем пока все не завершаться)

	// Close all thread handles and free memory allocations.
	for (int i = 0; i < n + m; i++)
	{
		CloseHandle(hThreadArray[i]);//очистка и возвращение памяти в систему		
	}


	PrintArray(C, n + m);

	

	delete[] C;
	delete[] T;
	delete [] hThreadArray;
	return 0;
}

int SearchBinary(int A[], int left, int right, int key)
{
	int middle;
	while (left < right)
	{
		middle = (left + right) / 2;
		if (key < A[middle])       
			right = middle-1;     
		else if (key > A[middle])  
			left = middle + 1;    
		                     
	}
	return left;
}
DWORD WINAPI MyThreadFunctionArrayA(LPVOID lpParam)
{
	int i = int(lpParam);
	int k = SearchBinary(B, 0, m, A[i]);
	C[i + k] = A[i];
	return 0;
}
DWORD WINAPI MyThreadFunctionArrayB(LPVOID lpParam)
{
	int i = int(lpParam);
	int k = SearchBinary(A, 0, n, B[i]);
	C[i + k] = B[i];
	return 0;
}



