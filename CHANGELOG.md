# Changelog

All notable changes to this project are documented here.
The format follows [Keep a Changelog](https://keepachangelog.com/en/1.1.0/)
and the project adheres to [Semantic Versioning](https://semver.org/).

## [2.0.0] — Architecture & feature release

### Fixed
- **Transaction history is now loaded on startup.** v1 wrote
  `transactions.json` but never read it back, so the revenue report was
  empty after every restart.
- **Decimal prices no longer truncated.** The CLI read prices with an
  integer parser (`24999.50` became `24999`).
- **Next-id collisions.** The next id was `size() + 1`; it is now
  `max(id) + 1`, so deleting vehicles can never cause a duplicate id.
- **EOF infinite loop.** Piping empty input into the old menu loop hung
  forever; the app now saves and exits cleanly.
- **State restore hack removed.** Loading a rented vehicle no longer
  "fakes" a `rent(1)` call — sold/rented state is part of the
  constructor and restored exactly.
- Save files are written atomically (`.tmp` + rename), so a crash can
  no longer leave truncated JSON behind. Corrupt files are reported and
  skipped instead of being silently ignored.

### Added
- **`Truck`** vehicle type (payload capacity in tons) — added with zero
  changes to `Inventory`, proving the interface design scales.
- **`VehicleFactory`** registry: deserialization no longer hard-codes an
  if/else chain on type names; each class owns its own `fromJson`.
- **Discount / promotion system** on `Saleable` (stateful mixin):
  `applyDiscount`, `clearDiscount`, `netPrice`; discounts are persisted
  and recorded on the sale transaction.
- **Statistics dashboard**: fleet breakdown by type and status, revenue
  split between sales and rentals.
- **Delete vehicle** (blocked while a vehicle is rented out).
- **Test-drive menu option** — the `drive()` polymorphic method finally
  has a user-facing home.
- **CSV export** of the transaction log (RFC-4180 escaping, atomic write).
- **CLI flags**: `--help`, `--version`, `--data-dir`, `--no-color` and a
  scripted `--demo` session for screenshots.
- **ANSI colors** with TTY / `NO_COLOR` auto-detection and Windows VT
  enablement.
- Input validation for years, speeds, prices, days and discount ranges.

### Changed
- Tests grew from 6 cases / 28 assertions to **24 cases / 166
  assertions**, split into `test_vehicles.cpp`, `test_inventory.cpp` and
  `test_factory.cpp`.
- CI now builds on **Linux, macOS and Windows** (Release), runs the
  suite under **ASan + UBSan**, and enforces **clang-format**.
- `nlohmann/json` is fetched from the official release tarball with a
  SHA-256 pin; `doctest` is fetched shallow for faster CI.
- Strict compiler warnings (`-Wall -Wextra -Wpedantic` / `/W4`) on every
  target; the codebase is warning-clean.

## [1.0.0] — Initial public version

- Car / Bicycle / Motorcycle hierarchy with `Saleable` / `Rentable`
  interfaces, RAII inventory, JSON persistence, interactive CLI,
  doctest suite and a basic GitHub Actions build.
