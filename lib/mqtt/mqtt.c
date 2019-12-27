#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "mqtt_client.h"
#include "esp_log.h"
#include "config.h"
#include "mqtt.h"
#include "stateMachine.h"
#include "homieDevice.h"

static const char *TAG = "SOFA_MQTT";
static const char *UP = "UP";
static const char *UP = "DOWN";
static const char *UP = "STOP";

static QueueHandle_t stateMachineEventQueue;

static esp_mqtt_client_handle_t client;

void mqtt_publish(char* topic, char* data){
    int msg_id;
    msg_id = esp_mqtt_client_publish(client, topic, data, 0, 1, 1);    
    ESP_LOGI(TAG, "sent publish successful, msg_id=%d", msg_id);
};

void mqtt_subscribe(char* topic){
    int msg_id;
    msg_id = esp_mqtt_client_subscribe(client, topic, 0);
    ESP_LOGI(TAG, "sent subscribe successful, msg_id=%d", msg_id);
}

static esp_err_t mqtt_event_handler(esp_mqtt_event_handle_t event)
{
    int msg_id;
    // your_context_t *context = event->context;
    event_t machineEvent;
    switch (event->event_id) {
        case MQTT_EVENT_CONNECTED:
            ESP_LOGI(TAG, "MQTT_EVENT_CONNECTED");
            raise(mqtt_conectado);
            break;

        case MQTT_EVENT_DISCONNECTED:
            ESP_LOGI(TAG, "MQTT_EVENT_DISCONNECTED");
            raise(mqtt_desconectado);
            break;

        case MQTT_EVENT_SUBSCRIBED:
            ESP_LOGI(TAG, "MQTT_EVENT_SUBSCRIBED, msg_id=%d", event->msg_id);
            raise(mqtt_suscrito);
            break;

        case MQTT_EVENT_UNSUBSCRIBED:
            ESP_LOGI(TAG, "MQTT_EVENT_UNSUBSCRIBED, msg_id=%d", event->msg_id);
            break;
        case MQTT_EVENT_PUBLISHED:
            ESP_LOGI(TAG, "MQTT_EVENT_PUBLISHED, msg_id=%d", event->msg_id);
            break;
        case MQTT_EVENT_DATA:
            ESP_LOGI(TAG, "MQTT_EVENT_DATA");
            printf("TOPIC=%.*s\r\n", event->topic_len, event->topic);
            printf("DATA=%.*s\r\n", event->data_len, event->data);
            raise_mqtt(event->topic, event->data);
            
            break;
        case MQTT_EVENT_ERROR:
            ESP_LOGI(TAG, "MQTT_EVENT_ERROR");
            break;
        default:
            ESP_LOGI(TAG, "Other event id:%d", event->event_id);
            break;
    }
    return ESP_OK;
}

void mqtt_app_start(QueueHandle_t eventQueue)
{
    stateMachineEventQueue = eventQueue;

    esp_mqtt_client_config_t mqtt_cfg = {
        .uri = CONFIG_BROKER_URL,
        .event_handle = mqtt_event_handler
    };

    client = esp_mqtt_client_init(&mqtt_cfg);
    esp_mqtt_client_start(client);
}