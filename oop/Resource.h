#ifndef RESOURCE_H
#define RESOURCE_H
#include <string>

class Resource {
private:
    int id;
    std::string name;
    std::string type;
    int zone;
    bool available;
public:
    Resource(int id, const std::string& name, const std::string& type, int zone);
    int getId() const;
    std::string getName() const;
    std::string getType() const;
    int getZone() const;
    bool isAvailable() const;
    void setAvailable(bool value);
};
#endif
