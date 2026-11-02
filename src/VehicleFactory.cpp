#include "VehicleFactory.hpp"
#include "Bicycle.hpp"
#include "Car.hpp"
#include "Motorcycle.hpp"
#include "Truck.hpp"

VehicleFactory& VehicleFactory::instance() {
    static VehicleFactory factory;
    return factory;
}

VehicleFactory::VehicleFactory() {
    registerType("Car", &Car::fromJson);
    registerType("Bicycle", &Bicycle::fromJson);
    registerType("Motorcycle", &Motorcycle::fromJson);
    registerType("Truck", &Truck::fromJson);
}

void VehicleFactory::registerType(std::string typeName, VehicleCreator creator) {
    creators_[std::move(typeName)] = std::move(creator);
}

std::unique_ptr<Vehicle> VehicleFactory::create(const json& data) const {
    if (!data.is_object() || !data.contains("type"))
        return nullptr;
    const auto it = creators_.find(data.value("type", ""));
    if (it == creators_.end())
        return nullptr;
    return it->second(data);
}
