#ifndef __SENSOR_H__
#define __SENSOR_H__

#include <iostream>
#include <string>
#include <map>
#include <unordered_map>
#include <set>
#include <optional>
#include "operator_types.h"
#include "global_properties.h"

#include "../../parser_json/src/packet_parser.h"

class BasicCondition;

class Sensor {
public:
    int id;
    std::string name;
    PacketParser *parser;
    std::map<std::string, Field> fieldsMap;

    // Contains the current values of various fields and a list of basic conditions associated with each field
    std::map<std::string, std::pair<void *, std::vector<BasicCondition *>>> fields;

    // C-tor initializes the id member variable.
    Sensor(int id, string name) : id(id), name(name)
    {
        parser = new PacketParser("./sensors_data/" + name + ".json");
        std::vector<Field> tempFields = parser->getFields();
        // cout << "tempFields:" << endl;
        for (auto field : tempFields) {
            cout << field.name << " : " << field.type << endl;
            fieldsMap[field.name] = field;
        }
    }

    // Updates the condition status according to the received field and returns the  list of the full conditions whose root is true
    void updateTrueRoots(std::string field, void *value, FieldType type);

    void handleMessage(void *msg);

private:
    template <typename T>
    bool applyComparison(T a, T b, OperatorTypes op);
};

#endif  // _SENSOR_H_