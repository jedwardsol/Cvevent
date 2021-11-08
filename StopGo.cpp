#include <array>
#include <iostream>
#include <thread>
#include <chrono>
#include <vector>
using namespace std::literals;

#include "CondVar.h"


namespace
{

std::mutex  ioLock;


void worker(CondVar::StopGo  &event, int i)
{
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



void testStopGo()
{
    CondVar::StopGo             stopGo;
    std::array<std::thread,10>  threads;
    
    std::this_thread::sleep_for(2s);


    for(int i{}; auto &thread : threads)
    {
        thread = std::thread(worker, std::ref(stopGo),i++);
    }

    std::this_thread::sleep_for(2s);

    std::cout << "---\n";

    for(int i=0;i<5;i++)
    {
        std::cout << "1 -\n";
        stopGo.goOne();
        std::this_thread::sleep_for(50ms);
        stopGo.reset();
        std::this_thread::sleep_for(500ms);
        std::cout << "-\n";
    }


    for (int i = 0; i < 5; i++)
    {
        std::cout << "---\n";
        stopGo.goAll();
        std::this_thread::sleep_for(50ms);
        stopGo.reset();
        std::this_thread::sleep_for(500ms);
    }


    stopGo.stopAll();       

    for (auto & thread : threads)
    {
        thread.join();
    }

}





