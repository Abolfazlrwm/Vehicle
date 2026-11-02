#pragma once
#include <memory>
#include "Saleable.hpp"
#include "Vehicle.hpp"

/**
 * @brief A cargo truck. Sale-only, with a payload capacity in tons.
 *
 * Adding this class required exactly three things: the class itself,
 * one `registerType` call in VehicleFactory, and one seed entry —
 * Inventory needed *zero* changes, which is the whole point of the
 * design (see README).
 */
class Truck : public Vehicle, public Saleable {
public:
    /// `sold` restores persisted state when loading from disk.
    Truck(int id, std::string factory, std::string model, std::string color, int makeYear,
          int maxSpeed, double listPrice, double cargoCapacityTons, bool sold = false);

    void showInfo(std::ostream& os) const override;
    void drive() const override;
    std::string typeName() const override { return "Truck"; }
    json toJson() const override;

    double listPrice() const override { return price_; }
    bool isSold() const override { return sold_; }
    bool sell() override;

    double cargoCapacityTons() const { return cargoCapacityTons_; }

    /// Rebuilds a Truck from its JSON representation (used by VehicleFactory).
    static std::unique_ptr<Vehicle> fromJson(const json& j);

private:
    double price_;
    double cargoCapacityTons_;
    bool sold_ = false;
};
