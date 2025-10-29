#include "Threads.h"
#include <random>
#include <iostream>
#include <vector>
#include <mutex>

const int TIME_TO_SLEEP = 5;

DWORD WINAPI marker(LPVOID lpParam) {
    ThreadData* data = static_cast<ThreadData*>(lpParam);
    if (!data) return 1;

    WaitForSingleObject(data->hStartEvent, INFINITE);
    srand(data->id);

    std::vector<int> markedIndices;
    int index, val;

    while (TRUE) {
        index = rand() % data->size;

        EnterCriticalSection(data->cs);
        val = data->arr[index];
        LeaveCriticalSection(data->cs);

        if (val == 0) {
            Sleep(TIME_TO_SLEEP);

            EnterCriticalSection(data->cs);
            if (data->arr[index] == 0) {
                data->arr[index] = data->id;
                markedIndices.push_back(index);
            }
            LeaveCriticalSection(data->cs);

            Sleep(TIME_TO_SLEEP);
        }
        else {
            EnterCriticalSection(data->cs);
            std::cout << "Thread ordinal number: " << data->id << "\n"
                << "Number of marked elements: " << markedIndices.size() << "\n"
                << "Array index that impossible to mark: " << index << "\n\n"
                << std::flush;
            LeaveCriticalSection(data->cs);
            SetEvent(data->hBlockEvent);

            HANDLE waitHandles[2] = {data->hContinueEvent, data->hTerminateEvent};
            DWORD waitRes = WaitForMultipleObjects(2, waitHandles, FALSE, INFINITE);

            if (waitRes == WAIT_OBJECT_0) {
                ResetEvent(data->hContinueEvent);
                continue;
            }
            else if (waitRes == WAIT_OBJECT_0 + 1) {
                EnterCriticalSection(data->cs);
                for (int idx : markedIndices)
                    data->arr[idx] = 0;
                LeaveCriticalSection(data->cs);
                break;
            }
        }
    }

    return 0;
}

void markerThreads(ThreadDataForThreads* data) {
    std::vector<int> markedIndices;
    std::mt19937 rng(data->id);
    std::uniform_int_distribution<int> dist(0, data->size - 1);

    while (true) {
        int index = dist(rng);
        int val;

        {
            std::lock_guard<std::mutex> lock(*data->arrMutex);
            val = data->arr[index];
        }

        if (val == 0) {
            std::this_thread::sleep_for(std::chrono::milliseconds(TIME_TO_SLEEP));
            std::lock_guard<std::mutex> lock(*data->arrMutex);
            if (data->arr[index] == 0) {
                data->arr[index] = data->id;
                markedIndices.push_back(index);
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(TIME_TO_SLEEP));
        }
        else {
            {
                std::lock_guard<std::mutex> lock(*data->arrMutex);
                std::cout << "Thread ordinal number: " << data->id << "\n"
                    << "Number of marked elements: " << markedIndices.size() << "\n"
                    << "Array index that impossible to mark: " << index << "\n\n";
            }

            data->blocked = true;
            std::unique_lock<std::mutex> lock(*data->arrMutex);
            data->cv.wait(lock, [&] { return data->shouldContinue || data->shouldTerminate; });

            if (data->shouldTerminate) {
                for (int idx : markedIndices)
                    data->arr[idx] = 0;
                break;
            }

            data->shouldContinue = false;
            data->blocked = false;
        }
    }
}