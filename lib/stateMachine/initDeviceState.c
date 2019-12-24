#include "config.h"
#include "initWifiState.h"
#include "initMqttState.h"
#include "initDeviceState.h"
#include "mqtt.h"
#include "homieDevice.h"



static char topic[256];


static void publishProperty(homie_node *node, homie_property *property){
    snprintf(topic, 256, HOMIE_PROPERTY_VALUE_TOPIC, CONFIG_ROOT_TOPIC, machineDevice->deviceID, node->nodeID, property->propertyID);
    mqtt_publish(topic, property->initialValue);
    snprintf(topic, 256, HOMIE_PROPERTY_NAME_TOPIC, CONFIG_ROOT_TOPIC, machineDevice->deviceID, node->nodeID, property->propertyID);
    mqtt_publish(topic, property->name);
    char datatype[15];
    getHomieDataType(property->datatype, datatype, 15);
    snprintf(topic, 256, HOMIE_PROPERTY_DATATYPE_TOPIC, CONFIG_ROOT_TOPIC, machineDevice->deviceID, node->nodeID, property->propertyID);
    mqtt_publish(topic, datatype);
    snprintf(topic, 256, HOMIE_PROPERTY_FORMAT_TOPIC, CONFIG_ROOT_TOPIC, machineDevice->deviceID, node->nodeID, property->propertyID);
    mqtt_publish(topic, property->format);
    snprintf(topic, 256, HOMIE_PROPERTY_SETTABLE_TOPIC, CONFIG_ROOT_TOPIC, machineDevice->deviceID, node->nodeID, property->propertyID);
    if(property->settable) {
        mqtt_publish(topic, "true");
    } else {
        mqtt_publish(topic, "false");
    }
    snprintf(topic, 256, HOMIE_PROPERTY_SETTABLE_TOPIC, CONFIG_ROOT_TOPIC, machineDevice->deviceID, node->nodeID, property->propertyID);
    if(property->retained) {
        mqtt_publish(topic, "true");
    } else {
        mqtt_publish(topic, "false");
    }
    snprintf(topic, 256, HOMIE_PROPERTY_UNIT_TOPIC, CONFIG_ROOT_TOPIC, machineDevice->deviceID, node->nodeID, property->propertyID);
    mqtt_publish(topic, property->unit);
};

static void publishNode(homie_node *node){
    snprintf(topic, 256, HOMIE_NODE_NAME_TOPIC, CONFIG_ROOT_TOPIC, machineDevice->deviceID, node->nodeID);
    mqtt_publish(topic, node->name);
    snprintf(topic, 256, HOMIE_NODE_TYPE_TOPIC, CONFIG_ROOT_TOPIC, machineDevice->deviceID, node->nodeID);
    mqtt_publish(topic, node->nodetype);
    char properties[256];
    getHomieNodeProperties(node, properties, 256);
    snprintf(topic, 256, HOMIE_DEVICE_NODES_TOPIC, CONFIG_ROOT_TOPIC, machineDevice->deviceID);
    mqtt_publish(topic, properties);

    int length = sizeof(node->properties) / sizeof(node->properties[0]);
    int i = 0;
    for(i=0; i<length; i++) {
        publishProperty(node, &(node->properties[i]));
    }
};

static void publishDevice(){
    snprintf(topic, 256, HOMIE_DEVICE_HOMIE_TOPIC, CONFIG_ROOT_TOPIC, machineDevice->deviceID);
    mqtt_publish(topic, machineDevice->version);

    char state[15];
    getHomieState(Init, state, 15);
    snprintf(topic, 256, HOMIE_DEVICE_STATE_TOPIC, CONFIG_ROOT_TOPIC, machineDevice->deviceID);
    mqtt_publish(topic, state);   

    snprintf(topic, 256, HOMIE_DEVICE_NAME_TOPIC, CONFIG_ROOT_TOPIC, machineDevice->deviceID);
    mqtt_publish(topic, machineDevice->name);

    snprintf(topic, 256, HOMIE_DEVICE_EXTENSIONS_TOPIC, CONFIG_ROOT_TOPIC, machineDevice->deviceID);
    mqtt_publish(topic, "");

    char nodes[256];
    getHomieDeviceNodes(machineDevice, nodes, 256);
    snprintf(topic, 256, HOMIE_DEVICE_NODES_TOPIC, CONFIG_ROOT_TOPIC, machineDevice->deviceID);
    mqtt_publish(topic, nodes);

    int length = sizeof(machineDevice->nodes) / sizeof(machineDevice->nodes[0]);
    int i = 0;
    for(i=0; i<length; i++) {
        publishNode(&(machineDevice->nodes[i]));
    }
}


static void initDeviceState_handle(event_t event, state_t **new_state) {
    *new_state = &initDeviceState;

    switch (event)
    {
        case mqtt_conectado:
            *new_state = &initDeviceState;
            break;
        
        case mqtt_desconectado:
            *new_state = &initMqttState;
            break;
        
        case wifi_desconectado:
            *new_state = &initWifiState;
            break;    
        
        default:
            break;
    }
}

static void initDeviceState_enter(void) {
    
}

static void initDeviceState_exit(void) {
    
}

state_t initWifiState = {
    .nombre = init_device,
    .handle = &initDeviceState_handle,
    .enter = &initDeviceState_enter,
    .exit = &initDeviceState_exit
};