#include "engine/matching_engine.hpp"

#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>

namespace {

lob::Price parse_price(const std::string& text) {
    const auto dot = text.find('.');
    const auto whole = std::stoll(text.substr(0, dot));
    if (dot == std::string::npos) return whole * 100;
    auto fraction = text.substr(dot + 1);
    fraction.resize(2, '0');
    return whole * 100 + std::stoll(fraction.substr(0, 2));
}

void print_trade(const lob::Trade& trade) {
    std::cout << "trade " << trade.sequence << " maker=" << trade.maker_id
              << " taker=" << trade.taker_id << " qty=" << trade.quantity
              << " px=" << trade.price / 100 << '.' << std::setfill('0')
              << std::setw(2) << trade.price % 100 << std::setfill(' ') << '\n';
}

} // namespace

int main() {
    lob::MatchingEngine engine;
    std::string line;
    std::cout << "commands: limit, market, cancel, modify, quote, quit\n";

    while (std::cout << "> " && std::getline(std::cin, line)) {
        std::istringstream in(line);
        std::string command;
        in >> command;
        try {
            if (command == "limit") {
                std::string symbol, side, price;
                lob::OrderId id;
                lob::Quantity quantity;
                in >> symbol >> id >> side >> quantity >> price;
                auto fills = engine.submit(symbol, {id,
                    side == "buy" ? lob::Side::Buy : lob::Side::Sell,
                    lob::OrderType::Limit, quantity, parse_price(price)});
                for (const auto& fill : fills) print_trade(fill);
            } else if (command == "market") {
                std::string symbol, side;
                lob::OrderId id;
                lob::Quantity quantity;
                in >> symbol >> id >> side >> quantity;
                auto fills = engine.submit(symbol, {id,
                    side == "buy" ? lob::Side::Buy : lob::Side::Sell,
                    lob::OrderType::Market, quantity});
                for (const auto& fill : fills) print_trade(fill);
            } else if (command == "cancel") {
                std::string symbol;
                lob::OrderId id;
                in >> symbol >> id;
                std::cout << (engine.cancel(symbol, id) ? "cancelled" : "not found") << '\n';
            } else if (command == "modify") {
                std::string symbol, price;
                lob::OrderId id;
                lob::Quantity quantity;
                in >> symbol >> id >> quantity >> price;
                for (const auto& fill : engine.modify(symbol, id, quantity, parse_price(price))) {
                    print_trade(fill);
                }
            } else if (command == "quote") {
                std::string symbol;
                in >> symbol;
                const auto* book = engine.find_book(symbol);
                if (!book) {
                    std::cout << "empty\n";
                    continue;
                }
                std::cout << "bid=" << (book->best_bid() ? std::to_string(*book->best_bid()) : "-")
                          << " ask=" << (book->best_ask() ? std::to_string(*book->best_ask()) : "-")
                          << " orders=" << book->order_count() << '\n';
            } else if (command == "quit" || command == "exit") {
                break;
            } else if (!command.empty()) {
                std::cout << "unknown command\n";
            }
        } catch (const std::exception& error) {
            std::cout << "error: " << error.what() << '\n';
        }
    }
}
