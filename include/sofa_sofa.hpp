#pragma once

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/event_groups.h"
#include "mqtt_client.h"

#include "sofa_asiento.hpp"
#include "sofa_state_machine.hpp"
#include "sofa_homie_device.hpp"

#define SOFA_MENSAJE_PARAR "STOP"
#define SOFA_MENSAJE_CERRAR "DOWN"
#define SOFA_MENSAJE_ABRIR "UP"

class SofaEventArgs;

class Sofa {
    private:
        Asiento* _derecha;
        Asiento* _centro;
        Asiento* _izquierda;
        gpio_num_t _pin_led_OK;
        gpio_num_t _pin_buzzer;
        HomieDevice* _sofaDevice;

        char *lastWillState = NULL;
        char *lastWillStateTopic = NULL;

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

        void publishNode(HomieNode *node);
        void publishProperty(HomieNode *node, HomieProperty *property);
        void subscribe();
        
    public:
        Sofa(Asiento* derecha,Asiento* centro,Asiento* izquierda, gpio_num_t pin_led_OK, gpio_num_t pin_buzzer, HomieDevice* _sofaDevice);
        ~Sofa();
        Asiento* getDerecha();
        Asiento* getCentro();
        Asiento* getIzquierda();
        void onButtonPressed(gpio_num_t pin, BaseType_t* woken);
        void onMQTTMessage(char *topic, char *data);
        void publishAsientoState(Asiento* asiento, const char * state);
        void buttonTask();
        void encenderOK();
        void apagarOK();
        void beep(int count);

        SofaStateMachine* getStateMachine();

        void wifi_init();
        void setConnectedBit();
        void clearConnectedBit();

        void mqtt_app_start();
        void mqtt_publish(const char* topic, const char* data);
        void mqtt_subscribe(char* topic);
        
        void publishDevice();
        void publishConnected();
};

class SofaEventArgs {
    public:
        SofaEventArgs(Sofa* pSofa, gpio_num_t pin_button):sofa(pSofa),pin(pin_button){};
        Sofa* sofa;
        gpio_num_t pin;
};