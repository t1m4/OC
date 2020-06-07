// 12.02.2020.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

#include "pch.h"
#include <windows.h>
#include <tchar.h>
#include <strsafe.h>
#include <iostream>
#include <stdlib.h>
using namespace std;
int n, m;
int* a; 
int* b;
int* c;
#define MAX_THREADS n+m+1

//беззнаковое целое
DWORD WINAPI MyThreadFunction(LPVOID lpParam);//параметр - указатель на тип, может быть любой тип данных


int _tmain()
{
	cout << "Input degree first polynom" << endl;
	cin >> n;
	a = new int[n + 1];
	cout << "Input coefficients first polynom" << endl;
	for (int i = 0; i < n + 1; i++) {
		cin >> a[i];
	}

	cout << "Input degree second polynom" << endl;
	cin >> m;
	b = new int[m + 1];
	cout << "Input coefficients second polynom" << endl;
	for (int i = 0; i < m + 1; i++) {
		cin >> b[i];
	}

	c = new int[n + m + 1];

	
	HANDLE  *hThreadArray = new HANDLE[MAX_THREADS];//дескриптор потоков -  число, с помощью которого можно идентифицировать ресурс.
	
	// Create MAX_THREADS worker threads.
	//создает максимальное колво потоков
	for (int i = 0; i < MAX_THREADS; i++)
	{
	
		hThreadArray[i] = CreateThread( 
			NULL,						// default security attributes
			0,							// use default stack size.  Размер стека
			MyThreadFunction,			// thread function name. Имя функции потока !важно
			LPVOID(i),					// argument to thread function. Аргументы для  функции потока !важно
			0,							// use default creation flags 
			0);		// returns the thread identifier.  Возвращяет идефикатор потока
					//возвращяет дескриптор нового потока
	

	
	} 

	
	//ждать завершения потоков
	WaitForMultipleObjects(MAX_THREADS, hThreadArray, TRUE, INFINITE);//описание в docs отслеживает состояние функций, блокирует исплнение потока до тех пор пока обьекты не станут signaled(завершился)  или не пройдет время
	//число, массив дескрипторов, true(ждать пока все потоки придут в signaled), задержка(ждем пока все не завершаться)
	
	// Close all thread handles and free memory allocations.
	for (int i = 0; i < MAX_THREADS; i++)
	{
		CloseHandle(hThreadArray[i]);//очистка и возвращение памяти в систему		
	}


	for (int i = 0; i < n + m + 1; i++) {
		cout << c[i] << " ";
	}

	delete[]a;
	delete[]b;
	delete[]c;
	delete[]hThreadArray;

	return 0;
}


DWORD WINAPI MyThreadFunction(LPVOID lpParam)
{
	int k = (int)lpParam;
	c[k] = 0;
	for (int i = max(0, k - n); i <= min(m, k); i++) {
		c[k] += a[k - i] * b[i];
	}
	return 0;
}



