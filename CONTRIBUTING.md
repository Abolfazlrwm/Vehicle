# Contributing

Thanks for considering a contribution! The project is deliberately small
and clean — it is meant as a showcase of well-engineered modern C++, so
quality matters more than quantity.

## Getting started

```bash
git clone https://github.com/Abolfazlrwm/Vehicle.git
cd Vehicle
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build --parallel
./build/vehicle_tests
```

## Ground rules

1. **Every feature ships with tests.** Add cases to the matching file
   under `tests/` and keep the whole suite green.
2. **Formatting is enforced by CI** — run
   `cmake --build build --target format` (or `clang-format -i`) before
   committing.
3. **No raw `new` / `delete`.** Ownership belongs in `std::unique_ptr`
   inside `Inventory`.
4. **Adding a vehicle type?** Create the class with a
   `static fromJson(const json&)`, add one `registerType` call in
   `VehicleFactory`, extend the tests — and `Inventory` should need
   *zero* changes. If it does, that's a bug in the design, not in your
   patch.
5. Keep the public headers documented with Doxygen-style comments.

## Commit style

Short imperative subject lines (`Add Truck type`, `Fix discount
round-trip`) and a body explaining *why* for anything non-trivial.

## Reporting bugs

Open an issue with the command you ran, the unexpected output and the
expected behaviour. `--data-dir` output (JSON files) attached as text is
usually enough to reproduce persistence bugs.
