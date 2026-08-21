#include "Inventory.hpp"
#include "Saleable.hpp"
#include "Rentable.hpp"
#include "Car.hpp"
#include "Bicycle.hpp"
#include "Motorcycle.hpp"
#include <iostream>
#include <fstream>
#include <algorithm>
#include <chrono>
#include <ctime>
#include <sstream>
#include <iomanip>

namespace {
std::string nowTimestamp() {
    auto t = std::time(nullptr);
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
} // namespace

void Inventory::addVehicle(std::unique_ptr<Vehicle> v) {
    vehicles_.push_back(std::move(v));
}

void Inventory::listAll() const {
    if (vehicles_.empty()) {
        std::cout << "Inventory is empty.\n";
        return;
    }
    for (const auto& v : vehicles_) {
        v->showInfo();
    }
}

std::vector<Vehicle*> Inventory::search(const std::string& keyword) const {
    std::vector<Vehicle*> results;
    std::string needle = keyword;
    std::transform(needle.begin(), needle.end(), needle.begin(), ::tolower);

    for (const auto& v : vehicles_) {
        std::string haystack = v->factory() + " " + v->model() + " " + v->typeName();
        std::transform(haystack.begin(), haystack.end(), haystack.begin(), ::tolower);
        if (haystack.find(needle) != std::string::npos) {
            results.push_back(v.get());
        }
    }
    return results;
}

Vehicle* Inventory::findById(int id) const {
    auto it = std::find_if(vehicles_.begin(), vehicles_.end(),
                            [id](const auto& v) { return v->id() == id; });
    return it != vehicles_.end() ? it->get() : nullptr;
}

bool Inventory::sellVehicle(int id) {
    Vehicle* v = findById(id);
    if (!v) return false;
    auto* saleable = dynamic_cast<Saleable*>(v);
    if (!saleable || !saleable->sell()) return false;
    recordTransaction(v->id(), v->model(), TransactionType::Sale, saleable->price());
    return true;
}

bool Inventory::rentVehicle(int id, int days) {
    Vehicle* v = findById(id);
    if (!v) return false;
    auto* rentable = dynamic_cast<Rentable*>(v);
    if (!rentable || !rentable->rent(days)) return false;
    recordTransaction(v->id(), v->model(), TransactionType::Rent,
                       rentable->calculateRentalCost(days));
    return true;
}

bool Inventory::returnVehicle(int id) {
    Vehicle* v = findById(id);
    if (!v) return false;
    auto* rentable = dynamic_cast<Rentable*>(v);
    if (!rentable || !rentable->returnItem()) return false;
    recordTransaction(v->id(), v->model(), TransactionType::Return, 0.0);
    return true;
}

double Inventory::totalRevenue() const {
    double total = 0.0;
    for (const auto& t : transactions_) {
        if (t.type != TransactionType::Return) total += t.amount;
    }
    return total;
}

void Inventory::printRevenueReport() const {
    std::cout << "\n===== Revenue Report =====\n";
    if (transactions_.empty()) {
        std::cout << "No transactions yet.\n";
    }
    for (const auto& t : transactions_) {
        std::cout << "[" << t.timestamp << "] " << toString(t.type)
                  << " - " << t.vehicleModel << " (#" << t.vehicleId << ")";
        if (t.type != TransactionType::Return) {
            std::cout << " - $" << t.amount;
        }
        std::cout << "\n";
    }
    std::cout << "---------------------------\n";
    std::cout << "Total revenue: $" << totalRevenue() << "\n";
    std::cout << "===========================\n";
}

void Inventory::recordTransaction(int vehicleId, const std::string& model,
                                   TransactionType type, double amount) {
    transactions_.push_back(Transaction{vehicleId, model, type, amount, nowTimestamp()});
}

void Inventory::saveToFile(const std::string& inventoryPath,
                            const std::string& transactionsPath) const {
    json inventoryJson = json::array();
    for (const auto& v : vehicles_) inventoryJson.push_back(v->toJson());

    json transactionsJson = json::array();
    for (const auto& t : transactions_) transactionsJson.push_back(t.toJson());

    std::ofstream invFile(inventoryPath);
    invFile << inventoryJson.dump(2);

    std::ofstream txFile(transactionsPath);
    txFile << transactionsJson.dump(2);
}

size_t Inventory::loadFromFile(const std::string& inventoryPath) {
    std::ifstream file(inventoryPath);
    if (!file.is_open()) return 0;

    json data;
    try {
        file >> data;
    } catch (...) {
        return 0;
    }

    size_t loaded = 0;
    for (const auto& item : data) {
        std::string type = item.value("type", "");
        int id = item.value("id", 0);
        std::string factory = item.value("factory", "");
        std::string model = item.value("model", "");
        std::string color = item.value("color", "");
        int makeYear = item.value("makeYear", 0);
        int maxSpeed = item.value("maxSpeed", 0);

        if (type == "Car") {
            auto car = std::make_unique<Car>(id, factory, model, color, makeYear,
                                              maxSpeed, item.value("price", 0.0));
            if (item.value("sold", false)) car->sell();
            addVehicle(std::move(car));
            ++loaded;
        } else if (type == "Bicycle") {
            auto bike = std::make_unique<Bicycle>(id, factory, model, color, makeYear,
                                                    maxSpeed, item.value("rentPricePerDay", 0.0));
            if (item.value("rented", false)) bike->rent(1);
            addVehicle(std::move(bike));
            ++loaded;
        } else if (type == "Motorcycle") {
            auto moto = std::make_unique<Motorcycle>(
                id, factory, model, color, makeYear, maxSpeed,
                item.value("price", 0.0), item.value("rentPricePerDay", 0.0));
            if (item.value("sold", false)) moto->sell();
            else if (item.value("rented", false)) moto->rent(1);
            addVehicle(std::move(moto));
            ++loaded;
        }
    }
    return loaded;
}
