#pragma once

#include <iterator>
#include <string_view>
#include <unordered_map>
#include <unordered_set>

template <typename It>
class Range {
public:
    using ValueType = typename std::iterator_traits<It>::value_type;

    Range(It begin, It end) : begin_(begin), end_(end) {}
    It begin() const { return begin_; }
    It end() const { return end_; }

private:
    It begin_;
    It end_;
};

template <typename C>
auto AsRange(const C& container) {
    return Range{ std::begin(container), std::end(container) };
}

template <typename It>
size_t ComputeUniqueItemsCount(Range<It> range) {
    return std::unordered_set<typename Range<It>::ValueType>{
        range.begin(), range.end()
    }.size();
}

template <typename K, typename V>
const V* GetValuePointer(const std::unordered_map<K, V>& map, const K& key) {
    if (auto it = map.find(key); it != end(map)) {
        return &it->second;
    }
    else {
        return nullptr;
    }
}

template <typename Number>
Number ReadNumberOnLine(std::istream& stream) {
    Number number;
    stream >> number;
    std::string dummy;
    std::getline(stream, dummy);
    return number;
}