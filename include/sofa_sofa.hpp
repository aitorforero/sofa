#pragma once
#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "sofa_asiento.hpp"

class SofaEventArgs;

class Sofa {
    private:
        Asiento* _derecha;
        Asiento* _centro;
        Asiento* _izquierda;
        QueueHandle_t _state_machine_event_queue;

        QueueHandle_t event_queue;

        SofaEventArgs* _derecha_abrir_event_args;
        SofaEventArgs* _derecha_cerrar_event_args;
        SofaEventArgs* _centro_abrir_event_args;
        SofaEventArgs* _centro_cerrar_event_args;
        SofaEventArgs* _izquierda_abrir_event_args;
        SofaEventArgs* _izquierda_cerrar_event_args;

        void inicializa_salidas();
        void inicializa_entradas();
        
    public:
        Sofa(Asiento* derecha,Asiento* centro,Asiento* izquierda, QueueHandle_t state_machine_event_queue);
        ~Sofa();
        Asiento* getDerecha();
        Asiento* getCentro();
        Asiento* getIzquierda();
        void onButtonPressed(gpio_num_t pin, BaseType_t* woken);
        void buttonTask();
};

class SofaEventArgs {
    public:
        SofaEventArgs(Sofa* pSofa, gpio_num_t pin_button):sofa(pSofa),pin(pin_button){};
        Sofa* sofa;
        gpio_num_t pin;
};