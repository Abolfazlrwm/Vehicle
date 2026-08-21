#pragma once
#include "Vehicle.hpp"
#include "Rentable.hpp"

/**
 * @brief A bicycle. Rental-only.
 */
class Bicycle : public Vehicle, public Rentable {
public:
    Bicycle(int id, std::string factory, std::string model, std::string color,
            int makeYear, int maxSpeed, double rentPricePerDay);

    void showInfo() const override;
    void drive() const override;
    std::string typeName() const override { return "Bicycle"; }
    json toJson() const override;

    double rentPricePerDay() const override { return rentPricePerDay_; }
    bool isRented() const override { return rented_; }
    bool rent(int days) override;
    bool returnItem() override;
    double calculateRentalCost(int days) const override;

private:
    double rentPricePerDay_;
    bool rented_ = false;
};
