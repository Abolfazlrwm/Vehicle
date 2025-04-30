#include "../Header/StoreSystem.h"
#include "../Header/Car.h"
#include "../Header/Motor.h"
#include "../Header/Bicycle.h"

int main()
{
    StoreSystem store;

    store.addVehicle(new Car("BMW", "M3 GTR", "White and Blue", 2005, 200, 45000));
    store.addVehicle(new Car("VOLVO", "XC 90", "Black", 2018, 220, 70000));
    store.addVehicle(new Motor("HARLY", "Street 750", "Black", 2020, 180, 9000, 50));
    store.addVehicle(new Motor("KAWASAKI", "NINJA H2R", "Black", 2021, 330, 40000, 100));
    store.addVehicle(new Bicycle("VIVA", "VIVA 28", "PINK", 2025, 500, 1));

    store.runMenu();

    return 0;
}