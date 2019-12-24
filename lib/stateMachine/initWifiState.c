#include <Wifi.h>
#include "stateMachine.h"
#include "initWifiState.h"
#include "initMqttState.h"

static void initWifiState_enter(void) {
    wifi_init(machineEvents);
}
static void initWifiState_handle(event_t event, state_t** new_state){
    *new_state = &initWifiState;

    switch (event)
    {
        case wifi_conectado:
            *new_state = &initMqttState;
            break;
        
        case wifi_desconectado:
            initWifiState_enter();
            break;
        
        default:
            break;
    }

}



static void initWifiState_exit(void) {
    
}

state_t initWifiState = {
    .nombre = init_wifi,
    .handle = &initWifiState_handle,
    .enter = &initWifiState_enter,
    .exit = &initWifiState_exit
};