#include <string.h>
#include "sofa_homie_device.hpp"

// Global defines 
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

static const char* HOMIE_DATA_TYPE_INTEGER = "integer";
static const char* HOMIE_DATA_TYPE_FLOAT = "float";
static const char* HOMIE_DATA_TYPE_BOOLEAN = "boolean";
static const char* HOMIE_DATA_TYPE_STRING = "string";
static const char* HOMIE_DATA_TYPE_ENUM = "enum";
static const char* HOMIE_DATA_TYPE_COLOR = "color";

static const char* HOMIE_STATE_INIT_VALUE = "init";
static const char* HOMIE_STATE_READY_VALUE = "ready";
static const char* HOMIE_STATE_DISCONNECTED_VALUE = "disconnected";
static const char* HOMIE_STATE_SLEEPING_VALUE = "sleeping";
static const char* HOMIE_STATE_LOST_VALUE = "lost";
static const char* HOMIE_STATE_ALERT_VALUE = "alert";


void HomieProperty::getHomieDataType(char* dataTypeString, size_t size){
    switch (this->datatype) 
    {
    case HOMIE_DATATYPE_STRING:
        strncpy(dataTypeString, HOMIE_DATA_TYPE_STRING, size);
        break;
    case HOMIE_DATATYPE_INTEGER:
        strncpy(dataTypeString, HOMIE_DATA_TYPE_INTEGER, size);
        break;
    case HOMIE_DATATYPE_FLOAT:
        strncpy(dataTypeString, HOMIE_DATA_TYPE_FLOAT, size);
        break;
    case HOMIE_DATATYPE_BOOLEAN:
        strncpy(dataTypeString, HOMIE_DATA_TYPE_BOOLEAN, size);
        break;
    case HOMIE_DATATYPE_ENUM:
        strncpy(dataTypeString, HOMIE_DATA_TYPE_ENUM, size);
        break;
    case HOMIE_DATATYPE_COLOR:
        strncpy(dataTypeString, HOMIE_DATA_TYPE_COLOR, size);
        break;
    default:
        break;
    }    
};

void HomieNode::getProperties(char* propertiesString, size_t size){
    int length = properties.size();
    int i = 0;
    for (HomieProperty property : properties) {
        strncat(propertiesString, property.propertyID, size);
        if(++i < length) {
            strncat(propertiesString, ",", size);
        }
    }    
};

void HomieDevice::getNodes(char* nodesString, size_t size){
    int length = nodes.size();
    int i = 0;
    for (HomieNode node : nodes) {
        strncat(nodesString, node.nodeID, size);
        if(++i < length) {
            strncat(nodesString, ",", size);
        }
    }
};

void HomieDevice::getHomieState(homie_state state, char* stateString, size_t size){
    switch (state)
    {
    case HOMIE_STATE_INIT:
        strncpy(stateString, HOMIE_STATE_INIT_VALUE, size);
        break;
    case HOMIE_STATE_READY:
        strncpy(stateString, HOMIE_STATE_READY_VALUE, size);
        break;  
    case HOMIE_STATE_DISCONNECTED:
        strncpy(stateString, HOMIE_STATE_DISCONNECTED_VALUE, size);
        break; 
    case HOMIE_STATE_SLEEPING:
        strncpy(stateString, HOMIE_STATE_SLEEPING_VALUE, size);
        break;      
    case HOMIE_STATE_LOST:
        strncpy(stateString, HOMIE_STATE_LOST_VALUE, size);
        break;
    case HOMIE_STATE_ALERT:
        strncpy(stateString, HOMIE_STATE_ALERT_VALUE, size);
        break;
    default:
        break;
    }
};

int HomieDevice::getHomieValues(char *topic, char *deviceID, size_t deviceID_size, char *nodeID, size_t nodeID_size, char *propertyID, size_t propertyID_size, char *command, size_t command_size){
   const char s[2] = "/";
   char *token;
   int tokenCount = 0;
   
   token = strtok(topic, s);
   
    while( token != NULL ) {
        tokenCount++;

        switch (tokenCount)
        {
        case 2:
            strncpy(deviceID, token, deviceID_size);
            break;

        case 3:
            strncpy(nodeID, token, nodeID_size);
            break;

        case 4:
            strncpy(propertyID, token, propertyID_size);
            break;

        case 5:
            strncpy(command, token, command_size);
            break;
        
        default:
            break;
        }  
        token = strtok(NULL, s);
   }
   
    return tokenCount;
};

