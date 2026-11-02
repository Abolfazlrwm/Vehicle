#pragma once
#include <functional>
#include <memory>
#include <string>
#include <unordered_map>
#include "Vehicle.hpp"

using json = nlohmann::json;

/**
 * @brief Factory + registry that turns JSON records back into concrete
 * Vehicle objects, without a single if/else chain on type names.
 *
 * Every concrete vehicle exposes a `static std::unique_ptr<Vehicle>
 * fromJson(const json&)` constructor and registers itself in the
 * factory's constructor. Deserialization, the CLI and the tests all go
 * through this one place — adding a new vehicle type means writing the
 * class, adding ONE registration line, and touching nothing else.
 */
using VehicleCreator = std::function<std::unique_ptr<Vehicle>(const json&)>;

class VehicleFactory {
public:
    /// Singleton access (the registry is process-global by nature).
    static VehicleFactory& instance();

    /// Adds a new type. Overwrites an existing registration with the
    /// same name — handy for tests that plug in mock types.
    void registerType(std::string typeName, VehicleCreator creator);

    /// Creates a vehicle from a JSON record.
    /// Returns nullptr if the "type" field is missing or unknown.
    std::unique_ptr<Vehicle> create(const json& data) const;

    /// Number of registered types (exposed so tests can assert on it).
    std::size_t typeCount() const { return creators_.size(); }

private:
    VehicleFactory();  // registers all built-in types

    std::unordered_map<std::string, VehicleCreator> creators_;
};
