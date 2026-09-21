# orderbook-engine

A small C++20 matching engine for a central limit order book. It keeps FIFO queues at each price level, matches at the resting order's price, and supports separate books per symbol.

## Features

- limit and market orders
- price-time priority
- partial fills
- cancel and cancel-replace style modify
- execution log with monotonic trade sequence numbers
- interactive command-line client
- deterministic throughput benchmark
- tests for matching, FIFO, cancellation, modification, and multiple symbols

Prices are stored as integer ticks. The sample client reads decimal prices and uses cents as the tick size.

## Build

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j
ctest --test-dir build --output-on-failure
```

## Try it

```sh
./build/orderbook
```

Example session:

```text
commands: limit, market, cancel, modify, quote, quit
> limit AMD 1 sell 100 155.25
> limit AMD 2 sell 40 155.10
> limit AMD 3 buy 60 155.25
trade 1 maker=2 taker=3 qty=40 px=155.10
trade 2 maker=1 taker=3 qty=20 px=155.25
> quote AMD
bid=- ask=15525 orders=1
> modify AMD 1 50 155.30
> cancel AMD 1
cancelled
```

Order syntax:

```text
limit <symbol> <id> <buy|sell> <quantity> <price>
market <symbol> <id> <buy|sell> <quantity>
cancel <symbol> <id>
modify <symbol> <id> <new quantity> <new price>
quote <symbol>
```

## Benchmark

```sh
./build/engine_bench 1000000
```

The benchmark uses a fixed random seed and prints total elapsed time and orders per second. It is meant for quick comparisons between builds, not as an exchange-grade latency test.
