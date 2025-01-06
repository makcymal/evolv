# `evolv` – Markov chains in C++ and Python

`evolv` aims to provide an efficient implementation of Markov chains in C++ and Python.

<p align="center" width="100%">
    <img src="fig/i-forgor-i-rember.png" width="50%">
    <br>
    <b><code>evolv</code></b> also has two sides
</p>

In `evolv`, a `MarkovChain` can be either _stateless_, forgetting about previous states like a standard Markov chain, or stateful, _remembering_ a fixed number of previous states.


### Usage

`MarkovChain` is the class that provides all the necessary functionality. Create an instance of it while deciding how many previous states the chain should remember. Then, use the `FeedSequence` method and pass the sequence of homogeneous elements that the chain should learn from. You can call this method as many times as needed, provided that the subsequent sequences contain homogeneous elements of the same type.

The chain initially starts in the state corresponding to the last elements of the lastly fed sequence. Use the `PredictState` method to predict the next state.


### Implementation notes

The core of the library is written in C++ and is located in the `src` directory. It is designed to be compiled into a shared library.

A Python module is implemented using the Python C API and is located in the `pymodule` directory. This module makes use of the previously compiled shared library and is also compiled into a shared library itself.


### Build

CMake output generator files is located in `.cmake` directory:
```shell
cmake -B .cmake
cd .cmake
```
Run your build system. Outputs are located in the `.build` directory


### Generate docs

```shell
doxygen
```


