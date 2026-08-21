#include "Bicycle.hpp"
#include <iostream>

Bicycle::Bicycle(int id, std::string factory, std::string model, std::string color,
                  int makeYear, int maxSpeed, double rentPricePerDay)
    : Vehicle(id, std::move(factory), std::move(model), std::move(color),
              makeYear, maxSpeed),
      rentPricePerDay_(rentPricePerDay) {}

void Bicycle::showInfo() const {
    std::cout << "[Bicycle #" << id_ << "] " << factory_ << " " << model_
              << " (" << makeYear_ << ", " << color_ << ") - "
              << maxSpeed_ << " km/h - $" << rentPricePerDay_ << "/day"
              << (rented_ ? " [RENTED]" : " [AVAILABLE]") << "\n";
}

void Bicycle::drive() const {
    std::cout << model_ << " pedals along quietly.\n";
}

json Bicycle::toJson() const {
    json j = Vehicle::toJson();
    j["rentPricePerDay"] = rentPricePerDay_;
    j["rented"] = rented_;
    return j;
}

bool Bicycle::rent(int days) {
    if (rented_ || days <= 0) return false;
    rented_ = true;
    return true;
}

bool Bicycle::returnItem() {
    if (!rented_) return false;
    rented_ = false;
    return true;
}

double Bicycle::calculateRentalCost(int days) const {
    return days > 0 ? days * rentPricePerDay_ : 0.0;
}
