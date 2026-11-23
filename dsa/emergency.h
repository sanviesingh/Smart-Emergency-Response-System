#ifndef EMERGENCY_H
#define EMERGENCY_H

#define MAX_TYPE 50
#define MAX_LOCATION 100
#define MAX_DESCRIPTION 200
#define MAX_STATUS 30

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    int id;
    char type[MAX_TYPE];
    char location[MAX_LOCATION];
    int severity; /* 4=Critical, 3=High, 2=Medium, 1=Low */
    char description[MAX_DESCRIPTION];
    char status[MAX_STATUS];
} EmergencyRecord;

int createEmergency(EmergencyRecord *emergency, int id, const char *type,
                    const char *location, int severity, const char *description);
void displayEmergency(const EmergencyRecord *emergency);
const char *getSeverityName(int severity);

#ifdef __cplusplus
}
#endif
#endif
