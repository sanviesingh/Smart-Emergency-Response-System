#ifndef RESPONSE_SYSTEM_H
#define RESPONSE_SYSTEM_H

#include <string>
#include <vector>
#include "../dsa/emergency.h"
#include "../dsa/priority_queue.h"
#include "../dsa/graph.h"
#include "Resource.h"
#include "Hospital.h"

struct ResponseResult {
    bool success;
    std::string message;
    int resourceId;
    std::string resourceName;
    std::string hospitalName;
    int distanceKm;
    std::string route;
};

class ResponseSystem {
private:
    std::vector<EmergencyRecord> emergencies;
    std::vector<Resource> resources;
    std::vector<Hospital> hospitals;
    PriorityQueue queue;
    int nextId;
    int graph[GRAPH_VERTICES][GRAPH_VERTICES];

    int zoneFromLocation(const std::string& location) const;
    int hospitalIndexForZone(int zone) const;
public:
    ResponseSystem();
    bool addEmergency(const std::string& type, const std::string& location,
                      int severity, const std::string& description,
                      int& createdId, std::string& error);
    std::vector<EmergencyRecord> getEmergencies() const;
    ResponseResult assignEmergency(int emergencyId);
    std::string getResourcesJson() const;
};

#endif
