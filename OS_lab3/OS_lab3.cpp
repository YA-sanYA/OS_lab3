//#include "OS_lab3.h"
//#include <stdexcept>
//
//int main()
//{
//    int* array = nullptr;
//    HANDLE hStartEvent = nullptr;
//    CRITICAL_SECTION cs;
//    bool csInitialized = false;
//
//    try {
//        int arraySize = readNaturalNumber("Enter array size: ", "Invalid array size");
//
//        array = new int[arraySize];
//        for (int i = 0; i < arraySize; i++) {
//            array[i] = 0;
//        }
//
//        int threadNumber = readNaturalNumber("Enter the number of threads: ", "Invalid thread number");
//        std::cout << "\n";
//
//        hStartEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
//        if (!hStartEvent)
//            throw std::runtime_error("Failed to create start event");
//
//        InitializeCriticalSection(&cs);
//        csInitialized = true;
//
//        ThreadData data;
//        data.arr = array;
//        data.size = arraySize;
//        data.hStartEvent = hStartEvent;
//        data.cs = &cs;
//        data.isActive = TRUE;
//
//        std::vector<ThreadData> threadData(threadNumber, data);
//        std::vector<HANDLE> threadHandles(threadNumber);
//
//        for (int i = 0; i < threadNumber; i++) {
//            threadData[i].id = i + 1;
//            threadData[i].hBlockEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
//            threadData[i].hContinueEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
//            threadData[i].hTerminateEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
//
//            HANDLE hThread = CreateThread(NULL, 0, marker, &threadData[i], 0, NULL);
//            if (!hThread)
//                throw std::runtime_error("Failed to create thread");
//
//            threadHandles[i] = hThread;
//        }
//
//        SetEvent(hStartEvent);
//        std::vector<HANDLE> activeEventsForBlock;
//
//        while (TRUE) {
//            activeEventsForBlock.clear();
//            for (auto& el : threadData) {
//                if (el.isActive) {
//                    activeEventsForBlock.push_back(el.hBlockEvent);
//                }
//            }
//
//            WaitForMultipleObjects((DWORD)activeEventsForBlock.size(), activeEventsForBlock.data(), TRUE, INFINITE);
//            Sleep(TIME_TO_OUTPUT_STREAM);
//
//            writeArray(array, arraySize);
//
//            int terminateId;
//            std::cout << "Enter the thread number that will finish the job: ";
//            std::cin >> terminateId;
//
//            if (std::cin.fail()) {
//                std::cin.clear();
//                std::cin.ignore(CHARS_TO_IGNORE, '\n');
//                std::cout << "Invalid input, please enter a number\n\n";
//                continue;
//            }
//
//            if (terminateId <= 0 || terminateId > threadNumber || !threadData[terminateId - 1].isActive) {
//                std::cout << "Invalid id, try again\n\n";
//                std::cin.ignore(CHARS_TO_IGNORE, '\n');
//                continue;
//            }
//
//            std::cout << "\n";
//
//            terminateId--;
//            SetEvent(threadData[terminateId].hTerminateEvent);
//            WaitForSingleObject(threadHandles[terminateId], INFINITE);
//            threadData[terminateId].isActive = FALSE;
//
//            CloseHandle(threadHandles[terminateId]);
//            CloseHandle(threadData[terminateId].hBlockEvent);
//            CloseHandle(threadData[terminateId].hContinueEvent);
//            CloseHandle(threadData[terminateId].hTerminateEvent);
//
//            writeArray(array, arraySize);
//
//            for (int i = 0; i < threadNumber; i++) {
//                if (threadData[i].isActive) {
//                    SetEvent(threadData[i].hContinueEvent);
//                    ResetEvent(threadData[i].hBlockEvent);
//                }
//            }
//
//            bool anyActive = false;
//            for (auto& el : threadData) {
//                anyActive |= el.isActive;
//            }
//            if (!anyActive)
//                break;
//        }
//
//        if (csInitialized) DeleteCriticalSection(&cs);
//        if (array) delete[] array;
//        if (hStartEvent) CloseHandle(hStartEvent);
//    } catch (const std::exception& ex) {
//        std::cerr << "Error: " << ex.what() << "\n";
//
//        if (csInitialized) DeleteCriticalSection(&cs);
//        if (array) delete[] array;
//        if (hStartEvent) CloseHandle(hStartEvent);
//
//        return 1;
//    }
//
//    return 0;
//}
