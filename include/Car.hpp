#pragma once
#include "Vehicle.hpp"
#include "Saleable.hpp"

/**
 * @brief A car. Sale-only (cars in this system are not offered for rent).
 */
class Car : public Vehicle, public Saleable {
public:
    Car(int id, std::string factory, std::string model, std::string color,
        int makeYear, int maxSpeed, double price);

    void showInfo() const override;
    void drive() const override;
    std::string typeName() const override { return "Car"; }
    json toJson() const override;

    double price() const override { return price_; }
    bool isSold() const override { return sold_; }
    bool sell() override;

private:
    double price_;
    bool sold_ = false;
};
