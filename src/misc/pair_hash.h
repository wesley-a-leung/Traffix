#ifndef PAIR_HASH_H
#define PAIR_HASH_H

#include <cstddef> // for size_t
#include <utility> // for pair
#include <functional> // for hash

template<typename T1, typename T2> struct pair_hash {
    size_t operator ()(const std::pair<T1, T2> &p) const {
        return 31 * std::hash<T1> {}(p.first) + std::hash<T2> {}(p.second);
    }
};

#endif
