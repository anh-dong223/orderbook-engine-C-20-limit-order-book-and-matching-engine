#pragma once

#include "engine/types.hpp"

#include <list>
#include <map>
#include <optional>
#include <unordered_map>
#include <vector>

namespace lob {

class OrderBook {
public:
    std::vector<Trade> submit(const OrderRequest& request);
    bool cancel(OrderId id);
    std::vector<Trade> modify(OrderId id, Quantity quantity, Price price);

    [[nodiscard]] std::optional<Price> best_bid() const;
    [[nodiscard]] std::optional<Price> best_ask() const;
    [[nodiscard]] Quantity open_quantity(OrderId id) const;
    [[nodiscard]] std::size_t order_count() const { return orders_.size(); }
    [[nodiscard]] const std::vector<Trade>& trades() const { return trades_; }
    void clear_trades() { trades_.clear(); }

private:
    struct RestingOrder {
        OrderId id;
        Side side;
        Quantity remaining;
        Price price;
    };

    using Queue = std::list<RestingOrder>;
    using Bids = std::map<Price, Queue, std::greater<Price>>;
    using Asks = std::map<Price, Queue>;

    struct Locator {
        Side side;
        Price price;
        Queue::iterator order;
    };

    template <typename Levels, typename Crosses>
    void match(OrderRequest& incoming, Levels& levels, Crosses crosses,
               std::vector<Trade>& fills);
    void rest(const OrderRequest& request);

    Bids bids_;
    Asks asks_;
    std::unordered_map<OrderId, Locator> orders_;
    std::vector<Trade> trades_;
    std::uint64_t next_trade_sequence_{1};
};

} // namespace lob
