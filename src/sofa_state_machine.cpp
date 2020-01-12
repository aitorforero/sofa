#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "esp_log.h"

#include "sofa_sofa.hpp"
#include "sofa_state_machine.hpp"

static const char *TAG = "SOFA_STATE_MACHINE";

bool SofaState::is(sofa_state_name name){
    return _name == name;
}

SofaStateMachine::SofaStateMachine(Sofa* sofa):_sofa(sofa) {
    ESP_LOGI(TAG, "Inicializando state machine...");

    // event_queue = xQueueCreate(10,sizeof(sofa_event_flags));

    currentState = getState(SIN_WIFI);

    ESP_LOGI(TAG, "State machine inicializada");
};

SofaStateMachine::~SofaStateMachine() {
    currentState->exit();
    delete currentState;
};

void SofaStateMachine::onEvent(sofa_event_flags event){
    sofa_state_name newStateName = currentState->handle(event);
    if(!currentState->is(newStateName)){
        // Hay que cambiar de estado                                                                                                           
        currentState->exit();
        delete currentState;
        currentState = getState(newStateName);
        currentState->enter();
    }

};

void SofaStateMachine::start(){
    currentState->enter();
}

SofaState* SofaStateMachine::getState(sofa_state_name name){
    SofaState* newState = NULL;
    switch (name) {
        case SIN_WIFI:
            newState = new SofaStateSinWifi(_sofa);
            break;
        
        case SIN_MQTT:
            newState = new SofaStateSinMQTT(_sofa);
            break;

        default:
            break;
    }

    return newState;
};

void SofaStateSinWifi::enter(){
    ESP_LOGI(TAG, "Entro en SIN_WIFI");
    _sofa->wifi_init();
};

sofa_state_name SofaStateSinWifi::handle(sofa_event_flags event){
    sofa_state_name newState = getName();

    if((event & SOFA_EVENT_WIFI_FLAG) == SOFA_EVENT_WIFI_FLAG) {
        if((event & SOFA_EVENT_CONECTADO_FLAG) == SOFA_EVENT_CONECTADO_FLAG) {
            ESP_LOGI(TAG, "Wifi conectado");
            newState = SIN_MQTT;
        } else {
            ESP_LOGW(TAG, "Wifi desconectado");
            _sofa->wifi_init();
        }
    }

    return newState;
};
