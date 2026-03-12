#include "ThreadPool.h"

signed long long numAccess = _HUGE1_;

extern ThreadPool pool;

/*****************************************************************************************
* Function Description: 
* @parameters: 
* @returntype: 
*****************************************************************************************/
ThreadPool::~ThreadPool()
{
    m_ThreadPoolQueue.Stop(false);
    for (int i = 0; i < m_Threads.size(); ++i) 
    {
        m_Threads[i].m_alive = false;
        if(m_Threads[i].m_p_Thread && m_Threads[i].m_p_Thread->joinable())
        {
            m_Threads[i].m_p_Thread->join();
        }
    }
}

/*****************************************************************************************
* Function Description: 
* @parameters: 
* @returntype: 
*****************************************************************************************/
template<class T> T ThreadSafeQueue<T>::fnPop()
{
    // std::unique_lock<std::mutex> tlock(m_lock);
    // while (m_fifoQueue.empty())
    // {
    //   m_condition.wait(tlock);
    // }
    // T val = m_fifoQueue.front();
    // m_fifoQueue.pop();
    // --numAccess;
    // return val;

    std::unique_lock<std::mutex> mlock(m_lock);
    m_condition.wait(mlock, [&]{return  cFlag;});
    T val;
    if(State::READY == m_queueState)
    {
        val = m_fifoQueue.front();
        m_fifoQueue.pop();
	}
	if(m_fifoQueue.size()==0)
	{
		cFlag = false; //setting accessFlag to false if size is zero;
	}
    --numAccess;
    return val;
};

/*****************************************************************************************
* Function Description: 
* @parameters: 
* @returntype: 
*****************************************************************************************/
// template<class T> void ThreadSafeQueue<T>::fnPush(const T& elem)
// {
//     std::unique_lock<std::mutex> mlock(m_lock);
//     m_fifoQueue.push(elem);
//     mlock.unlock();
//     cFlag = true;
//     m_condition.notify_all();
// }

/*****************************************************************************************
* Function Description: 
* @parameters: 
* @returntype: 
*****************************************************************************************/
template<class T> void ThreadSafeQueue<T>::fnClear()
{
    std::unique_lock<std::mutex> mlock(m_lock);
    while(!m_fifoQueue.empty()) m_fifoQueue.pop();
    mlock.unlock();
    m_condition.notify_all();
}


/******************************************************************************
* Function Description: 
* @parameters: 
* @returntype: void 
*******************************************************************************/
template<class T> void ThreadSafeQueue<T>::Stop(bool waitCompletion) 
{
    std::unique_lock<std::mutex> mlock(m_lock);
    m_queueState = State::STOPPED;
    cFlag = true;
    m_condition.notify_all();
    if(waitCompletion) 
    {
        // Wait till all events have been consumed.
        while(!m_fifoQueue.empty())
            m_condition.wait(mlock);
    }
    else 
    {
        // Cancel all pending events.
        fnClear();
    }
}

/*****************************************************************************************
* Function Description: 
* @parameters: 
* @returntype: 
*****************************************************************************************/
void ThreadPool::fnStart()
{
    for (int i = 0; i < m_Threads.size(); ++i)
    {
        m_Threads[i].m_id = i;
        m_Threads[i].m_alive = true;
        m_Threads[i].m_p_Thread = new std::thread(std::bind(&ThreadPool::fnListen, this, &m_Threads[i]));
    }
    return;
}

/*****************************************************************************************
* Function Description: 
* @parameters: 
* @returntype: 
*****************************************************************************************/
void ThreadPool::operator()()   //fnStart()
{
    for (int i = 0; i < m_Threads.size(); ++i)
    {
        m_Threads[i].m_id = i;
        m_Threads[i].m_alive = true;
        m_Threads[i].m_p_Thread = new std::thread(std::bind(&ThreadPool::fnListen, this, &m_Threads[i]));
    }
    return;
}

/*****************************************************************************************
* Function Description: 
* @parameters: 
* @returntype: 
*****************************************************************************************/
void ThreadPool::operator()(bool runThread)   
{
    for (int i = 0; i < m_Threads.size(); ++i)
    {
        m_Threads[i].m_alive = runThread;
        m_Threads[i].m_p_Thread->join();
    }
    return;
}

/*****************************************************************************************
* Function Description: 
* @parameters: 
* @returntype: 
*****************************************************************************************/
void ThreadPool::fnStop()
{
    for (int i = 0; i < m_Threads.size(); ++i)
    {
        m_Threads[i].m_alive = false;
    }
    usleep(50);
    for (int i = 0; i < m_Threads.size(); ++i) 
    {
      if(m_Threads[i].m_p_Thread && m_Threads[i].m_p_Thread->joinable())
      {
        m_Threads[i].m_p_Thread->join();
      }
    }
    usleep(50);
    m_ThreadPoolQueue.fnClear();
}

/*****************************************************************************************
* Function Description: 
* @parameters: 
* @returntype: 
*****************************************************************************************/
void ThreadPool::fnListen(ThreadsParams_t* curThread)
{
    while(curThread->m_alive)
    {
        std::function<void()> tempFun = m_ThreadPoolQueue.fnPop();
        tempFun();
    }
}

/*****************************************************************************************
* Function Description: 
* @parameters: 
* @returntype: 
*****************************************************************************************/
void testFun(ThreadPool* pool)                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                            
{
    #define LARGE 10000000
    #define MEM_SIZE 50000
    static std::atomic<unsigned long int> s(0);

#if 0
    while(true)
    {
        if(t_lock.try_lock())
        {
            if(s%LARGE == 0)
            {
                std::cout<<"Value of S is "<<s<<std::endl;
                if((s%(4*LARGE) ==0) && s) exit(0);
            }
            s++;
            t_lock.unlock();
            break;
        }
    }
#endif
     //std::thread::id this_id = std::this_thread::get_id();
     //std::cout<<"this thread is "<<this_id<<"\n";
    if(0)
    {
        
        //pool->putDummyInQueue();
        return;
    }
    int *test = (int *)malloc(MEM_SIZE*sizeof(int));
    for(int i = 0; i < MEM_SIZE; i++)
    {
        volatile int a;
        a = 10;
        test[i] = i+a;
    }
    free(test);
    return;
}

/*****************************************************************************************
* Function Description: 
* @parameters: 
* @returntype: 
*****************************************************************************************/
void ThreadPool::fnPutDummyInQueue()
{
    m_ThreadPoolQueue.fnPush(std::bind(testFun, this));
}
