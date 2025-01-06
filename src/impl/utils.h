#pragma once

#include <concepts>
#include <iterator>


//! Namespace for utilities that cannot be placed in evolv::internal
namespace evolv::utils {

//! Concept for checking if IterT is iterator on elements
//! of type DataT, DataT & or const DataT &
template <class IterT, class DataT>
concept is_iterator =
    std::is_same_v<typename std::iterator_traits<IterT>::value_type, DataT> ||
    std::is_same_v<typename std::iterator_traits<IterT>::value_type, DataT &> ||
    std::is_same_v<typename std::iterator_traits<IterT>::value_type,
                   const DataT &>;

}  // namespace evolv::utils
