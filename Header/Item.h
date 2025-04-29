#ifndef ITEM_H
#define ITEM_H

#include "Vehicle.h"

class Item : public Vehicle
{
public:
    string factory;
    string model;
    string color;
    int makeDate;

    virtual void showDetails() const;
    virtual ~Item();
};

#endif