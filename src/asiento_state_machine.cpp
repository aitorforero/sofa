#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "esp_log.h"

#include "sofa_sofa.hpp"
#include "sofa_state_machine.hpp"
#include "asiento_state_machine.hpp"

static const char *TAG = "ASIENTO_STATE_MACHINE";

bool AsientoState::is(asiento_state_name name){
    return _name == name;
}


AsientoStateMachine::AsientoStateMachine(Sofa* sofa, Asiento* asiento):_sofa(sofa), _asiento(asiento){
    ESP_LOGI(TAG, "Inicializando asiento state machine...");

    // event_queue = xQueueCreate(10,sizeof(sofa_event_flags));

    currentState = getState(IDLE);

    ESP_LOGI(TAG, "Asiento state machine inicializada");
};

AsientoStateMachine::~AsientoStateMachine() {
    currentState->exit();
    delete currentState;
};

void AsientoStateMachine::onEvent(sofa_event_flags event){
    asiento_state_name newStateName = currentState->handle(event);
    
    if(!currentState->is(newStateName)){
        // Hay que cambiar de estado                                                                                                           
        currentState->exit();
        delete currentState;
        currentState = getState(newStateName);
        currentState->enter();
    }

};

void AsientoStateMachine::start(){
    currentState->enter();
}

AsientoState* AsientoStateMachine::getState(asiento_state_name name){
    AsientoState* newState = NULL;
    switch (name) {
        case IDLE:
            newState = new AsientoStateIdle(_sofa, _asiento);
            break;
        
        case ABRIR_AUTOMATICO:
            newState = new AsientoStateAbrirAutomatico(_sofa, _asiento);
            break;

        case CERRAR_AUTOMATICO:
            newState = new AsientoStateCerrarAutomatico(_sofa, _asiento);
            break;

        case ABRIR_MANUAL:
            newState = new AsientoStateAbrirManual(_sofa, _asiento);
            break;

        case CERRAR_MANUAL:
            newState = new AsientoStateCerrarManual(_sofa, _asiento);
            break;

        default:
            ESP_LOGE(TAG, "getState => Estado no definido!!!");
            break;
    }

    return newState;
};

void AsientoStateIdle::enter(){
    ESP_LOGI(TAG, "Entro en Idle");
    _asiento->parar();
};

asiento_state_name AsientoStateIdle::handle(sofa_event_flags event){
    asiento_state_name newState = getName();

    if((event & SOFA_EVENT_MQTT_FLAG) == SOFA_EVENT_MQTT_FLAG) {
        if((event & SOFA_EVENT_MENSAJE_FLAG) == SOFA_EVENT_MENSAJE_FLAG) {
            if((event & SOFA_EVENT_ABRIR_FLAG) == SOFA_EVENT_ABRIR_FLAG) {
                ESP_LOGI(TAG, "MQTT Abrir");
                newState = ABRIR_AUTOMATICO;
            } else if((event & SOFA_EVENT_CERRAR_FLAG) == SOFA_EVENT_CERRAR_FLAG) {
                ESP_LOGI(TAG, "MQTT Cerrar");
                newState = CERRAR_AUTOMATICO;
            }
        }
    } else if((event & SOFA_EVENT_BOTON_FLAG) == SOFA_EVENT_BOTON_FLAG) {
        if((event & SOFA_EVENT_PULSADO_FLAG) == SOFA_EVENT_PULSADO_FLAG) {
            if((event & SOFA_EVENT_ABRIR_FLAG) == SOFA_EVENT_ABRIR_FLAG) {
                ESP_LOGI(TAG, "Boton Abrir pulsado");
                newState = ABRIR_MANUAL;
            } else if((event & SOFA_EVENT_CERRAR_FLAG) == SOFA_EVENT_CERRAR_FLAG) {
                ESP_LOGI(TAG, "Boton Cerrar pulsado");
                newState = CERRAR_MANUAL;
            }
        }

    } 

    return newState;
};

void AsientoStateAbrirAutomatico::enter(){
    ESP_LOGI(TAG, "Entro en Abrir Automatico");
    _asiento->abrir();
};

