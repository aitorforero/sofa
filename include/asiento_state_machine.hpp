#pragma once

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"


#include "sofa_state_machine.hpp"

class Asiento;
class Sofa;

enum asiento_state_name
{
    IDLE,
    ABRIR_AUTOMATICO,
    CERRAR_AUTOMATICO,
    ABRIR_MANUAL,
    CERRAR_MANUAL
};

class AsientoState {
    private:
        asiento_state_name _name;

    protected:
        Sofa* _sofa;
        Asiento* _asiento;

    public:
        AsientoState(Sofa* sofa, Asiento* asiento, asiento_state_name name):_name(name),_sofa(sofa),_asiento(asiento){};
        asiento_state_name getName(){return _name;};
        virtual ~AsientoState(){};
        virtual void enter(){};
        virtual asiento_state_name handle(sofa_event_flags event){return getName();};
        virtual void exit(){};

        bool is(asiento_state_name name);
};

class AsientoStateMachine {
    private:
        Sofa* _sofa;
        Asiento* _asiento;
        AsientoState* currentState;
        AsientoState* getState(asiento_state_name);

    public:
        AsientoStateMachine(Sofa* sofa, Asiento* asiento);
        ~AsientoStateMachine();
        void onEvent(sofa_event_flags event);
        void start();
};

class AsientoStateIdle:public AsientoState {
    public:
        AsientoStateIdle(Sofa* sofa, Asiento* asiento):AsientoState(sofa, asiento, IDLE){};
        void enter();
        asiento_state_name handle(sofa_event_flags event);
};

class AsientoStateAbrirAutomatico:public AsientoState {
    public:
        AsientoStateAbrirAutomatico(Sofa* sofa, Asiento* asiento):AsientoState(sofa, asiento, ABRIR_AUTOMATICO){};
        void enter();
        asiento_state_name handle(sofa_event_flags event);
};

class AsientoStateAbrirManual:public AsientoState {
    public:
        AsientoStateAbrirManual(Sofa* sofa, Asiento* asiento):AsientoState(sofa, asiento, ABRIR_MANUAL){};
        void enter();
        asiento_state_name handle(sofa_event_flags event);
};

class AsientoStateCerrarAutomatico:public AsientoState {
    public:
        AsientoStateCerrarAutomatico(Sofa* sofa, Asiento* asiento):AsientoState(sofa, asiento, CERRAR_AUTOMATICO){};
        void enter();
        asiento_state_name handle(sofa_event_flags event);
};

class AsientoStateCerrarManual:public AsientoState {
    public:
        AsientoStateCerrarManual(Sofa* sofa, Asiento* asiento):AsientoState(sofa, asiento, CERRAR_MANUAL){};
        void enter();
        asiento_state_name handle(sofa_event_flags event);
};