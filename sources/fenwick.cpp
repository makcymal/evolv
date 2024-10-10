#include "fenwick.hpp"


template <typename T>
struct Fenwick<T>::Impl {
  std::vector<T> tree;
};

template <typename T>
Fenwick<T>::Fenwick(std::size_t n) : impl(new Impl) {
  impl->tree.reserve(n + 1);
  impl->tree.push_back(0);
}

template <typename T>
Fenwick<T>::Fenwick(const std::vector<T> &arr) {
  impl->tree.push_back(0);
  if (arr.empty()) {
    return;
  }

  std::vector<T> pref{arr.front()};
  impl->tree.push_back(arr.front());

  for (int i = 2; i <= arr.size(); ++i) {
    pref.push_back(pref.back() + arr[i - 1]);
    impl->tree.push_back(pref.back());
    if (i != (1 << std::__lg(i)))
      impl->tree.back() -= pref[i - (i & -i) - 1];
  }
}

template <typename T>
Fenwick<T>::~Fenwick() {
  delete impl;
}

template <typename T>
std::size_t Fenwick<T>::size() const {
  return impl->tree.size() - 1;
}

template <typename T>
void Fenwick<T>::resize(std::size_t n) {
  impl->tree.resize(n + 1, 0);
}

template <typename T>
void Fenwick<T>::reset() {
  fill(impl->tree.begin(), impl->tree.end(), 0);
}

template <typename T>
void Fenwick<T>::clear() {
  impl->tree.resize(1);
}

template <typename T>
T Fenwick<T>::sum(int rb) const {
  rb++;
  int res = 0;
  for (; rb > 0; rb -= (rb & -rb))
    res += impl->tree[rb];
  return res;
}

template <typename T>
T Fenwick<T>::sum(int lb, int rb) const {
  rb++;
  lb++;
  return sum(rb) - sum(lb - 1);
}

template <typename T>
T Fenwick<T>::get(int idx) const {
  idx++;
  return sum(idx) - sum(idx - 1);
}

template <typename T>
void Fenwick<T>::add(int idx, T x) {
  idx++;
  for (; idx < impl->tree.size(); idx += (idx & -idx))
    impl->tree[idx] += x;
}

template <typename T>
void Fenwick<T>::set(int idx, T x) {
  idx++;
  T diff = x - get(idx);
  add(idx, diff);
}

template <typename T>
void Fenwick<T>::append(T x) {
  impl->tree.push_back(x);

  int rb = impl->tree.size() - 1;
  int lb = rb - (rb & -rb) + 1;
  if (1 <= lb and lb <= rb - 1)
    impl->tree.back() += sum(lb, rb - 1);
}

template <typename T>
int Fenwick<T>::upper_bound(int x) const {
  int idx = 0;
  for (int lmb = std::__lg(impl->tree.size()); lmb >= 0; --lmb) {
    if (idx + (1 << lmb) < impl->tree.size() and
        impl->tree[idx + (1 << lmb)] <= x) {
      idx += (1 << lmb);
      x -= impl->tree[idx];
    }
  }
  return idx;
}
