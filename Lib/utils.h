#pragma once

#include <windows.h>
#include <iostream>
#include <string>
#include <mutex>

const int TIME_TO_OUTPUT_STREAM = 100;
const int CHARS_TO_IGNORE = 1000000;

struct ThreadData {
	int* arr;
	int size;
	int id;
	CRITICAL_SECTION* cs;

	HANDLE hStartEvent;
	HANDLE hBlockEvent;
	HANDLE hContinueEvent;
	HANDLE hTerminateEvent;

	bool isActive;
};

struct ThreadDataForThreads {
	int id;
	int* arr;
	int size;
	std::mutex* arrMutex;

	bool isActive = true;
	bool shouldContinue = false;
	bool shouldTerminate = false;
	bool blocked = false;

	std::condition_variable cv;
};

void writeArray(int*, int);

int readNaturalNumber(std::string, std::string);