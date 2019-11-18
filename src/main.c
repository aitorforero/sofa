#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "driver/gpio.h"
#include <stdio.h>
#include <SofaIO.h>


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


static QueueHandle_t events;

void app_main(void){
    printf("Empiezo!\n");

    events = xQueueCreate(10,sizeof(sofaio_boton_t));

    sofaIO_init(&sofa, events);

    sofaio_boton_t *element;

    for(;;){
        if(xQueueReceive(events, &element, portMAX_DELAY)) {
            printf("Pulsado\n");
        }
    }



}