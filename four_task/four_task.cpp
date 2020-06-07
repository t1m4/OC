// four_task.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

#include <iostream>
#include <windows.h>
#include <tchar.h>
#include <strsafe.h>
#include <algorithm>
#include <ctime> 
#include <cstdlib>
#include <string>
#define MAX_THREADS 3
using namespace std;
int n, m, q;
int** a;
int* b, * c;

HANDLE* ghMutex;
#define BUF_SIZE 255

DWORD WINAPI MyThreadFunction(LPVOID lpParam);

void PrintArray(int* A, int n) {
	for (int i = 0; i < n; i++)
	{
		cout << A[i] << ' ';
	}
	cout << endl;
}
void  multiplication(int** a, int* b, int  n, int m) {
	cout << "This is multiplication:" << endl;
	int* multi = new int[n];
	for (int i = 0; i < n; i++)
	{
		multi[i] = 0;
		for (int j = 0; j < m; j++)
			multi[i] += a[i][j] * b[j];
		cout << multi[i] << " ";
	}
	cout << "\n\n";
	delete[] multi;
}
struct Block {
	int x;
	int y;
};

int main(void)
{
	cin >> n >> m >> q;
	a = new int* [n];
	for (int i = 0; i < n; i++) {
		a[i] = new int[m];
	}

	//srand(time(NULL));
	for (int i = 0; i < n; i++)
	{
		for (int j = 0; j < m; j++)
		{
			a[i][j] = rand() % 10;
			cout << a[i][j] << " ";
		}
		cout << endl;
	}
	cout << endl;
	b = new int[m];
	for (int i = 0; i < m; i++) {
		b[i] = rand() % 10;
		cout << b[i] << endl;
	}
	cout << endl;
	multiplication(a, b, n, m);
	
	c = new int[n];
	for (int i = 0; i < n; i++) {
		c[i] = 0;
	}

	// Create a mutex with no initial owner
	ghMutex = new HANDLE[q];
	for (int i = 0; i < q; i++) {
		ghMutex[i] = CreateMutex(
			NULL,              // default security attributes 
			FALSE,             // initially not owned
			NULL);             // unnamed mutex
		if (ghMutex[i] == NULL)
		{
			printf("CreateMutex error: %d\n", GetLastError());
			return 1;
		}
	}

	


	HANDLE* hThreadArray = new HANDLE[q * q];

	Block * array_struct = new Block[q * q];
	

	for (int i = 0, thread_iter = 0; i < q; i++)
	{
		for (int j = 0; j < q; j++) {
			
			array_struct[thread_iter].x = n / q * i;
			array_struct[thread_iter].y = m / q * j;
			
			hThreadArray[thread_iter] = CreateThread(
				0,                      // атрибуты безопасности по умолчанию
				0,                      // использовать размер стека по умолчанию
				MyThreadFunction,       // имя функции поток
				(LPVOID)&array_struct[thread_iter],        // argument to thread function 
				0,                      // использовать флаги создания по умолчанию
				0);
			thread_iter++;
		}


	}

	
	WaitForMultipleObjects(q * q, hThreadArray, TRUE, INFINITE);

	// Close thread and mutex handles
	for (int i = 0; i < q * q; i++)
		CloseHandle(hThreadArray[i]);

	for (int i = 0; i < q; i++) {
		CloseHandle(ghMutex[i]);
	}
	PrintArray(c, n);


	delete[] array_struct;
	delete[] hThreadArray;
	delete[] ghMutex;
	for (int i = 0; i < n; i++)
		delete[] a[i];
	delete[] a;
	delete[] b;
	delete[] c;
	
	return 0;
}

DWORD WINAPI MyThreadFunction(LPVOID lpParam) {
	Block* element = (Block*)lpParam;
	int x = element->x;
	int y = element->y;

	int* c_current = new int[n / q];
	for (int i = 0; i < n / q; i++) {
		c_current[i] = 0;
	}

	for (int i = x, k = 0; i < n / q + x; i++, k++) {
		for (int j = y; j < m / q + y; j++) {
			c_current[k] += a[i][j] * b[j];
		}
	}

	WaitForSingleObject(
		ghMutex[x * q / n], INFINITE);

	for (int i = 0; i < n / q; i++) {
		c[x+i] += c_current[i];
	}


	ReleaseMutex(ghMutex[x * q / n]);

	HANDLE hStdout;
	CHAR msgBuf[BUF_SIZE];
	size_t cchStringSize;
	DWORD dwChars;

	string a = "";
	for (int i = 0; i < n / q; i++)
		a += to_string(c_current[i]) + " "; //to_string - функция преобразования в строку типа string

	hStdout = GetStdHandle(STD_OUTPUT_HANDLE);//извлекает дескриптор указанного стандартного устройства
	if (hStdout == INVALID_HANDLE_VALUE)
		return 1;

	StringCchPrintfA(msgBuf, BUF_SIZE, "The number of the block(%d, %d), Result %s\n", x * q / n, y * q / m, a.c_str());
	//Записывает отформатированные данные в указанную строку(ANSI). Буфер, размер буфера, строка
	//Функция c_str() возвращает указатель на символьный массив
	StringCchLengthA(msgBuf, BUF_SIZE, &cchStringSize);//Определяет, превышает ли строка указанную длину в символах.
	WriteConsoleA(hStdout, msgBuf, (DWORD)cchStringSize, &dwChars, NULL);
	
	delete[] c_current;
	return 0;

	
}
