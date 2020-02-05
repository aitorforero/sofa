#pragma once

#include <string.h>
#include <vector>

extern const char *HOMIE_DEVICE_HOMIE_TOPIC;
extern const char *HOMIE_DEVICE_NAME_TOPIC;
extern const char *HOMIE_DEVICE_STATE_TOPIC;
extern const char *HOMIE_DEVICE_EXTENSIONS_TOPIC;
extern const char *HOMIE_DEVICE_NODES_TOPIC;

extern const char *HOMIE_NODE_NAME_TOPIC;
extern const char *HOMIE_NODE_TYPE_TOPIC;
extern const char *HOMIE_NODE_PROPERTIES_TOPIC;

extern const char *HOMIE_PROPERTY_VALUE_TOPIC;
extern const char *HOMIE_PROPERTY_NAME_TOPIC;
extern const char *HOMIE_PROPERTY_DATATYPE_TOPIC;
extern const char *HOMIE_PROPERTY_FORMAT_TOPIC;
extern const char *HOMIE_PROPERTY_SETTABLE_TOPIC;
extern const char *HOMIE_PROPERTY_RETAINED_TOPIC;
extern const char *HOMIE_PROPERTY_UNIT_TOPIC;
extern const char *HOMIE_PROPERTY_COMMAND_TOPIC;

extern const char *HOMIE_DEVICE_STATE_INIT;
extern const char *HOMIE_DEVICE_STATE_READY;

extern const char* HOMIE_UNIT_DEGREE_CELSIUS;
extern const char* HOMIE_UNIT_DEGREE_FAHRENHEIT;
extern const char* HOMIE_UNIT_DEGREE;
extern const char* HOMIE_UNIT_LITER;
extern const char* HOMIE_UNIT_GALON;
extern const char* HOMIE_UNIT_VOLTS; 
extern const char* HOMIE_UNIT_WATT;
extern const char* HOMIE_UNIT_AMPERE;
extern const char* HOMIE_UNIT_PERCENT;
extern const char* HOMIE_UNIT_METER;
extern const char* HOMIE_UNIT_FEET; 
extern const char* HOMIE_UNIT_PASCAL;
extern const char* HOMIE_UNIT_PSI;
extern const char* HOMIE_UNIT_AMOUNT;

enum homie_datatype {
    HOMIE_DATATYPE_INTEGER, 
    HOMIE_DATATYPE_FLOAT, 
    HOMIE_DATATYPE_BOOLEAN, 
    HOMIE_DATATYPE_STRING, 
    HOMIE_DATATYPE_ENUM, 
    HOMIE_DATATYPE_COLOR
};

enum homie_state {
    HOMIE_STATE_INIT, 
    HOMIE_STATE_READY, 
    HOMIE_STATE_DISCONNECTED, 
    HOMIE_STATE_SLEEPING, 
    HOMIE_STATE_LOST, 
    HOMIE_STATE_ALERT
};

class HomieProperty{
    public:
        const char* propertyID;
        const char* name; 
        homie_datatype datatype;    
        const char* format;
        bool settable;
        bool retained;
        const char* unit;
        const char* initialValue;

        void getHomieDataType(char* dataTypeString, size_t size);
};

class HomieNode{
    public:
        const char* nodeID;
        const char* name; 
        const char* nodetype;    
        std::vector<HomieProperty> properties;

        void getProperties(char* propertiesString, size_t size);
};

class HomieDevice {
    public:
        const char* name;    
        const char* deviceID;
        const char* version; 
        std::vector<HomieNode> nodes;

        void getNodes(char* nodesString, size_t size);
        
        static void getHomieState(homie_state state, char* stateString, size_t size);
        static void getLastWill(homie_state state, char* stateString, size_t size);
        static int getHomieValues(char *topic, char *deviceID, size_t deviceID_size, char *nodeID, size_t nodeID_size, char *propertyID, size_t propertyID_size, char *command, size_t command_size);
};
