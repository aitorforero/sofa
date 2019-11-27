#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "nvs_flash.h"
#include "esp_log.h"
#include <stdio.h>
#include <SofaIO.h>
#include <Wifi.h>
#include <mqtt.h> 

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











void app_main(void){
    ESP_LOGI(TAG, "Empiezo");

    nvs_flash_init();
    sofaIO_init(&sofa);
    wifi_init();
    mqtt_app_start();

    ESP_LOGI(TAG, "Comienzo el bucle");

    for(;;){
        gpio_set_level(sofa.asiento_derecha.motor_abrir, !sofa.asiento_derecha.boton_abrir.pulsado);  
        gpio_set_level(sofa.asiento_derecha.motor_cerrar, !sofa.asiento_derecha.boton_cerrar.pulsado);
        gpio_set_level(sofa.asiento_centro.motor_abrir, !sofa.asiento_centro.boton_abrir.pulsado);  
        gpio_set_level(sofa.asiento_centro.motor_cerrar, !sofa.asiento_centro.boton_cerrar.pulsado);
        gpio_set_level(sofa.asiento_izquierda.motor_abrir, !sofa.asiento_izquierda.boton_abrir.pulsado);  
        gpio_set_level(sofa.asiento_izquierda.motor_cerrar, !sofa.asiento_izquierda.boton_cerrar.pulsado);

        vTaskDelay(10 / portTICK_PERIOD_MS);
    }



}