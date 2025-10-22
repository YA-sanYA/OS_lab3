#pragma once

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