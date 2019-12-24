#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "nvs_flash.h"
#include "esp_log.h"

#include "SofaIO.h"
#include "homieDevice.h"
#include "Wifi.h"
#include "mqtt.h" 
#include "stateMachine.h"
#include "initWifiState.h"

static const char *TAG = "SOFA_MAIN";

sofaio_sofa_t sofa = {
    .asiento_derecha = {
        .boton_abrir = {
            .pin = GPIO_NUM_16,
            .pulsado = false
        },
        .boton_cerrar =  {
            .pin = GPIO_NUM_4,
            .pulsado = false
        },
        .motor_abrir = GPIO_NUM_27,
        .motor_cerrar = GPIO_NUM_14
    },
    .asiento_centro = {
        .boton_abrir =  {
            .pin = GPIO_NUM_18,
            .pulsado = false
        },
        .boton_cerrar =  {
            .pin = GPIO_NUM_17,
            .pulsado = false
        },
        .motor_abrir = GPIO_NUM_25,
        .motor_cerrar = GPIO_NUM_26
    },
    .asiento_izquierda = {
        .boton_abrir =  {
            .pin = GPIO_NUM_21,
            .pulsado = false
        },
        .boton_cerrar = {
            .pin = GPIO_NUM_19,
            .pulsado = false
        },
        .motor_abrir = GPIO_NUM_32,
        .motor_cerrar = GPIO_NUM_33
    }
};

homie_property asiento_properties[1] = 
{
    {
        .propertyID = "state",
        .name = "State",
        .datatype = Enum,
        .format = "UP,DOWN,STOP",
        .settable = true,
        .retained = true,
        .unit = "",
        .initialValue = "STOP"
    }
};

homie_node asientos[3] =
{
    {
        .nodeID = "ad",
        .name = "Asiento derecha",
        .nodetype = "asiento",
        .properties =  asiento_properties
    },
    {
        .nodeID = "ac",
        .name = "Asiento centro",
        .nodetype = "asiento",
        .properties =  asiento_properties
    },
    {
        .nodeID = "ai",
        .name = "Asiento izquierda",
        .nodetype = "asiento",
        .properties =  asiento_properties
    }
};

homie_device device = 
{
    .version = "4.0",
    .deviceID = "A4CF126C25BC",
    .name = "Sofa sala",
    .nodes = asientos
};


state_t *currentState;

QueueHandle_t stateMachineEventQueue;

void app_main(void){
    ESP_LOGI(TAG, "Empiezo");

    nvs_flash_init();


    sofaIO_init(&sofa);

    // inicializo la cola de eventos
    stateMachineEventQueue = xQueueCreate(10,sizeof(event_t));
    stateMachine_init(stateMachineEventQueue, &device);

    currentState = &initWifiState;
    currentState->enter();

    ESP_LOGI(TAG, "Comienzo el bucle");

    for(;;){
        event_t receivedEvent;
        state_t *newState = currentState;
        if(xQueueReceive(stateMachineEventQueue, &receivedEvent, 0)){
            currentState->handle(receivedEvent, &newState);
            if(newState->nombre!=currentState->nombre){
                // Hay que cambiar de estado                                                                                                           
                currentState->exit();
                currentState = newState;
                currentState->enter();
            }

        // gpio_set_level(sofa.asiento_derecha.motor_abrir, !sofa.asiento_derecha.boton_abrir.pulsado);  
        // gpio_set_level(sofa.asiento_derecha.motor_cerrar, !sofa.asiento_derecha.boton_cerrar.pulsado);
        // gpio_set_level(sofa.asiento_centro.motor_abrir, !sofa.asiento_centro.boton_abrir.pulsado);  
        // gpio_set_level(sofa.asiento_centro.motor_cerrar, !sofa.asiento_centro.boton_cerrar.pulsado);
        // gpio_set_level(sofa.asiento_izquierda.motor_abrir, !sofa.asiento_izquierda.boton_abrir.pulsado);  
        // gpio_set_level(sofa.asiento_izquierda.motor_cerrar, !sofa.asiento_izquierda.boton_cerrar.pulsado);

            vTaskDelay(10 / portTICK_PERIOD_MS);
        }
    }



}