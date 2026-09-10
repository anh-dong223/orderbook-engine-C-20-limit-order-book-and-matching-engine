#pragma once

#include "engine/order_book.hpp"

#include <string>
#include <unordered_map>

namespace lob {

class MatchingEngine {
public:
    std::vector<Trade> submit(const std::string& symbol, const OrderRequest& order);
    bool cancel(const std::string& symbol, OrderId id);
    std::vector<Trade> modify(const std::string& symbol, OrderId id,
                              Quantity quantity, Price price);
    [[nodiscard]] const OrderBook* find_book(const std::string& symbol) const;

private:
    std::unordered_map<std::string, OrderBook> books_;
};

} // namespace lob
