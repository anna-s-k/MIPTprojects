#ifndef BARRIER
#define BARRIER

#include <condition_variable>
#include <mutex>
#include <atomic>

class barrier{
public:
    barrier(std::size_t num_threads);
    void enter();
private:
    std::mutex mtx;
    std::condition_variable cv_enter;
    std::size_t counter;
    std::size_t num_threads;
    std::size_t gate_counter;
    bool gone;

};

barrier::barrier(std::size_t _num_threads) {
    num_threads = _num_threads;
    counter = 0;
    gate_counter = 0;
    gone = false;
}

void barrier::enter(){
    std::unique_lock<std::mutex> lck(mtx);
    counter++;
    cv_enter.wait(lck, [this] () { return (counter == num_threads);}); // ждет пока все соберутся
    cv_enter.notify_one(); // для последнего выполнен предикат
    gate_counter ++;
    cv_enter.wait(lck, [this] () { return (gate_counter == num_threads);});
    cv_enter.notify_one();
    counter--;
    if (counter == 0) // все вышли из барьера
        gate_counter = 0;
}

#endif // BARRIER

