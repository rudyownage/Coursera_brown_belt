#include <memory>
#include "common.h"
#include <unordered_map>
#include <mutex>
#include <list>
using namespace std;

class LruCache : public ICache {
    Settings settings_;
    shared_ptr<IBooksUnpacker> books_unpacker_;
    list<BookPtr> cache;
    unordered_map<string, list<BookPtr>::iterator> storage;
    size_t free_space;
    mutable mutex mutex_;
public:
    LruCache(shared_ptr<IBooksUnpacker> books_unpacker, const Settings& settings) :
    settings_(settings), books_unpacker_(move(books_unpacker)), free_space(settings.max_memory){

    }

    BookPtr GetBook(const string& book_name) override {
        lock_guard guard(mutex_);
        auto book_it = storage.find(book_name);
        // if book presented in cache
        if (book_it != storage.end()) {
            auto book = *book_it->second;
            cache.erase(storage[book_name]);
            cache.push_front(move(book));
            storage[book_name] = cache.begin();
            return *storage[book_name];
        }

        // not presented
        auto new_book = books_unpacker_->UnpackBook(book_name);
        size_t new_book_size = new_book->GetContent().size();
        
        // if book size > cache
        if (new_book_size > settings_.max_memory) {
            return new_book;
        }

        // if not enought memory
        while (free_space < new_book_size) {
            auto deleted_book = cache.back();
            free_space += deleted_book->GetContent().size();
            storage.erase(deleted_book->GetName());
            cache.pop_back();
        }

        free_space -= new_book_size;
        cache.push_front(move(new_book));
        storage[book_name] = cache.begin();

        return *storage[book_name];
    }
};


unique_ptr<ICache> MakeCache(shared_ptr<IBooksUnpacker> books_unpacker, const ICache::Settings& settings) {

    return make_unique<LruCache>(move(books_unpacker), settings);
}