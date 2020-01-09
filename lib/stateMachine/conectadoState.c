#include "config.h"
#include "mqtt.h"
#include "homieDevice.h"

#include "initWifiState.h"
#include "initMqttState.h"
#include "conectadoState.h"
#include "idleState.h"




 static asiento_state_t *asiento_derecha_state;
 static asiento_state_t *asiento_centro_state;
 static asiento_state_t *asiento_izquierda_state;

static void initDeviceState_handle(event_t event, state_t **new_state) {
    *new_state = &conectadoState;

    switch (event)
    {
       
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
    asiento_derecha_state = &idleState;
    asiento_centro_state = &idleState;
    asiento_izquierda_state = &idleState;
}

static void initDeviceState_exit(void) {
    
}

state_t initWifiState = {
    .nombre = init_device,
    .handle = &initDeviceState_handle,
    .enter = &initDeviceState_enter,
    .exit = &initDeviceState_exit
};