#include <doctest/doctest.h>
#include <fstream>
#include <sstream>

#include "Bicycle.hpp"
#include "Car.hpp"
#include "Inventory.hpp"
#include "Motorcycle.hpp"
#include "Rentable.hpp"
#include "Saleable.hpp"
#include "Truck.hpp"

namespace {
Inventory makeSampleFleet() {
    Inventory inv;
    inv.addVehicle(std::make_unique<Car>(1, "Toyota", "Corolla", "White", 2022, 180, 20000));
    inv.addVehicle(std::make_unique<Bicycle>(2, "Giant", "Escape 3", "Black", 2023, 30, 10));
    inv.addVehicle(std::make_unique<Motorcycle>(3, "Honda", "CB500X", "Red", 2021, 170, 7000, 40));
    inv.addVehicle(std::make_unique<Truck>(4, "Volvo", "FH16", "Blue", 2023, 140, 118000, 25));
    return inv;
}
}  // namespace

TEST_CASE("Inventory sell/rent flow updates revenue correctly") {
    Inventory inv = makeSampleFleet();

    CHECK(inv.sellVehicle(1) == true);
    CHECK(inv.rentVehicle(2, 4) == true);
    CHECK(inv.totalRevenue() == doctest::Approx(20040.0));

    CHECK(inv.sellVehicle(1) == false);      // already sold
    CHECK(inv.sellVehicle(99) == false);     // doesn't exist
    CHECK(inv.sellVehicle(2) == false);      // bicycle is not sellable
    CHECK(inv.rentVehicle(99, 1) == false);  // doesn't exist
    CHECK(inv.rentVehicle(1, 2) == false);   // car is not rentable
    CHECK(inv.rentVehicle(3, 0) == false);   // invalid length
}

TEST_CASE("Discounts flow through selling revenue") {
    Inventory inv;
    inv.addVehicle(std::make_unique<Car>(1, "Toyota", "Corolla", "White", 2022, 180, 20000));

    CHECK(inv.applyDiscount(1, 25.0) == true);
    CHECK(inv.sellVehicle(1) == true);
    CHECK(inv.totalRevenue() == doctest::Approx(15000.0));

    CHECK(inv.applyDiscount(1, 10.0) == false);  // already sold
    CHECK(inv.clearDiscount(1) == false);        // already sold
}

TEST_CASE("applyDiscount rejects non-sellable and unknown vehicles") {
    Inventory inv = makeSampleFleet();
    CHECK(inv.applyDiscount(2, 10.0) == false);   // bicycle: not sellable
    CHECK(inv.applyDiscount(99, 10.0) == false);  // unknown id
    CHECK(inv.clearDiscount(2) == false);
}

TEST_CASE("Search matches factory, model, color, type and id, case-insensitively") {
    Inventory inv = makeSampleFleet();

    CHECK(inv.search("toyota").size() == 1);
    CHECK(inv.search("BICYCLE").size() == 1);
    CHECK(inv.search("white").size() == 1);  // color match
    CHECK(inv.search("4").size() == 1);      // id match
    CHECK(inv.search("honda cb500x").size() == 1);
    CHECK(inv.search("nonexistent").empty());
}

TEST_CASE("removeVehicle deletes, but refuses to delete rented vehicles") {
    Inventory inv = makeSampleFleet();

    inv.rentVehicle(2, 1);
    CHECK(inv.removeVehicle(2) == false);  // currently rented out
    CHECK(inv.findById(2) != nullptr);

    inv.returnVehicle(2);
    CHECK(inv.removeVehicle(2) == true);
    CHECK(inv.findById(2) == nullptr);
    CHECK(inv.size() == 3);

    CHECK(inv.removeVehicle(42) == false);  // unknown id
}

TEST_CASE("nextId is max(existing ids) + 1, so sparse ids never collide") {
    Inventory inv;
    inv.addVehicle(std::make_unique<Car>(5, "A", "B", "C", 2020, 100, 1000));
    inv.addVehicle(std::make_unique<Car>(12, "A", "B", "C", 2020, 100, 1000));
    CHECK(inv.nextId() == 13);

    CHECK(inv.removeVehicle(12) == true);
    CHECK(inv.nextId() == 6);
}

TEST_CASE("Statistics dashboard data is correct") {
    Inventory inv = makeSampleFleet();
    inv.sellVehicle(1);     // $20,000
    inv.rentVehicle(3, 3);  // $120
    inv.rentVehicle(2, 2);  // $20
    inv.returnVehicle(2);   // $0
    inv.sellVehicle(4);     // $118,000

    const InventoryStats s = inv.stats();
    CHECK(s.total == 4);
    CHECK(s.byType.at("Car") == 1);
    CHECK(s.byType.at("Bicycle") == 1);
    CHECK(s.byType.at("Motorcycle") == 1);
    CHECK(s.byType.at("Truck") == 1);
    CHECK(s.sold == 2);
    CHECK(s.rented == 1);
    CHECK(s.available == 1);
    CHECK(s.saleCount == 2);
    CHECK(s.rentCount == 2);
    CHECK(s.returnCount == 1);
    CHECK(s.salesRevenue == doctest::Approx(138000.0));
    CHECK(s.rentalRevenue == doctest::Approx(140.0));
    CHECK(s.totalRevenue == doctest::Approx(138140.0));
}

