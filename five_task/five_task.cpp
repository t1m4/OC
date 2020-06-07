// five_task.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//


#include <iostream>
#include <windows.h>
#include <tchar.h>
#include <strsafe.h>
#include <algorithm>
#include <ctime> 
#include <cstdlib>
#include <string>
#include <cmath>
using namespace std;
int n;
int* a;


DWORD WINAPI MyThreadFunction(LPVOID lpParam);

void PrintArray(int* A, int n) {
	for (int i = 0; i < n; i++)
	{
		cout << A[i] << ' ';
	}
	cout << endl;
}

struct Block {
	int x;
	int y;
};
int main()
{
	cin >> n;
	a = new int[n];
	//srand(GetTickCount());
	for (int i = 0, j = n-1; i < n; i++, j--)
		a[j] = i+1;
	random_shuffle(a, a + n);
	PrintArray(a, n);


	HANDLE* hThreadArray = new HANDLE[n / 2];
	Block* c = new Block[n / 2];

	for (int phase = 1; phase < n ; phase *= 2)//идем по фазам
	{
		int len = phase;
		int k = 0;
		for (;len > 0; len /= 2)//идет по длине(глубине) внутри фазы
		{
			for (int i = len % phase; i + len < n; i += len * 2)//идет по сравнивающим устройствам. len % phase т.к. не всегда начинается с нуля
			{
				//if ((j + len < 2 * phase && j < 2 * phase) || (j + len >= 2 * phase && j >= 2 * phase)//учитывает не все
				//проверка того чтобы одинаковые merger[] за заходили друг в друга. 2 * phase - Длина одного merger[]
				//if ((i+len<2*phase  && i < 2*phase) || (i + len >= 2 * phase && i >= 2 * phase && (i + len) % (2 * phase) != 0))
				if ((i + len) % (2 * phase) != 0)

				{
					for (int j = 0; j < len; j++) //идет по ступенькам 
					{
						//проверка того чтобы устройства не выходили за границы сети
						if ((j + i + len < n))
						{
							c[k].x = j + i;
							c[k].y = j + i + len;
							hThreadArray[k] = CreateThread(
								0,
								0,
								MyThreadFunction,
								(LPVOID)&c[k],
								0,
								0);
							k++;
						}
					}
				}
				else {
					cout << phase << ' ' << len << " " << i << endl;
				}
			}
			WaitForMultipleObjects(k, hThreadArray, TRUE, INFINITE);
			for (int i = 0; i < k; i++)
				CloseHandle(hThreadArray[i]);
			k = 0;
		}
	}

	for (int i = 0; i < n; i++)
		cout << a[i] << " ";
	cout << endl;

	delete[] c;
	delete[] hThreadArray;
	delete[] a;

	return 0;
}


DWORD WINAPI MyThreadFunction(LPVOID lpParam)
{

	Block* c = (Block*)lpParam;
	int i = c->x;
	int j = c->y;
	if (a[i] > a[j])
		swap(a[i], a[j]);
	return 0;
}
