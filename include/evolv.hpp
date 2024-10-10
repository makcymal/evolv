#pragma once

#include <vector>
#include <boost/bimap.hpp>


enum class TransitDensity { Dense, Sparse };


template <typename T>
class ForgorChain {
 public:
  ForgorChain();

  ForgorChain(TransitDensity dens);

  ~ForgorChain();

  void fit(const std::vector<T> &seq, bool incl_start = 1, bool incl_end = 1);

  void fit(const std::vector<std::vector<T>> &vseq, bool incl_start = 1,
           bool incl_end = 1);

  void partial_fit(const std::vector<T> &seq, bool incl_start = 1,
                   bool incl_end = 1);

  void partial_fit(const std::vector<std::vector<T>> &vseq, bool incl_start = 1,
                   bool incl_end = 1);

  T predict(const T &);

 private:
  struct Impl;
  Impl *impl;
};


template <typename T>
class RemberChain {
 public:
  RemberChain();

  RemberChain(TransitDensity dens);

  ~RemberChain();

  void fit(const std::vector<T> &seq, bool incl_start = 1, bool incl_end = 1);

  void fit(const std::vector<std::vector<T>> &vseq, bool incl_start = 1,
           bool incl_end = 1);  

  void partial_fit(const std::vector<T> &seq, bool incl_start = 1,
                   bool incl_end = 1);

  void partial_fit(const std::vector<std::vector<T>> &vseq, bool incl_start = 1,
                   bool incl_end = 1);

  T predict(const T &);

 private:
  struct Impl;
  Impl *impl;
};
