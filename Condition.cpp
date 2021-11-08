#include <iostream>
#include <thread>
#include <chrono>
#include <vector>
using namespace std::literals;

#include "CondVar.h"


namespace
{


void periodic()
{
    CondVar::Condition  event;

    auto worker = [&]
    {
        std::cout << "periodic worker Starting\n";

        while (event.wait(3s) == CondVar::Condition::WaitResult::TimedOut)
        {
            std::cout << "Waiting...\n";
        }

        std::cout << "Waited...\n";
    };

    std::thread thread{ worker };


    std::this_thread::sleep_for(10s);

    std::cout << "signalling\n";
    event.signalOne();
    thread.join();
    std::cout << "done\n";
}



void oneShot()
{
    CondVar::Condition     event;

    auto worker = [&]
    {
        std::cout << "oneshot worker Starting\n";

        event.wait();

        std::cout << "Waited...\n";
    };

    std::thread thread{ worker };

    std::this_thread::sleep_for(3s);

    std::cout << "signalling\n";
    event.signalAll();
    thread.join();
    std::cout << "done\n";
}



void endTimeWorker(CondVar::Condition& event)
{
    std::cout << "endTime worker Starting\n";

    auto endTime = std::chrono::steady_clock::now() + 6s;

    if (event.wait(endTime) == CondVar::Condition::WaitResult::TimedOut)
    {
        std::cout << "endTime reached\n";
    }
    else
    {
        std::cout << "signalled...\n";
    }
};


void endTime1()
{
    CondVar::Condition     event;

    std::thread thread{ endTimeWorker, std::ref(event) };

    std::cout << "not signalling\n";
    thread.join();
    std::cout << "Done\n";
}


void endTime2()
{
    CondVar::Condition     event;

    std::thread thread{ endTimeWorker, std::ref(event) };

    std::this_thread::sleep_for(3s);
    std::cout << "signalling\n";
    event.signalOne();
    thread.join();
    std::cout << "Done\n";
}


void multiWorker(CondVar::Condition& event, int id)
{
    static std::mutex  ioLock;

    ioLock.lock();
    std::cout << "multiWorker " << id << " starting\n";
    ioLock.unlock();


    event.wait();

    ioLock.lock();
    std::cout << "multiWorker " << id << " ending\n";
    ioLock.unlock();
};




void signalAll()
{
    CondVar::Condition          event;

    std::vector<std::thread>    threads;

    for (int i = 0; i < 5; i++)
    {
        threads.emplace_back(multiWorker, std::ref(event), i);
    }


    std::this_thread::sleep_for(3s);

    std::cout << "signalling\n";
    event.signalAll();

    for (auto& thread : threads)
    {
        thread.join();
    }

    std::cout << "done\n";
}



void signalOne()
{
    CondVar::Condition            event;

    std::vector<std::thread>    threads;

    for (int i = 0; i < 5; i++)
    {
        threads.emplace_back(multiWorker, std::ref(event), i);
    }

    for (int i = 0; i < 5; i++)
    {
        std::this_thread::sleep_for(1s);
        event.signalOne();
    }


    for (auto& thread : threads)
    {
        thread.join();
    }

    std::cout << "done\n";
}


}

void testCondition()
{
    periodic();

    std::cout << "\n---\n\n";

    endTime1();

    std::cout << "\n---\n\n";

    endTime2();

    std::cout << "\n---\n\n";

    oneShot();

    std::cout << "\n---\n\n";

    signalAll();

    std::cout << "\n---\n\n";

    signalOne();

}