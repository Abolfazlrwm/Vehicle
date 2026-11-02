#include "Motorcycle.hpp"
#include <cstdio>
#include <iostream>
#include <ostream>
#include "Cli.hpp"

Motorcycle::Motorcycle(int id, std::string factory, std::string model, std::string color,
                       int makeYear, int maxSpeed, double listPrice, double rentPricePerDay,
                       bool sold, bool rented)
    : Vehicle(id, std::move(factory), std::move(model), std::move(color), makeYear, maxSpeed),
      price_(listPrice),
      rentPricePerDay_(rentPricePerDay),
      sold_(sold),
      rented_(sold ? false : rented) {}

void Motorcycle::showInfo(std::ostream& os) const {
    os << "[Motorcycle #" << id_ << "] " << factory_ << ' ' << model_ << " (" << makeYear_ << ", "
       << color_ << ") - " << maxSpeed_ << " km/h - " << cli::formatMoney(price_) << " to buy / "
       << cli::formatMoney(rentPricePerDay_) << " per day";
    if (discountPercent() > 0.0) {
        char pct[16];
        std::snprintf(pct, sizeof(pct), "%g", discountPercent());
        os << " (" << pct << "% off: " << cli::formatMoney(netPrice()) << ")";
    }
    if (sold_)
        os << ' ' << cli::statusTag("SOLD");
    if (rented_)
        os << ' ' << cli::statusTag("RENTED");
    if (!sold_ && !rented_)
        os << ' ' << cli::statusTag("AVAILABLE");
    os << '\n';
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
    j["discountPercent"] = discountPercent();
    return j;
}

bool Motorcycle::sell() {
    if (sold_ || rented_)
        return false;
    sold_ = true;
    return true;
}

bool Motorcycle::rent(int days) {
    if (sold_ || rented_ || days <= 0)
        return false;
    rented_ = true;
    return true;
}

bool Motorcycle::returnItem() {
    if (!rented_)
        return false;
    rented_ = false;
    return true;
}

double Motorcycle::calculateRentalCost(int days) const {
    return days > 0 ? days * rentPricePerDay_ : 0.0;
}

std::unique_ptr<Vehicle> Motorcycle::fromJson(const json& j) {
    auto moto = std::make_unique<Motorcycle>(
        j.value("id", 0), j.value("factory", ""), j.value("model", ""), j.value("color", ""),
        j.value("makeYear", 2000), j.value("maxSpeed", 0), j.value("price", 0.0),
        j.value("rentPricePerDay", 0.0), j.value("sold", false), j.value("rented", false));
    const double discount = j.value("discountPercent", 0.0);
    moto->restoreDiscount(discount);
    return moto;
}
