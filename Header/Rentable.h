#ifndef RENTABLE_H
#define RENTABLE_H

class Rentable
{
public:
    virtual bool rent(int ) = 0;
    virtual void returnItem() = 0;
    virtual double calculateRentalCost(int ) const = 0;
    virtual ~Rentable() {}
};

#endif