# `evolv` – Markov chains in C++

`evolv` aims to provide an efficient implementation of Markov chains in C++.

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

To use it with CMake project add it via `ExternalProject`:
```cmake
include(ExternalProject)

ExternalProject_Add(
  evolv
  GIT_REPOSITORY https://github.com/makcymal/evolv.git
  GIT_TAG master
  SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/lib/evolv
  CONFIGURE_COMMAND ""
  BUILD_COMMAND ""
  INSTALL_COMMAND ""
  LOG_DOWNLOAD ON
)
```

And use in your code:
```c++
#include "lib/evolv/src/evolv.h"
```


## Generate docs

```shell
doxygen
```


