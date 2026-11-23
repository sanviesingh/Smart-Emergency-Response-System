#include <stdio.h>
#include <string.h>
#include "emergency.h"

int createEmergency(EmergencyRecord *e, int id, const char *type,
                    const char *location, int severity, const char *description) {
    if (!e || !type || !location || !description) return 0;
    if (id <= 0 || severity < 1 || severity > 4) return 0;
    if (!type[0] || !location[0]) return 0;
    if (strlen(type) >= MAX_TYPE || strlen(location) >= MAX_LOCATION ||
        strlen(description) >= MAX_DESCRIPTION) return 0;

    e->id = id;
    e->severity = severity;
    snprintf(e->type, sizeof(e->type), "%s", type);
    snprintf(e->location, sizeof(e->location), "%s", location);
    snprintf(e->description, sizeof(e->description), "%s", description);
    snprintf(e->status, sizeof(e->status), "%s", "Pending");
    return 1;
}

const char *getSeverityName(int severity) {
    switch (severity) {
        case 4: return "Critical";
        case 3: return "High";
        case 2: return "Medium";
        case 1: return "Low";
        default: return "Invalid";
    }
}

void displayEmergency(const EmergencyRecord *e) {
    if (!e) return;
    printf("\nEmergency #%d | %s | %s | %s | %s\n",
           e->id, e->type, e->location, getSeverityName(e->severity), e->status);
}
