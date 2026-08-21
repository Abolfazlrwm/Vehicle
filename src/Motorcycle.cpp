#include "Motorcycle.hpp"
#include <iostream>

Motorcycle::Motorcycle(int id, std::string factory, std::string model, std::string color,
                        int makeYear, int maxSpeed, double price, double rentPricePerDay)
    : Vehicle(id, std::move(factory), std::move(model), std::move(color),
              makeYear, maxSpeed),
      price_(price), rentPricePerDay_(rentPricePerDay) {}

void Motorcycle::showInfo() const {
    std::cout << "[Motorcycle #" << id_ << "] " << factory_ << " " << model_
              << " (" << makeYear_ << ", " << color_ << ") - "
              << maxSpeed_ << " km/h - $" << price_ << " to buy / $"
              << rentPricePerDay_ << " per day"
              << (sold_ ? " [SOLD]" : "")
              << (rented_ ? " [RENTED]" : "") << "\n";
}

void Motorcycle::drive() const {
    std::cout << model_ << " roars off the line.\n";
}

json Motorcycle::toJson() const {
    json j = Vehicle::toJson();
    j["price"] = price_;
    j["rentPricePerDay"] = rentPricePerDay_;
    j["sold"] = sold_;
    j["rented"] = rented_;
    return j;
}

bool Motorcycle::sell() {
    if (sold_ || rented_) return false;
    sold_ = true;
    return true;
}

bool Motorcycle::rent(int days) {
    if (sold_ || rented_ || days <= 0) return false;
    rented_ = true;
    return true;
}

bool Motorcycle::returnItem() {
    if (!rented_) return false;
    rented_ = false;
    return true;
}

double Motorcycle::calculateRentalCost(int days) const {
    return days > 0 ? days * rentPricePerDay_ : 0.0;
}
