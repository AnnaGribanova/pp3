#include <iostream>
#include "ThreadPool.h"
#include <ctime>
#include <fstream>
#include <mutex>

mutex fileMutex;
ofstream* outputFile;

const int ArraySize = 10000000;

void saveToFile(int value) {
	lock_guard<mutex> lock(fileMutex);
	*outputFile << value << endl;
}

void randomArraySum(int randomGen)
{
	srand(randomGen);
	int* arr = new int[ArraySize];
	for (int i = 0; i < ArraySize; ++i) {
		arr[i] = rand() % 10;
	}
	long sum = 0;
	for (int i = 0; i < ArraySize; ++i) {
		sum += arr[i];
	}
	delete[] arr;
	saveToFile(sum);
}

int main()
{
	srand((unsigned int)time(0));
	outputFile = new ofstream("output.txt");

	{
		ThreadPool pool;

		cout << "Start finding random array sum" << endl;

		for (int i = 0; i < 20; ++i)
		{
			pool.enqueue(bind(randomArraySum, rand()));
		}

		while (!pool.empty());
	}

	delete outputFile;
	outputFile = nullptr;
	cout << "Results saved to file output.txt" << endl;
}