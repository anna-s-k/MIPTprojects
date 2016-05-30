#ifndef THREAD_POOL
#define THREAD_POOL

#include <future>
#include <functional>
#include <thread>
#include <vector>
#include <thread>
#include <iostream>
#include <atomic>
#include "thread_safe_queue.h"

std::size_t defaul_num_workers(){
    return (std::thread::hardware_concurrency() == 0 ? 2 : std::thread::hardware_concurrency());
}



template<class R>
class thread_pool{
public:

    explicit thread_pool(std::size_t num_threads = defaul_num_workers());
    std::future<R> submit(std::function<R()> func);
    void shutdown();

private:
    //static const std::size_t defaul_num_workers = (std::thread::hardware_concurrency() == 0 ? 2 : std::thread::hardware_concurrency());
    thread_safe_queue<std::packaged_task<R()> > queue;
    std::vector<std::thread> workers;
    std::atomic<bool> access;

    //std::size_t defaul_num_workers();

};




template<class R>
thread_pool<R>::thread_pool(std::size_t num_threads) : queue(std::numeric_limits<std::size_t>::max()){
    access.store(true);
    for (std::size_t i = 0; i < num_threads; i++){
        workers.push_back(std::move(std::thread([this](){
            while(access.load()){
                try{
                    std::packaged_task<R()> task;

                    queue.pop(task);
                    task();
                }
                catch(...){
                    //std::cout << "exc" << std::endl;
                    //throw std::exception();

                }
            }
        })));
    }
}

template<class R>
std::future<R> thread_pool<R>::submit(std::function<R()> _func){
    try{
        std::packaged_task<R()> func(_func);
        std::future<R> ftr = func.get_future();

        queue.enqueue(std::move(func));
        return ftr;
    }
    catch(...){
        //std::cout << "exc_enq" << std::endl;
        throw std::exception();
    }
}

template<class R>
void thread_pool<R>::shutdown(){
    queue.shutdown();
    //worker-ы работают всегда, поэтому после shutdown очередь будет пуста
    access.store(false);
    for(std::size_t i = 0; i < workers.size(); i++){

        workers[i].join();
    }
}

#endif // THREAD_POOL

