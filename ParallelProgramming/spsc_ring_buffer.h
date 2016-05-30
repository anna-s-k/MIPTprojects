#ifndef SPSC_RING_BUFFER
#define SPSC_RING_BUFFER

/************************
 * Tail может изменяться только в enqueu, значит для чтения tail достаточно использовать relaxed
 * Аналогично с Head в dequeue
 * Между оставшимися операциями должно существовать отношение SW, т.к. мы должны получать информацию от другого потока,
 * чтобы узнать, не пуста ли / не переполнена ли очередь.*/

#include <vector>
#include <atomic>

template <class Value>
class spsc_ring_buffer {
public:
    explicit spsc_ring_buffer(size_t size) :
        capacity(size + 1),
        buffer(capacity){}

    bool enqueue(Value v);
    bool dequeue(Value& v);
private:

    class node_t{
    public:
        node_t(Value v): val(v){}
        node_t() : val(0) {}
        Value val;
        char pad[64-sizeof(Value) % 64];
    };

    std::size_t capacity;
    std::vector<node_t> buffer;
    //std::vector<Value> buffer;
    std::atomic<std::size_t> head{0};
    std::atomic<std::size_t> tail{0};

    std::size_t next(std::size_t index);

};

template <class Value>
std::size_t spsc_ring_buffer<Value>::next(std::size_t index){
    return (index + 1) % capacity;
}

template <class Value>
bool spsc_ring_buffer<Value>::enqueue(Value v){

    std::size_t cur_head = head.load( std::memory_order_acquire );
    std::size_t cur_tail = tail.load( std::memory_order_relaxed );


    if (next(cur_tail) == cur_head){
        return false;
    }
    else{
        buffer[cur_tail] = node_t(v);
        tail.store(next(cur_tail), std::memory_order_release);
        return true;
    }
}

template <class Value>
bool spsc_ring_buffer<Value>::dequeue(Value &v){

    std::size_t cur_tail = tail.load( std::memory_order_acquire );
    std::size_t cur_head = head.load( std::memory_order_relaxed );

    if (cur_tail == cur_head){
        return false;
    }
    else{
        v = buffer[cur_head].val;
        head.store(next(cur_head), std::memory_order_release);
        return true;
    }
}



#endif // SPSC_RING_BUFFER

