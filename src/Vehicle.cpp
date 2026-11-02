#include "Vehicle.hpp"
#include <ostream>

Vehicle::Vehicle(int id, std::string factory, std::string model, std::string color, int makeYear,
                 int maxSpeed)
    : id_(id),
      factory_(std::move(factory)),
      model_(std::move(model)),
      color_(std::move(color)),
      makeYear_(makeYear),
      maxSpeed_(maxSpeed) {}

json Vehicle::toJson() const {
    return json{
        {"id", id_},       {"type", typeName()},    {"factory", factory_},   {"model", model_},
        {"color", color_}, {"makeYear", makeYear_}, {"maxSpeed", maxSpeed_},
    };
}
