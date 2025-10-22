#include <gtest/gtest.h>
#include <windows.h>
#include <vector>
#include "Threads.h"
#include "utils.h"

const int TIME_TO_OUTPUT_STREAM = 50;
const int CHARS_TO_IGNORE = 1000000;

TEST(MarkerThreadTest, MultipleThreadsFillAndTerminate) {
    const int arraySize = 10;
    const int threadNumber = 3;

    int* array = new int[arraySize]();

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

        threadHandles[i] = CreateThread(NULL, 0, marker, &threadData[i], 0, NULL);
    }

    SetEvent(hStartEvent);

    int terminateId = 0;
    while (true) {
        std::vector<HANDLE> activeEventsForBlock;
        for (auto& el : threadData) {
            if (el.isActive) {
                activeEventsForBlock.push_back(el.hBlockEvent);
            }
        }

        if (activeEventsForBlock.empty()) break;

        WaitForMultipleObjects(static_cast<DWORD>(activeEventsForBlock.size()),
            activeEventsForBlock.data(), TRUE, INFINITE);
        Sleep(TIME_TO_OUTPUT_STREAM);

        for (int i = 0; i < arraySize; ++i) {
            EXPECT_TRUE(array[i] == 0 || (array[i] >= 1 && array[i] <= threadNumber))
                << "Array[" << i << "] = " << array[i];
        }

        SetEvent(threadData[terminateId].hTerminateEvent);
        WaitForSingleObject(threadHandles[terminateId], INFINITE);
        threadData[terminateId].isActive = FALSE;

        CloseHandle(threadHandles[terminateId]);
        CloseHandle(threadData[terminateId].hBlockEvent);
        CloseHandle(threadData[terminateId].hContinueEvent);
        CloseHandle(threadData[terminateId].hTerminateEvent);

        terminateId++;

        for (int i = 0; i < threadNumber; i++) {
            if (threadData[i].isActive) {
                SetEvent(threadData[i].hContinueEvent);
                ResetEvent(threadData[i].hBlockEvent);
            }
        }
    }

    DeleteCriticalSection(&cs);
    delete[] array;
}

TEST(MarkerThreadTest, MultipleThreadsFillAndTerminateButBigger) {
    const int arraySize = 100;
    const int threadNumber = 50;

    int* array = new int[arraySize]();

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

        threadHandles[i] = CreateThread(NULL, 0, marker, &threadData[i], 0, NULL);
    }

    SetEvent(hStartEvent);

    int terminateId = 0;
    while (true) {
        std::vector<HANDLE> activeEventsForBlock;
        for (auto& el : threadData) {
            if (el.isActive) {
                activeEventsForBlock.push_back(el.hBlockEvent);
            }
        }

        if (activeEventsForBlock.empty()) break;

        WaitForMultipleObjects(static_cast<DWORD>(activeEventsForBlock.size()),
            activeEventsForBlock.data(), TRUE, INFINITE);
        Sleep(TIME_TO_OUTPUT_STREAM);

        for (int i = 0; i < arraySize; ++i) {
            EXPECT_TRUE(array[i] == 0 || (array[i] >= 1 && array[i] <= threadNumber))
                << "Array[" << i << "] = " << array[i];
        }

        SetEvent(threadData[terminateId].hTerminateEvent);
        WaitForSingleObject(threadHandles[terminateId], INFINITE);
        threadData[terminateId].isActive = FALSE;

        CloseHandle(threadHandles[terminateId]);
        CloseHandle(threadData[terminateId].hBlockEvent);
        CloseHandle(threadData[terminateId].hContinueEvent);
        CloseHandle(threadData[terminateId].hTerminateEvent);

        terminateId++;

        for (int i = 0; i < threadNumber; i++) {
            if (threadData[i].isActive) {
                SetEvent(threadData[i].hContinueEvent);
                ResetEvent(threadData[i].hBlockEvent);
            }
        }
    }

    DeleteCriticalSection(&cs);
    delete[] array;
}