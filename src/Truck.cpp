#include "Truck.hpp"
#include <iostream>
#include <ostream>
#include "Cli.hpp"

Truck::Truck(int id, std::string factory, std::string model, std::string color, int makeYear,
             int maxSpeed, double listPrice, double cargoCapacityTons, bool sold)
    : Vehicle(id, std::move(factory), std::move(model), std::move(color), makeYear, maxSpeed),
      price_(listPrice),
      cargoCapacityTons_(cargoCapacityTons),
      sold_(sold) {}

void Truck::showInfo(std::ostream& os) const {
    os << "[Truck #" << id_ << "] " << factory_ << ' ' << model_ << " (" << makeYear_ << ", "
       << color_ << ") - " << maxSpeed_ << " km/h - " << cargoCapacityTons_ << "t cargo - "
       << cli::formatMoney(price_) << ' ' << cli::statusTag(sold_ ? "SOLD" : "AVAILABLE") << '\n';
}

void Truck::drive() const {
    std::cout << model_ << " hauls heavy cargo down the highway.\n";
}

json Truck::toJson() const {
    json j = Vehicle::toJson();
    j["price"] = price_;
    j["cargoCapacityTons"] = cargoCapacityTons_;
    j["sold"] = sold_;
    j["discountPercent"] = discountPercent();
    return j;
}

bool Truck::sell() {
    if (sold_)
        return false;
    sold_ = true;
    return true;
}

std::unique_ptr<Vehicle> Truck::fromJson(const json& j) {
    auto truck = std::make_unique<Truck>(
        j.value("id", 0), j.value("factory", ""), j.value("model", ""), j.value("color", ""),
        j.value("makeYear", 2000), j.value("maxSpeed", 0), j.value("price", 0.0),
        j.value("cargoCapacityTons", 0.0), j.value("sold", false));
    const double discount = j.value("discountPercent", 0.0);
    truck->restoreDiscount(discount);
    return truck;
}
