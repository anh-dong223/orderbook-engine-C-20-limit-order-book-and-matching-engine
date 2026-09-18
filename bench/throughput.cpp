#include "engine/order_book.hpp"

#include <chrono>
#include <cstdint>
#include <iomanip>
#include <iostream>
#include <random>

int main(int argc, char** argv) {
    std::uint64_t count = 1'000'000;
    if (argc > 1) count = std::stoull(argv[1]);

    lob::OrderBook book;
    std::mt19937_64 random(42);
    std::uniform_int_distribution<int> spread(-50, 50);
    std::uniform_int_distribution<int> quantity(1, 100);

    const auto start = std::chrono::steady_clock::now();
    for (std::uint64_t i = 1; i <= count; ++i) {
        const auto side = i % 2 == 0 ? lob::Side::Buy : lob::Side::Sell;
        const auto price = 10'000 + spread(random) + (side == lob::Side::Buy ? -10 : 10);
        book.submit({i, side, lob::OrderType::Limit,
                     static_cast<lob::Quantity>(quantity(random)), price});
        if (i % 5 == 0 && book.open_quantity(i - 2) > 0) book.cancel(i - 2);
    }
    const auto elapsed = std::chrono::duration<double>(
        std::chrono::steady_clock::now() - start).count();
    std::cout << count << " orders in " << std::fixed << std::setprecision(3)
              << elapsed << "s (" << static_cast<std::uint64_t>(count / elapsed)
              << " orders/s)\n";
}
