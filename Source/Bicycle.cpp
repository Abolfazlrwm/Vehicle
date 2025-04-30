#include "../Header/Bicycle.h"

Bicycle::Bicycle(string f, string m, string c, int md, int ms, double rp)
    : Vehicle(f, m, c, md, ms)
{
    rentPrice = rp;
    rented = false;
}

void Bicycle::showInfo() const
{
    cout << "Factory: " << factory << ", Model: " << model
         << ", Color: " << color << ", Make Date: " << makeDate
         << ", Max Speed: " << maxSpeed << endl;
    cout << "Rental price per day: " << rentPrice
         << ", Status: " << (rented ? "Rented" : "Available") << endl;
}

void Bicycle::drive() const
{
    for (int i = 1; i <= maxSpeed; ++i)
        cout << "pedaling";
    cout << endl;
}

bool Bicycle::rent(int days)
{
    if (rented)
    {
        cout << "Already rented." << endl;
        return false;
    }
    cout << "The rental price for " << days << "days is equal to: " << days * rentPrice << endl;
    rented = true;
    return true;
}

void Bicycle::returnItem()
{
    rented = false;
    cout << "Bicycle returned." << endl;
}

double Bicycle::calculateRentalCost(int days) const
{
    return days * rentPrice;
}