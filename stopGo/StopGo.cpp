#include <array>
#include <iostream>
#include <thread>
#include <chrono>
#include <vector>
using namespace std::literals;

#include "condvar/CondVar.h"


namespace
{



void singleWorker(CondVar::StopGo& event, int i)
{
    while (event.wait() == CondVar::StopGo::WaitResult::Go)
    {
        {
            event.reset();
            std::cout << "Thread " << i << " going\n";
        }
    }

    std::cout << "Thread " << i << " stopping\n";
}





void batchWorker(CondVar::StopGo  &event, int i)
{
    static std::mutex  ioLock;

    while(event.wait() == CondVar::StopGo::WaitResult::Go)
    {
        {
            std::unique_lock    _{ioLock};
            std::cout << "Thread " << i << " going\n";
        }

        std::this_thread::sleep_for(100ms);
    }

    std::cout << "Thread " << i << " stopping\n";

}


}



void testStopGo_single()
{
    CondVar::StopGo             stopGo;
    std::array<std::thread,5>  threads;
    

    for(int i{}; auto &thread : threads)
    {
        thread = std::thread(singleWorker, std::ref(stopGo),i++);
    }


    for(int i=0;i<10;i++)
    {
        stopGo.goOne();
        std::this_thread::sleep_for(500ms);
    }

    stopGo.stopAll();

    for (auto& thread : threads)
    {
        thread.join();
    }
}



void testStopGo_batch()
{
    CondVar::StopGo             stopGo;
    std::array<std::thread, 5>  threads;


    for (int i{}; auto & thread : threads)
    {
        thread = std::thread(batchWorker, std::ref(stopGo), i++);
    }


    for (int i = 0; i < 5; i++)
    {
        stopGo.goAll();
        std::this_thread::sleep_for(100ms);         // poor.
        stopGo.reset();
        std::this_thread::sleep_for(500ms);
    }

    stopGo.stopAll();

    for (auto& thread : threads)
    {
        thread.join();
    }
}





int main()
{
    testStopGo_single();

    std::cout << "\n----\n";

    testStopGo_batch();
}