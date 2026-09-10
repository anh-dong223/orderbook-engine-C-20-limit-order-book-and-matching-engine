#include "engine/matching_engine.hpp"

namespace lob {

std::vector<Trade> MatchingEngine::submit(const std::string& symbol,
                                          const OrderRequest& order) {
    return books_[symbol].submit(order);
}

bool MatchingEngine::cancel(const std::string& symbol, OrderId id) {
    auto book = books_.find(symbol);
    return book != books_.end() && book->second.cancel(id);
}

std::vector<Trade> MatchingEngine::modify(const std::string& symbol, OrderId id,
                                          Quantity quantity, Price price) {
    return books_.at(symbol).modify(id, quantity, price);
}

const OrderBook* MatchingEngine::find_book(const std::string& symbol) const {
    auto found = books_.find(symbol);
    return found == books_.end() ? nullptr : &found->second;
}

} // namespace lob
