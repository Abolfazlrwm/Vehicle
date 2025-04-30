#ifndef VEHICLE_H
#define VEHICLE_H

#include <string>
#include <iostream>
using namespace std;

class Vehicle
{
public:
    string factory, model, color;
    int makeDate, maxSpeed;

    Vehicle(string f, string m, string c, int md, int ms);

    virtual void showInfo() const = 0;
    virtual void drive() const = 0;
    virtual ~Vehicle() {}
};

#endif