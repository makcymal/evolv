#include "fenwick_tree.h"

namespace evolv::internal {

template <class DataT, class SizeT>
  requires std::integral<DataT> && std::signed_integral<SizeT>
FenwickTree<DataT, SizeT>::FenwickTree(SizeT size)
: tree_(static_cast<std::size_t>(size) + 1, 0) {
}

template <class DataT, class SizeT>
  requires std::integral<DataT> && std::signed_integral<SizeT>
SizeT FenwickTree<DataT, SizeT>::Size() const {
  return static_cast<SizeT>(tree_.size()) - 1;
}

template <class DataT, class SizeT>
  requires std::integral<DataT> && std::signed_integral<SizeT>
void FenwickTree<DataT, SizeT>::Resize(SizeT new_size) {
  tree_.resize(static_cast<std::size_t>(new_size) + 1, 0);
}

template <class DataT, class SizeT>
  requires std::integral<DataT> && std::signed_integral<SizeT>
DataT FenwickTree<DataT, SizeT>::Sum(SizeT rb) const {
  rb++;
  DataT res = 0;
  for (; rb > 0; rb -= (rb & -rb)) {
    res += tree_[rb];
  }
  return res;
}

template <class DataT, class SizeT>
  requires std::integral<DataT> && std::signed_integral<SizeT>
DataT FenwickTree<DataT, SizeT>::Sum(SizeT lb, SizeT rb) const {
  rb++;
  lb++;
  return sum(rb) - sum(lb - 1);
}

template <class DataT, class SizeT>
  requires std::integral<DataT> && std::signed_integral<SizeT>
void FenwickTree<DataT, SizeT>::Add(SizeT idx, DataT x) {
  if (idx >= Size()) {
    Resize(idx + 1);
  }
  idx++;
  for (; idx < tree_.size(); idx += (idx & -idx)) {
    tree_[idx] += x;
  }
}

template <class DataT, class SizeT>
  requires std::integral<DataT> && std::signed_integral<SizeT>
SizeT FenwickTree<DataT, SizeT>::UpperBound(DataT x) const {
  SizeT idx = 0;
  for (SizeT lmb = __bit_floor(tree_.size()); lmb >= 1; lmb >>= 1) {
    if (idx + lmb < tree_.size() and tree_[idx + lmb] <= x) {
      idx += lmb;
      x -= tree_[idx];
    }
  }
  return idx;
}

}  // namespace evolv::internal
