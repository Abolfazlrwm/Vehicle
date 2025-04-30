#include "../Header/Motor.h"

Motor::Motor(string f, string m, string c, int md, int ms, double sp, double rp)
    : Vehicle(f, m, c, md, ms), salePrice(sp), rentPrice(rp), rented(false) {}

void Motor::showInfo() const
{
    cout << "Factory: " << factory << ", Model: " << model
         << ", Color: " << color << ", Make Date: " << makeDate
         << ", Max Speed: " << maxSpeed << endl;
    cout << "Sale Price: " << salePrice
         << ", Rental price per day: " << rentPrice
         << ", Status: " << (rented ? "Rented" : "Available") << endl;
}

void Motor::drive() const
{
    cout << "Drive from 0 to " << maxSpeed / 2 << endl;
}

void Motor::sell()
{
    if (!rented)
        cout << "Selling motor for " << salePrice << endl;
    else
        cout << "Can NOT sell while rented." << endl;
}

bool Motor::rent(int days)
{
    if (rented)
    {
        cout << "Already rented!" << endl;
        return false;
    }
    cout << "The rental price for " << days << "days is equal to: " << days * rentPrice << endl;
    rented = true;
    return true;
}

void Motor::returnItem()
{
    rented = false;
    cout << "Motor returned." << endl;
}

double Motor::calculateRentalCost(int days) const
{
    return days * rentPrice;
}