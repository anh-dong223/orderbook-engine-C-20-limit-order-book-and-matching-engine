#include "engine/order_book.hpp"

#include <algorithm>
#include <chrono>
#include <cstdint>
#include <iostream>
#include <random>
#include <string>
#include <vector>

// Measures per-order submit latency on the same workload as throughput.cpp
// and reports percentiles in nanoseconds.
int main(int argc, char** argv) {
    std::uint64_t count = 1'000'000;
    if (argc > 1) count = std::stoull(argv[1]);

    lob::OrderBook book;
    std::mt19937_64 random(42);
    std::uniform_int_distribution<int> spread(-50, 50);
    std::uniform_int_distribution<int> quantity(1, 100);

    std::vector<std::uint64_t> samples;
    samples.reserve(count);

    for (std::uint64_t i = 1; i <= count; ++i) {
        const auto side = i % 2 == 0 ? lob::Side::Buy : lob::Side::Sell;
        const auto price = 10'000 + spread(random) + (side == lob::Side::Buy ? -10 : 10);
        const lob::OrderRequest order{i, side, lob::OrderType::Limit,
                                      static_cast<lob::Quantity>(quantity(random)), price};

        const auto start = std::chrono::steady_clock::now();
        book.submit(order);
        const auto stop = std::chrono::steady_clock::now();
        samples.push_back(static_cast<std::uint64_t>(
            std::chrono::duration_cast<std::chrono::nanoseconds>(stop - start).count()));

        if (i % 5 == 0 && book.open_quantity(i - 2) > 0) book.cancel(i - 2);
    }

    std::sort(samples.begin(), samples.end());
    const auto percentile = [&](double p) {
        return samples[static_cast<std::size_t>(p * static_cast<double>(samples.size() - 1))];
    };
    std::cout << count << " orders: p50=" << percentile(0.50) << "ns p99="
              << percentile(0.99) << "ns p99.9=" << percentile(0.999) << "ns\n";
}
