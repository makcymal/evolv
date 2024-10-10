class MatrixTransit {
 public:
  MatrixTransit(int states_num);

  ~MatrixTransit();

  void clear();

  void add_transition(int state, int count = 1);

  int gen_transition() const;

 private:
  struct Impl;
  Impl *impl;
};
