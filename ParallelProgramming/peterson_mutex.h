#ifndef PETERSON_MUTEX_H
#define PETERSON_MUTEX_H

#include <thread>
#include <array>
#include <atomic>

class peterson_mutex {
public:
     peterson_mutex();
     peterson_mutex(peterson_mutex&&):victim(0) {}
     void lock(int t) ;

     void unlock(int t);

private:
    std::array<std::atomic<bool>, 2> want;
    std::atomic<int> victim;
};

inline peterson_mutex::peterson_mutex(){
    want[0].store(false);
    want[1].store(false);
    victim.store(0);
}

void peterson_mutex::lock(int t){
    want[t].store(true);
    victim.store(t);
    while (want[1 - t].load() && victim.load() == t) {
        std::this_thread::yield();
    }
}

void peterson_mutex::unlock(int t){
    want[t].store(false);
}

#endif // PETERSON_MUTEX_H

