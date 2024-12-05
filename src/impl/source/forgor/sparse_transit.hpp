class SparseTransit {
 public:
  SparseTransit(int approx_state_num = 0);

  ~SparseTransit();

  void clear();

  void add_transition(int state, int count = 1);

  int gen_transition() const;

 private:
  struct Impl;
  Impl *impl;
};
