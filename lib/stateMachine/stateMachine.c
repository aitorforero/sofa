#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "homieDevice.h"
#include "stateMachine.h"

void stateMachine_init(QueueHandle_t eventQueue, homie_device *device) {
    machineEvents = eventQueue;
    machineDevice = device;
}

void raise(event_t event){
    xQueueSendToBack(machineEvents, &event, 0);
};

void raise_mqtt(char *topic, char *data){
    size_t deviceID_size = 20; // tener en cuenta \0
    size_t nodeID_size = 3;
    size_t propertyID_size = 21;
    size_t command_size = 4;

    char deviceID[deviceID_size];
    char nodeID[nodeID_size];
    char propertyID[propertyID_size];
    char command[command_size];
    int tokens = getHomieValues(topic, deviceID, deviceID_size, nodeID, nodeID_size, propertyID, propertyID_size, command, command_size);

    event_t event = evento_no_valido;

    if(tokens == 5 && strncmp(command, "set", command_size)==0) {
        if(strncmp(nodeID, machineDevice->nodes[0].nodeID, nodeID_size)==0) {
            event = asiento_derecha;
        } else if(strncmp(nodeID, machineDevice->nodes[1].nodeID, nodeID_size)==0) {
            event = asiento_centro;
        } else if(strncmp(nodeID, machineDevice->nodes[2].nodeID, nodeID_size)==0) {
            event = asiento_izquierda;
        } 

        if(!event== evento_no_valido) {
            if(strcmp(topic, "STOP")==0) {
                event |= mqtt_mensaje_parar;
            } else if(strcmp(topic, "UP")==0) {
                event |= mqtt_mensaje_abrir;
            } else if(strcmp(topic, "DOWN")==0){
                event |= mqtt_mensaje_cerrar;
            } else {
                event = evento_no_valido;
            }
        }
    }

    raise(event);
}