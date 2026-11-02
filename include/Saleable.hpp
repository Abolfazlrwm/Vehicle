#pragma once

/**
 * @brief Interface for vehicles that can be sold outright.
 *
 * The discount logic is a stateful mixin: every Saleable gets a default,
 * tested implementation of applyDiscount()/netPrice() "for free", so the
 * three sellable classes do not have to duplicate the math. Concrete
 * classes only provide listPrice()/isSold()/sell().
 */
class Saleable {
public:
    virtual ~Saleable() = default;

    /// Sticker price before any discount.
    virtual double listPrice() const = 0;

    virtual bool isSold() const = 0;

    /// Marks the item as sold. Returns false if it was already sold.
    virtual bool sell() = 0;

    /// Applies a promotion. Percent must be in [0, 100); a sold item
    /// cannot be discounted any more. Returns false and does nothing
    /// on invalid input.
    bool applyDiscount(double percent) {
        if (percent < 0.0 || percent >= 100.0 || isSold())
            return false;
        discountPercent_ = percent;
        return true;
    }

    /// Removes an active promotion. Returns false if the item was sold.
    bool clearDiscount() {
        if (isSold())
            return false;
        discountPercent_ = 0.0;
        return true;
    }

    double discountPercent() const { return discountPercent_; }

    /// Restores a persisted discount WITHOUT the isSold() guard: a sold
    /// vehicle keeps the historical discount it was sold with, and
    /// deserialization must be able to reproduce that state exactly.
    /// Only intended for the fromJson() factories.
    void restoreDiscount(double percent) { discountPercent_ = percent; }

    /// Price actually charged: listPrice() reduced by the discount.
    double netPrice() const { return listPrice() * (1.0 - discountPercent_ / 100.0); }

private:
    double discountPercent_ = 0.0;
};
