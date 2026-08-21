#pragma once
#include "Vehicle.hpp"
#include "Saleable.hpp"
#include "Rentable.hpp"

/**
 * @brief A motorcycle. Both sellable and rentable at the same time,
 * which is why it implements both interfaces — the interesting case
 * for the diamond-free multiple inheritance design in this project.
 */
class Motorcycle : public Vehicle, public Saleable, public Rentable {
public:
    Motorcycle(int id, std::string factory, std::string model, std::string color,
               int makeYear, int maxSpeed, double price, double rentPricePerDay);

    void showInfo() const override;
    void drive() const override;
    std::string typeName() const override { return "Motorcycle"; }
    json toJson() const override;

    double price() const override { return price_; }
    bool isSold() const override { return sold_; }
    bool sell() override;

    double rentPricePerDay() const override { return rentPricePerDay_; }
    bool isRented() const override { return rented_; }
    bool rent(int days) override;
    bool returnItem() override;
    double calculateRentalCost(int days) const override;

private:
    double price_;
    double rentPricePerDay_;
    bool sold_ = false;
    bool rented_ = false;
};
