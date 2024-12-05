#include "fenwick_tree.h"


template <class DataT, class SizeT>
  requires std::integral<DataT> && std::signed_integral<SizeT>
FenwickTree<DataT, SizeT>::FenwickTree(SizeT size)
: tree_(static_cast<std::size_t>(size) + 1, 0) {
}

template <class DataT, class SizeT>
  requires std::integral<DataT> && std::signed_integral<SizeT>
SizeT FenwickTree<DataT, SizeT>::size() const {
  return static_cast<SizeT>(tree_.size()) - 1;
}

template <class DataT, class SizeT>
  requires std::integral<DataT> && std::signed_integral<SizeT>
void FenwickTree<DataT, SizeT>::resize(SizeT new_size) {
  tree_.resize(static_cast<std::size_t>(new_size) + 1, 0);
}

template <class DataT, class SizeT>
  requires std::integral<DataT> && std::signed_integral<SizeT>
DataT FenwickTree<DataT, SizeT>::sum(SizeT rb) const {
  rb++;
  DataT res = 0;
  for (; rb > 0; rb -= (rb & -rb)) {
    res += tree_[rb];
  }
  return res;
}

template <class DataT, class SizeT>
  requires std::integral<DataT> && std::signed_integral<SizeT>
DataT FenwickTree<DataT, SizeT>::sum(SizeT lb, SizeT rb) const {
  rb++;
  lb++;
  return sum(rb) - sum(lb - 1);
}

template <class DataT, class SizeT>
  requires std::integral<DataT> && std::signed_integral<SizeT>
void FenwickTree<DataT, SizeT>::add(SizeT idx, DataT x) {
  idx++;
  for (; idx < tree_.size(); idx += (idx & -idx)) {
    tree_[idx] += x;
  }
}

template <class DataT, class SizeT>
  requires std::integral<DataT> && std::signed_integral<SizeT>
void FenwickTree<DataT, SizeT>::append(DataT x) {
  tree_.push_back(x);

  SizeT rb = tree_.size() - 1;
  SizeT lb = rb - (rb & -rb) + 1;
  if (1 <= lb and lb <= rb - 1) {
    tree_.back() += sum(lb, rb - 1);
  }
}

template <class DataT, class SizeT>
  requires std::integral<DataT> && std::signed_integral<SizeT>
SizeT FenwickTree<DataT, SizeT>::upper_bound(DataT x) const {
  SizeT idx = 0;
  for (SizeT lmb = __bit_floor(tree_.size()); lmb >= 1; lmb >>= 1) {
    if (idx + lmb < tree_.size() and tree_[idx + lmb] <= x) {
      idx += lmb;
      x -= tree_[idx];
    }
  }
  return idx;
}
