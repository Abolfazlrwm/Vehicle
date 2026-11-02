#include <algorithm>
#include <deque>
#include <filesystem>
#include <iostream>
#include <limits>
#include <optional>
#include <string>
#include <vector>

#ifdef _WIN32
#include <windows.h>
#endif

#include "Bicycle.hpp"
#include "Car.hpp"
#include "Cli.hpp"
#include "Inventory.hpp"
#include "Motorcycle.hpp"
#include "Rentable.hpp"
#include "Saleable.hpp"
#include "Truck.hpp"

#ifndef APP_VERSION
#define APP_VERSION "dev"
#endif

namespace {

namespace fs = std::filesystem;

fs::path g_dataDir = "data";

fs::path inventoryFilePath() {
    return g_dataDir / "inventory.json";
}
fs::path transactionsFilePath() {
    return g_dataDir / "transactions.json";
}
fs::path csvFilePath() {
    return g_dataDir / "transactions.csv";
}

// ------------------------------------------------------------- input I/O

/**
 * @brief Console input helper with two modes: interactive (std::cin) and
 * scripted (--demo replays a fixed sequence, used for README screenshots).
 * On EOF the optionals come back empty and the caller shuts down cleanly
 * instead of spinning in an infinite loop.
 */
class Input {
public:
    static Input& instance() {
        static Input input;
        return input;
    }

    void startDemo(std::deque<std::string> script) {
        demo_ = true;
        script_ = std::move(script);
    }
    bool isDemo() const { return demo_; }

    std::optional<std::string> readLine(const std::string& prompt) {
        std::cout << prompt;
        if (demo_)
            return nextScripted();
        std::string line;
        if (!std::getline(std::cin, line))
            return std::nullopt;
        return line;
    }

    std::optional<int> readInt(const std::string& prompt) {
        while (true) {
            auto line = readLine(prompt);
            if (!line)
                return std::nullopt;
            const std::string trimmed = trim(*line);
            try {
                std::size_t consumed = 0;
                const int value = std::stoi(trimmed, &consumed);
                if (consumed == trimmed.size() && !trimmed.empty())
                    return value;
            } catch (const std::exception&) {
                // fall through to the error message
            }
            if (demo_)
                return std::nullopt;  // a broken script must not loop
            std::cout << cli::paint("Please enter a whole number.\n", cli::style::kRed);
        }
    }

    std::optional<double> readDouble(const std::string& prompt) {
        while (true) {
            auto line = readLine(prompt);
            if (!line)
                return std::nullopt;
            const std::string trimmed = trim(*line);
            try {
                std::size_t consumed = 0;
                const double value = std::stod(trimmed, &consumed);
                if (consumed == trimmed.size() && !trimmed.empty())
                    return value;
            } catch (const std::exception&) {
                // fall through to the error message
            }
            if (demo_)
                return std::nullopt;
            std::cout << cli::paint("Please enter a number.\n", cli::style::kRed);
        }
    }

private:
    Input() = default;

    static std::string trim(const std::string& s) {
        const auto first = s.find_first_not_of(" \t\r\n");
        if (first == std::string::npos)
            return "";
        const auto last = s.find_last_not_of(" \t\r\n");
        return s.substr(first, last - first + 1);
    }

    std::optional<std::string> nextScripted() {
        if (script_.empty())
            return std::nullopt;
        std::string line = script_.front();
        script_.pop_front();
        std::cout << line << "\n";  // echo so demo output looks like a real session
        return line;
    }

