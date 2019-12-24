#include "mqtt.h"
#include "stateMachine.h"
#include "initMqttState.h"
#include "initWifiState.h"
#include "initDeviceState.h"

static void initMqttState_handle(event_t event, state_t **new_state) {
    *new_state = &initMqttState;

    switch (event)
    {
        case mqtt_conectado:
            *new_state = &initDeviceState;
            break;
        
        case mqtt_desconectado:
            mqtt_app_start(machineEvents);
            break;
        
        case wifi_desconectado:
            *new_state = &initWifiState;
            break;    
        
        default:
            break;
    }
}

static void initMqttState_enter(void) {
    mqtt_app_start(machineEvents);
}

static void initMqttState_exit(void) {
    
}

state_t initWifiState = {
    .nombre = init_mqtt,
    .handle = &initMqttState_handle,
    .enter = &initMqttState_enter,
    .exit = &initMqttState_exit
};