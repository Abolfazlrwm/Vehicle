#pragma once

/**
 * @brief Interface for vehicles that can be rented out by the day.
 */
class Rentable {
public:
    virtual ~Rentable() = default;

    virtual double rentPricePerDay() const = 0;
    virtual bool isRented() const = 0;

    /// Rents the item for `days` days. Returns false if already rented.
    virtual bool rent(int days) = 0;

    /// Returns the item from rental. Returns false if it wasn't rented.
    virtual bool returnItem() = 0;

    virtual double calculateRentalCost(int days) const = 0;
};
