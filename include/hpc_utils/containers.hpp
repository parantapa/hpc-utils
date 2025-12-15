#pragma once

#include <vector>
#include <parallel_hashmap/phmap.h>

#include <hpc_utils/allocator.hpp>
#include <hpc_utils/static_array.hpp>

namespace hpc_utils {
template <typename Type>
using Vector = std::vector<Type, aligned_allocator<Type>>;

template <typename Key, typename Value>
using HashMap =
    phmap::parallel_flat_hash_map<Key, Value, phmap::priv::hash_default_hash<Key>, phmap::priv::hash_default_eq<Key>,
                                  aligned_allocator<phmap::priv::Pair<const Key, Value>>, 6, phmap::NullMutex>;

template <typename Key>
using HashSet =
    phmap::parallel_flat_hash_set<Key, phmap::priv::hash_default_hash<Key>, phmap::priv::hash_default_eq<Key>,
                                  aligned_allocator<Key>, 6, phmap::NullMutex>;

template <typename Key, typename Value>
using SmallHashMap =
    phmap::flat_hash_map<Key, Value, phmap::priv::hash_default_hash<Key>, phmap::priv::hash_default_eq<Key>,
                         aligned_allocator<phmap::priv::Pair<const Key, Value>>>;

template <typename Key>
using SmallHashSet = phmap::flat_hash_set<Key, phmap::priv::hash_default_hash<Key>, phmap::priv::hash_default_eq<Key>,
                                          aligned_allocator<Key>>;

} // namespace hpc_utils
