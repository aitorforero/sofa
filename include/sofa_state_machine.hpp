#pragma once

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"


class Sofa;

enum sofa_event_flags
{
    /**********************************************
     * 
     * BOTON
     * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
     * |0|0|1|0|0|0|0|a|a|a|b|b|b|c|c| 
     * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
     *  ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^ ^  
     *  | | | | | | | | | | | | | ---- Estado
     *  | | | | | | | | | | ---------- Accion
     *  | | | | | | | ---------------- Asiento
     *  | | | ------------------------ Tipo
     *  ------------------------------ Clase
     * 
     **********************************************/

     // Clase
    SOFA_EVENT_BOTON_FLAG = 0b001000000000000, 
    SOFA_EVENT_MQTT_FLAG  = 0b010000000000000, 
    SOFA_EVENT_WIFI_FLAG  = 0b100000000000000, 

    // Tipo
    SOFA_EVENT_CONECTADO_FLAG    = 0b000100000000,
    SOFA_EVENT_DESCONECTADO_FLAG = 0b001000000000,
    SOFA_EVENT_SUSCRITO_FLAG     = 0b010000000000,
    SOFA_EVENT_MENSAJE_FLAG      = 0b100000000000,

    // Asiento
    SOFA_EVENT_DERECHA_FLAG   = 0b00100000,
    SOFA_EVENT_CENTRO_FLAG    = 0b01000000,
    SOFA_EVENT_IZQUIERDA_FLAG = 0b10000000,
    
    // Accion
    SOFA_EVENT_PARAR_FLAG  = 0b00100,
    SOFA_EVENT_ABRIR_FLAG  = 0b01000,
    SOFA_EVENT_CERRAR_FLAG = 0b10000,

    // Estado
    SOFA_EVENT_PULSADO_FLAG   = 0b01,
    SOFA_EVENT_LEVANTADO_FLAG = 0b10

};

inline sofa_event_flags& operator |=(sofa_event_flags& a, sofa_event_flags b)
{
       return a=(sofa_event_flags)(a | b);
}


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
        void enter();
        sofa_state_name handle(sofa_event_flags event);
};

class SofaStateAnunciando:public SofaState {
    public:
        SofaStateAnunciando(Sofa* sofa):SofaState(sofa, ANUNCIANDO){};
        void enter();
        sofa_state_name handle(sofa_event_flags event);
};

class SofaStateConectado:public SofaState {
    public:
        SofaStateConectado(Sofa* sofa):SofaState(sofa, CONECTADO){};
        void enter();
        sofa_state_name handle(sofa_event_flags event);
};

