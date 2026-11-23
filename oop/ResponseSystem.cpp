#include "ResponseSystem.h"
#include "../dsa/graph.h"
#include "../dsa/dijkstra.h"
#include <algorithm>
#include <cstring>
#include <sstream>
#include <cctype>
#include <cstdio>

ResponseSystem::ResponseSystem() : nextId(1001) {
    pq_init(&queue);
    graph_init(graph);

    resources.emplace_back(1, "Ambulance 01", "Medical", 1);
    resources.emplace_back(2, "Ambulance 02", "Medical", 3);
    resources.emplace_back(3, "Fire Unit 01", "Fire", 2);
    resources.emplace_back(4, "Rescue Unit 01", "Rescue", 4);
    resources.emplace_back(5, "Police Unit 01", "Police", 1);

    hospitals.emplace_back(1, "North General Hospital", 1, 20);
    hospitals.emplace_back(2, "East City Hospital", 2, 15);
    hospitals.emplace_back(3, "South Medical Centre", 3, 18);
    hospitals.emplace_back(4, "West Care Hospital", 4, 12);
}

int ResponseSystem::zoneFromLocation(const std::string& location) const {
    std::string s = location;
    std::transform(s.begin(), s.end(), s.begin(),
                   [](unsigned char c) { return (char)std::tolower(c); });
    if (s.find("north") != std::string::npos) return 1;
    if (s.find("east") != std::string::npos) return 2;
    if (s.find("south") != std::string::npos) return 3;
    if (s.find("west") != std::string::npos) return 4;
    return 1; // Default demo zone when a free-text location has no zone keyword.
}

int ResponseSystem::hospitalIndexForZone(int zone) const {
    for (int i = 0; i < (int)hospitals.size(); ++i)
        if (hospitals[i].getZone() == zone && hospitals[i].getAvailableBeds() > 0)
            return i;
    for (int i = 0; i < (int)hospitals.size(); ++i)
        if (hospitals[i].getAvailableBeds() > 0) return i;
    return -1;
}

bool ResponseSystem::addEmergency(const std::string& type,
                                  const std::string& location,
                                  int severity, const std::string& description,
                                  int& createdId, std::string& error) {
    EmergencyRecord e{};
    createdId = nextId++;
    if (!createEmergency(&e, createdId, type.c_str(), location.c_str(),
                         severity, description.c_str())) {
        error = "Invalid emergency details. Check type, location, description and severity (1-4).";
        return false;
    }
    if (!pq_enqueue(&queue, &e)) {
        error = "Priority queue is full. Cannot add another emergency.";
        return false;
    }
    emergencies.push_back(e);
    error.clear();
    return true;
}

std::vector<EmergencyRecord> ResponseSystem::getEmergencies() const {
    std::vector<EmergencyRecord> result = emergencies;
    std::stable_sort(result.begin(), result.end(),
        [](const EmergencyRecord& a, const EmergencyRecord& b) {
            if (a.severity != b.severity) return a.severity > b.severity;
            return a.id < b.id;
        });
    return result;
}

ResponseResult ResponseSystem::assignEmergency(int emergencyId) {
    ResponseResult result{false, "Emergency not found.", 0, "", "", 0, ""};
    auto it = std::find_if(emergencies.begin(), emergencies.end(),
        [emergencyId](const EmergencyRecord& e) { return e.id == emergencyId; });
    if (it == emergencies.end()) return result;
    if (std::string(it->status) != "Pending") {
        result.message = "This emergency has already been assigned or closed.";
        return result;
    }

    /* Resource matching: medical -> ambulance, fire -> fire unit, etc.
       If the preferred type is busy, a compatible rescue unit is the backup. */
    std::string type = it->type;
    std::transform(type.begin(), type.end(), type.begin(),
                   [](unsigned char c) { return (char)std::tolower(c); });
    std::string preferred = "Medical";
    if (type.find("fire") != std::string::npos) preferred = "Fire";
    else if (type.find("accident") != std::string::npos ||
             type.find("rescue") != std::string::npos) preferred = "Rescue";
    else if (type.find("crime") != std::string::npos ||
             type.find("police") != std::string::npos) preferred = "Police";

    int resourceIndex = -1;
    for (int i = 0; i < (int)resources.size(); ++i) {
        if (resources[i].isAvailable() && resources[i].getType() == preferred) {
            resourceIndex = i; break;
        }
    }
    if (resourceIndex == -1) {
        for (int i = 0; i < (int)resources.size(); ++i) {
            if (resources[i].isAvailable() &&
                (resources[i].getType() == "Rescue" ||
                 resources[i].getType() == "Medical")) {
                resourceIndex = i; break;
            }
        }
    }
    if (resourceIndex == -1) {
        result.message = "No suitable resource is currently available. Emergency remains pending.";
        return result;
    }

    int zone = zoneFromLocation(it->location);
    int hospitalIndex = hospitalIndexForZone(zone);
    if (hospitalIndex == -1) {
        result.message = "No hospital has available capacity. Emergency remains pending.";
        return result;
    }

    int path[GRAPH_VERTICES], pathLength = 0, distance = 0;
    if (!dijkstra(graph, 0, zone, &distance, path, &pathLength)) {
        result.message = "No route is available in the demo road network.";
        return result;
    }

    resources[resourceIndex].setAvailable(false);
    hospitals[hospitalIndex].admitPatient();
    std::snprintf(it->status, sizeof(it->status), "%s", "Assigned");

    std::ostringstream route;
    for (int i = 0; i < pathLength; ++i) {
        if (i) route << " -> ";
        route << graph_vertex_name(path[i]);
    }

    result.success = true;
    result.message = "Resource assigned successfully.";
    result.resourceId = resources[resourceIndex].getId();
    result.resourceName = resources[resourceIndex].getName();
    result.hospitalName = hospitals[hospitalIndex].getName();
    result.distanceKm = distance;
    result.route = route.str();
    return result;
}

std::string ResponseSystem::getResourcesJson() const {
    std::ostringstream out;
    out << "[";
    for (size_t i = 0; i < resources.size(); ++i) {
        if (i) out << ",";
        out << "{\"id\":" << resources[i].getId()
            << ",\"name\":\"" << resources[i].getName()
            << "\",\"type\":\"" << resources[i].getType()
            << "\",\"zone\":" << resources[i].getZone()
            << ",\"available\":" << (resources[i].isAvailable() ? "true" : "false")
            << "}";
    }
    out << "]";
    return out.str();
}

