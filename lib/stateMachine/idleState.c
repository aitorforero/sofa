#include <Wifi.h>
#include "stateMachine.h"
#include "initWifiState.h"
#include "initMqttState.h"
#include "idleState.h"

static void idleState_enter(struct asiento_state_s* asientoState) {

}

static void idleState_handle(event_t event, struct asiento_state_s* asientoState,struct asiento_state_s** new_state){
    *asiento_state_s = &idleState;

    switch (event)
    {
        case 
            abrir_manual
        
        default:
            break;
    }

}



static void idleState_exit(struct asiento_state_s* asientoState) {
    
}

asiento_state_t idleState = {
    .nombre = idle,
    .handle = &idleState_handle,
    .enter = &idleState_enter,
    .exit = &idleState_exit,
};