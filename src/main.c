#include <stdio.h>
#include <SofaIO.h>
#include "freertos/semphr.h"
#include "driver/gpio.h"

sofaio_sofa_t sofa = {
    .asiento_derecha = {
        .boton_abrir = GPIO_NUM_16,
        .boton_cerrar = GPIO_NUM_4,
        .motor_abrir = GPIO_NUM_27,
        .motor_cerrar = GPIO_NUM_14 
    },
    .asiento_centro = {
        .boton_abrir = GPIO_NUM_18,
        .boton_cerrar = GPIO_NUM_17,
        .motor_abrir = GPIO_NUM_25,
        .motor_cerrar = GPIO_NUM_26 
    },
    .asiento_izquierda = {
        .boton_abrir = GPIO_NUM_21,
        .boton_cerrar = GPIO_NUM_19,
        .motor_abrir = GPIO_NUM_32,
        .motor_cerrar = GPIO_NUM_33 
    }
};


static QueueHandle_t events;

void app_main(void){
    printf("Empiezo!\n");
    sofaIO_init(&sofa, events);
}