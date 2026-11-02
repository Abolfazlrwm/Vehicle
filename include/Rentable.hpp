#pragma once

/**
 * @brief Interface for vehicles that can be rented out by the day.
 */
class Rentable {
public:
    virtual ~Rentable() = default;

    virtual double rentPricePerDay() const = 0;
    virtual bool isRented() const = 0;

    /// Rents the item for `days` days. Returns false if it is already
    /// rented, sold, or `days` is not positive.
    virtual bool rent(int days) = 0;

    /// Returns the item from rental. Returns false if it wasn't rented.
    virtual bool returnItem() = 0;

    /// Total rental cost for `days` days (0 if `days` is not positive).
    virtual double calculateRentalCost(int days) const = 0;
};
