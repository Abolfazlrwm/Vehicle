#include "Bicycle.hpp"
#include <iostream>
#include <ostream>
#include "Cli.hpp"

Bicycle::Bicycle(int id, std::string factory, std::string model, std::string color, int makeYear,
                 int maxSpeed, double rentPricePerDay, bool rented)
    : Vehicle(id, std::move(factory), std::move(model), std::move(color), makeYear, maxSpeed),
      rentPricePerDay_(rentPricePerDay),
      rented_(rented) {}

void Bicycle::showInfo(std::ostream& os) const {
    os << "[Bicycle #" << id_ << "] " << factory_ << ' ' << model_ << " (" << makeYear_ << ", "
       << color_ << ") - " << maxSpeed_ << " km/h - " << cli::formatMoney(rentPricePerDay_)
       << "/day " << cli::statusTag(rented_ ? "RENTED" : "AVAILABLE") << '\n';
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
    if (rented_ || days <= 0)
        return false;
    rented_ = true;
    return true;
}

bool Bicycle::returnItem() {
    if (!rented_)
        return false;
    rented_ = false;
    return true;
}

double Bicycle::calculateRentalCost(int days) const {
    return days > 0 ? days * rentPricePerDay_ : 0.0;
}

std::unique_ptr<Vehicle> Bicycle::fromJson(const json& j) {
    return std::make_unique<Bicycle>(j.value("id", 0), j.value("factory", ""), j.value("model", ""),
                                     j.value("color", ""), j.value("makeYear", 2000),
                                     j.value("maxSpeed", 0), j.value("rentPricePerDay", 0.0),
                                     j.value("rented", false));
}
