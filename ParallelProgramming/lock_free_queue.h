#ifndef LOCK_FREE_QUEUE
#define LOCK_FREE_QUEUE

#include <atomic>
#include <iostream>

template <class T>
class lock_free_queue{
public:
    ~lock_free_queue();
    void enqueue(T item);
    bool dequeue(T& item);
private:
    struct node_t{
        node_t(node_t* _next = nullptr, T _val = 0) : next(_next), value(_val) {}
        ~node_t() = default;

        std::atomic<node_t*> next;
        T value;
    };

    std::atomic<node_t*> Head {new node_t};
    std::atomic<node_t*> Tail {Head.load()};

    std::atomic<std::size_t> threads_in_pop {0};
    std::atomic<node_t*> to_be_deleted{Head.load()};
    void delete_nodes(node_t* nodes, node_t* cur_head);
    void try_reclaim(node_t* cur_head);

};

template<class T>
lock_free_queue<T>::~lock_free_queue(){
    for (node_t* it = to_be_deleted.load(); it != nullptr;){
        node_t* next = it->next.load();
        delete it;
        it = next;
    }
}

template <class T>
void lock_free_queue<T>::enqueue(T item){
    node_t* new_node = new node_t;
    new_node->value = item;
    node_t* curr_tail;
    node_t* curr_tail_next;
    threads_in_pop.fetch_add(1);
    while(true){

        curr_tail = Tail.load();
        curr_tail_next = curr_tail->next;

        if (curr_tail_next == nullptr){

            if (Tail.load()->next.compare_exchange_weak(curr_tail_next, new_node))
                break;
        } else{
            Tail.compare_exchange_weak(curr_tail, curr_tail_next);
        }

    }
    Tail.compare_exchange_weak(curr_tail, new_node);
    threads_in_pop.fetch_sub(1);

}

template <class T>
bool lock_free_queue<T>::dequeue(T &item){
    node_t* curr_head;
    node_t* curr_tail;
    node_t* curr_head_next;

    while (true){
        threads_in_pop.fetch_add(1);
        curr_head = Head.load();
        curr_tail = Tail.load();
        curr_head_next = curr_head->next;

        if (curr_head == curr_tail) {
            if (curr_head_next == nullptr){
                //try_reclaim(curr_head);
                threads_in_pop.fetch_sub(1);
                return false;
            }
            else
                Tail.compare_exchange_strong(curr_head, curr_head_next);//кто-то добавил, но tail не передвинул
        } else {
            if (Head.compare_exchange_strong(curr_head, curr_head_next)) {
                item = curr_head_next->value;
                try_reclaim(curr_head_next);
                return true;
            }
        }
        try_reclaim(curr_head);
    }
}


template <class T>
void lock_free_queue<T>::try_reclaim(node_t* cur_head){
    if (threads_in_pop.load() == 1){

        node_t* nodes_to_delete = to_be_deleted.load();
        delete_nodes(nodes_to_delete, cur_head);
    }
    threads_in_pop.fetch_sub(1);
}


template <class T>
void lock_free_queue<T>::delete_nodes(node_t *nodes, node_t* cur_head){
    //node_t* cur_head = Head.load();

    while (nodes != cur_head){
        node_t* next = nodes->next;
        delete nodes;
        nodes = next;
    }
    to_be_deleted.store(cur_head);
}

/*template <class T>
void lock_free_queue<T>::try_reclaim(node_t* cur_head){
    if (threads_in_pop.load() == 1){
        node_t* nodes_to_delete = to_be_deleted.load();
        threads_in_pop.fetch_sub(1);
        if (!threads_in_pop.load()){//настал момент, когда все вышли
            delete_nodes(nodes_to_delete, cur_head);
        }

    } else{
        threads_in_pop.fetch_sub(1);
    }
}*/




#endif // LOCK_FREE_QUEUE

