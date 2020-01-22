#pragma once

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/event_groups.h"
#include "mqtt_client.h"

#include "sofa_asiento.hpp"
#include "sofa_state_machine.hpp"
#include "sofa_homie_device.hpp"

class SofaEventArgs;

class Sofa {
    private:
        Asiento* _derecha;
        Asiento* _centro;
        Asiento* _izquierda;
        gpio_num_t _pin_led_OK;
        HomieDevice* _sofaDevice;

        QueueHandle_t event_queue;
        EventGroupHandle_t wifi_event_group;
        esp_mqtt_client_handle_t client;


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
        Sofa(Asiento* derecha,Asiento* centro,Asiento* izquierda, gpio_num_t pin_led_OK, HomieDevice* _sofaDevice);
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

        void mqtt_app_start();
        void mqtt_publish(char* topic, char* data);
        void mqtt_subscribe(char* topic);
};

class SofaEventArgs {
    public:
        SofaEventArgs(Sofa* pSofa, gpio_num_t pin_button):sofa(pSofa),pin(pin_button){};
        Sofa* sofa;
        gpio_num_t pin;
};