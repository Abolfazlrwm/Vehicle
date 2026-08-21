#pragma once

/**
 * @brief Interface for vehicles that can be sold outright.
 */
class Saleable {
public:
    virtual ~Saleable() = default;

    virtual double price() const = 0;
    virtual bool isSold() const = 0;

    /// Marks the item as sold. Returns false if it was already sold.
    virtual bool sell() = 0;
};
