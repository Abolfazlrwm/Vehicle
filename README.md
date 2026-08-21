# 🚗 Vehicle Rental & Sales System

[![CI](https://github.com/Abolfazlrwm/Vehicle/actions/workflows/ci.yml/badge.svg)](https://github.com/Abolfazlrwm/Vehicle/actions/workflows/ci.yml)
![C++](https://img.shields.io/badge/C%2B%2B-17-blue.svg)
![License](https://img.shields.io/badge/license-MIT-green.svg)

A small but properly engineered C++ inventory system for a vehicle dealership
that both **sells** and **rents** vehicles — cars, bicycles, and motorcycles —
with JSON persistence, a transaction/revenue log, and a full test suite.

Started as a class exercise in OOP and multiple inheritance; rebuilt from the
ground up with a real architecture, automated tests, and CI.

## Why this project exists

Most "vehicle store" class exercises stop at printing `showInfo()` to the
console. This one goes further to demonstrate real software design:

- **Interface segregation** — `Saleable` and `Rentable` are independent
  interfaces. A `Car` only sells, a `Bicycle` only rents, and a `Motorcycle`
  does both — without any class being forced to implement behavior it
  doesn't have.
- **RAII ownership** — the fleet is a `std::vector<std::unique_ptr<Vehicle>>`,
  no raw `new`/`delete`, no manual linked list.
- **Persistence** — the entire fleet and transaction history are saved to
  JSON on exit and reloaded on the next run, via
  [nlohmann/json](https://github.com/nlohmann/json).
- **Tested** — 6 test cases / 28 assertions with
  [doctest](https://github.com/doctest/doctest), covering sell/rent state
  machines, revenue calculation, search, and save/load round-trips.
- **CI** — every push is built and tested on GitHub Actions.

## Architecture

```
Vehicle (abstract)
 ├─ Car         : Vehicle, Saleable
 ├─ Bicycle     : Vehicle, Rentable
 └─ Motorcycle  : Vehicle, Saleable, Rentable

Inventory
 ├─ owns the fleet (vector<unique_ptr<Vehicle>>)
 ├─ dynamic_cast<Saleable*> / dynamic_cast<Rentable*> to sell/rent
 │   polymorphically, regardless of concrete vehicle type
 ├─ records every sale/rent/return as a Transaction
 └─ persists both to data/*.json
```

```
include/            Public headers (Vehicle, Saleable, Rentable, Car,
                     Bicycle, Motorcycle, Transaction, Inventory)
src/                 Implementations + main.cpp (CLI)
tests/               doctest unit tests
data/                Runtime JSON storage (git-ignored)
.github/workflows/   CI pipeline
```

## Features

- Add, list, and search vehicles (by factory, model, or type)
- Sell a `Car` or `Motorcycle`
- Rent a `Bicycle` or `Motorcycle` for N days, and return it later
- Full revenue report with timestamped transaction history
- Save/load — your inventory survives between runs
- Seeds itself with 3 demo vehicles on first launch, so it's usable
  immediately with no setup

## Build & run

Requires a C++17 compiler and CMake ≥ 3.16. Dependencies
(nlohmann/json, doctest) are fetched automatically via `FetchContent` —
no manual install needed.

```bash
git clone https://github.com/Abolfazlrwm/Vehicle.git
cd Vehicle
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j4

./build/vehicle_app     # run the interactive CLI
./build/vehicle_tests   # run the test suite
```

## Example session

```
No saved data found — seeded with 3 demo vehicles.

===== Vehicle Rental & Sales System =====
1. List all vehicles
2. Add a vehicle
3. Search vehicles
4. Sell a vehicle
5. Rent a vehicle
6. Return a rented vehicle
7. Revenue report
8. Save & exit
Choose an option: 1
[Car #1] Toyota Corolla (2022, White) - 180 km/h - $21000 [AVAILABLE]
[Bicycle #2] Giant Escape 3 (2023, Black) - 30 km/h - $8/day [AVAILABLE]
[Motorcycle #3] Honda CB500X (2021, Red) - 170 km/h - $7200 to buy / $45 per day
```

## Possible next steps

- Swap the console UI for a small REST API (Crow / cpp-httplib) so the
  same `Inventory` core could power a web front end
- Add a `Truck` type to show the interface design scales to a new
  vehicle kind with zero changes to `Inventory`
- Add discount/promotion rules to `Saleable`

## License

MIT — see [LICENSE](LICENSE).
