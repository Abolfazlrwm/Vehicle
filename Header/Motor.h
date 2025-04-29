#ifndef MOTOR_H
#define MOTOR_H

#include "Vehicle.h"

class Motor : public Vehicle
{
private:
    double salePrice;
    double rentPrice;
    bool rented;

public:
    Motor(string f, string m, string c, int md, int ms, double s, double r);
    void showInfo() const override;
    void drive() const override;
    bool rent(int days);
    void sell();
};

#endif