#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "driver/gpio.h"

#include <stdio.h>
#include <SofaIO.h>

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




static QueueHandle_t buttonEvents;






void app_main(void){
    printf("Empiezo!\n");

    nvs_flash_init();

    buttonEvents = xQueueCreate(10,sizeof(sofaio_boton_t));

    sofaIO_init(&sofa, buttonEvents);

    sofaio_boton_t *element;

    for(;;){
        if(xQueueReceive(buttonEvents, &element, portMAX_DELAY)) {
            element->pulsado = (gpio_get_level(element->pin) == 0); // esta pull up

            gpio_set_level(sofa.asiento_derecha.motor_abrir, !sofa.asiento_derecha.boton_abrir.pulsado);  
            gpio_set_level(sofa.asiento_derecha.motor_cerrar, !sofa.asiento_derecha.boton_cerrar.pulsado);
            gpio_set_level(sofa.asiento_centro.motor_abrir, !sofa.asiento_centro.boton_abrir.pulsado);  
            gpio_set_level(sofa.asiento_centro.motor_cerrar, !sofa.asiento_centro.boton_cerrar.pulsado);
            gpio_set_level(sofa.asiento_izquierda.motor_abrir, !sofa.asiento_izquierda.boton_abrir.pulsado);  
            gpio_set_level(sofa.asiento_izquierda.motor_cerrar, !sofa.asiento_izquierda.boton_cerrar.pulsado);
        }
    }



}