#include "../Header/Car.h"

Car::Car(string f, string m, string c, int md, int ms, double p)
    : Vehicle(f, m, c, md, ms) { price = p; }

void Car::showInfo() const
{
    cout << "Factory: " << factory << ", Model: " << model
         << ", Color: " << color << ", Make Date: " << makeDate
         << ", Max Speed: " << maxSpeed << endl;
    cout << "Price: " << price << endl;
}

void Car::drive() const
{
    cout << "Drive from 0 to " << maxSpeed << endl;
}

void Car::sell()
{
    cout << "Selling car with" << price << endl;
}