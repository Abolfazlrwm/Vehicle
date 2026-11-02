#include "Inventory.hpp"
#include <algorithm>
#include <chrono>
#include <cstdio>
#include <ctime>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include "Cli.hpp"
#include "Rentable.hpp"
#include "Saleable.hpp"
#include "VehicleFactory.hpp"

namespace {

std::string nowTimestamp() {
    const auto t = std::time(nullptr);
    std::tm tm{};
#if defined(_WIN32)
    localtime_s(&tm, &t);
#else
    localtime_r(&t, &tm);
#endif
    std::ostringstream oss;
    oss << std::put_time(&tm, "%Y-%m-%d %H:%M:%S");
    return oss.str();
}

std::string toLower(std::string s) {
    std::transform(s.begin(), s.end(), s.begin(),
                   [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
    return s;
}

/// Atomic file write: write to `path.tmp`, then rename over the target.
/// A crash mid-write can never leave a truncated data file behind.
bool writeFileAtomically(const std::string& path, const std::string& content) {
    const std::string tmp = path + ".tmp";
    {
        std::ofstream out(tmp, std::ios::binary | std::ios::trunc);
        if (!out.is_open())
            return false;
        out << content;
        out.close();
        if (!out.good())
            return false;
    }
    return std::rename(tmp.c_str(), path.c_str()) == 0;
}

/// RFC-4180 field escaping for the CSV export.
std::string csvField(const std::string& raw) {
    if (raw.find_first_of(",\"\n\r") == std::string::npos)
        return raw;
    std::string escaped = "\"";
    for (const char c : raw) {
        if (c == '"')
            escaped += "\"\"";
        else
            escaped += c;
    }
    escaped += '"';
    return escaped;
}

}  // namespace

// ---------------------------------------------------------------- fleet

void Inventory::addVehicle(std::unique_ptr<Vehicle> v) {
    if (v)
        vehicles_.push_back(std::move(v));
}

bool Inventory::removeVehicle(int id) {
    Vehicle* v = findById(id);
    if (!v)
        return false;
    if (auto* rentable = dynamic_cast<Rentable*>(v); rentable && rentable->isRented()) {
        return false;  // a vehicle that is out on rent cannot be deleted
    }
    vehicles_.erase(std::remove_if(vehicles_.begin(), vehicles_.end(),
                                   [id](const auto& up) { return up->id() == id; }),
                    vehicles_.end());
    return true;
}

void Inventory::listAll(std::ostream& os) const {
    if (vehicles_.empty()) {
        os << "Inventory is empty.\n";
        return;
    }
    for (const auto& v : vehicles_) v->showInfo(os);
}

std::vector<Vehicle*> Inventory::search(const std::string& keyword) const {
    std::vector<Vehicle*> results;
    const std::string needle = toLower(keyword);

    for (const auto& v : vehicles_) {
        const std::string haystack = toLower(v->factory() + " " + v->model() + " " + v->typeName() +
                                             " " + v->color() + " " + std::to_string(v->id()));
        if (haystack.find(needle) != std::string::npos) {
            results.push_back(v.get());
        }
    }
    return results;
}

Vehicle* Inventory::findById(int id) const {
    const auto it = std::find_if(vehicles_.begin(), vehicles_.end(),
                                 [id](const auto& v) { return v->id() == id; });
    return it != vehicles_.end() ? it->get() : nullptr;
}

int Inventory::nextId() const {
    int maxId = 0;
    for (const auto& v : vehicles_) maxId = std::max(maxId, v->id());
    return maxId + 1;
}

// ------------------------------------------------------- business logic

bool Inventory::sellVehicle(int id) {
    Vehicle* v = findById(id);
    if (!v)
        return false;
    auto* saleable = dynamic_cast<Saleable*>(v);
    if (!saleable || !saleable->sell())
        return false;
    recordTransaction(v->id(), v->model(), TransactionType::Sale, saleable->netPrice(), 0,
                      saleable->discountPercent());
    return true;
}

bool Inventory::rentVehicle(int id, int days) {
    Vehicle* v = findById(id);
    if (!v)
        return false;
    auto* rentable = dynamic_cast<Rentable*>(v);
    if (!rentable || !rentable->rent(days))
        return false;
    recordTransaction(v->id(), v->model(), TransactionType::Rent,
                      rentable->calculateRentalCost(days), days);
    return true;
}

bool Inventory::returnVehicle(int id) {
    Vehicle* v = findById(id);
    if (!v)
        return false;
    auto* rentable = dynamic_cast<Rentable*>(v);
    if (!rentable || !rentable->returnItem())
        return false;
    recordTransaction(v->id(), v->model(), TransactionType::Return, 0.0);
    return true;
}

bool Inventory::applyDiscount(int id, double percent) {
    Vehicle* v = findById(id);
    if (!v)
        return false;
    auto* saleable = dynamic_cast<Saleable*>(v);
    return saleable && saleable->applyDiscount(percent);
}

bool Inventory::clearDiscount(int id) {
    Vehicle* v = findById(id);
    if (!v)
        return false;
    auto* saleable = dynamic_cast<Saleable*>(v);
    return saleable && saleable->clearDiscount();
}

// ----------------------------------------------------------- reporting

double Inventory::totalRevenue() const {
    double total = 0.0;
    for (const auto& t : transactions_) {
        if (t.type != TransactionType::Return)
            total += t.amount;
    }
    return total;
}

InventoryStats Inventory::stats() const {
    InventoryStats s;
    s.total = vehicles_.size();

    for (const auto& v : vehicles_) {
        ++s.byType[v->typeName()];
        const auto* saleable = dynamic_cast<const Saleable*>(v.get());
        const auto* rentable = dynamic_cast<const Rentable*>(v.get());
        if (saleable && saleable->isSold())
            ++s.sold;
        else if (rentable && rentable->isRented())
            ++s.rented;
        else
            ++s.available;
    }

    for (const auto& t : transactions_) {
        switch (t.type) {
            case TransactionType::Sale:
                s.salesRevenue += t.amount;
                ++s.saleCount;
                break;
            case TransactionType::Rent:
                s.rentalRevenue += t.amount;
                ++s.rentCount;
                break;
            case TransactionType::Return:
                ++s.returnCount;
                break;
        }
    }
    s.totalRevenue = s.salesRevenue + s.rentalRevenue;
    return s;
}

void Inventory::printRevenueReport(std::ostream& os) const {
    os << "\n===== Revenue Report =====\n";
    if (transactions_.empty()) {
        os << "No transactions yet.\n";
    }
    for (const auto& t : transactions_) {
        os << "[" << t.timestamp << "] " << toString(t.type) << " - " << t.vehicleModel << " (#"
           << t.vehicleId << ")";
        if (t.type == TransactionType::Sale) {
            os << " - " << cli::formatMoney(t.amount);
            if (t.discount > 0.0)
                os << " (" << t.discount << "% off)";
        } else if (t.type == TransactionType::Rent) {
            os << " - " << cli::formatMoney(t.amount) << " (" << t.days << " days)";
        }
        os << "\n";
    }
    os << "---------------------------\n"
       << "Total revenue: " << cli::formatMoney(totalRevenue()) << "\n"
       << "===========================\n";
}

// ------------------------------------------------------------ CSV export

std::string Inventory::transactionsCsv() const {
    std::ostringstream csv;
    csv << "timestamp,vehicle_id,vehicle_model,type,amount,days,discount_percent\n";
    for (const auto& t : transactions_) {
        csv << csvField(t.timestamp) << ',' << t.vehicleId << ',' << csvField(t.vehicleModel) << ','
            << toString(t.type) << ',' << t.amount << ',' << t.days << ',' << t.discount << '\n';
    }
    return csv.str();
}

bool Inventory::exportCsv(const std::string& path) const {
    return writeFileAtomically(path, transactionsCsv());
}

// ---------------------------------------------------------- persistence

void Inventory::recordTransaction(int vehicleId, const std::string& model, TransactionType type,
                                  double amount, int days, double discount) {
    transactions_.push_back(
        Transaction{vehicleId, model, type, amount, nowTimestamp(), days, discount});
}

bool Inventory::saveToFile(const std::string& inventoryPath,
                           const std::string& transactionsPath) const {
    json inventoryJson = json::array();
    for (const auto& v : vehicles_) inventoryJson.push_back(v->toJson());

    json transactionsJson = json::array();
    for (const auto& t : transactions_) transactionsJson.push_back(t.toJson());

    const bool invOk = writeFileAtomically(inventoryPath, inventoryJson.dump(2));
    const bool txOk = writeFileAtomically(transactionsPath, transactionsJson.dump(2));
    return invOk && txOk;
}

std::size_t Inventory::loadFromFile(const std::string& inventoryPath) {
    std::ifstream file(inventoryPath);
    if (!file.is_open())
        return 0;

    json data;
    try {
        file >> data;
    } catch (const json::exception&) {
        std::cerr << "Warning: " << inventoryPath
                  << " is corrupt — starting with an empty fleet.\n";
        return 0;
    }
    if (!data.is_array())
        return 0;

    std::size_t loaded = 0;
    for (const auto& item : data) {
        auto vehicle = VehicleFactory::instance().create(item);
        if (!vehicle) {
            std::cerr << "Warning: skipping an unknown vehicle record.\n";
            continue;
        }
        vehicles_.push_back(std::move(vehicle));
        ++loaded;
    }
    return loaded;
}

std::size_t Inventory::loadTransactions(const std::string& transactionsPath) {
    std::ifstream file(transactionsPath);
    if (!file.is_open())
        return 0;

    json data;
    try {
        file >> data;
    } catch (const json::exception&) {
        std::cerr << "Warning: " << transactionsPath
                  << " is corrupt — starting with an empty history.\n";
        return 0;
    }
    if (!data.is_array())
        return 0;

    std::size_t loaded = 0;
    for (const auto& item : data) {
        if (!item.is_object())
            continue;
        transactions_.push_back(Transaction::fromJson(item));
        ++loaded;
    }
    return loaded;
}
