#ifndef BICYCLE_H
#define BICYCLE_H

#include "Vehicle.h"

class Bicycle : public Vehicle
{
private:
    double rentPrice;
    bool rented;

public:
    Bicycle(string f, string m, string c, int md, int ms, double r);
    void showInfo() const override;
    void drive() const override;
    bool rent(int);
    void returnBike();
};

#endif