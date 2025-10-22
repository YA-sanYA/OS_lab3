#include "OS_lab3.h"

const int TIME_TO_OUTPUT_STREAM = 50;
const int CHARS_TO_IGNORE = 1000000;

int main()
{
	int arraySize;
	std::cout << "Enter array size: ";
	if (!(std::cin >> arraySize) || arraySize <= 0) {
		std::cout << "Invalid array size";
		return 0;
	}

	int* array = new int[arraySize];
	for (int i = 0; i < arraySize; i++) {
		array[i] = 0;
	}

	int threadNumber;
	std::cout << "Enter the number of threads: ";
	if (!(std::cin >> threadNumber) || threadNumber <= 0) {
		std::cout << "Invalid thread number";
		delete[] array;
		return 0;
	}

	std::cout << "\n";

	HANDLE hStartEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	CRITICAL_SECTION cs;
	InitializeCriticalSection(&cs);

	ThreadData data;
	data.arr = array;
	data.size = arraySize;
	data.hStartEvent = hStartEvent;
	data.cs = &cs;
	data.isActive = TRUE;


	std::vector<ThreadData> threadData(threadNumber, data);
	std::vector<HANDLE> threadHandles(threadNumber);

	for (int i = 0; i < threadNumber; i++) {
		threadData[i].id = i + 1;
		threadData[i].hBlockEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
		threadData[i].hContinueEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
		threadData[i].hTerminateEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

		HANDLE hThread = CreateThread(NULL, 0, marker, &threadData[i], 0, NULL);
		threadHandles[i] = hThread;
	}

	SetEvent(hStartEvent);

	std::vector<HANDLE> activeEventsForBlock;

	while (TRUE) {
		activeEventsForBlock.clear();
		for (auto& el : threadData) {
			if (el.isActive) {
				activeEventsForBlock.push_back(el.hBlockEvent);
			}
		}

		WaitForMultipleObjects(threadNumber, activeEventsForBlock.data(), TRUE, INFINITE);
		Sleep(TIME_TO_OUTPUT_STREAM);

		std::cout << "Array contents: ";
		for (int i = 0; i < arraySize; i++) {
			std::cout << array[i] << " ";
		}
		std::cout << "\n\n";

		int terminateId;
		std::cout << "Enter the thread number that will finish the job: ";
		std::cin >> terminateId;

		if (std::cin.fail()) {
			std::cin.clear();
			std::cin.ignore(CHARS_TO_IGNORE, '\n');
			std::cout << "Invalid input, please enter a number\n\n";
			continue;
		}

		if (terminateId <= 0 || terminateId > threadNumber || !threadData[terminateId - 1].isActive) {
			std::cout << "Invalid id, try again\n\n";
			std::cin.ignore(CHARS_TO_IGNORE, '\n');
			continue;
		}

		terminateId--;

		std::cout << "\n";

		SetEvent(threadData[terminateId].hTerminateEvent);

		WaitForSingleObject(threadHandles[terminateId], INFINITE);
		threadData[terminateId].isActive = FALSE;

		CloseHandle(threadHandles[terminateId]);
		CloseHandle(threadData[terminateId].hBlockEvent);
		CloseHandle(threadData[terminateId].hContinueEvent);
		CloseHandle(threadData[terminateId].hTerminateEvent);

		std::cout << "Array contents: ";
		for (int i = 0; i < arraySize; i++) {
			std::cout << array[i] << " ";
		}
		std::cout << "\n\n";

		for (int i = 0; i < threadNumber; i++) {
			if (threadData[i].isActive) {
				SetEvent(threadData[i].hContinueEvent);
				ResetEvent(threadData[i].hBlockEvent);
			}
		}

		bool anyActive = FALSE;
		for (auto& el : threadData) {
			anyActive |= el.isActive;
		}
		if (!anyActive) {
			break;
		}
	}
	DeleteCriticalSection(&cs);
	delete[] array;
	return 0;
}
