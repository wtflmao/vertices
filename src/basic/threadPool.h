//
// Created by root on 24-4-12.
//

#ifndef VERTICES_THREADPOOL_H
#define VERTICES_THREADPOOL_H


#include "functionWrapper.h"
#include "threadSafeQueue.h"
#include <future>
#include <thread>
#include <vector>
#include <memory>

struct join_threads final
{
    explicit join_threads(std::vector<std::thread>&) {}
};

class ThreadPool final {
private:
    std::atomic_bool done;
    // we use our own FunctionWrapper instead of std::function here
    //ThreadSafeQueue<FunctionWrapper> work_queue;
    ThreadSafeQueue<std::function<void()>> work_queue;
    std::vector<std::thread> threads;
    join_threads joiner;
    void workerThread() {
        while (!done) {
            //FunctionWrapper task;
            std::function<void()> task;
            if (work_queue.try_pop(task))
                task();
            else
                std::this_thread::yield();
        }
    }

public:
    ThreadPool() : done(false), joiner(threads) {
        unsigned const thread_count = std::max(1u, std::thread::hardware_concurrency());
        threads.reserve(thread_count);
        try {
            for (unsigned i = 0; i < thread_count; ++i)
                threads.emplace_back(&ThreadPool::workerThread, this);
        } catch (...) {
            done = true;
            throw;
        }
    }
    ~ThreadPool() {
        done = true;
    }
    /*template<typename FunctionType>
    std::future<std::result_of_t<FunctionType()>> submit(FunctionType f) {
        typedef std::result_of_t<FunctionType()> result_type;
        std::packaged_task<result_type()> task(std::move(f));
        std::future<result_type> res(task.get_future());
        work_queue.push(std::move(task));
        return res;
    }*/
    template<typename FunctionType>
    std::future<typename std::result_of<FunctionType()>::type> submit(FunctionType f) {
        typedef typename std::result_of<FunctionType()>::type result_type;

        auto ptask = std::make_shared<std::packaged_task<result_type()>>(std::move(f));
        auto res = ptask->get_future();
        //std::packaged_task<result_type()> task(std::move(f));
        //std::future<result_type> res(task.get_future());
        //work_queue.push(std::move(task));
        work_queue.push([ptask]() { (*ptask)(); });
        return res;
    }
};



#endif //VERTICES_THREADPOOL_H
