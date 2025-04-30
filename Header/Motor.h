#ifndef MOTOR_H
#define MOTOR_H

#include "Vehicle.h"
#include "Saleable.h"
#include "Rentable.h"

class Motor : public Vehicle, public Saleable, public Rentable
{
    double salePrice, rentPrice;
    bool rented;

public:
    Motor(string f, string m, string c, int md, int ms, double sp, double rp);
    void showInfo() const override;
    void drive() const override;
    void sell() override;
    bool rent(int) override;
    void returnItem() override;
    double calculateRentalCost(int) const override;
};

#endif