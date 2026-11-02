#pragma once
#include <memory>
#include "Rentable.hpp"
#include "Vehicle.hpp"

/**
 * @brief A bicycle. Rental-only.
 */
class Bicycle : public Vehicle, public Rentable {
public:
    /// `rented` restores persisted state when loading from disk.
    Bicycle(int id, std::string factory, std::string model, std::string color, int makeYear,
            int maxSpeed, double rentPricePerDay, bool rented = false);

    void showInfo(std::ostream& os) const override;
    void drive() const override;
    std::string typeName() const override { return "Bicycle"; }
    json toJson() const override;

    double rentPricePerDay() const override { return rentPricePerDay_; }
    bool isRented() const override { return rented_; }
    bool rent(int days) override;
    bool returnItem() override;
    double calculateRentalCost(int days) const override;

    /// Rebuilds a Bicycle from its JSON representation (used by VehicleFactory).
    static std::unique_ptr<Vehicle> fromJson(const json& j);

private:
    double rentPricePerDay_;
    bool rented_ = false;
};
