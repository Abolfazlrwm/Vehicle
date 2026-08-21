#include "Car.hpp"
#include <iostream>

Car::Car(int id, std::string factory, std::string model, std::string color,
         int makeYear, int maxSpeed, double price)
    : Vehicle(id, std::move(factory), std::move(model), std::move(color),
              makeYear, maxSpeed),
      price_(price) {}

void Car::showInfo() const {
    std::cout << "[Car #" << id_ << "] " << factory_ << " " << model_
              << " (" << makeYear_ << ", " << color_ << ") - "
              << maxSpeed_ << " km/h - $" << price_
              << (sold_ ? " [SOLD]" : " [AVAILABLE]") << "\n";
}

void Car::drive() const {
    std::cout << model_ << " accelerates smoothly down the road.\n";
}

json Car::toJson() const {
    json j = Vehicle::toJson();
    j["price"] = price_;
    j["sold"] = sold_;
    return j;
}

bool Car::sell() {
    if (sold_) return false;
    sold_ = true;
    return true;
}
