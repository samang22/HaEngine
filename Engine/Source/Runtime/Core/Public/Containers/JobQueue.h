#pragma once
#include "CoreTypes.h"

// Multiple Producer Single Consumer를 가정해서 작성
class CORE_API FJobQueue
{
public:
    void Push(std::function<void()> NewJob)
    {
        if (!NewJob)
        {
            if (!bShutdown)
            {
                _ASSERT(false);
                return;
            }
        }

        {
            std::lock_guard<std::mutex> Lock(Mutex);
            Jobs.push(std::move(NewJob));
        }
        CV.notify_one(); // Thread 하나를 wait 중인 경우 깨운다. wait이 아니라면 무시
    }

    std::function<void()> Pop()
    {
        std::unique_lock<std::mutex> Lock(Mutex);


        CV.wait(Lock, [this] { return !Jobs.empty(); });

        std::function<void()> Job = std::move(Jobs.front());
        Jobs.pop();
        return Job;
    }

    bool Empty() const
    {
        std::lock_guard<std::mutex> Lock(Mutex);
        return Jobs.empty();
    }

    bool IsShutdown() const
    {
        return bShutdown;
    }

    void Shutdown()
    {
        bShutdown = true;
        Push(nullptr);
    }

private:
    bool bShutdown = false;
    mutable std::mutex Mutex;
    std::condition_variable CV;
    std::queue<std::function<void()>> Jobs;
};