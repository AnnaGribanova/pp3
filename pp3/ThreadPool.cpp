#include "ThreadPool.h"

#include <iostream>
#include <vector>
#include <thread>
#include <mutex>
#include <queue>
#include <functional>
#include <condition_variable>
#include <stdexcept>

using namespace std;

ThreadPool::ThreadPool() : stop(false)
{
    unsigned int tCount = thread::hardware_concurrency();
    if (tCount == 0)
        tCount = 1;
    for (unsigned int i = 0; i < tCount; ++i)
    {
#if defined(_WIN32)  || defined(_WIN64)
        HANDLE thread = (HANDLE) _beginthreadex(NULL, 0, [](void* param) -> unsigned {
            return static_cast<ThreadPool*>(param)->run();
            }, this, 0, NULL);
        if (thread != 0) {
            workers.emplace_back(thread);
        }
#elif defined(__linux__)
        pthread_t thread;
        int result = pthread_create(&thread, nullptr, [](void* param) -> void* {
            return static_cast<ThreadPool*>(param)->run();
            }, this);
        if (result == 0) {
            workers.emplace_back(thread);
        }
#else
        static_assert(false, "Bad platform");
#endif
    }
    cout << "ThreadPool started, thread count: " << tCount << endl;
}

ThreadPool::~ThreadPool()
{
    stop = true;
    condition.notify_all();

    cout << "ThreadPool destroying..." << endl;

#if defined (_WIN32) || defined (_WIN64)
    WaitForMultipleObjects(workers.size(), workers.data(), TRUE, INFINITE);
#endif

    for (unsigned int i = 0; i < workers.size(); ++i)
    {
#if defined(_WIN32)  || defined(_WIN64)
        CloseHandle(workers[i]);
#elif defined(__linux__)
        pthread_join(workers[i], nullptr);

#endif
    }
    cout << "ThreadPool destroyed" << endl;
}

void ThreadPool::enqueue(const function<void()>& job)
{
    {
        lock_guard<mutex> lock(queueMutex);
        tasks.emplace(job);
    }
    condition.notify_one();
}

bool ThreadPool::empty()
{
    return tasks.empty();
}

#if defined (_WIN32) || defined (_WIN64)
unsigned __stdcall ThreadPool::run()
#else
void* ThreadPool::run()
#endif
{
    for (;;) {
        function<void()> task;

        {
            unique_lock<mutex> lock(this->queueMutex);
            this->condition.wait(lock, [this]
                {
                    return this->stop || !this->tasks.empty();
                });

            if (this->stop)
            {
#if defined(_WIN32) || defined(_WIN64)
                return 0;
#else
                return nullptr;
#endif
            }

            task = move(this->tasks.front());
            this->tasks.pop();
        }

        task();
    }
}