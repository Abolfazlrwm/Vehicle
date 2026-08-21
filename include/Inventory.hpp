#pragma once
#include <memory>
#include <vector>
#include <string>
#include <optional>
#include "Vehicle.hpp"
#include "Transaction.hpp"

/**
 * @brief Owns the fleet of vehicles and the transaction log.
 *
 * Replaces the original linked-list StoreSystem with a
 * std::vector<std::unique_ptr<Vehicle>> (RAII, no manual memory
 * management) plus JSON persistence so data survives between runs.
 */
class Inventory {
public:
    void addVehicle(std::unique_ptr<Vehicle> v);

    void listAll() const;
    std::vector<Vehicle*> search(const std::string& keyword) const;

    /// Returns nullptr if not found.
    Vehicle* findById(int id) const;

    bool sellVehicle(int id);
    bool rentVehicle(int id, int days);
    bool returnVehicle(int id);

    double totalRevenue() const;
    void printRevenueReport() const;

    /// Persist the current fleet + transaction log to disk as JSON.
    void saveToFile(const std::string& inventoryPath,
                     const std::string& transactionsPath) const;

    /// Load a previously saved fleet from disk. Returns number of
    /// vehicles loaded (0 if the file didn't exist or was empty).
    size_t loadFromFile(const std::string& inventoryPath);

    size_t size() const { return vehicles_.size(); }

private:
    std::vector<std::unique_ptr<Vehicle>> vehicles_;
    std::vector<Transaction> transactions_;

    void recordTransaction(int vehicleId, const std::string& model,
                            TransactionType type, double amount);
};
