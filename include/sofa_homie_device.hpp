#pragma once

#include <string.h>

const char *HOMIE_DEVICE_SUBSCRIBE_TOPIC = "%s/%s/#";
const char *HOMIE_DEVICE_HOMIE_TOPIC = "%s/%s/$homie";
const char *HOMIE_DEVICE_NAME_TOPIC = "%s/%s/$name";
const char *HOMIE_DEVICE_STATE_TOPIC = "%s/%s/$state";
const char *HOMIE_DEVICE_EXTENSIONS_TOPIC = "%s/%s/$extensions";
const char *HOMIE_DEVICE_NODES_TOPIC = "%s/%s/$nodes";

const char *HOMIE_NODE_NAME_TOPIC = "%s/%s/%s/$name";
const char *HOMIE_NODE_TYPE_TOPIC = "%s/%s/%s/$type";
const char *HOMIE_NODE_PROPERTIES_TOPIC = "%s/%s/%s/$properties";

const char *HOMIE_PROPERTY_VALUE_TOPIC = "%s/%s/%s/%s";
const char *HOMIE_PROPERTY_NAME_TOPIC = "%s/%s/%s/%s/$name";
const char *HOMIE_PROPERTY_DATATYPE_TOPIC = "%s/%s/%s/%s/$datatype";
const char *HOMIE_PROPERTY_FORMAT_TOPIC = "%s/%s/%s/%s/$format";
const char *HOMIE_PROPERTY_SETTABLE_TOPIC = "%s/%s/%s/%s/$settable";
const char *HOMIE_PROPERTY_RETAINED_TOPIC = "%s/%s/%s/%s/$retained";
const char *HOMIE_PROPERTY_UNIT_TOPIC = "%s/%s/%s/%s/$unit";

const char *HOMIE_DEVICE_STATE_INIT = "init";
const char *HOMIE_DEVICE_STATE_READY = "ready";

const char* HOMIE_UNIT_DEGREE_CELSIUS = "°C";
const char* HOMIE_UNIT_DEGREE_FAHRENHEIT = "°F";
const char* HOMIE_UNIT_DEGREE = "°";
const char* HOMIE_UNIT_LITER = "L";
const char* HOMIE_UNIT_GALON = "gal";
const char* HOMIE_UNIT_VOLTS = "V"; 
const char* HOMIE_UNIT_WATT = "W";
const char* HOMIE_UNIT_AMPERE = "A";
const char* HOMIE_UNIT_PERCENT = "%";
const char* HOMIE_UNIT_METER = "m";
const char* HOMIE_UNIT_FEET = "ft"; 
const char* HOMIE_UNIT_PASCAL = "Pa";
const char* HOMIE_UNIT_PSI = "psi";
const char* HOMIE_UNIT_AMOUNT = "#";

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
        HomieProperty* properties;

        void getProperties(char* propertiesString, size_t size);
};

class HomieDevice {
    public:
        const char* name;    
        const char* deviceID;
        const char* version; 
        HomieNode* nodes;

        void getNodes(char* nodesString, size_t size);
        
        static void getHomieState(homie_state state, char* stateString, size_t size);
        static int getHomieValues(char *topic, char *deviceID, size_t deviceID_size, char *nodeID, size_t nodeID_size, char *propertyID, size_t propertyID_size, char *command, size_t command_size);
};