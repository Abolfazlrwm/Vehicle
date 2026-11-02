#pragma once
#include <memory>
#include "Rentable.hpp"
#include "Saleable.hpp"
#include "Vehicle.hpp"

/**
 * @brief A motorcycle. Both sellable and rentable at the same time,
 * which is why it implements both interfaces — the interesting case
 * for the diamond-free multiple inheritance design in this project.
 */
class Motorcycle : public Vehicle, public Saleable, public Rentable {
public:
    /// `sold` / `rented` restore persisted state when loading from disk.
    /// A motorcycle is never both sold and rented at once; if a JSON
    /// record contains both flags, `sold` wins.
    Motorcycle(int id, std::string factory, std::string model, std::string color, int makeYear,
               int maxSpeed, double listPrice, double rentPricePerDay, bool sold = false,
               bool rented = false);

    void showInfo(std::ostream& os) const override;
    void drive() const override;
    std::string typeName() const override { return "Motorcycle"; }
    json toJson() const override;

    double listPrice() const override { return price_; }
    bool isSold() const override { return sold_; }
    bool sell() override;

    double rentPricePerDay() const override { return rentPricePerDay_; }
    bool isRented() const override { return rented_; }
    bool rent(int days) override;
    bool returnItem() override;
    double calculateRentalCost(int days) const override;

    /// Rebuilds a Motorcycle from its JSON representation (VehicleFactory).
    static std::unique_ptr<Vehicle> fromJson(const json& j);

private:
    double price_;
    double rentPricePerDay_;
    bool sold_ = false;
    bool rented_ = false;
};
