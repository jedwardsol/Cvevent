#include <iostream>
#include <thread>
#include <chrono>
#include <vector>
using namespace std::literals;

#include "cvevent.h"




void periodic()
{
    Cvevent     event;

    auto worker = [&]
    {
        std::cout << "periodic worker Starting\n";

        while (event.wait(3s) == Cvevent::WaitResult::TimedOut)
        {
            std::cout << "Waiting...\n";
        }

        std::cout << "Waited...\n";
    };

    std::thread thread{worker};
    

    std::this_thread::sleep_for(10s);

    std::cout << "signalling\n";
    event.signalOne();
    thread.join();
    std::cout << "done\n";
}



void oneShot()
{
    Cvevent     event;

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



void endTimeWorker(Cvevent &event)
{
    std::cout << "endTime worker Starting\n";

    auto endTime = std::chrono::steady_clock::now() + 6s;

    if (event.wait(endTime) == Cvevent::WaitResult::TimedOut)
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
    Cvevent     event;

    std::thread thread{ endTimeWorker, std::ref(event) };

    std::cout << "not signalling\n";
    thread.join();
    std::cout << "Done\n";
}


void endTime2()
{
    Cvevent     event;

    std::thread thread{ endTimeWorker, std::ref(event) };

    std::this_thread::sleep_for(3s);
    std::cout << "signalling\n";
    event.signalOne();
    thread.join();
    std::cout << "Done\n";
}


void multiWorker(Cvevent& event,int id)
{
    std::cout << "multiWorker " << id << " starting\n";

    event.wait();

    std::cout << "multiWorker " << id << " ending\n";
};




void signalAll()
{
    Cvevent                     event;

    std::vector<std::thread>    threads;

    for(int i=0;i<5;i++)
    {
        threads.emplace_back(multiWorker,std::ref(event),i);
    }


    std::this_thread::sleep_for(3s);

    std::cout << "signalling\n";
    event.signalAll();

    for(auto &thread : threads)
    {
        thread.join();
    }

    std::cout << "done\n";
}



void signalOne()
{
    Cvevent                     event;

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



int main()
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