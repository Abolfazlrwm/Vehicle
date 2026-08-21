#include "Transaction.hpp"
#include <chrono>
#include <ctime>
#include <sstream>
#include <iomanip>

std::string toString(TransactionType t) {
    switch (t) {
        case TransactionType::Sale: return "SALE";
        case TransactionType::Rent: return "RENT";
        case TransactionType::Return: return "RETURN";
    }
    return "UNKNOWN";
}

json Transaction::toJson() const {
    return json{
        {"vehicleId", vehicleId},
        {"vehicleModel", vehicleModel},
        {"type", toString(type)},
        {"amount", amount},
        {"timestamp", timestamp},
    };
}
