#ifndef OOP_EMERGENCY_H
#define OOP_EMERGENCY_H
#include "../dsa/emergency.h"
#include <string>

class Emergency {
private:
    EmergencyRecord record;
public:
    Emergency();
    bool create(int id, const std::string& type, const std::string& location,
                int severity, const std::string& description);
    EmergencyRecord getRecord() const;
    void setStatus(const std::string& status);
};
#endif
