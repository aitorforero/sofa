#include <stdio.h>
#include <SofaIO.h>
#include "driver/gpio.h"

sofaio_sofa_t sofa = {
    .asiento_derecha = {
        .boton_abrir = GPIO_NUM_13,
        .boton_cerrar = GPIO_NUM_16,
        .motor_abrir = GPIO_NUM_22,
        .motor_cerrar = GPIO_NUM_23 
    },
    .asiento_centro = {
        .boton_abrir = GPIO_NUM_17,
        .boton_cerrar = GPIO_NUM_18,
        .motor_abrir = GPIO_NUM_25,
        .motor_cerrar = GPIO_NUM_26 
    },
    .asiento_izquierda = {
        .boton_abrir = GPIO_NUM_19,
        .boton_cerrar = GPIO_NUM_21,
        .motor_abrir = GPIO_NUM_27,
        .motor_cerrar = GPIO_NUM_32 
    }
};

void app_main(void){
    printf("Empiezo!\n");
    sofaIO_init(&sofa);
}