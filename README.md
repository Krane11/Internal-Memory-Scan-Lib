C++ library that lets you scan for and change different types of data internally.

Example usage:
```cpp
#include "memscan.h"

int main() {
    Mem mem;
    std::vector<size_t> addrs;
//store float values of 50 into "addrs"
    mem.FindFloat(addrs, 50.0f);

//change addresses in "addrs" to 100
    mem.ReplaceFloat(addrs, 100.0f);
}
```