    bool demo_ = false;
    std::deque<std::string> script_;
};

// -------------------------------------------------------------- printing

void printBanner() {
    const std::string line(58, '=');
    std::cout << cli::paint(line, cli::style::kCyan) << "\n"
              << cli::paint("   VEHICLE  RENTAL  &  SALES  SYSTEM   v" APP_VERSION,
                            cli::style::kBold)
              << "\n"
              << cli::paint("      C++17  |  RAII  |  JSON persistence", cli::style::kCyan) << "\n"
              << cli::paint(line, cli::style::kCyan) << "\n";
}

void printMenu() {
    std::cout << "\n"
              << cli::paint("Menu", cli::style::kCyan) << "\n"
              << " 1. List all vehicles\n"
              << " 2. Add a vehicle\n"
              << " 3. Search vehicles\n"
              << " 4. Sell a vehicle\n"
              << " 5. Rent a vehicle\n"
              << " 6. Return a rented vehicle\n"
              << " 7. Delete a vehicle\n"
              << " 8. Apply a discount to a vehicle\n"
              << " 9. Test-drive a vehicle\n"
              << "10. Statistics dashboard\n"
              << "11. Revenue report & transaction history\n"
              << "12. Export transactions to CSV\n"
              << cli::paint(" 0. Save & exit", cli::style::kBold) << "\n";
}

void printUsage() {
    std::cout << "Vehicle Rental & Sales System v" APP_VERSION "\n\n"
              << "Usage: vehicle_app [options]\n"
              << "  --help          Show this help and exit\n"
              << "  --version       Print version and exit\n"
              << "  --data-dir DIR  Where to store inventory.json / transactions.json\n"
              << "                  (default: ./data)\n"
              << "  --no-color      Disable ANSI colors\n"
              << "  --demo          Run a scripted demo session (nothing is saved)\n";
}

void printStatistics(const Inventory& inv) {
    const InventoryStats s = inv.stats();

    std::cout << "\n"
              << cli::paint("======== Statistics Dashboard ========", cli::style::kCyan) << "\n"
              << "Fleet size:        " << s.total << "\n"
              << "  By type:         ";
    for (const auto& [type, count] : s.byType) std::cout << type << " x" << count << "  ";
    std::cout << "\n"
              << "  Available:       " << s.available << "\n"
              << "  Sold:            " << s.sold << "\n"
              << "  Rented:          " << s.rented << "\n"
              << "Transactions:      " << (s.saleCount + s.rentCount + s.returnCount)
              << "  (sales: " << s.saleCount << ", rents: " << s.rentCount
              << ", returns: " << s.returnCount << ")\n"
              << "Sales revenue:     " << cli::formatMoney(s.salesRevenue) << "\n"
              << "Rental revenue:    " << cli::formatMoney(s.rentalRevenue) << "\n"
              << cli::paint("Total revenue:     " + cli::formatMoney(s.totalRevenue),
                            cli::style::kBold)
              << "\n"
              << "=======================================\n";
}

// ------------------------------------------------------------- UI flows

void addVehicleFlow(Inventory& inv) {
    auto* in = &Input::instance();

    const auto typeChoice = in->readInt("Type (1=Car, 2=Bicycle, 3=Motorcycle, 4=Truck): ");
    if (!typeChoice)
        return;
    const int type = *typeChoice;

    const auto factory = in->readLine("Factory: ");
    if (!factory)
        return;
    const auto model = in->readLine("Model: ");
    if (!model)
        return;
    const auto color = in->readLine("Color: ");
    if (!color)
        return;
    const auto year = in->readInt("Make year (1900-2030): ");
    if (!year || *year < 1900 || *year > 2030) {
        std::cout << cli::paint("Invalid year, cancelled.\n", cli::style::kRed);
        return;
    }
    const auto speed = in->readInt("Max speed (km/h): ");
    if (!speed || *speed < 0) {
        std::cout << cli::paint("Invalid speed, cancelled.\n", cli::style::kRed);
        return;
    }

    const int id = inv.nextId();
    switch (type) {
        case 1: {
            const auto price = in->readDouble("Price: ");
            if (!price || *price < 0) {
                std::cout << cli::paint("Invalid price, cancelled.\n", cli::style::kRed);
                return;
            }
            inv.addVehicle(
                std::make_unique<Car>(id, *factory, *model, *color, *year, *speed, *price));
            break;
        }
        case 2: {
            const auto rent = in->readDouble("Rent price per day: ");
            if (!rent || *rent < 0) {
                std::cout << cli::paint("Invalid price, cancelled.\n", cli::style::kRed);
                return;
            }
            inv.addVehicle(
                std::make_unique<Bicycle>(id, *factory, *model, *color, *year, *speed, *rent));
            break;
        }
        case 3: {
            const auto price = in->readDouble("Price: ");
            if (!price || *price < 0) {
                std::cout << cli::paint("Invalid price, cancelled.\n", cli::style::kRed);
                return;
            }
            const auto rent = in->readDouble("Rent price per day: ");
            if (!rent || *rent < 0) {
                std::cout << cli::paint("Invalid price, cancelled.\n", cli::style::kRed);
                return;
            }
            inv.addVehicle(std::make_unique<Motorcycle>(id, *factory, *model, *color, *year, *speed,
                                                        *price, *rent));
            break;
        }
        case 4: {
            const auto price = in->readDouble("Price: ");
            if (!price || *price < 0) {
                std::cout << cli::paint("Invalid price, cancelled.\n", cli::style::kRed);
                return;
            }
            const auto cargo = in->readDouble("Cargo capacity (tons): ");
            if (!cargo || *cargo < 0) {
                std::cout << cli::paint("Invalid capacity, cancelled.\n", cli::style::kRed);
                return;
            }
            inv.addVehicle(std::make_unique<Truck>(id, *factory, *model, *color, *year, *speed,
                                                   *price, *cargo));
            break;
        }
        default:
            std::cout << cli::paint("Unknown type, cancelled.\n", cli::style::kRed);
            return;
    }
    std::cout << cli::paint("Added vehicle #" + std::to_string(id) + ".", cli::style::kGreen)
              << "\n";
}

void searchFlow(const Inventory& inv) {
    auto keyword =
        Input::instance().readLine("Search keyword (factory, model, color, type or id): ");
    if (!keyword)
        return;
    const auto results = inv.search(*keyword);
    if (results.empty()) {
        std::cout << "No matches.\n";
        return;
    }
    std::cout << results.size() << " match(es):\n";
    for (auto* v : results) v->showInfo(std::cout);
}

void sellFlow(Inventory& inv) {
    const auto id = Input::instance().readInt("Vehicle ID to sell: ");
    if (!id)
        return;
    if (inv.sellVehicle(*id)) {
        if (auto* v = inv.findById(*id)) {
            if (auto* s = dynamic_cast<Saleable*>(v)) {
                std::cout << cli::paint("Sold for " + cli::formatMoney(s->netPrice()) + ".",
                                        cli::style::kGreen)
                          << "\n";
            }
        }
    } else {
        std::cout << cli::paint("Could not sell (not found, not sellable, or already sold/rented).",
                                cli::style::kRed)
                  << "\n";
    }
}

void rentFlow(Inventory& inv) {
    auto* in = &Input::instance();
    const auto id = in->readInt("Vehicle ID to rent: ");
    if (!id)
        return;
    const auto days = in->readInt("Days: ");
    if (!days)
        return;
    if (inv.rentVehicle(*id, *days)) {
        std::cout << cli::paint("Rented for " + std::to_string(*days) + " day(s).",
                                cli::style::kGreen)
                  << "\n";
    } else {
        std::cout << cli::paint(
                         "Could not rent (not found, not rentable, bad days, or unavailable).",
                         cli::style::kRed)
                  << "\n";
    }
}

void returnFlow(Inventory& inv) {
    const auto id = Input::instance().readInt("Vehicle ID to return: ");
    if (!id)
        return;
    std::cout << (inv.returnVehicle(*id) ? "Returned.\n"
                                         : "Could not return (not found or wasn't rented).\n");
}

void deleteFlow(Inventory& inv) {
    const auto id = Input::instance().readInt("Vehicle ID to delete: ");
    if (!id)
        return;
    std::cout << (inv.removeVehicle(*id)
                      ? "Deleted.\n"
                      : "Could not delete (not found or currently rented out).\n");
}

void discountFlow(Inventory& inv) {
    auto* in = &Input::instance();
    const auto id = in->readInt("Vehicle ID to discount: ");
    if (!id)
        return;
    const auto pct = in->readDouble("Discount percent (0-99): ");
    if (!pct)
        return;
    if (inv.applyDiscount(*id, *pct)) {
        std::cout << cli::paint("Discount applied.", cli::style::kGreen) << "\n";
    } else {
        std::cout
            << cli::paint(
                   "Could not apply discount (not found, not sellable, sold, or invalid percent).",
                   cli::style::kRed)
            << "\n";
    }
}

void driveFlow(const Inventory& inv) {
    const auto id = Input::instance().readInt("Vehicle ID to test-drive: ");
    if (!id)
        return;
    if (auto* v = inv.findById(*id)) {
        std::cout << cli::paint("Test drive:", cli::style::kCyan) << " ";
        v->drive();
    } else {
        std::cout << "No vehicle with id " << *id << ".\n";
    }
}

void exportFlow(const Inventory& inv) {
    if (Input::instance().isDemo()) {
        std::cout << "(CSV export skipped in demo mode)\n";
        return;
    }
    const fs::path path = csvFilePath();
    if (inv.exportCsv(path.string())) {
        std::cout << cli::paint("Exported to " + path.string(), cli::style::kGreen) << "\n";
    } else {
        std::cout << cli::paint("Could not write " + path.string(), cli::style::kRed) << "\n";
    }
}

void seedDemoFleet(Inventory& inv) {
    inv.addVehicle(std::make_unique<Car>(1, "Toyota", "Corolla", "White", 2022, 180, 21000));
    inv.addVehicle(std::make_unique<Bicycle>(2, "Giant", "Escape 3", "Black", 2023, 30, 8));
    inv.addVehicle(std::make_unique<Motorcycle>(3, "Honda", "CB500X", "Red", 2021, 170, 7200, 45));
    inv.addVehicle(std::make_unique<Truck>(4, "Volvo", "FH16", "Blue", 2023, 140, 118000, 25));
}

}  // namespace

