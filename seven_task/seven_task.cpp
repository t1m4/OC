// ConsoleApplication1.cpp: определяет точку входа для консольного приложения.
//

//https://docs.microsoft.com/ru-ru/cpp/c-runtime-library/reference/srand?view=vs-2019
#include <iostream>
#include <windows.h>
#include <tchar.h>
#include <strsafe.h>
#include <algorithm>
#include <ctime> 
#include <cstdlib>
#include <string>
using namespace std;

int N, k;

HANDLE* ghMutex;
HANDLE ghSemaphore;
DWORD WINAPI MyThreadFunction(LPVOID lpParam); 



int main()
{
	cout << "Enter 2 or more philosopher" << endl;
	cin >> N >> k;

	ghMutex = new HANDLE[N];
	for (int i = 0; i < N; i++)
	{
		ghMutex[i] = CreateMutex(
			NULL,              
			FALSE,             
			NULL);             

	}
	ghSemaphore = CreateSemaphore(
		NULL,   // default security attributes
		N - 1,  // initial count
		N - 1,  // maximum count
		NULL);

	HANDLE* hThreadArray = new HANDLE[N];
	for (int i = 0; i < N; i++)
	{
		hThreadArray[i] = CreateThread(
			0,                      
			0,                      
			MyThreadFunction,       
			(LPVOID)(i),            
			0,                      
			0);                     
	}

	WaitForMultipleObjects(N, hThreadArray, TRUE, INFINITE); 

															 
	for (int i = 0; i < N; i++)
		CloseHandle(hThreadArray[i]);

	for (int i = 0; i < N; i++)
		CloseHandle(ghMutex[i]);

	CloseHandle(ghSemaphore);

	delete[] hThreadArray;
	delete[] ghMutex;
	return 0;
}


DWORD WINAPI MyThreadFunction(LPVOID lpParam)
{	
	int i = (int)lpParam;
	srand(time(0) + i);//задает начальное значение для регенирации псевдослучайных чисел
	int n = k;
	int thinking = rand() % 5 + 1;
	int lunch = rand() % 3 + 1;
	for (int n = k; n > 0; n--)
	{
		printf("Philosopher %d is thinking\n", i + 1);
		Sleep(thinking);
		WaitForSingleObject(ghSemaphore, INFINITE);
		WaitForSingleObject(ghMutex[i], INFINITE);//take left spoon
		
		printf("Philosopher %d is taking the left spoon\n", i + 1);
		if (i == N - 1)//take right spoon
			WaitForSingleObject(ghMutex[0], INFINITE);
		else
			WaitForSingleObject(ghMutex[i + 1], INFINITE);

		printf("Philosopher %d is taking the right spoon\n", i + 1);
		printf("Philosopher %d is having lunch\n", i + 1);
		Sleep(lunch);

		ReleaseMutex(ghMutex[i]);//put left spoon
		ReleaseMutex(ghMutex[i + 1]);//put right spoon
		ReleaseSemaphore(
			ghSemaphore, // handle to semaphore 
			1, // increase count by one 
			NULL); // not interested in previous count
	}
	return 0;
}