#ifndef BICYCLE_H
#define BICYCLE_H

#include "Vehicle.h"
#include "Rentable.h"

class Bicycle : public Vehicle, public Rentable
{
    double rentPrice;
    bool rented;

public:
    Bicycle(string f, string m, string c, int md, int ms, double rp);
    void showInfo() const override;
    void drive() const override;
    bool rent(int) override;
    void returnItem() override;
    double calculateRentalCost(int) const override;
};

#endif