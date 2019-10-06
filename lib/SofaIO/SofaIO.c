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



static SemaphoreHandle_t xSemaphore = NULL;



static void IRAM_ATTR button_isr_handler(void* arg){
    xSemaphoreGiveFromISR(xSemaphore, NULL);
}

static void button_task(void* arg) {
  for(;;) {
    if(xSemaphoreTake(xSemaphore,portMAX_DELAY) == pdTRUE) {
        printf("Cambio estado\n");
        fflush(stdout);
        gpio_set_level(LED_GPIO, ! gpio_get_level(BUTTON_GPIO));
    }
  }
}

void sofaIO_init(sofaio_sofa_t* sofa) {
    printf("Inicializando E/S...");
    // LED
    gpio_config_t io_conf;
    io_conf.intr_type = GPIO_PIN_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.pin_bit_mask = (1ULL<<LED_GPIO);
    io_conf.pull_down_en = 0;
    io_conf.pull_up_en = 0;
    gpio_config(&io_conf);

    // BUTTON
    io_conf.intr_type = GPIO_INTR_ANYEDGE;
    io_conf.mode = GPIO_MODE_INPUT;
    io_conf.pin_bit_mask = (1ULL<<BUTTON_GPIO);
    io_conf.pull_down_en = 0;
    io_conf.pull_up_en = 1;
    gpio_config(&io_conf);

    xSemaphore = xSemaphoreCreateBinary();


    xTaskCreate(button_task, "button_task", 2048, NULL, 10, NULL);


    //install gpio isr service
    gpio_install_isr_service(ESP_INTR_FLAG_DEFAULT);
    //hook isr handler for specific gpio pin
    gpio_isr_handler_add(BUTTON_GPIO, button_isr_handler, (void*) BUTTON_GPIO);
    
    printf("OK\n");
 
}








