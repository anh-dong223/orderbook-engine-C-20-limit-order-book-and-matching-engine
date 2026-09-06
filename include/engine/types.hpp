#pragma once

#include <cstdint>
#include <string>

namespace lob {

using OrderId = std::uint64_t;
using Quantity = std::uint64_t;
using Price = std::int64_t; // integer ticks

enum class Side { Buy, Sell };
enum class OrderType { Limit, Market };

struct OrderRequest {
    OrderId id;
    Side side;
    OrderType type;
    Quantity quantity;
    Price price{0};
};

struct Trade {
    std::uint64_t sequence;
    OrderId maker_id;
    OrderId taker_id;
    Quantity quantity;
    Price price;
};

inline std::string side_name(Side side) {
    return side == Side::Buy ? "BUY" : "SELL";
}

} // namespace lob
