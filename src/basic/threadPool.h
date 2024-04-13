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

class join_threads {
public:
    explicit join_threads(std::vector<std::thread> &threads) : threads_(threads) { }

    ~join_threads()
    {
        for (auto &t : threads_)
            if (t.joinable()) { t.join(); }
    }

private:
    std::vector<std::thread> &threads_;
};

class ThreadPool final {
private:
    std::atomic_bool done;
    // we use our own FunctionWrapper instead of std::function here
    //ThreadSafeQueue<FunctionWrapper> work_queue;
    ThreadSafeQueue<std::function<void()> > work_queue;
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
        unsigned const thread_count = 1;//std::max(1u, std::thread::hardware_concurrency());
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
        for(auto & thread : threads){
          if(thread.joinable()) thread.join();
        }
    }

    template<typename FunctionType>
    std::future<std::invoke_result_t<FunctionType>> submit(FunctionType f) {
        typedef std::invoke_result_t<FunctionType> result_type;

        auto ptask = std::make_shared<std::packaged_task<result_type()> >(std::move(f));
        auto res = ptask->get_future();
        work_queue.push([ptask]() { (*ptask)(); });
        return res;
    }
};


#endif //VERTICES_THREADPOOL_H
