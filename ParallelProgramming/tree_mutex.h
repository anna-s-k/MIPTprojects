#ifndef TREE_MUTEX
#define TREE_MUTEX

#include <stdlib.h>
#include <array>
#include <atomic>
#include <vector>
#include <thread>
#include "peterson_mutex.h"

class tree_mutex{
public:
    tree_mutex(std::size_t _num_threads);

    void lock(std::size_t thread_index);
    void unlock(std::size_t thread_index);

private:

    std::size_t num_threads;
    std::vector<peterson_mutex> tree;
    std::size_t tree_size;

    std::size_t parent_index(std::size_t tree_index);
    std::size_t get_child(std::size_t cur_tree_index, std::size_t thread_index);


};

unsigned int upper_log2(unsigned int n){
    std::size_t y = 1;
    std::size_t ans = 0;
    do{
        y *= 2;
        ans++;
    }while (y < n);
    return ans;
}

inline tree_mutex::tree_mutex(std::size_t _num_threads){
    num_threads = _num_threads;
    std::size_t t = upper_log2(num_threads);
    tree_size = (1 << t ) - 1;
    tree.resize(tree_size);
}

inline void tree_mutex::lock(std::size_t thread_index){
    std::size_t cur_tree_index = thread_index;
    std::size_t next_mutex = tree_size - 1 - thread_index/2; //first mutex
    tree[next_mutex].lock((cur_tree_index + 1) % 2);
    cur_tree_index = next_mutex;

    while (cur_tree_index != 0){// while not in root
        next_mutex = parent_index(cur_tree_index);
        tree[next_mutex].lock((cur_tree_index + 1) % 2);         // peterson_mutex
        cur_tree_index = next_mutex;           // go up to the root
    }
}

inline void tree_mutex::unlock(std::size_t thread_index){
    std::size_t cur_tree_index = 0;
    std::size_t first_mutex = tree_size - 1 - thread_index/2;
    // from the root
    if (cur_tree_index == first_mutex){
        tree[cur_tree_index].unlock((thread_index + 1) % 2);
    }    
    else{
    do{

        std::size_t next_mutex = get_child(cur_tree_index, thread_index);

        tree[cur_tree_index].unlock((next_mutex + 1) % 2);      // odd - 0, even - 1
        cur_tree_index = next_mutex;
    } while(cur_tree_index != first_mutex);
    tree[cur_tree_index].unlock((thread_index + 1) % 2);

    }
}

inline std::size_t tree_mutex::parent_index(std::size_t tree_index){
    return (tree_index - 1)/2;
}

inline std::size_t tree_mutex::get_child (std::size_t cur_tree_index, std::size_t thread_index){
    std::size_t thread_first_mutex = tree_size - 1 - thread_index/2; // should come here
    std::size_t tree_height = upper_log2(tree_size);
    std::size_t cur_height = upper_log2(cur_tree_index + 2);
    std::size_t height = tree_height - cur_height; //should go throug that distance down in tree to the last mutex
    if ((1 << height) * cur_tree_index + (3 * ((1 << height) - 1)) / 2 >= thread_first_mutex)
        return (2 * cur_tree_index + 1);
    else
        return (2 * cur_tree_index + 2);
}



#endif // TREE_MUTEX

