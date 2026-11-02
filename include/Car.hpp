#pragma once
#include <memory>
#include "Saleable.hpp"
#include "Vehicle.hpp"

/**
 * @brief A car. Sale-only (cars in this system are not offered for rent).
 */
class Car : public Vehicle, public Saleable {
public:
    /// `sold` restores persisted state when loading from disk.
    Car(int id, std::string factory, std::string model, std::string color, int makeYear,
        int maxSpeed, double listPrice, bool sold = false);

    void showInfo(std::ostream& os) const override;
    void drive() const override;
    std::string typeName() const override { return "Car"; }
    json toJson() const override;

    double listPrice() const override { return price_; }
    bool isSold() const override { return sold_; }
    bool sell() override;

    /// Rebuilds a Car from its JSON representation (used by VehicleFactory).
    static std::unique_ptr<Vehicle> fromJson(const json& j);

private:
    double price_;
    bool sold_ = false;
};
