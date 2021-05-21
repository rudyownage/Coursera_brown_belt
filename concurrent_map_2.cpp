#include <functional>
#include <string>
#include <optional>
#include <mutex>
#include <numeric>
#include <deque>
#include <future>
#include <random>
#include "test_runner.h"
#include "profile.h"

using namespace std;

template <typename K, typename V, typename Hash = std::hash<K>>
class ConcurrentMap {
public:
    using MapType = unordered_map<K, V, Hash>;
private:
    struct Bucket {
        MapType data;
        mutable mutex m;

    };
    Hash hasher;
    vector<Bucket> buckets;

    size_t GetIndex(const K& key) const {
        return hasher(key) % buckets.size();
    }

public:
    struct WriteAccess : lock_guard<mutex> {
        V& ref_to_value;

        WriteAccess(const K& key, Bucket& bucket)
            : lock_guard(bucket.m)
            , ref_to_value(bucket.data[key])
        {
        }
    };

    struct ReadAccess : lock_guard<mutex> {
        const V& ref_to_value;

        ReadAccess(const K& key, const Bucket& bucket)
            : lock_guard(bucket.m)
            , ref_to_value(bucket.data.at(key))
        {
        }
    };
	
	// Possible implementation
	/*struct WriteAccess {
        lock_guard<mutex> guard;
        V& ref_to_value;

        WriteAccess(const K& key, Bucket& bucket)
            : guard(bucket.m)
            , ref_to_value(bucket.data[key])
        {
        }
    };

    struct ReadAccess {
        lock_guard<mutex> guard;
        const V& ref_to_value;

        ReadAccess(const K& key, const Bucket& bucket)
            : guard(bucket.m)
            , ref_to_value(bucket.data.at(key))
        {
        }
    };*/

    explicit ConcurrentMap(size_t bucket_count) : buckets(bucket_count){

    }

    WriteAccess operator[](const K& key) {
        return {key, buckets[GetIndex(key)]};
    }
    ReadAccess At(const K& key) const {
        return { key, buckets[GetIndex(key)] };
    }

    bool Has(const K& key) const {
        auto& bucket = buckets[GetIndex(key)];
        lock_guard g(bucket.m);
        return bucket.data.count(key) > 0;
    }

    MapType BuildOrdinaryMap() const {
        MapType result;
        for (auto& [data, mtx] : buckets) {
            lock_guard g(mtx);
            result.insert(data.begin(), data.end());
        }
        return result;
    }
};