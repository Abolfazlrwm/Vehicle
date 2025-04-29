#ifndef CAR_H
#define CAR_H

#include "Vehicle.h"

class Car : public Vehicle
{
private:
    double price;

public:
    Car(string f, string m, string c, int md, int ms, double p);
    void showInfo() const override;
    void drive() const override;
};

#endif