asiento_state_name AsientoStateAbrirAutomatico::handle(sofa_event_flags event){
    asiento_state_name newState = getName();

    if((event & SOFA_EVENT_BOTON_FLAG) == SOFA_EVENT_BOTON_FLAG) {
        ESP_LOGI(TAG, "Evento boton. Cualquiera, me para");
        newState = IDLE;
    } else if(((event & SOFA_EVENT_MQTT_FLAG) == SOFA_EVENT_MQTT_FLAG) && ((event & SOFA_EVENT_MENSAJE_FLAG) == SOFA_EVENT_MENSAJE_FLAG)){
        if((event & SOFA_EVENT_CERRAR_FLAG) == SOFA_EVENT_CERRAR_FLAG) {
            ESP_LOGI(TAG, "Mqtt cerrar.");
            newState = ABRIR_AUTOMATICO; 
        } else if((event & SOFA_EVENT_PARAR_FLAG) == SOFA_EVENT_PARAR_FLAG) {
            ESP_LOGI(TAG, "Mqtt Parar.");
            newState = IDLE; 
        }
    }

    return newState;
};

void AsientoStateCerrarAutomatico::enter(){
    ESP_LOGI(TAG, "Entro en Cerrar Automatico");
    _asiento->cerrar();
};

asiento_state_name AsientoStateCerrarAutomatico::handle(sofa_event_flags event){
    asiento_state_name newState = getName();

    if((event & SOFA_EVENT_BOTON_FLAG) == SOFA_EVENT_BOTON_FLAG) {
        ESP_LOGI(TAG, "Evento boton. Cualquiera, me para");
        newState = IDLE;
    } else if(((event & SOFA_EVENT_MQTT_FLAG) == SOFA_EVENT_MQTT_FLAG) && ((event & SOFA_EVENT_MENSAJE_FLAG) == SOFA_EVENT_MENSAJE_FLAG)){
        if((event & SOFA_EVENT_ABRIR_FLAG) == SOFA_EVENT_ABRIR_FLAG) {
            ESP_LOGI(TAG, "Mqtt Abrir.");
            newState = ABRIR_AUTOMATICO; 
        } else if((event & SOFA_EVENT_PARAR_FLAG) == SOFA_EVENT_PARAR_FLAG) {
            ESP_LOGI(TAG, "Mqtt Parar.");
            newState = IDLE; 
        }
    }

    return newState;
};


void AsientoStateAbrirManual::enter(){
    ESP_LOGI(TAG, "Entro en Abrir Manual");
    _asiento->abrir();
};

asiento_state_name AsientoStateAbrirManual::handle(sofa_event_flags event){
    asiento_state_name newState = getName();

    if((event & SOFA_EVENT_BOTON_FLAG) == SOFA_EVENT_BOTON_FLAG) {
        if(((event & SOFA_EVENT_PULSADO_FLAG) == SOFA_EVENT_PULSADO_FLAG) && ((event & SOFA_EVENT_CERRAR_FLAG) == SOFA_EVENT_CERRAR_FLAG)) {
            ESP_LOGI(TAG, "Boton Cerrar pulsado. Los dos pulsados me paran");
            newState = IDLE; // Los dos pulsados me paran
        }else if(((event & SOFA_EVENT_LEVANTADO_FLAG) == SOFA_EVENT_LEVANTADO_FLAG) && ((event & SOFA_EVENT_ABRIR_FLAG) == SOFA_EVENT_ABRIR_FLAG)) {
            ESP_LOGI(TAG, "Boton Abrir levantado. Nada pulsado me paran");
            newState = IDLE; // Nada pulsado me paran
        }
    } 

    return newState;
};


void AsientoStateCerrarManual::enter(){
    ESP_LOGI(TAG, "Entro en Cerrar Manual");
    _asiento->cerrar();
};

asiento_state_name AsientoStateCerrarManual::handle(sofa_event_flags event){
    asiento_state_name newState = getName();

    if((event & SOFA_EVENT_BOTON_FLAG) == SOFA_EVENT_BOTON_FLAG) {
        if(((event & SOFA_EVENT_PULSADO_FLAG) == SOFA_EVENT_PULSADO_FLAG) && ((event & SOFA_EVENT_ABRIR_FLAG) == SOFA_EVENT_ABRIR_FLAG)) {
            ESP_LOGI(TAG, "Boton Abrir pulsado. Los dos pulsados me paran");
            newState = IDLE;
        }else if(((event & SOFA_EVENT_LEVANTADO_FLAG) == SOFA_EVENT_LEVANTADO_FLAG) && ((event & SOFA_EVENT_CERRAR_FLAG) == SOFA_EVENT_CERRAR_FLAG)) {
            ESP_LOGI(TAG, "Boton Cerrar levantado. Nada pulsado me paran");
            newState = IDLE; 
        }
    } 

    return newState;
};
