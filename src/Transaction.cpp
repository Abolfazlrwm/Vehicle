#include "Transaction.hpp"

std::string toString(TransactionType t) {
    switch (t) {
        case TransactionType::Sale:
            return "SALE";
        case TransactionType::Rent:
            return "RENT";
        case TransactionType::Return:
            return "RETURN";
    }
    return "UNKNOWN";
}

TransactionType transactionTypeFromString(const std::string& s) {
    if (s == "SALE")
        return TransactionType::Sale;
    if (s == "RENT")
        return TransactionType::Rent;
    if (s == "RETURN")
        return TransactionType::Return;
    return TransactionType::Return;  // safe fallback for corrupt files
}

json Transaction::toJson() const {
    return json{
        {"vehicleId", vehicleId}, {"vehicleModel", vehicleModel},
        {"type", toString(type)}, {"amount", amount},
        {"days", days},           {"discount", discount},
        {"timestamp", timestamp},
    };
}

Transaction Transaction::fromJson(const json& j) {
    Transaction t;
    t.vehicleId = j.value("vehicleId", 0);
    t.vehicleModel = j.value("vehicleModel", "");
    t.type = transactionTypeFromString(j.value("type", ""));
    t.amount = j.value("amount", 0.0);
    t.days = j.value("days", 0);
    t.discount = j.value("discount", 0.0);
    t.timestamp = j.value("timestamp", "");
    return t;
}
