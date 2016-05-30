#ifndef STRIPED_HASH_SET
#define STRIPED_HASH_SET

#include <iostream>

#include <vector>
#include <forward_list>
#include <array>
#include <mutex>
#include <shared_mutex>
#include <algorithm>
#include <atomic>

template <class Value, class Hash = std::hash<Value>>
class striped_hash_set {
public:
    explicit striped_hash_set(std::size_t n_stripes, std::size_t _load_factor = 2, std::size_t _growth_factor = 2) :
                load_factor(_load_factor),
                growth_factor(_growth_factor),
                num_of_elements(0),
                stripes(n_stripes),
                buckets(_growth_factor * n_stripes) {}
    void add(const Value& v);
    void remove(const Value& v);
    bool contains(const Value& v) const;


private:
    void rehash(std::size_t num_of_buckets);

    std::size_t get_bucket_index(std::size_t hash_value) const;
    std::size_t get_stripe_index(std::size_t hash_value) const;


    const std::size_t load_factor;
    const std::size_t growth_factor;
    std::atomic<std::size_t> num_of_elements;

    mutable std::vector<std::shared_timed_mutex> stripes;
    std::vector<std::forward_list<Value>> buckets;


};

template<class Value, class Hash>
std::size_t striped_hash_set<Value, Hash>::get_bucket_index(std::size_t hash_value) const{
    return hash_value % buckets.size();
}

template<class Value, class Hash>
std::size_t striped_hash_set<Value, Hash>::get_stripe_index(std::size_t hash_value) const{
    return hash_value % stripes.size();
}

template<class Value, class Hash>
void striped_hash_set<Value, Hash>::add(const Value &v){
    std::size_t hash = Hash()(v);
    std::size_t stripe_index = get_stripe_index(hash);
    std::unique_lock<std::shared_timed_mutex> lck(stripes[stripe_index]);

    std::size_t bucket_index = get_bucket_index(hash);
    std::forward_list<Value>& bucket = buckets[bucket_index];
    auto value_it = std::find(bucket.cbegin(), bucket.cend(), v);
    if ( value_it == bucket.cend()){
        bucket.push_front(v);
        num_of_elements.fetch_add(1);
    }

    if (num_of_elements.load() / buckets.size() >= load_factor){
        std::size_t num_of_buckets = buckets.size();
        lck.unlock();
        rehash(num_of_buckets);
    }
}

template<class Value, class Hash>
void striped_hash_set<Value, Hash>::remove(const Value &v){
    std::size_t hash = Hash()(v);
    std::size_t stripe_index = get_stripe_index(hash);
    std::unique_lock<std::shared_timed_mutex> lck(stripes[stripe_index]);

    std::size_t bucket_index = get_bucket_index(hash);
    std::forward_list<Value>& bucket = buckets[bucket_index];

    auto value_it = std::find(bucket.cbegin(), bucket.cend(), v);
    if ( value_it != bucket.cend()){
        bucket.remove(v);
        num_of_elements.fetch_add(-1);
    }


}

template<class Value, class Hash>
bool striped_hash_set<Value, Hash>::contains(const Value &v) const{
    std::size_t hash = Hash()(v);
    std::size_t stripe_index = get_stripe_index(hash);

    std::shared_lock<std::shared_timed_mutex> lck(stripes[stripe_index]);

    std::size_t bucket_index = get_bucket_index(hash);
    const std::forward_list<Value>& bucket = buckets[bucket_index];
    return (std::find(bucket.cbegin(), bucket.cend(), v) != bucket.cend());
}


template<class Value, class Hash>
void striped_hash_set<Value, Hash>::rehash(std::size_t num_of_buckets){
    std::vector<std::unique_lock<std::shared_timed_mutex>> locks;

    locks.emplace_back(std::unique_lock<std::shared_timed_mutex>(stripes[0]));
    if (num_of_buckets == buckets.size()){

        for (auto it = ++stripes.begin(); it != stripes.end(); it++){
            locks.emplace_back(std::unique_lock<std::shared_timed_mutex>(*it));
        }

        std::vector<std::forward_list<Value>> new_buckets(growth_factor * buckets.size());
        for(auto it_v = buckets.begin(); it_v != buckets.end(); it_v++){
            for (auto it_l = it_v->begin(); it_l != it_v->end(); it_l++){
                new_buckets[Hash()(*it_l) % new_buckets.size()].push_front(*it_l);
            }
        }

        buckets.swap(new_buckets);
    }
}

#endif // STRIPED_HASH_SET

