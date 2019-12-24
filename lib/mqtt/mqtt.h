#ifndef MQTT_DEFINED
#define MQTT_DEFINED
    #include "freertos/FreeRTOS.h"
    #include "freertos/queue.h"

    void mqtt_app_start(QueueHandle_t eventQueue);
    void mqtt_publish(char* topic, char* data);

#endif
