#include <doctest/doctest.h>

#include "Bicycle.hpp"
#include "Car.hpp"
#include "Motorcycle.hpp"
#include "Truck.hpp"
#include "VehicleFactory.hpp"

TEST_CASE("Factory creates every registered vehicle type from JSON") {
    auto& factory = VehicleFactory::instance();
    CHECK(factory.typeCount() >= 4);

    const json carJson = {
        {"id", 1},          {"type", "Car"},    {"factory", "Toyota"}, {"model", "Corolla"},
        {"color", "White"}, {"makeYear", 2022}, {"maxSpeed", 180},     {"price", 21000.0},
        {"sold", false},
    };
    auto car = factory.create(carJson);
    REQUIRE(car != nullptr);
    CHECK(car->typeName() == "Car");
    CHECK(car->model() == "Corolla");

    const json bikeJson = {
        {"id", 2},          {"type", "Bicycle"}, {"factory", "Giant"}, {"model", "Escape 3"},
        {"color", "Black"}, {"makeYear", 2023},  {"maxSpeed", 30},     {"rentPricePerDay", 8.0},
        {"rented", true},
    };
    auto bike = factory.create(bikeJson);
    REQUIRE(bike != nullptr);
    CHECK(bike->typeName() == "Bicycle");
    auto* rentable = dynamic_cast<Rentable*>(bike.get());
    REQUIRE(rentable != nullptr);
    CHECK(rentable->isRented() == true);

    const json motoJson = {
        {"id", 3},           {"type", "Motorcycle"}, {"factory", "Honda"},
        {"model", "CB500X"}, {"color", "Red"},       {"makeYear", 2021},
        {"maxSpeed", 170},   {"price", 7200.0},      {"rentPricePerDay", 45.0},
    };
    auto moto = factory.create(motoJson);
    REQUIRE(moto != nullptr);
    CHECK(moto->typeName() == "Motorcycle");

    const json truckJson = {
        {"id", 4},         {"type", "Truck"},   {"factory", "Volvo"},
        {"model", "FH16"}, {"color", "Blue"},   {"makeYear", 2023},
        {"maxSpeed", 140}, {"price", 118000.0}, {"cargoCapacityTons", 25.0},
    };
    auto truck = factory.create(truckJson);
    REQUIRE(truck != nullptr);
    CHECK(truck->typeName() == "Truck");
    CHECK(dynamic_cast<Truck*>(truck.get())->cargoCapacityTons() == doctest::Approx(25.0));
}

TEST_CASE("Factory rejects missing or unknown types") {
    auto& factory = VehicleFactory::instance();

    CHECK(factory.create(json::object()) == nullptr);               // no "type"
    CHECK(factory.create(json{{"type", "Spaceship"}}) == nullptr);  // unknown
    CHECK(factory.create(json::array()) == nullptr);                // not an object
}

TEST_CASE("toJson -> fromJson -> toJson round-trips are lossless") {
    const json motoJson = {
        {"id", 9},           {"type", "Motorcycle"}, {"factory", "Honda"},
        {"model", "CB500X"}, {"color", "Red"},       {"makeYear", 2021},
        {"maxSpeed", 170},   {"price", 7200.0},      {"rentPricePerDay", 45.0},
        {"sold", false},     {"rented", true},       {"discountPercent", 0.0},
    };
    auto moto = VehicleFactory::instance().create(motoJson);
    REQUIRE(moto != nullptr);
    CHECK(moto->toJson() == motoJson);  // nlohmann/json deep comparison
}

TEST_CASE("Round-trip keeps discounts across a save/load cycle") {
    const json carJson = {
        {"id", 2},          {"type", "Car"},           {"factory", "Toyota"}, {"model", "Corolla"},
        {"color", "White"}, {"makeYear", 2022},        {"maxSpeed", 180},     {"price", 20000.0},
        {"sold", false},    {"discountPercent", 25.0},
    };
    auto car = VehicleFactory::instance().create(carJson);
    REQUIRE(car != nullptr);
    auto* saleable = dynamic_cast<Saleable*>(car.get());
    REQUIRE(saleable != nullptr);
    CHECK(saleable->discountPercent() == doctest::Approx(25.0));
    CHECK(saleable->netPrice() == doctest::Approx(15000.0));
    CHECK(car->toJson() == carJson);
}
