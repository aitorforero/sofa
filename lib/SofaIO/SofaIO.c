#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "esp_system.h"
#include "driver/gpio.h"

#include "SofaIO.h"

#define LED_GPIO 18
#define BUTTON_GPIO 4
#define ESP_INTR_FLAG_DEFAULT 0



static QueueHandle_t event_queue = NULL;



static void IRAM_ATTR button_isr_handler(void* arg){
    sofaio_boton_t* boton = (sofaio_boton_t *) arg;
    boton->pulsado = (gpio_get_level(boton->pin) == 1);

    BaseType_t woken;
    xQueueSendToBackFromISR(event_queue, boton, woken);

    if(woken){
      taskYIELD_FROM_ISR(arg);
    }
}



static void sofaIO_inicializa_salidas(sofaio_sofa_t* sofa) {
  printf("Inicializando salidas...");
  gpio_config_t io_conf;
  io_conf.intr_type = GPIO_PIN_INTR_DISABLE;
  io_conf.mode = GPIO_MODE_OUTPUT;

  // Mascara MOTORES
  io_conf.pin_bit_mask = (1ULL<<sofa->asiento_izquierda.motor_abrir) |
                          (1ULL<<sofa->asiento_izquierda.motor_cerrar) |
                          (1ULL<<sofa->asiento_centro.motor_abrir) |
                          (1ULL<<sofa->asiento_centro.motor_cerrar) |
                          (1ULL<<sofa->asiento_derecha.motor_abrir) |
                          (1ULL<<sofa->asiento_derecha.motor_cerrar) ;

  io_conf.pull_down_en = 0;
  io_conf.pull_up_en = 0;
  gpio_config(&io_conf);
}

static void sofaIO_inicializa_entradas(sofaio_sofa_t* sofa) {
  printf("Inicializando entradas...");
  gpio_config_t io_conf;
  io_conf.intr_type = GPIO_INTR_ANYEDGE;
  io_conf.mode = GPIO_MODE_INPUT;
  io_conf.pin_bit_mask = (1ULL<<sofa->asiento_izquierda.boton_abrir.pin) |
                          (1ULL<<sofa->asiento_izquierda.boton_cerrar.pin) |
                          (1ULL<<sofa->asiento_centro.boton_abrir.pin) |
                          (1ULL<<sofa->asiento_centro.boton_cerrar.pin) |
                          (1ULL<<sofa->asiento_derecha.boton_abrir.pin) |
                          (1ULL<<sofa->asiento_derecha.boton_cerrar.pin) ;
  io_conf.pull_down_en = 0;
  io_conf.pull_up_en = 1;
  gpio_config(&io_conf);

  //install gpio isr service
  gpio_install_isr_service(ESP_INTR_FLAG_DEFAULT);
  //hook isr handler for specific gpio pin
  gpio_isr_handler_add(sofa->asiento_izquierda.boton_abrir.pin, button_isr_handler, (void*) &sofa->asiento_izquierda.boton_abrir);
  gpio_isr_handler_add(sofa->asiento_izquierda.boton_cerrar.pin, button_isr_handler, (void*) &sofa->asiento_izquierda.boton_cerrar);
  gpio_isr_handler_add(sofa->asiento_centro.boton_abrir.pin, button_isr_handler, (void*) &sofa->asiento_centro.boton_abrir);
  gpio_isr_handler_add(sofa->asiento_centro.boton_cerrar.pin, button_isr_handler, (void*) &sofa->asiento_centro.boton_cerrar);
  gpio_isr_handler_add(sofa->asiento_derecha.boton_abrir.pin, button_isr_handler, (void*) &sofa->asiento_derecha.boton_abrir);
  gpio_isr_handler_add(sofa->asiento_derecha.boton_cerrar.pin, button_isr_handler, (void*) &sofa->asiento_derecha.boton_cerrar);
}

void sofaIO_init(sofaio_sofa_t* sofa, QueueHandle_t events) {
    event_queue = events;
    sofaIO_inicializa_salidas(sofa);    
    sofaIO_inicializa_entradas(sofa);
    printf("OK\n");
}








