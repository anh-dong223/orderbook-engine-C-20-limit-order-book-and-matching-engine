# orderbook-engine

A small C++20 matching engine for a central limit order book. It keeps FIFO queues at each price level, matches at the resting order's price, and supports separate books per symbol.

## Build

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j
ctest --test-dir build --output-on-failure
```
