#include "Resource.h"

Resource::Resource(int i, const std::string& n, const std::string& t, int z)
    : id(i), name(n), type(t), zone(z), available(true) {}
int Resource::getId() const { return id; }
std::string Resource::getName() const { return name; }
std::string Resource::getType() const { return type; }
int Resource::getZone() const { return zone; }
bool Resource::isAvailable() const { return available; }
void Resource::setAvailable(bool value) { available = value; }
