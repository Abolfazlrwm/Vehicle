#include "../Header/Vehicle.h"

Vehicle::Vehicle(string f, string m, string c, int md, int ms)
    : factory(f), model(m), color(c), makeDate(md), maxSpeed(ms) {}