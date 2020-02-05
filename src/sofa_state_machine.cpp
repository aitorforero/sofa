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

        case ANUNCIANDO:
            newState = new SofaStateAnunciando(_sofa);
            break;

        case CONECTADO:
            newState = new SofaStateConectado(_sofa);
            break;

        default:
            ESP_LOGE(TAG, "getState => Estado no definido!!!");
            break;
    }

    return newState;
};

void SofaStateSinWifi::enter(){
    ESP_LOGI(TAG, "Entro en SIN_WIFI");
    _sofa->beep(3);
    _sofa->wifi_init();
};

sofa_state_name SofaStateSinWifi::handle(sofa_event_flags event){
    sofa_state_name newState = getName();
            ESP_LOGW(TAG, "Wifi state handle");

    if((event & SOFA_EVENT_WIFI_FLAG) == SOFA_EVENT_WIFI_FLAG) {
        if((event & SOFA_EVENT_DESCONECTADO_FLAG) == SOFA_EVENT_DESCONECTADO_FLAG) {
            ESP_LOGW(TAG, "Wifi desconectado");
            _sofa->wifi_init();
        } else {
             ESP_LOGI(TAG, "Wifi conectado");
            newState = SIN_MQTT;
       }
    }

    return newState;
};

void SofaStateSinMQTT::enter(){
    ESP_LOGI(TAG, "Entro en SIN_MQTT");
    _sofa->beep(2);
    _sofa->mqtt_app_start();
};

sofa_state_name SofaStateSinMQTT::handle(sofa_event_flags event){
    sofa_state_name newState = getName();

    if((event & SOFA_EVENT_WIFI_FLAG) == SOFA_EVENT_WIFI_FLAG) {
        if((event & SOFA_EVENT_DESCONECTADO_FLAG) == SOFA_EVENT_DESCONECTADO_FLAG) {
            ESP_LOGW(TAG, "Wifi desconectado");
            newState = SIN_WIFI;
        }
    } else if((event & SOFA_EVENT_MQTT_FLAG) == SOFA_EVENT_MQTT_FLAG) {
        if((event & SOFA_EVENT_DESCONECTADO_FLAG) == SOFA_EVENT_DESCONECTADO_FLAG) {
            ESP_LOGW(TAG, "MQTT Desconectado");
            _sofa->mqtt_app_start();
        } else if((event & SOFA_EVENT_CONECTADO_FLAG) == SOFA_EVENT_CONECTADO_FLAG) {
            ESP_LOGW(TAG, "MQTT Conectado");
            newState = ANUNCIANDO;
        }

    }

    return newState;
};

void SofaStateAnunciando::enter(){
    _sofa->beep(1);
    ESP_LOGI(TAG, "Entro en Anunciando");
    _sofa->publishDevice();
};

sofa_state_name SofaStateAnunciando::handle(sofa_event_flags event){
    sofa_state_name newState = getName();

    if(((event & SOFA_EVENT_WIFI_FLAG) == SOFA_EVENT_WIFI_FLAG) && ((event & SOFA_EVENT_DESCONECTADO_FLAG) == SOFA_EVENT_DESCONECTADO_FLAG)) {
            ESP_LOGW(TAG, "Wifi desconectado");
            newState = SIN_WIFI;
    } else if((event & SOFA_EVENT_MQTT_FLAG) == SOFA_EVENT_MQTT_FLAG) {
        if((event & SOFA_EVENT_DESCONECTADO_FLAG) == SOFA_EVENT_DESCONECTADO_FLAG) {
            ESP_LOGW(TAG, "MQTT Desconectado");
            newState = SIN_MQTT;
        } else if((event & SOFA_EVENT_SUSCRITO_FLAG) == SOFA_EVENT_SUSCRITO_FLAG) {
            ESP_LOGW(TAG, "MQTT Suscrito");
            newState = CONECTADO;
        }
    }

    return newState;
};

void SofaStateConectado::enter(){
    ESP_LOGI(TAG, "Entro en Conectado");
    _sofa->getDerecha()->initializeStateMachine(_sofa);
    _sofa->getCentro()->initializeStateMachine(_sofa);
    _sofa->getIzquierda()->initializeStateMachine(_sofa);
    _sofa->publishConnected();
    _sofa->encenderOK();
};

sofa_state_name SofaStateConectado::handle(sofa_event_flags event){
    sofa_state_name newState = getName();

    if(((event & SOFA_EVENT_WIFI_FLAG) == SOFA_EVENT_WIFI_FLAG) && ((event & SOFA_EVENT_DESCONECTADO_FLAG) == SOFA_EVENT_DESCONECTADO_FLAG)) {
        ESP_LOGW(TAG, "Wifi desconectado");
        newState = SIN_WIFI;
    } else if(((event & SOFA_EVENT_MQTT_FLAG) == SOFA_EVENT_MQTT_FLAG)  && ((event & SOFA_EVENT_DESCONECTADO_FLAG) == SOFA_EVENT_DESCONECTADO_FLAG)) {
        ESP_LOGW(TAG, "MQTT Desconectado");
        newState = SIN_MQTT;
    } else {
        // el resto de casos se lo mandamos al asiento que le corresponda
        if ((event & SOFA_EVENT_DERECHA_FLAG) == SOFA_EVENT_DERECHA_FLAG) {
           _sofa->getDerecha()->getStateMachine()->onEvent(event); 
        } else if ((event & SOFA_EVENT_CENTRO_FLAG) == SOFA_EVENT_CENTRO_FLAG) {
           _sofa->getCentro()->getStateMachine()->onEvent(event); 
        } else if ((event & SOFA_EVENT_IZQUIERDA_FLAG) == SOFA_EVENT_IZQUIERDA_FLAG) {
           _sofa->getIzquierda()->getStateMachine()->onEvent(event); 
        }
    }

    return newState;
};

