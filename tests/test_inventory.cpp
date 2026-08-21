#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>
#include "Inventory.hpp"
#include "Car.hpp"
#include "Bicycle.hpp"
#include "Motorcycle.hpp"

TEST_CASE("Car can be sold exactly once") {
    Car car(1, "Toyota", "Corolla", "White", 2022, 180, 20000);
    CHECK(car.isSold() == false);
    CHECK(car.sell() == true);
    CHECK(car.isSold() == true);
    CHECK(car.sell() == false); // already sold
}

TEST_CASE("Bicycle rent/return cycle") {
    Bicycle bike(2, "Giant", "Escape 3", "Black", 2023, 30, 10);
    CHECK(bike.isRented() == false);
    CHECK(bike.rent(3) == true);
    CHECK(bike.isRented() == true);
    CHECK(bike.rent(2) == false); // already rented
    CHECK(bike.calculateRentalCost(3) == doctest::Approx(30.0));
    CHECK(bike.returnItem() == true);
    CHECK(bike.isRented() == false);
    CHECK(bike.returnItem() == false); // wasn't rented
}

TEST_CASE("Motorcycle cannot be sold while rented, or rented while sold") {
    Motorcycle moto(3, "Honda", "CB500X", "Red", 2021, 170, 7000, 40);
    CHECK(moto.rent(2) == true);
    CHECK(moto.sell() == false); // currently rented
    CHECK(moto.returnItem() == true);
    CHECK(moto.sell() == true);
    CHECK(moto.rent(1) == false); // now sold
}

TEST_CASE("Inventory sell/rent flow updates revenue correctly") {
    Inventory inv;
    inv.addVehicle(std::make_unique<Car>(1, "Toyota", "Corolla", "White", 2022, 180, 20000));
    inv.addVehicle(std::make_unique<Bicycle>(2, "Giant", "Escape 3", "Black", 2023, 30, 10));

    CHECK(inv.sellVehicle(1) == true);
    CHECK(inv.rentVehicle(2, 4) == true);
    CHECK(inv.totalRevenue() == doctest::Approx(20040.0));

    CHECK(inv.sellVehicle(1) == false); // already sold
    CHECK(inv.rentVehicle(99, 1) == false); // doesn't exist
}

TEST_CASE("Inventory search matches factory, model, and type case-insensitively") {
    Inventory inv;
    inv.addVehicle(std::make_unique<Car>(1, "Toyota", "Corolla", "White", 2022, 180, 20000));
    inv.addVehicle(std::make_unique<Bicycle>(2, "Giant", "Escape 3", "Black", 2023, 30, 10));

    CHECK(inv.search("toyota").size() == 1);
    CHECK(inv.search("BICYCLE").size() == 1);
    CHECK(inv.search("nonexistent").empty());
}

TEST_CASE("Inventory save/load round-trip preserves vehicles") {
    Inventory inv;
    inv.addVehicle(std::make_unique<Car>(1, "Toyota", "Corolla", "White", 2022, 180, 20000));
    inv.addVehicle(std::make_unique<Bicycle>(2, "Giant", "Escape 3", "Black", 2023, 30, 10));
    inv.sellVehicle(1);
    inv.saveToFile("test_inventory.json", "test_transactions.json");

    Inventory loaded;
    size_t count = loaded.loadFromFile("test_inventory.json");
    CHECK(count == 2);
    CHECK(loaded.findById(1) != nullptr);
    CHECK(dynamic_cast<Saleable*>(loaded.findById(1))->isSold() == true);
}
