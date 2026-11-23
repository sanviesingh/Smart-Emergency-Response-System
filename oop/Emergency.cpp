#include "Emergency.h"
#include <cstring>
#include <cstdio>

Emergency::Emergency() {
    std::memset(&record, 0, sizeof(record));
}

bool Emergency::create(int id, const std::string& type,
                       const std::string& location, int severity,
                       const std::string& description) {
    return createEmergency(&record, id, type.c_str(), location.c_str(),
                           severity, description.c_str()) == 1;
}

EmergencyRecord Emergency::getRecord() const { return record; }

void Emergency::setStatus(const std::string& status) {
    std::snprintf(record.status, sizeof(record.status), "%s", status.c_str());
}
