#pragma once
#include <mutex>
#include <condition_variable>
#include <chrono>


namespace CondVar
{
class Condition
{
public:

    void    reset()
    {
        mutex.lock();
        condition = false;
        mutex.unlock();
    }

    void    signalAll()
    {
        mutex.lock();
        condition=true;
        mutex.unlock();
        condition_variable.notify_all();           
    }

    void    signalOne()
    {
        mutex.lock();
        condition = true;
        mutex.unlock();
        condition_variable.notify_one();
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



class StopGo
{
public:

    void    reset()
    {
        mutex.lock();
        go = false;
        mutex.unlock();
    }


    void    stopAll()
    {
        mutex.lock();
        stop = true;
        mutex.unlock();
        condition_variable.notify_all();
    }

    void    goAll()
    {
        mutex.lock();
        go = true;
        mutex.unlock();
        condition_variable.notify_all();
    }

    void    goOne()
    {
        mutex.lock();
        go = true;
        mutex.unlock();
        condition_variable.notify_one();
    }


    enum class WaitResult
    {
        Stop, Go
    };


    WaitResult wait()
    {
        std::unique_lock    lock{ mutex };
        condition_variable.wait(lock, [this] {return stop || go; });

        return stop ? WaitResult::Stop : WaitResult::Go;
    }


private:
    std::mutex              mutex;
    bool                    go  { false };
    bool                    stop{ false };
    std::condition_variable condition_variable;
};




}