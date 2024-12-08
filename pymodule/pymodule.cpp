#define PY_SSIZE_T_CLEAN
#include <python3.12/Python.h>


#include "../include/evolv.hpp"

#include <string>
#include <unordered_map>

// clang-format off
struct MarkovChainObject {
  PyObject_HEAD
  std::unordered_map<std::string, int> state_encoder;
  std::vector<std::string> state_decoder;
  
};
// clang-format on

// clang-format off
static PyTypeObject MarkovChainType = {
  .ob_base = PyVarObject_HEAD_INIT(nullptr, 0)
  .tp_name = "evolv.MarkovChain",
  .tp_basicsize = sizeof(MarkovChainObject),
  .tp_itemsize = 0,
  .tp_flags = Py_TPFLAGS_DEFAULT,
  .tp_doc = PyDoc_STR("Markov chain with or without memory"),
  .tp_new = PyType_GenericNew,
};
// clang-format on

static PyModuleDef evolv_m = {
  .m_base = PyModuleDef_HEAD_INIT,
  .m_name = "evolv",
  .m_doc = "Module containing Markov chain implementation",
  .m_size = -1,
};

PyMODINIT_FUNC PyInit_evolv(void) {
  PyObject *m;
  if (PyType_Ready(&MarkovChainType) < 0) {
    return nullptr;
  }

  m = PyModule_Create(&evolv_m);
  if (m == nullptr) {
    return nullptr;
  }

  if (PyModule_AddObjectRef(
        m, "MarkovChain", reinterpret_cast<PyObject *>(&MarkovChainType)) < 0) {
    Py_DECREF(m);
    return nullptr;
  }

  return m;
}