int main(int argc, char* argv[]) {
    bool demo = false;
    bool colorForcedOff = false;

    for (int i = 1; i < argc; ++i) {
        const std::string arg = argv[i];
        if (arg == "--help") {
            printUsage();
            return 0;
        } else if (arg == "--version") {
            std::cout << "vehicle_app " << APP_VERSION << "\n";
            return 0;
        } else if (arg == "--no-color") {
            colorForcedOff = true;
        } else if (arg == "--demo") {
            demo = true;
        } else if (arg == "--data-dir" && i + 1 < argc) {
            g_dataDir = argv[++i];
        } else {
            std::cerr << "Unknown option: " << arg << "\n\n";
            printUsage();
            return 1;
        }
    }

#ifdef _WIN32
    // Enable ANSI escape sequences in the classic Windows console.
    HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD mode = 0;
    if (GetConsoleMode(handle, &mode)) {
        SetConsoleMode(handle, mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
    }
    SetConsoleOutputCP(CP_UTF8);
#endif

    cli::detectColorSupport();
    if (colorForcedOff)
        cli::setColorEnabled(false);

    std::error_code fsError;
    fs::create_directories(g_dataDir, fsError);

    Inventory inventory;
    inventory.loadFromFile(inventoryFilePath().string());
    inventory.loadTransactions(transactionsFilePath().string());

    if (demo) {
        Input::instance().startDemo({
            "1",  // list the seeded fleet
            "2",  // add a vehicle
            "1",     "BMW",  "M4 Competition",
            "Blue",  "2024", "250",
            "78000", "8",    "1",
            "10",                  // 10% discount on car #1
            "4",     "1",          // sell car #1
            "5",     "3",    "3",  // rent motorcycle #3 for 3 days
            "9",     "2",          // test-drive bicycle #2
            "10",                  // statistics dashboard
            "11",                  // revenue report
            "12",                  // CSV export
            "0",                   // exit
        });
        std::cout << "(demo mode — a scripted session runs; nothing is saved)\n";
    }

    const bool hadSavedData = inventory.size() > 0;
    if (hadSavedData) {
        std::cout << "Loaded " << inventory.size() << " vehicle(s) and "
                  << inventory.transactions().size() << " transaction(s) from "
                  << inventoryFilePath().string() << ".\n";
    } else {
        seedDemoFleet(inventory);
        std::cout << "No saved data found — seeded a demo fleet (4 vehicles).\n";
    }

    printBanner();

    bool running = true;
    while (running) {
        printMenu();
        const auto choice = Input::instance().readInt("Choose an option: ");
        if (!choice)
            break;  // EOF / end of demo script

        switch (*choice) {
            case 1:
                inventory.listAll(std::cout);
                break;
            case 2:
                addVehicleFlow(inventory);
                break;
            case 3:
                searchFlow(inventory);
                break;
            case 4:
                sellFlow(inventory);
                break;
            case 5:
                rentFlow(inventory);
                break;
            case 6:
                returnFlow(inventory);
                break;
            case 7:
                deleteFlow(inventory);
                break;
            case 8:
                discountFlow(inventory);
                break;
            case 9:
                driveFlow(inventory);
                break;
            case 10:
                printStatistics(inventory);
                break;
            case 11:
                inventory.printRevenueReport(std::cout);
                break;
            case 12:
                exportFlow(inventory);
                break;
            case 0:
                running = false;
                break;
            default:
                std::cout << cli::paint("Invalid option.", cli::style::kRed) << "\n";
        }
    }

    if (demo) {
        std::cout << "\n(demo finished — data files were not touched)\n";
        return 0;
    }

    if (inventory.saveToFile(inventoryFilePath().string(), transactionsFilePath().string())) {
        std::cout << "Saved " << inventory.size() << " vehicle(s) and "
                  << inventory.transactions().size() << " transaction(s) to " << g_dataDir.string()
                  << ". Goodbye!\n";
    } else {
        std::cerr << "Warning: could not save data files to " << g_dataDir.string() << ".\n";
        return 1;
    }
    return 0;
}
