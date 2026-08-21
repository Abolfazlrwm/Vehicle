#pragma once
#include <string>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

enum class TransactionType { Sale, Rent, Return };

std::string toString(TransactionType t);

/**
 * @brief An immutable record of a sale/rent/return event, used for the
 * revenue report and for persisting business history to disk.
 */
struct Transaction {
    int vehicleId;
    std::string vehicleModel;
    TransactionType type;
    double amount;
    std::string timestamp; // ISO-8601

    json toJson() const;
};
