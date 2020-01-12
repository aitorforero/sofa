#pragma once
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/event_groups.h"

#include "sofa_asiento.hpp"
#include "sofa_state_machine.hpp"

class SofaEventArgs;

class Sofa {
    private:
        Asiento* _derecha;
        Asiento* _centro;
        Asiento* _izquierda;
        gpio_num_t _pin_led_OK;
        QueueHandle_t event_queue;
        EventGroupHandle_t wifi_event_group;

        SofaEventArgs* _derecha_abrir_event_args;
        SofaEventArgs* _derecha_cerrar_event_args;
        SofaEventArgs* _centro_abrir_event_args;
        SofaEventArgs* _centro_cerrar_event_args;
        SofaEventArgs* _izquierda_abrir_event_args;
        SofaEventArgs* _izquierda_cerrar_event_args;

        SofaStateMachine* state_machine;

        void inicializa_salidas();
        void inicializa_entradas();
        
    public:
        Sofa(Asiento* derecha,Asiento* centro,Asiento* izquierda, gpio_num_t pin_led_OK);
        ~Sofa();
        Asiento* getDerecha();
        Asiento* getCentro();
        Asiento* getIzquierda();
        void onButtonPressed(gpio_num_t pin, BaseType_t* woken);
        void buttonTask();
        void encenderOK();
        void apagarOK();

        SofaStateMachine* getStateMachine();

        void wifi_init();
        void setConnectedBit();
        void clearConnectedBit();
};

class SofaEventArgs {
    public:
        SofaEventArgs(Sofa* pSofa, gpio_num_t pin_button):sofa(pSofa),pin(pin_button){};
        Sofa* sofa;
        gpio_num_t pin;
};