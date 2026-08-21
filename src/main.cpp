#include <iostream>
#include <limits>
#include "Inventory.hpp"
#include "Car.hpp"
#include "Bicycle.hpp"
#include "Motorcycle.hpp"

namespace {

const char* INVENTORY_FILE = "data/inventory.json";
const char* TRANSACTIONS_FILE = "data/transactions.json";

void printMenu() {
    std::cout << "\n===== Vehicle Rental & Sales System =====\n"
              << "1. List all vehicles\n"
              << "2. Add a vehicle\n"
              << "3. Search vehicles\n"
              << "4. Sell a vehicle\n"
              << "5. Rent a vehicle\n"
              << "6. Return a rented vehicle\n"
              << "7. Revenue report\n"
              << "8. Save & exit\n"
              << "Choose an option: ";
}

int readInt() {
    int value;
    while (!(std::cin >> value)) {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "Please enter a number: ";
    }
    return value;
}

void addVehicleFlow(Inventory& inv, int& nextId) {
    std::cout << "Type (1=Car, 2=Bicycle, 3=Motorcycle): ";
    int type = readInt();
    std::cin.ignore();

    std::string factory, model, color;
    std::cout << "Factory: "; std::getline(std::cin, factory);
    std::cout << "Model: ";   std::getline(std::cin, model);
    std::cout << "Color: ";   std::getline(std::cin, color);
    std::cout << "Make year: "; int year = readInt();
    std::cout << "Max speed (km/h): "; int speed = readInt();

    int id = nextId++;
    if (type == 1) {
        std::cout << "Price: "; double price = readInt();
        inv.addVehicle(std::make_unique<Car>(id, factory, model, color, year, speed, price));
    } else if (type == 2) {
        std::cout << "Rent price per day: "; double rent = readInt();
        inv.addVehicle(std::make_unique<Bicycle>(id, factory, model, color, year, speed, rent));
    } else if (type == 3) {
        std::cout << "Price: "; double price = readInt();
        std::cout << "Rent price per day: "; double rent = readInt();
        inv.addVehicle(std::make_unique<Motorcycle>(id, factory, model, color, year, speed, price, rent));
    } else {
        std::cout << "Unknown type, cancelled.\n";
        --nextId;
        return;
    }
    std::cout << "Added vehicle #" << id << ".\n";
}

} // namespace

int main() {
    Inventory inventory;
    size_t loaded = inventory.loadFromFile(INVENTORY_FILE);
    int nextId = static_cast<int>(loaded) + 1;

    if (loaded == 0) {
        // Seed with a few demo vehicles on first run so the app is
        // useful out of the box.
        inventory.addVehicle(std::make_unique<Car>(1, "Toyota", "Corolla", "White", 2022, 180, 21000));
        inventory.addVehicle(std::make_unique<Bicycle>(2, "Giant", "Escape 3", "Black", 2023, 30, 8));
        inventory.addVehicle(std::make_unique<Motorcycle>(3, "Honda", "CB500X", "Red", 2021, 170, 7200, 45));
        nextId = 4;
        std::cout << "No saved data found — seeded with 3 demo vehicles.\n";
    } else {
        std::cout << "Loaded " << loaded << " vehicles from " << INVENTORY_FILE << ".\n";
    }

    bool running = true;
    while (running) {
        printMenu();
        int choice = readInt();
        switch (choice) {
            case 1:
                inventory.listAll();
                break;
            case 2:
                addVehicleFlow(inventory, nextId);
                break;
            case 3: {
                std::cin.ignore();
                std::cout << "Search keyword: ";
                std::string keyword;
                std::getline(std::cin, keyword);
                auto results = inventory.search(keyword);
                if (results.empty()) {
                    std::cout << "No matches.\n";
                } else {
                    for (auto* v : results) v->showInfo();
                }
                break;
            }
            case 4: {
                std::cout << "Vehicle ID to sell: ";
                int id = readInt();
                std::cout << (inventory.sellVehicle(id)
                                  ? "Sold.\n"
                                  : "Could not sell (not found, not sellable, or already sold).\n");
                break;
            }
            case 5: {
                std::cout << "Vehicle ID to rent: ";
                int id = readInt();
                std::cout << "Days: ";
                int days = readInt();
                std::cout << (inventory.rentVehicle(id, days)
                                  ? "Rented.\n"
                                  : "Could not rent (not found, not rentable, or unavailable).\n");
                break;
            }
            case 6: {
                std::cout << "Vehicle ID to return: ";
                int id = readInt();
                std::cout << (inventory.returnVehicle(id)
                                  ? "Returned.\n"
                                  : "Could not return (not found or wasn't rented).\n");
                break;
            }
            case 7:
                inventory.printRevenueReport();
                break;
            case 8:
                running = false;
                break;
            default:
                std::cout << "Invalid option.\n";
        }
    }

    inventory.saveToFile(INVENTORY_FILE, TRANSACTIONS_FILE);
    std::cout << "Saved " << inventory.size() << " vehicles to " << INVENTORY_FILE << ". Goodbye!\n";
    return 0;
}
