#pragma once
#include <mutex>
#include <condition_variable>
#include <chrono>

class Cvevent
{
public:
    void    signal()
    {
        mutex.lock();
        condition=true;
        mutex.unlock();
        condition_variable.notify_all();           
    }

    void wait() 
    {
        std::unique_lock    lock{mutex};
        condition_variable.wait(lock, [this]{return condition;});
    }

    enum class WaitResult
    {
        Signalled, TimedOut
    };
    
    template< class Rep, class Period >
    WaitResult [[nodiscard]] wait(std::chrono::duration<Rep, Period> const &duration)
    {
        std::unique_lock    lock{ mutex };

        bool signalled = condition_variable.wait_for(lock, duration, [this] {return condition; });

        return signalled ? WaitResult::Signalled : WaitResult::TimedOut ;
    }


    template< class Clock, class Duration>
    WaitResult [[nodiscard]] wait(std::chrono::time_point<Clock, Duration> const &endTime)
    {
        std::unique_lock    lock{ mutex };

        bool signalled = condition_variable.wait_until(lock, endTime, [this] {return condition; });

        return signalled ? WaitResult::Signalled : WaitResult::TimedOut;
    }

private:
    std::mutex              mutex;
    bool                    condition{false};
    std::condition_variable condition_variable;
};

