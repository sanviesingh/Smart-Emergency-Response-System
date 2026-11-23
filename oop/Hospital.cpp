#include "Hospital.h"

Hospital::Hospital(int i, const std::string& n, int z, int c)
    : id(i), name(n), zone(z), capacity(c), occupied(0) {}
int Hospital::getId() const { return id; }
std::string Hospital::getName() const { return name; }
int Hospital::getZone() const { return zone; }
int Hospital::getAvailableBeds() const { return capacity - occupied; }
bool Hospital::admitPatient() {
    if (occupied >= capacity) return false;
    ++occupied;
    return true;
}
void Hospital::releasePatient() { if (occupied > 0) --occupied; }

