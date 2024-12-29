#include <iostream>
#include <vector>
#include <thread>
#include <mutex>
#include <queue>
#include <functional>
#include <condition_variable>
#if defined(_WIN32) || defined(_WIN64)
#include <Windows.h>
#else
#include <pthread.h>
#endif

using namespace std;

class ThreadPool
{
public:
    ThreadPool();
    ~ThreadPool();

    void enqueue(const function<void()>& job);
    bool empty();

private:
#if defined (_WIN32) || defined (_WIN64)
    unsigned __stdcall run();
#else
    void* run();
#endif

#if defined (_WIN32) || defined (_WIN64)
    vector<HANDLE> workers;
#else
    vector<pthread_t> workers;
#endif
    queue<function<void()>> tasks;

    mutex queueMutex;
    condition_variable condition;
    bool stop;
};