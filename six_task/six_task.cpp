// ConsoleApplication2.cpp: определяет точку входа для консольного приложения.
//

//#include "stdafx.h"

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
int p;
int m;




DWORD WINAPI MyThreadFunction(LPVOID lpParam); 

DWORD WINAPI MyThreadFunction_SortBlock(LPVOID lpParam); 											   
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
	srand(GetTickCount());
	for (int i = 0; i < n; i++)
		a[i] = i + 1;
	random_shuffle(a, a + n);
	PrintArray(a, n);
	int P;
	//cin >> p;
	cin >> P;	
	p = 2 * P;

	m = n / p;
	if (n % p != 0)
	{
		m += 1; //увеличивается количество элементов в одном блоке

		int s = n / m;
		//cout << "Blocks - " <<s <<" resudue "<< n % (s * m)<< endl;
		
		//if (n % (s * m) == 0)
		if (n == (s * m))
			p = s;
		else
			p = s + 1; //увеличивается количество блоков
		
	}
	
	int num = p / 2 + p % 2; //от четности блоков, он увеличивает кол-во дескриптеров
	HANDLE* hThreadArray_Blocks = new HANDLE[num];
	Block* c_Blocks = new Block[num];
	
	for (int i = 0; i < num; i++)
	{
		c_Blocks[i].x = i * 2 * m;
		c_Blocks[i].y = i * 2 * m + m;
		//cout << c1[i].x << " " << c1[i].y << endl;
		hThreadArray_Blocks[i] = CreateThread(
			0,                      
			0,                      
			MyThreadFunction_SortBlock,       
			(LPVOID)&c_Blocks[i],            
			0,                      
			0);
	}
	WaitForMultipleObjects(num, hThreadArray_Blocks, TRUE, INFINITE);

	cout << "It is p " <<p << endl;
	//PrintArray(a, n);
	for (int i = 0; i < num; i++)
		CloseHandle(hThreadArray_Blocks[i]);
	HANDLE* hThreadArray = new HANDLE[p / 2];
	Block* c = new Block[p / 2];


	for (int phase = 1; phase < p; phase *= 2)//идем по фазам
	{
		int len = phase;
		int k = 0;
		for (; len > 0; len /= 2)//идет по длине(глубине) внутри фазы
		{
			for (int i = len % phase; i + len < p; i += len * 2)//идет по сравнивающим устройствам. len % phase т.к. не всегда начинается с нуля
			{
				//проверка того чтобы одинаковые merger[] за заходили друг в друга. 2 * phase - Длина одного merger[]
				if ((i + len < 2 * phase && i < 2 * phase) || (i + len >= 2 * phase && i >= 2 * phase && (i + len) % (2 * phase) != 0))
					for (int j = 0; j < len; j++) //идет по ступенькам 
					{
						//проверка того чтобы устройства не выходили за границы сети
						if ((j + i + len < p))
						{
							c[k].x = (j + i) * m;
							c[k].y = (j + i + len) * m;
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
			WaitForMultipleObjects(k, hThreadArray, TRUE, INFINITE);
			for (int i = 0; i < k; i++)
				CloseHandle(hThreadArray[i]);
			k = 0;
		}
	}



	PrintArray(a, n);
	
	delete[] hThreadArray;
	delete[] c;
	delete[] hThreadArray_Blocks;
	delete[] c_Blocks;
	delete[] a;

	return 0;
}

DWORD WINAPI MyThreadFunction_SortBlock(LPVOID lpParam)
{
	Block* c1 = (Block*)lpParam;
	int x = c1->x;
	int y = c1->y;
	if (x + m < n)//1 блок   
		sort(a + x, a + x + m);
	else
		sort(a + x, a + n); 

	if (y + m < n) //2 блок 
		sort(a + y, a + y + m);
	else if (y < n)//если 2 блок существует 
		sort(a + y, a + n);

	return 0;
}



DWORD WINAPI MyThreadFunction(LPVOID lpParam)
{

	Block* c = (Block*)lpParam;
	int x = c->x;
	int y = c->y;
	int* block;

	int m_last = m;
	if (y + m > n)
		m_last = n % m;//количество элементов в последнем блоке

	block = new int[m + m_last];
	int i = 0, j = 0, k = 0;
	while (i < m && j < m_last)
	{
		if (a[x + i] < a[y + j])
		{
			block[k] = a[x + i];
			i++;
		}
		else
		{
			block[k] = a[y + j];
			j++;
		}
		k++;
	}

	while (i < m)
	{
		block[k] = a[x + i];
		i++; 
		k++;
	}
	while (j < m_last)
	{
		block[k] = a[y + j];
		j++;
		k++;
	}
	for (int f = 0; f < m; f++)
		a[x + f] = block[f];

	for (int g = 0; g < m_last; g++)
	{
		a[y + g] = block[g + m];
	}



	delete[] block;


	return 0;
}