#ifndef  WIFI_DEFINED
#define WIFI_DEFINED
    #include "freertos/FreeRTOS.h"
    #include "freertos/queue.h"

    void wifi_init(QueueHandle_t events);
#endif