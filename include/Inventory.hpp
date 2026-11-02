#pragma once
#include <iosfwd>
#include <map>
#include <memory>
#include <string>
#include <vector>
#include "Transaction.hpp"
#include "Vehicle.hpp"

/**
 * @brief Snapshot of the fleet and the transaction log, computed by
 * Inventory::stats() and rendered by the CLI statistics dashboard.
 */
struct InventoryStats {
    std::size_t total = 0;
    std::size_t available = 0;
    std::size_t sold = 0;
    std::size_t rented = 0;

    /// Vehicle count per type name ("Car", "Bicycle", ...).
    std::map<std::string, std::size_t> byType;

    double salesRevenue = 0.0;
    double rentalRevenue = 0.0;
    double totalRevenue = 0.0;

    std::size_t saleCount = 0;
    std::size_t rentCount = 0;
    std::size_t returnCount = 0;
};

/**
 * @brief Owns the fleet of vehicles and the transaction log.
 *
 * - The fleet is a std::vector<std::unique_ptr<Vehicle>> (RAII, no raw
 *   new/delete anywhere in the project).
 * - Selling/renting works through dynamic_cast<Saleable*> /
 *   dynamic_cast<Rentable*>, so Inventory is polymorphic over vehicle
 *   capabilities, not concrete types.
 * - Everything persists to JSON; both files are written atomically
 *   (tmp file + rename) and the transaction history is loaded again on
 *   startup, so revenue survives between runs.
 */
class Inventory {
public:
    /// Takes ownership of a vehicle.
    void addVehicle(std::unique_ptr<Vehicle> v);

    /// Deletes a vehicle. Rented vehicles cannot be deleted (the shop
    /// does not own them while they are out). Returns false if the id
    /// is unknown or the vehicle is currently rented.
    bool removeVehicle(int id);

    void listAll(std::ostream& os) const;
    std::vector<Vehicle*> search(const std::string& keyword) const;

    /// Returns nullptr if not found.
    Vehicle* findById(int id) const;

    bool sellVehicle(int id);
    bool rentVehicle(int id, int days);
    bool returnVehicle(int id);

    /// Promotion handling on the first Saleable capability of a vehicle.
    bool applyDiscount(int id, double percent);
    bool clearDiscount(int id);

    InventoryStats stats() const;

    double totalRevenue() const;
    void printRevenueReport(std::ostream& os) const;

    /// Full transaction log as RFC-4180 CSV (header + one row per event).
    std::string transactionsCsv() const;

    /// Writes transactionsCsv() to `path` atomically. Returns success.
    bool exportCsv(const std::string& path) const;

    /// Persists fleet + transactions atomically. Returns success.
    bool saveToFile(const std::string& inventoryPath, const std::string& transactionsPath) const;

    /// Loads the fleet. Returns the number of vehicles restored
    /// (0 if the file didn't exist or could not be parsed).
    std::size_t loadFromFile(const std::string& inventoryPath);

    /// Loads the transaction history. Returns the number of events.
    std::size_t loadTransactions(const std::string& transactionsPath);

    const std::vector<Transaction>& transactions() const { return transactions_; }

    std::size_t size() const { return vehicles_.size(); }

    /// The next free vehicle id: max(existing ids) + 1. Using max()
    /// instead of size() avoids collisions when ids are sparse
    /// (e.g. after deleting vehicles).
    int nextId() const;

private:
    std::vector<std::unique_ptr<Vehicle>> vehicles_;
    std::vector<Transaction> transactions_;

    void recordTransaction(int vehicleId, const std::string& model, TransactionType type,
                           double amount, int days = 0, double discount = 0.0);
};
