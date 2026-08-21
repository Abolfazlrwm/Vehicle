#pragma once
#include <string>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

/**
 * @brief Abstract base class representing any vehicle in the system.
 *
 * All concrete vehicle types (Car, Bicycle, Motorcycle, ...) derive from
 * this class and must implement showInfo() and drive(). Polymorphic
 * behaviour for selling / renting is added separately via the Saleable
 * and Rentable interfaces so a vehicle only "opts in" to the
 * capabilities it actually has.
 */
class Vehicle {
public:
    Vehicle(int id, std::string factory, std::string model,
            std::string color, int makeYear, int maxSpeed);
    virtual ~Vehicle() = default;

    virtual void showInfo() const = 0;
    virtual void drive() const = 0;
    virtual std::string typeName() const = 0;

    virtual json toJson() const;

    int id() const { return id_; }
    const std::string& factory() const { return factory_; }
    const std::string& model() const { return model_; }
    const std::string& color() const { return color_; }
    int makeYear() const { return makeYear_; }
    int maxSpeed() const { return maxSpeed_; }

protected:
    int id_;
    std::string factory_;
    std::string model_;
    std::string color_;
    int makeYear_;
    int maxSpeed_;
};
