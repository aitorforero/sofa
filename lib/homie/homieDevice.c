#include <string.h>
#include <stdbool.h> 
#include "homieDevice.h"

static const char* HOMIE_DATA_TYPE_INTEGER = "integer";
static const char* HOMIE_DATA_TYPE_FLOAT = "float";
static const char* HOMIE_DATA_TYPE_BOOLEAN = "boolean";
static const char* HOMIE_DATA_TYPE_STRING = "string";
static const char* HOMIE_DATA_TYPE_ENUM = "enum";
static const char* HOMIE_DATA_TYPE_COLOR = "color";

static const char* HOMIE_STATE_INIT = "init";
static const char* HOMIE_STATE_READY = "ready";
static const char* HOMIE_STATE_DISCONNECTED = "disconnected";
static const char* HOMIE_STATE_SLEEPING = "sleeping";
static const char* HOMIE_STATE_LOST = "lost";
static const char* HOMIE_STATE_ALERT = "alert";

void getHomieState(homie_states state, char* stateString, size_t size){
    switch (state)
    {
    case Init:
        strncpy(stateString, HOMIE_STATE_INIT, size);
        break;
    case Ready:
        strncpy(stateString, HOMIE_STATE_READY, size);
        break;  
    case Disconnected:
        strncpy(stateString, HOMIE_STATE_DISCONNECTED, size);
        break; 
    case Sleeping:
        strncpy(stateString, HOMIE_STATE_SLEEPING, size);
        break;      
    case Lost:
        strncpy(stateString, HOMIE_STATE_LOST, size);
        break;
    case Alert:
        strncpy(stateString, HOMIE_STATE_ALERT, size);
        break;
    default:
        break;
    }
};
void getHomieDataType(homie_datatypes datatype, char* dataTypeString, size_t size){
    switch (datatype) 
    {
    case String:
        strncpy(dataTypeString, HOMIE_DATA_TYPE_STRING, size);
        break;
    case Integer:
        strncpy(dataTypeString, HOMIE_DATA_TYPE_INTEGER, size);
        break;
    case Float:
        strncpy(dataTypeString, HOMIE_DATA_TYPE_FLOAT, size);
        break;
    case Boolean:
        strncpy(dataTypeString, HOMIE_DATA_TYPE_BOOLEAN, size);
        break;
    case Enum:
        strncpy(dataTypeString, HOMIE_DATA_TYPE_ENUM, size);
        break;
    case Color:
        strncpy(dataTypeString, HOMIE_DATA_TYPE_COLOR, size);
        break;
    default:
        break;
    }
};

void getHomieDeviceNodes(homie_device *device, char* nodesString, size_t size){
    int length = sizeof(device->nodes) / sizeof(device->nodes[0]);
    int i = 0;
    for(i=0; i<length; i++) {
        strncat(nodesString, device->nodes[i].name, size);
        if(i<length-1) {
            strncat(nodesString, ",", size);
        }
    }
};

void getHomieNodeProperties(homie_node *node, char* propertiesString, size_t size){
    int length = sizeof(node->properties) / sizeof(node->properties[0]);
    int i = 0;
    for(i=0; i<length; i++) {
        strncat(propertiesString, node->properties[i].name, size);
        if(i<length-1) {
            strncat(propertiesString, ",", size);
        }
    }    
};

int getHomieValues(char *topic, char *deviceID, size_t deviceID_size, char *nodeID, size_t nodeID_size, char *propertyID, size_t propertyID_size, char *command, size_t command_size){
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

