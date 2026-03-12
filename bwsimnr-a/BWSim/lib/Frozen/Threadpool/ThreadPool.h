#ifndef _THREAD_POOL_H_
#define _THREAD_POOL_H_

#include <iostream>
#include <mutex>
#include <atomic>
#include <queue>
#include <unistd.h>
#include <thread>
#include <vector>
#include <condition_variable>
#include<functional>
#define _HUGE1_ 100000 /* For testing */

//! A Structure 
 /*!
  Structure description:
 */
typedef struct ThreadParams
{
    int m_id;
    std::atomic<bool> m_alive{false};
    std::thread *m_p_Thread;
}ThreadsParams_t;

//! A class
 /*!
 class description:
 */
template <typename T>
class ThreadSafeQueue {

    private:

        std::queue<T> m_fifoQueue;
        std::mutex m_lock;
        std::condition_variable m_condition;
        bool cFlag = false;
        enum class State
        {
            STOPPED,
            READY
        };
        State m_queueState;
    public:
        ThreadSafeQueue():m_queueState(State::READY){}
        T fnPop();
        void fnPush(const T& elem)
        {
            std::unique_lock<std::mutex> mlock(m_lock);
            m_fifoQueue.push(elem);
            mlock.unlock();
            cFlag = true;
            m_condition.notify_all();
        }
        void fnClear();
        void Stop(bool);
        // void operator()(const T& elem);
};

//! A class
 /*!
 class description:
 */
class ThreadPool{

    private:

        int m_threadId;
        std::vector<ThreadsParams_t> m_Threads;
        ThreadSafeQueue<std::function<void()>> m_ThreadPoolQueue;

    public:
        ThreadPool(const int numThreads)
        : m_Threads(std::vector<ThreadsParams_t>(numThreads)) {}
        ~ThreadPool();
        void fnListen(ThreadsParams_t* curThread);
        void fnStart();
        void fnStop();
        void fnPutDummyInQueue();
        void operator()();
        void operator()(bool);
	template <typename T>
	// void fnPush(const T& elem) {
	//   m_ThreadPoolQueue.fnPush(elem);
	// }
    void operator()(const T& elem) {
	  m_ThreadPoolQueue.fnPush(elem);
	}
};

#endif