#include "engine/matching_engine.hpp"

#include <cstdlib>
#include <iostream>

#define CHECK(expr) do { if (!(expr)) { \
    std::cerr << "check failed: " #expr << " at line " << __LINE__ << '\n'; \
    std::exit(1); }} while (false)

using namespace lob;

void price_time_priority() {
    OrderBook book;
    book.submit({1, Side::Sell, OrderType::Limit, 5, 10100});
    book.submit({2, Side::Sell, OrderType::Limit, 7, 10100});
    book.submit({3, Side::Sell, OrderType::Limit, 3, 10000});

    auto fills = book.submit({9, Side::Buy, OrderType::Limit, 10, 10100});
    CHECK(fills.size() == 3);
    CHECK(fills[0].maker_id == 3 && fills[0].quantity == 3);
    CHECK(fills[1].maker_id == 1 && fills[1].quantity == 5);
    CHECK(fills[2].maker_id == 2 && fills[2].quantity == 2);
    CHECK(book.open_quantity(2) == 5);
}

void market_order_does_not_rest() {
    OrderBook book;
    book.submit({1, Side::Sell, OrderType::Limit, 4, 5000});
    auto fills = book.submit({2, Side::Buy, OrderType::Market, 10});
    CHECK(fills.size() == 1 && fills[0].quantity == 4);
    CHECK(book.order_count() == 0);
}

void cancel_and_modify() {
    OrderBook book;
    book.submit({1, Side::Buy, OrderType::Limit, 8, 9900});
    CHECK(book.cancel(1));
    CHECK(!book.cancel(1));
    book.submit({2, Side::Buy, OrderType::Limit, 5, 9900});
    book.modify(2, 12, 10000);
    CHECK(book.open_quantity(2) == 12);
    CHECK(book.best_bid() == 10000);
}

void matching_engine_separates_symbols() {
    MatchingEngine engine;
    engine.submit("AMD", {1, Side::Buy, OrderType::Limit, 10, 15000});
    engine.submit("NVDA", {2, Side::Sell, OrderType::Limit, 10, 20000});
    CHECK(engine.find_book("AMD")->best_bid() == 15000);
    CHECK(engine.find_book("NVDA")->best_ask() == 20000);
}

void rejects_bad_orders() {
    OrderBook book;
    bool threw = false;
    try {
        book.submit({1, Side::Buy, OrderType::Limit, 0, 10000});
    } catch (const std::invalid_argument&) {
        threw = true;
    }
    CHECK(threw);

    book.submit({2, Side::Buy, OrderType::Limit, 5, 10000});
    threw = false;
    try {
        book.submit({2, Side::Sell, OrderType::Limit, 5, 10100});
    } catch (const std::invalid_argument&) {
        threw = true;
    }
    CHECK(threw);
}

int main() {
    price_time_priority();
    market_order_does_not_rest();
    cancel_and_modify();
    matching_engine_separates_symbols();
    rejects_bad_orders();
    std::cout << "all tests passed\n";
}
