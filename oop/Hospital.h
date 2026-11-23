#ifndef HOSPITAL_H
#define HOSPITAL_H
#include <string>

class Hospital {
private:
    int id;
    std::string name;
    int zone;
    int capacity;
    int occupied;
public:
    Hospital(int id, const std::string& name, int zone, int capacity);
    int getId() const;
    std::string getName() const;
    int getZone() const;
    int getAvailableBeds() const;
    bool admitPatient();
    void releasePatient();
};
#endif
