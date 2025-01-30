# `evolv` – Markov chains in C++ and Python

`evolv` aims to provide an efficient implementation of Markov chains in C++ and Python.

<p align="center" width="100%">
    <img src="fig/i-forgor-i-rember.png" width="50%">
    <br>
    <b><code>evolv</code></b> also has two sides
</p>

By definition, Markov chain is memoryless, which means that chains memory is limited to only one current state. `evolv` proposes extension - Markov chain tracking some number
of previously visited states - a Markov chain with *memory*. So, the first case is implemented in `ForgorChain` class, _forgetting_ about previous states, and the second one - in `RemberChain` class, _remembering_ a fixed number of previous states.


## Example

Create Markov chain with the number of previous states to remember, feed it the sequence, optionally set the current state (from where to begin) and try to predict the following state! Note: the predictions are probabilistic, not deterministic.

```c++
#include <string>
#include <vector>
#include "src/evolv.h"

int main() {
  std::vector<std::string> sentenses{
      "The",     "morning", "follows", "night",   ".",
      "The",     "day",     "follows", "morning", ".",
      "The",     "evening", "follows", "day",     ".",
      "The",     "night",   "follows", "evening", "."
  };
  evolv::MarkovChain<std::string> chain(/*memorize_previous=*/2,
                                        /*random_state=*/32);
  chain.FeedSequence(sentenses.begin(), sentenses.end());
  std::vector<std::string> memory{"The", "evening", "follows"};
  chain.UpdateMemory(memory.begin(), memory.end());
  std::string next = chain.PredictState(/*update_memory=*/true);
  assert(next == "day");
}
```


## Usage

`MarkovChain` is the class that provides all the necessary functionality. Create an instance of it and decide how many previous states the chain should remember. Then, use the `FeedSequence` method and pass the sequence of homogeneous elements that the chain should learn from. You can call this method as many times as needed, provided that the subsequent sequences contain homogeneous elements of the same type.

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


