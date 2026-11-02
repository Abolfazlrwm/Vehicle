#include "Car.hpp"
#include <cstdio>
#include <iostream>
#include <ostream>
#include "Cli.hpp"

Car::Car(int id, std::string factory, std::string model, std::string color, int makeYear,
         int maxSpeed, double listPrice, bool sold)
    : Vehicle(id, std::move(factory), std::move(model), std::move(color), makeYear, maxSpeed),
      price_(listPrice),
      sold_(sold) {}

void Car::showInfo(std::ostream& os) const {
    os << "[Car #" << id_ << "] " << factory_ << ' ' << model_ << " (" << makeYear_ << ", "
       << color_ << ") - " << maxSpeed_ << " km/h - " << cli::formatMoney(price_);
    if (discountPercent() > 0.0) {
        char pct[16];
        std::snprintf(pct, sizeof(pct), "%g", discountPercent());
        os << " (" << pct << "% off: " << cli::formatMoney(netPrice()) << ")";
    }
    os << ' ' << cli::statusTag(sold_ ? "SOLD" : "AVAILABLE") << '\n';
}

void Car::drive() const {
    std::cout << model_ << " accelerates smoothly down the road.\n";
}

json Car::toJson() const {
    json j = Vehicle::toJson();
    j["price"] = price_;
    j["sold"] = sold_;
    j["discountPercent"] = discountPercent();
    return j;
}

bool Car::sell() {
    if (sold_)
        return false;
    sold_ = true;
    return true;
}

std::unique_ptr<Vehicle> Car::fromJson(const json& j) {
    auto car = std::make_unique<Car>(j.value("id", 0), j.value("factory", ""), j.value("model", ""),
                                     j.value("color", ""), j.value("makeYear", 2000),
                                     j.value("maxSpeed", 0), j.value("price", 0.0),
                                     j.value("sold", false));
    const double discount = j.value("discountPercent", 0.0);
    car->restoreDiscount(discount);
    return car;
}
