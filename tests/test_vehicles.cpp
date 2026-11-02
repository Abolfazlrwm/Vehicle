#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>
#include <sstream>

#include "Bicycle.hpp"
#include "Car.hpp"
#include "Cli.hpp"
#include "Motorcycle.hpp"
#include "Truck.hpp"

// Ensure deterministic (uncolored) output inside these tests.
struct DisableColors {
    DisableColors() { cli::setColorEnabled(false); }
};

TEST_CASE("Car can be sold exactly once") {
    DisableColors guard;
    Car car(1, "Toyota", "Corolla", "White", 2022, 180, 20000);
    CHECK(car.isSold() == false);
    CHECK(car.sell() == true);
    CHECK(car.isSold() == true);
    CHECK(car.sell() == false);  // already sold
}

TEST_CASE("Car discount maths and rules") {
    DisableColors guard;
    Car car(1, "Toyota", "Corolla", "White", 2022, 180, 20000);

    CHECK(car.discountPercent() == doctest::Approx(0.0));
    CHECK(car.netPrice() == doctest::Approx(20000.0));

    CHECK(car.applyDiscount(10.0) == true);
    CHECK(car.discountPercent() == doctest::Approx(10.0));
    CHECK(car.netPrice() == doctest::Approx(18000.0));

    CHECK(car.applyDiscount(-1.0) == false);                // negative
    CHECK(car.applyDiscount(100.0) == false);               // must stay below 100
    CHECK(car.applyDiscount(150.0) == false);               // out of range
    CHECK(car.discountPercent() == doctest::Approx(10.0));  // unchanged

    CHECK(car.applyDiscount(0.0) == true);  // clearing via 0 is legal
    CHECK(car.applyDiscount(25.0) == true);
    CHECK(car.clearDiscount() == true);
    CHECK(car.discountPercent() == doctest::Approx(0.0));

    car.sell();
    CHECK(car.applyDiscount(10.0) == false);  // sold cars cannot be discounted
    CHECK(car.clearDiscount() == false);
}

TEST_CASE("Bicycle rent/return cycle") {
    DisableColors guard;
    Bicycle bike(2, "Giant", "Escape 3", "Black", 2023, 30, 10);
    CHECK(bike.isRented() == false);
    CHECK(bike.rent(3) == true);
    CHECK(bike.isRented() == true);
    CHECK(bike.rent(2) == false);   // already rented
    CHECK(bike.rent(0) == false);   // invalid length
    CHECK(bike.rent(-5) == false);  // negative length
    CHECK(bike.calculateRentalCost(3) == doctest::Approx(30.0));
    CHECK(bike.calculateRentalCost(0) == doctest::Approx(0.0));
    CHECK(bike.returnItem() == true);
    CHECK(bike.isRented() == false);
    CHECK(bike.returnItem() == false);  // wasn't rented
}

TEST_CASE("Motorcycle cannot be sold while rented, or rented while sold") {
    DisableColors guard;
    Motorcycle moto(3, "Honda", "CB500X", "Red", 2021, 170, 7000, 40);
    CHECK(moto.rent(2) == true);
    CHECK(moto.sell() == false);  // currently rented
    CHECK(moto.returnItem() == true);
    CHECK(moto.sell() == true);
    CHECK(moto.rent(1) == false);  // now sold
}

TEST_CASE("Motorcycle constructor never allows sold AND rented at once") {
    DisableColors guard;
    Motorcycle both(3, "Honda", "CB500X", "Red", 2021, 170, 7000, 40, true, true);
    CHECK(both.isSold() == true);
    CHECK(both.isRented() == false);  // sold wins
}

TEST_CASE("Truck behaviour and pricing") {
    DisableColors guard;
    Truck truck(4, "Volvo", "FH16", "Blue", 2023, 140, 118000, 25);
    CHECK(truck.typeName() == "Truck");
    CHECK(truck.cargoCapacityTons() == doctest::Approx(25.0));
    CHECK(truck.listPrice() == doctest::Approx(118000.0));

    CHECK(truck.applyDiscount(5.0) == true);  // discount BEFORE selling
    CHECK(truck.netPrice() == doctest::Approx(118000.0 * 0.95));
    CHECK(truck.sell() == true);
    CHECK(truck.sell() == false);
    CHECK(truck.applyDiscount(10.0) == false);                    // sold: no new discounts
    CHECK(truck.netPrice() == doctest::Approx(118000.0 * 0.95));  // sold price keeps its discount
}

TEST_CASE("showInfo prints a parseable one-line description") {
    DisableColors guard;
    Car car(7, "Toyota", "Corolla", "White", 2022, 180, 21000);
    car.applyDiscount(10.0);

    std::ostringstream os;
    car.showInfo(os);

    const std::string text = os.str();
    CHECK(text.find("[Car #7]") != std::string::npos);
    CHECK(text.find("Toyota Corolla") != std::string::npos);
    CHECK(text.find("(2022, White)") != std::string::npos);
    CHECK(text.find("180 km/h") != std::string::npos);
    CHECK(text.find("$21,000") != std::string::npos);
    CHECK(text.find("10% off: $18,900") != std::string::npos);
    CHECK(text.find("[AVAILABLE]") != std::string::npos);
}

TEST_CASE("cli::formatMoney groups thousands and shows cents only when needed") {
    DisableColors guard;
    CHECK(cli::formatMoney(0) == "$0");
    CHECK(cli::formatMoney(9) == "$9");
    CHECK(cli::formatMoney(21000) == "$21,000");
    CHECK(cli::formatMoney(1234567) == "$1,234,567");
    CHECK(cli::formatMoney(1899.5) == "$1,899.50");
    CHECK(cli::formatMoney(0.99) == "$0.99");
    CHECK(cli::formatMoney(-42) == "-$42");
    CHECK(cli::formatMoney(10.005) == "$10.01");  // rounds half up
}

TEST_CASE("cli::statusTag maps statuses to fixed text when colors are off") {
    DisableColors guard;
    CHECK(cli::statusTag("AVAILABLE") == "[AVAILABLE]");
    CHECK(cli::statusTag("SOLD") == "[SOLD]");
    CHECK(cli::statusTag("RENTED") == "[RENTED]");
}
