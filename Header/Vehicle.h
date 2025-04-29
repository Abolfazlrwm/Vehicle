#ifndef VEHICLE_H
#define VEHICLE_H

#include <string>
#include <iostream>
using namespace std;

class Vehicle
{
protected:
    string factory;
    string model;
    string color;
    int makeDate;
    int maxSpeed;

public:
    Vehicle(string f, string m, string c, int md, int ms);
    virtual void showInfo() const;
    virtual void drive() const;
    virtual ~Vehicle() {}
};

#endif