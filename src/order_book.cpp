#include "engine/order_book.hpp"

#include <algorithm>
#include <stdexcept>

namespace lob {

template <typename Levels, typename Crosses>
void OrderBook::match(OrderRequest& incoming, Levels& levels, Crosses crosses,
                      std::vector<Trade>& fills) {
    while (incoming.quantity > 0 && !levels.empty()) {
        auto level = levels.begin();
        if (!crosses(level->first)) {
            break;
        }

        auto& queue = level->second;
        while (incoming.quantity > 0 && !queue.empty()) {
            auto maker = queue.begin();
            const auto quantity = std::min(incoming.quantity, maker->remaining);
            Trade trade{next_trade_sequence_++, maker->id, incoming.id,
                        quantity, maker->price};
            fills.push_back(trade);
            trades_.push_back(trade);

            incoming.quantity -= quantity;
            maker->remaining -= quantity;
            if (maker->remaining == 0) {
                orders_.erase(maker->id);
                queue.erase(maker);
            }
        }
        if (queue.empty()) {
            levels.erase(level);
        }
    }
}

std::vector<Trade> OrderBook::submit(const OrderRequest& request) {
    if (request.id == 0 || request.quantity == 0) {
        throw std::invalid_argument("order id and quantity must be non-zero");
    }
    if (request.type == OrderType::Limit && request.price <= 0) {
        throw std::invalid_argument("limit price must be positive");
    }
    if (orders_.contains(request.id)) {
        throw std::invalid_argument("duplicate order id");
    }

    auto incoming = request;
    std::vector<Trade> fills;
    if (incoming.side == Side::Buy) {
        match(incoming, asks_, [&](Price ask) {
            return incoming.type == OrderType::Market || incoming.price >= ask;
        }, fills);
    } else {
        match(incoming, bids_, [&](Price bid) {
            return incoming.type == OrderType::Market || incoming.price <= bid;
        }, fills);
    }

    if (incoming.quantity > 0 && incoming.type == OrderType::Limit) {
        rest(incoming);
    }
    return fills;
}

void OrderBook::rest(const OrderRequest& request) {
    if (request.side == Side::Buy) {
        auto& queue = bids_[request.price];
        queue.push_back({request.id, request.side, request.quantity, request.price});
        orders_.emplace(request.id,
                        Locator{request.side, request.price, std::prev(queue.end())});
    } else {
        auto& queue = asks_[request.price];
        queue.push_back({request.id, request.side, request.quantity, request.price});
        orders_.emplace(request.id,
                        Locator{request.side, request.price, std::prev(queue.end())});
    }
}

bool OrderBook::cancel(OrderId id) {
    const auto found = orders_.find(id);
    if (found == orders_.end()) {
        return false;
    }
    const auto locator = found->second;
    if (locator.side == Side::Buy) {
        auto level = bids_.find(locator.price);
        level->second.erase(locator.order);
        if (level->second.empty()) bids_.erase(level);
    } else {
        auto level = asks_.find(locator.price);
        level->second.erase(locator.order);
        if (level->second.empty()) asks_.erase(level);
    }
    orders_.erase(found);
    return true;
}

std::vector<Trade> OrderBook::modify(OrderId id, Quantity quantity, Price price) {
    const auto found = orders_.find(id);
    if (found == orders_.end()) {
        throw std::invalid_argument("unknown order id");
    }
    if (quantity == 0) {
        cancel(id);
        return {};
    }
    const auto side = found->second.side;
    cancel(id);
    return submit({id, side, OrderType::Limit, quantity, price});
}

std::optional<Price> OrderBook::best_bid() const {
    if (bids_.empty()) return std::nullopt;
    return bids_.begin()->first;
}

std::optional<Price> OrderBook::best_ask() const {
    if (asks_.empty()) return std::nullopt;
    return asks_.begin()->first;
}

Quantity OrderBook::open_quantity(OrderId id) const {
    const auto found = orders_.find(id);
    return found == orders_.end() ? 0 : found->second.order->remaining;
}

} // namespace lob
