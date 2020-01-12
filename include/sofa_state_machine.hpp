#pragma once
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"

#include "sofa_sofa.hpp"

class Sofa;

enum sofa_event_flags
{
    // BOTON
    // +-+-+-+-+-+-+-+
    // |0|0|p|p|a|a|e| :: pp: Posicion, aa: Accion, e:Estado 
    // +-+-+-+-+-+-+-+

    // MQTT
    // +-+-+-+-+-+-+-+
    // |0|1|p|p|0|0|0| :: pp: Tipo 
    // +-+-+-+-+-+-+-+

    // WIFI
    // +-+-+-+-+-+-+-+
    // |1|0|0|p|0|0|0| :: p: Tipo
    // +-+-+-+-+-+-+-+

    SOFA_EVENT_BOTON_FLAG = 0b0000000, 
    SOFA_EVENT_MQTT_FLAG  = 0b0100000, 
    SOFA_EVENT_WIFI_FLAG  = 0b1000000, 

    // Posicion
    SOFA_EVENT_DERECHA_FLAG   = 0b00000,
    SOFA_EVENT_CENTRO_FLAG    = 0b01000,
    SOFA_EVENT_IZQUIERDA_FLAG = 0b10000,

    // Tipo
    SOFA_EVENT_CONECTADO_FLAG    = 0b00000,
    SOFA_EVENT_DESCONECTADO_FLAG = 0b01000,
    SOFA_EVENT_SUSCRITO_FLAG     = 0b10000,
    SOFA_EVENT_MENSAJE_FLAG      = 0b11000,

    // Accion
    SOFA_EVENT_PARAR_FLAG  = 0b100,
    SOFA_EVENT_ABRIR_FLAG  = 0b010,
    SOFA_EVENT_CERRAR_FLAG = 0b000,

    // Estado
    SOFA_EVENT_PULSADO_FLAG   = 0b1,
    SOFA_EVENT_LEVANTADO_FLAG = 0b0
};

enum sofa_state_name
{
    SIN_WIFI,
    SIN_MQTT,
    ANUNCIANDO,
    CONECTADO
};


class SofaState {
    private:
        sofa_state_name _name;

    protected:
        Sofa* _sofa;

    public:
        SofaState(Sofa* sofa, sofa_state_name name):_name(name),_sofa(sofa){};
        sofa_state_name getName(){return _name;};
        virtual ~SofaState(){};
        virtual void enter(){};
        virtual sofa_state_name handle(sofa_event_flags event){return getName();};
        virtual void exit(){};

        bool is(sofa_state_name name);

};

class SofaStateMachine {
    private:
        Sofa* _sofa;
        // QueueHandle_t event_queue;
        SofaState* currentState;

        SofaState* getState(sofa_state_name);

    public:
        SofaStateMachine(Sofa* sofa);
        ~SofaStateMachine();
        void onEvent(sofa_event_flags event);
        void start();
};

class SofaStateSinWifi:public SofaState {
    public:
        SofaStateSinWifi(Sofa* sofa):SofaState(sofa, SIN_WIFI){};
        void enter();
        sofa_state_name handle(sofa_event_flags event);
};

class SofaStateSinMQTT:public SofaState {
    public:
        SofaStateSinMQTT(Sofa* sofa):SofaState(sofa, SIN_WIFI){};
};

class SofaStateAnunciando:public SofaState {
    public:
        SofaStateAnunciando(Sofa* sofa):SofaState(sofa, ANUNCIANDO){};
};

class SofaStateConectado:public SofaState {
    public:
        SofaStateConectado(Sofa* sofa):SofaState(sofa, CONECTADO){};
};