TEST_CASE("Revenue report renders transactions and totals") {
    Inventory inv = makeSampleFleet();
    inv.sellVehicle(1);
    inv.rentVehicle(2, 3);

    std::ostringstream os;
    inv.printRevenueReport(os);
    const std::string text = os.str();

    CHECK(text.find("SALE") != std::string::npos);
    CHECK(text.find("RENT") != std::string::npos);
    CHECK(text.find("Corolla (#1)") != std::string::npos);
    CHECK(text.find("3 days") != std::string::npos);
    CHECK(text.find("Total revenue: $20,030") != std::string::npos);
}

TEST_CASE("CSV export writes a header, one row per transaction and escapes special chars") {
    Inventory inv;
    inv.addVehicle(
        std::make_unique<Car>(1, "Acme Motors", "Model, X \"Pro\"", "White", 2022, 180, 20000));
    inv.sellVehicle(1);
    CHECK(inv.rentVehicle(1, 2) == false);  // car is not rentable — no extra row

    const std::string csv = inv.transactionsCsv();
    CHECK(csv.find("timestamp,vehicle_id,vehicle_model,type,amount,days,discount_percent") == 0);
    CHECK(csv.find("\"Model, X \"\"Pro\"\"\"") != std::string::npos);  // comma + quotes escaped
    CHECK(csv.find("SALE") != std::string::npos);
    CHECK(csv.find("RENT") == std::string::npos);

    const bool ok = inv.exportCsv("test_transactions_export.csv");
    CHECK(ok == true);
    std::ifstream file("test_transactions_export.csv");
    REQUIRE(file.is_open());
    std::stringstream buffer;
    buffer << file.rdbuf();
    CHECK(buffer.str() == csv);
    std::remove("test_transactions_export.csv");
}

TEST_CASE("Save/load round-trip preserves vehicles, states, discounts and history") {
    Inventory inv = makeSampleFleet();
    inv.applyDiscount(1, 15.0);
    inv.sellVehicle(1);     // sold with 15% discount
    inv.rentVehicle(2, 7);  // bicycle stays rented in the snapshot
    inv.sellVehicle(4);     // truck sold

    REQUIRE(inv.saveToFile("test_roundtrip_inventory.json", "test_roundtrip_transactions.json"));

    Inventory loaded;
    loaded.loadFromFile("test_roundtrip_inventory.json");
    loaded.loadTransactions("test_roundtrip_transactions.json");

    CHECK(loaded.size() == 4);
    CHECK(loaded.transactions().size() == 3);
    CHECK(loaded.totalRevenue() == doctest::Approx(inv.totalRevenue()));

    auto* car = loaded.findById(1);
    REQUIRE(car != nullptr);
    auto* carSale = dynamic_cast<Saleable*>(car);
    REQUIRE(carSale != nullptr);
    CHECK(carSale->isSold() == true);
    CHECK(carSale->discountPercent() == doctest::Approx(15.0));
    CHECK(carSale->netPrice() == doctest::Approx(17000.0));

    auto* bike = loaded.findById(2);
    REQUIRE(bike != nullptr);
    CHECK(dynamic_cast<Rentable*>(bike)->isRented() == true);

    auto* truck = loaded.findById(4);
    REQUIRE(truck != nullptr);
    CHECK(dynamic_cast<Saleable*>(truck)->isSold() == true);

    // The next id must continue where the loaded fleet left off.
    CHECK(loaded.nextId() == 5);

    // And the loaded fleet is itself usable (state machine still enforced).
    CHECK(loaded.sellVehicle(1) == false);
    CHECK(loaded.returnVehicle(2) == true);

    std::remove("test_roundtrip_inventory.json");
    std::remove("test_roundtrip_transactions.json");
}

TEST_CASE("Loading missing or corrupt files is safe, not fatal") {
    Inventory inv;
    CHECK(inv.loadFromFile("definitely_missing.json") == 0);
    CHECK(inv.loadTransactions("definitely_missing.json") == 0);
    CHECK(inv.size() == 0);

    {  // a corrupt inventory file must not throw
        std::ofstream bad("test_corrupt.json");
        bad << "{ this is not [ valid json";
    }
    CHECK(inv.loadFromFile("test_corrupt.json") == 0);
    std::remove("test_corrupt.json");
}
