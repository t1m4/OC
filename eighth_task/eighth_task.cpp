
#include <iostream>
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <queue>

using namespace std;

int client, k, hairdresser;

queue <int> tableInWaitRoom;
queue <int> tableInWorkRoom;

HANDLE clientMutex;
HANDLE hairdresserMutex;
HANDLE clientReadySemaphore;
HANDLE hairdresserReadySemaphore;
HANDLE clientLeaveSemaphore;
HANDLE* endHaircutSemaphore;



DWORD WINAPI hairdresserThread(LPVOID lpParam);
DWORD WINAPI сlientThread(LPVOID lpParam);


int main()
{
	setlocale(LC_ALL, "RUS");
	int sofaSize;
	cin >> client >> k >> hairdresser >> sofaSize;

	for (int i = 0; i < hairdresser + sofaSize; i++)
		tableInWaitRoom.push(i);

	clientMutex = CreateMutex(NULL, FALSE, NULL);
	hairdresserMutex = CreateMutex(NULL, FALSE, NULL);
	clientReadySemaphore = CreateSemaphore(NULL, 0, hairdresser, NULL);
	hairdresserReadySemaphore = CreateSemaphore(NULL, hairdresser, hairdresser, NULL);
	clientLeaveSemaphore = CreateSemaphore(NULL, 0, hairdresser, NULL);

	endHaircutSemaphore = new HANDLE[hairdresser + sofaSize];
	for (int i = 0; i < hairdresser + sofaSize; i++)
		endHaircutSemaphore[i] = CreateSemaphore(NULL, 0, 1, NULL);

	HANDLE* сlientThreadArray = new HANDLE[client];
	HANDLE* hairdresserThreadArray = new HANDLE[hairdresser];

	for (int i = 0; i < hairdresser; i++)
		hairdresserThreadArray[i] = CreateThread(
			0,
			0,
			hairdresserThread,
			(LPVOID)i,
			0,
			0);
	for (int i = 0; i < client; i++)
		сlientThreadArray[i] = CreateThread(
			0,
			0,
			сlientThread,
			(LPVOID)i,
			0,
			0);

	WaitForMultipleObjects(client, сlientThreadArray, TRUE, INFINITE);

	for (int i = 0; i < client; i++)
		CloseHandle(сlientThreadArray[i]);

	CloseHandle(clientMutex);
	CloseHandle(hairdresserMutex);
	CloseHandle(clientReadySemaphore);
	CloseHandle(hairdresserReadySemaphore);
	CloseHandle(clientLeaveSemaphore);
	for (int i = 0; i < hairdresser + sofaSize; i++)
		CloseHandle(endHaircutSemaphore[i]);


	delete[] endHaircutSemaphore;
	delete[] сlientThreadArray;
	delete[] hairdresserThreadArray;
	return 0;
}


DWORD WINAPI hairdresserThread(LPVOID lpParam)
{
	int i = (int)lpParam;
	srand(time(NULL) + i);
	while (true)
	{
		WaitForSingleObject(clientReadySemaphore, INFINITE); //Ждет извещения, что клиент готов к стрижке
		WaitForSingleObject(hairdresserMutex, INFINITE);
		
		int number = tableInWorkRoom.front();//Берет номерок клиента со стола
		tableInWorkRoom.pop();
		ReleaseMutex(hairdresserMutex);
		printf("Парикмахер %d стрижет клиента с номерком %d\n", i, number);
		Sleep((rand() % 3 + 1) * 1000);

		ReleaseSemaphore(endHaircutSemaphore[number], 1, NULL);//Извещает того клиента, которого стриг, что стрижка закончена                    
		WaitForSingleObject(clientLeaveSemaphore, INFINITE);//Ждет извещение, что клиент покинул кресло
		WaitForSingleObject(clientMutex, INFINITE);

		tableInWaitRoom.push(number);//Возвращает номерок для повторного использования
		ReleaseMutex(clientMutex);
		printf("Парикмахер %d готов принять следующего клиента\n", i);
		ReleaseSemaphore(hairdresserReadySemaphore, 1, NULL); //Посылает извещение, что кресло освободилось                      
	}


	return 0;
}
DWORD WINAPI сlientThread(LPVOID lpParam)
{
	int i = (int)lpParam;
	srand(time(NULL) + i);
	int j = 0;
	while(j < k)
	{
		WaitForSingleObject(clientMutex, INFINITE); 
		if (!tableInWaitRoom.empty())
		{
			int number = tableInWaitRoom.front();//Если есть свободные места, заходит и берет номерок со стола
			tableInWaitRoom.pop();
			ReleaseMutex(clientMutex);
			printf("Клиент %d зашел в парикмахерскую и взял номерок %d\n", i, number);

			WaitForSingleObject(hairdresserReadySemaphore, INFINITE);//Ждет извещения о свободном кресле
			WaitForSingleObject(hairdresserMutex, INFINITE);

			tableInWorkRoom.push(number);//Дождавшись извещения, заходит в рабочую комнату, кладет номерок
			ReleaseMutex(hairdresserMutex);
			printf("Клиент %d садится в кресло парикмахера\n", i);

			ReleaseSemaphore(clientReadySemaphore, 1, NULL);//Шлет извещение, что готов к стрижке

			WaitForSingleObject(endHaircutSemaphore[number], INFINITE);//Ждет извещения, что стрижка окончена

			ReleaseSemaphore(clientLeaveSemaphore, 1, NULL);//Шлет извещение, что покинул кресло.
			printf("Клиент %d постригся и уходит из парикмахерской\n", i);
			j++;
		}
		else
		{
			printf("Свободных мест нет. Клиент %d уходит из парикмахерской\n", i);
			ReleaseMutex(clientMutex);
		}
		Sleep((rand() % 5 + 1) * 1000);
	}

	return 0;
}