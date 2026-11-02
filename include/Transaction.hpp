#pragma once
#include <nlohmann/json.hpp>
#include <string>

using json = nlohmann::json;

enum class TransactionType { Sale, Rent, Return };

std::string toString(TransactionType t);

/// Inverse of toString(); returns TransactionType::Return for unknown
/// input only as a safe fallback (loaders validate before using it).
TransactionType transactionTypeFromString(const std::string& s);

/**
 * @brief An immutable record of a sale/rent/return event, used for the
 * revenue report, the statistics dashboard, CSV export and for
 * persisting business history to disk.
 */
struct Transaction {
    int vehicleId = 0;
    std::string vehicleModel;
    TransactionType type = TransactionType::Sale;
    double amount = 0.0;
    std::string timestamp;  // "YYYY-MM-DD HH:MM:SS" (local time)
    int days = 0;           // rental length in days; 0 for sale/return
    double discount = 0.0;  // discount percent applied at sale time

    json toJson() const;

    /// Rebuilds a Transaction from JSON. Unknown/missing fields fall
    /// back to the struct defaults, so old data files still load.
    static Transaction fromJson(const json& j);
};
