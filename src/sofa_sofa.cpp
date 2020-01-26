#include <string.h>
#include "config.hpp"

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include "esp_err.h"
#include "esp_event.h"
#include "esp_wifi.h"
#include "esp_log.h"
#include "tcpip_adapter.h"
#include "esp_event_loop.h"
#include "mqtt_client.h"

#include "sofa_asiento.hpp"
#include "sofa_sofa.hpp"
#include "sofa_state_machine.hpp"
#include "sofa_homie_device.hpp"

#define MAX_TOPIC_SIZE 256

static const int CONNECTED_BIT = BIT0;

static const char *TAG = "SOFA_SOFA";


void IRAM_ATTR button_isr_handler(void* arg){
    BaseType_t woken = pdFALSE;
    SofaEventArgs* sofaEventArgs = (SofaEventArgs*) arg;

    sofaEventArgs->sofa->onButtonPressed(sofaEventArgs->pin, &woken);

    if(woken){
      portYIELD_FROM_ISR();
    }
}

static void button_task( void * pvParameters ){
  ESP_LOGI(TAG, "Comienzo button_task");
  Sofa* sofa = (Sofa*) pvParameters;
  sofa->buttonTask();
}

esp_err_t wifi_event_handler(void *ctx, system_event_t *event){
    ESP_LOGI(TAG, "Evento WIFI");
    Sofa* sofa = (Sofa*)ctx;

    switch (event->event_id) {
        case SYSTEM_EVENT_STA_START:
            esp_wifi_connect();
            break;
        case SYSTEM_EVENT_STA_GOT_IP:
            sofa->setConnectedBit();
            break;
        case SYSTEM_EVENT_STA_DISCONNECTED:
            sofa->clearConnectedBit();
            break;
        default:
            break;
    }
    return ESP_OK;
}

static esp_err_t mqtt_event_handler(esp_mqtt_event_handle_t event){
    int msg_id;
    
    Sofa* sofa = (Sofa*)event->user_context;

    switch (event->event_id) {
        case MQTT_EVENT_CONNECTED:
            ESP_LOGI(TAG, "MQTT_EVENT_CONNECTED");
            sofa->getStateMachine()->onEvent((sofa_event_flags)(SOFA_EVENT_MQTT_FLAG | SOFA_EVENT_CONECTADO_FLAG));
            break;

        case MQTT_EVENT_DISCONNECTED:
            ESP_LOGI(TAG, "MQTT_EVENT_DISCONNECTED");
            sofa->getStateMachine()->onEvent((sofa_event_flags)(SOFA_EVENT_MQTT_FLAG | SOFA_EVENT_DESCONECTADO_FLAG));
            break;

        case MQTT_EVENT_SUBSCRIBED:
            ESP_LOGI(TAG, "MQTT_EVENT_SUBSCRIBED, msg_id=%d", event->msg_id);
            sofa->getStateMachine()->onEvent((sofa_event_flags)(SOFA_EVENT_MQTT_FLAG | SOFA_EVENT_SUSCRITO_FLAG));
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
            //raise_mqtt(event->topic, event->data);
            
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

Sofa::Sofa(Asiento* derecha,Asiento* centro,Asiento* izquierda, gpio_num_t pin_led_OK, HomieDevice* sofaDevice): _derecha(derecha),  _centro(centro), _izquierda(izquierda), _pin_led_OK(pin_led_OK),_sofaDevice(sofaDevice){
  ESP_LOGI(TAG, "Inicializando sofa...");

  event_queue = xQueueCreate(10,sizeof(int));

  lastWillState = new char[MAX_TOPIC_SIZE];
  lastWillStateTopic = new char[MAX_TOPIC_SIZE];
  HomieDevice::getHomieState(HOMIE_STATE_LOST, lastWillState, MAX_TOPIC_SIZE);
  snprintf(lastWillStateTopic, MAX_TOPIC_SIZE, HOMIE_DEVICE_STATE_TOPIC, CONFIG_ROOT_TOPIC, _sofaDevice->deviceID);

 
  this->_derecha_abrir_event_args = new SofaEventArgs(this, this->getDerecha()->getPinBotonAbrir());
  this->_derecha_cerrar_event_args = new SofaEventArgs(this, this->getDerecha()->getPinBotonCerrar());
  this->_centro_abrir_event_args = new SofaEventArgs(this, this->getCentro()->getPinBotonAbrir());
  this->_centro_cerrar_event_args = new SofaEventArgs(this, this->getCentro()->getPinBotonCerrar());
  this->_izquierda_abrir_event_args = new SofaEventArgs(this, this->getIzquierda()->getPinBotonAbrir());
  this->_izquierda_cerrar_event_args = new SofaEventArgs(this, this->getIzquierda()->getPinBotonCerrar());
  
  this->inicializa_entradas();
  this->inicializa_salidas();

  this->state_machine = new SofaStateMachine(this);
  this->state_machine->start();

  ESP_LOGI(TAG, "Sofa inicializado");
};

Sofa::~Sofa() {
  delete this->_derecha_abrir_event_args;
  delete this->_derecha_cerrar_event_args;
  delete this->_centro_abrir_event_args;
  delete this->_centro_cerrar_event_args;
  delete this->_izquierda_abrir_event_args;
  delete this->_izquierda_cerrar_event_args;

  delete this->state_machine;
}

void Sofa::onButtonPressed(gpio_num_t pin, BaseType_t* pWoken){
    xQueueSendToBackFromISR(this->event_queue, (void*)&pin, pWoken);
}

void Sofa::buttonTask(){
  ESP_LOGI(TAG, "Comienzo Sofa::buttonTask");
  for( ;; ) {
      gpio_num_t pin;
      if(xQueueReceive(this->event_queue, &pin, 0)){
        sofa_event_flags stateMachineEvent = SOFA_EVENT_BOTON_FLAG;

        if(gpio_get_level(pin)==0) {
          ESP_LOGI(TAG, "Pulsado:");
          stateMachineEvent = (sofa_event_flags)(stateMachineEvent | SOFA_EVENT_PULSADO_FLAG);
        } else {
          ESP_LOGI(TAG, "Levantado:");
          stateMachineEvent =  (sofa_event_flags)(stateMachineEvent | SOFA_EVENT_LEVANTADO_FLAG);
        }; 

        if(this->getDerecha()->getPinBotonAbrir()==pin) {
          ESP_LOGI(TAG, "Derecha Abrir");
          stateMachineEvent = (sofa_event_flags)(stateMachineEvent | SOFA_EVENT_DERECHA_FLAG | SOFA_EVENT_ABRIR_FLAG);
        } else if(this->getDerecha()->getPinBotonCerrar()==pin) {
          ESP_LOGI(TAG, "Derecha Cerrar");
          stateMachineEvent = (sofa_event_flags)(stateMachineEvent | SOFA_EVENT_DERECHA_FLAG | SOFA_EVENT_CERRAR_FLAG);
        } else if(this->getCentro()->getPinBotonAbrir()==pin) {
          ESP_LOGI(TAG, "Centro Abrir");
          stateMachineEvent = (sofa_event_flags)(stateMachineEvent | SOFA_EVENT_CENTRO_FLAG | SOFA_EVENT_ABRIR_FLAG);
        } else if(this->getCentro()->getPinBotonCerrar()==pin) {
          ESP_LOGI(TAG, "Centro Cerrar");
          stateMachineEvent = (sofa_event_flags)(stateMachineEvent | SOFA_EVENT_CENTRO_FLAG | SOFA_EVENT_CERRAR_FLAG);
        } else if(this->getIzquierda()->getPinBotonAbrir()==pin) {
          ESP_LOGI(TAG, "Izquierda Abrir");
          stateMachineEvent = (sofa_event_flags)(stateMachineEvent | SOFA_EVENT_IZQUIERDA_FLAG | SOFA_EVENT_ABRIR_FLAG);
        } else if(this->getIzquierda()->getPinBotonCerrar()==pin) {
          ESP_LOGI(TAG, "Izquierda Cerrar");
          stateMachineEvent = (sofa_event_flags)(stateMachineEvent | SOFA_EVENT_IZQUIERDA_FLAG | SOFA_EVENT_CERRAR_FLAG);
        }

        state_machine->onEvent(stateMachineEvent);

      }
       vTaskDelay(10 / portTICK_PERIOD_MS);
   }
}


Asiento* Sofa::getDerecha() {
  return _derecha;
};

Asiento* Sofa::getCentro() {
  return _centro;
};

Asiento* Sofa::getIzquierda() {
  return _izquierda;
};

void Sofa::inicializa_salidas() {
  ESP_LOGI(TAG, "Inicializando salidas...");
  gpio_config_t io_conf;
  io_conf.intr_type = GPIO_INTR_DISABLE;
  io_conf.mode = GPIO_MODE_OUTPUT;

  // Mascara MOTORES
  io_conf.pin_bit_mask = (1ULL<<this->getDerecha()->getPinMotorAbrir()) |
                          (1ULL<<this->getDerecha()->getPinMotorCerrar()) |
                          (1ULL<<this->getCentro()->getPinMotorAbrir()) |
                          (1ULL<<this->getCentro()->getPinMotorCerrar()) |
                          (1ULL<<this->getIzquierda()->getPinMotorAbrir()) |
                          (1ULL<<this->getIzquierda()->getPinMotorCerrar())|
                          (1ULL<<this->_pin_led_OK) ;

  io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
  io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
  gpio_config(&io_conf);

  this->getDerecha()->parar();
  this->getCentro()->parar();
  this->getIzquierda()->parar();
  ESP_LOGI(TAG, "Salidas inicializadas");

}

void Sofa::inicializa_entradas() {
  ESP_LOGI(TAG, "Inicializando entradas...");
  gpio_config_t io_conf;
  io_conf.intr_type = GPIO_INTR_ANYEDGE;
  io_conf.mode = GPIO_MODE_INPUT;
  io_conf.pin_bit_mask = (1ULL<<this->getDerecha()->getPinBotonAbrir()) |
                          (1ULL<<this->getDerecha()->getPinBotonCerrar()) |
                          (1ULL<<this->getCentro()->getPinBotonAbrir()) |
                          (1ULL<<this->getCentro()->getPinBotonCerrar()) |
                          (1ULL<<this->getIzquierda()->getPinBotonAbrir()) |
                          (1ULL<<this->getIzquierda()->getPinBotonCerrar()) ;
  io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
  io_conf.pull_up_en = GPIO_PULLUP_ENABLE;
  gpio_config(&io_conf);


  //Start Task that will handle the Pushbutton
  xTaskCreate(button_task, "button task", 2048, this, 10, NULL);

  //install gpio isr service
  ESP_ERROR_CHECK(gpio_install_isr_service(0));
  //hook isr handler for specific gpio pin
  gpio_isr_handler_add(this->getDerecha()->getPinBotonAbrir(), button_isr_handler, (void*)this->_derecha_abrir_event_args);
  gpio_isr_handler_add(this->getDerecha()->getPinBotonCerrar(), button_isr_handler, (void*)this->_derecha_cerrar_event_args);
  gpio_isr_handler_add(this->getCentro()->getPinBotonAbrir(), button_isr_handler, (void*)this->_centro_abrir_event_args);
  gpio_isr_handler_add(this->getCentro()->getPinBotonCerrar(), button_isr_handler, (void*)this->_centro_cerrar_event_args);
  gpio_isr_handler_add(this->getIzquierda()->getPinBotonAbrir(), button_isr_handler, (void*)this->_izquierda_abrir_event_args);
  gpio_isr_handler_add(this->getIzquierda()->getPinBotonCerrar(), button_isr_handler, (void*)this->_izquierda_cerrar_event_args);
  ESP_LOGI(TAG, "Entradas inicializadas");
}

void Sofa::encenderOK(){
    gpio_set_level(this->_pin_led_OK, 1);
}

void Sofa::apagarOK(){
    gpio_set_level(this->_pin_led_OK, 0);  
}

SofaStateMachine* Sofa::getStateMachine(){
  return state_machine;
}

void Sofa::wifi_init(){
    tcpip_adapter_init();
    wifi_event_group = xEventGroupCreate();
    ESP_ERROR_CHECK(esp_event_loop_init(wifi_event_handler, this));
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));
    //Allocate storage for the struct
    wifi_config_t wifi_config = {};

    //Assign ssid & password strings
    strcpy((char*)wifi_config.sta.ssid, CONFIG_SSID);
    strcpy((char*)wifi_config.sta.password, CONFIG_PASSWORD);
    wifi_config.sta.bssid_set = false;


    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config));
    ESP_LOGI(TAG, "start the WIFI SSID:[%s]", wifi_config.sta.ssid);
    ESP_ERROR_CHECK(esp_wifi_start());
    ESP_LOGI(TAG, "Waiting for wifi");
    xEventGroupWaitBits(wifi_event_group, CONNECTED_BIT, false, true, portMAX_DELAY);
}

void Sofa::setConnectedBit(){
  xEventGroupSetBits(wifi_event_group, CONNECTED_BIT);
  state_machine->onEvent((sofa_event_flags)(SOFA_EVENT_WIFI_FLAG | SOFA_EVENT_CONECTADO_FLAG));
};

void Sofa::clearConnectedBit(){
  xEventGroupClearBits(wifi_event_group, CONNECTED_BIT);
  state_machine->onEvent((sofa_event_flags)(SOFA_EVENT_WIFI_FLAG | SOFA_EVENT_DESCONECTADO_FLAG));
};

void Sofa::mqtt_app_start()
{
    esp_mqtt_client_config_t mqtt_cfg = {};
    mqtt_cfg.event_handle = mqtt_event_handler;
    mqtt_cfg.uri = CONFIG_BROKER_URL;
    mqtt_cfg.user_context = this;
    mqtt_cfg.lwt_msg = lastWillState;
    mqtt_cfg.lwt_topic = lastWillStateTopic;

    client = esp_mqtt_client_init(&mqtt_cfg);
    esp_mqtt_client_start(client);
}

void Sofa::mqtt_publish(const char* topic,const char* data){
    int msg_id;
    msg_id = esp_mqtt_client_publish(client, topic, data, 0, 1, 1);    
    ESP_LOGI(TAG, "sent publish successful, msg_id=%d", msg_id);
};

void Sofa::mqtt_subscribe(char* topic){
    int msg_id;
    msg_id = esp_mqtt_client_subscribe(client, topic, 0);
    ESP_LOGI(TAG, "sent subscribe successful, msg_id=%d", msg_id);
}



 void Sofa::publishProperty(HomieNode *node, HomieProperty *property){
    char topic[MAX_TOPIC_SIZE];
    snprintf(topic, 256, HOMIE_PROPERTY_VALUE_TOPIC, CONFIG_ROOT_TOPIC, _sofaDevice->deviceID, node->nodeID, property->propertyID);
    mqtt_publish(topic, property->initialValue);
    snprintf(topic, 256, HOMIE_PROPERTY_NAME_TOPIC, CONFIG_ROOT_TOPIC, _sofaDevice->deviceID, node->nodeID, property->propertyID);
    mqtt_publish(topic, property->name);
    char datatype[15];
    property->getHomieDataType(datatype, 15);
    snprintf(topic, 256, HOMIE_PROPERTY_DATATYPE_TOPIC, CONFIG_ROOT_TOPIC, _sofaDevice->deviceID, node->nodeID, property->propertyID);
    mqtt_publish(topic, datatype);
    snprintf(topic, 256, HOMIE_PROPERTY_FORMAT_TOPIC, CONFIG_ROOT_TOPIC, _sofaDevice->deviceID, node->nodeID, property->propertyID);
    mqtt_publish(topic, property->format);
    snprintf(topic, 256, HOMIE_PROPERTY_SETTABLE_TOPIC, CONFIG_ROOT_TOPIC, _sofaDevice->deviceID, node->nodeID, property->propertyID);
    if(property->settable) {
        mqtt_publish(topic, "true");
    } else {
        mqtt_publish(topic, "false");
    }
    snprintf(topic, 256, HOMIE_PROPERTY_SETTABLE_TOPIC, CONFIG_ROOT_TOPIC, _sofaDevice->deviceID, node->nodeID, property->propertyID);
    if(property->retained) {
        mqtt_publish(topic, "true");
    } else {
        mqtt_publish(topic, "false");
    }
    snprintf(topic, 256, HOMIE_PROPERTY_UNIT_TOPIC, CONFIG_ROOT_TOPIC, _sofaDevice->deviceID, node->nodeID, property->propertyID);
    mqtt_publish(topic, property->unit);
};

void Sofa::publishNode(HomieNode *node){
    char topic[MAX_TOPIC_SIZE];
    snprintf(topic, MAX_TOPIC_SIZE, HOMIE_NODE_NAME_TOPIC, CONFIG_ROOT_TOPIC, _sofaDevice->deviceID, node->nodeID);
    mqtt_publish(topic, node->name);
    snprintf(topic, MAX_TOPIC_SIZE, HOMIE_NODE_TYPE_TOPIC, CONFIG_ROOT_TOPIC, _sofaDevice->deviceID, node->nodeID);
    mqtt_publish(topic, node->nodetype);
    char properties[MAX_TOPIC_SIZE];
    node->getProperties(properties, MAX_TOPIC_SIZE);
    snprintf(topic, MAX_TOPIC_SIZE, HOMIE_DEVICE_NODES_TOPIC, CONFIG_ROOT_TOPIC, _sofaDevice->deviceID);
    mqtt_publish(topic, properties);

    int length = sizeof(node->properties) / sizeof(node->properties[0]);
    int i = 0;
    for(i=0; i<length; i++) {
        publishProperty(node, &(node->properties[i]));
    }
};

void Sofa::publishDevice(){
    char topic[MAX_TOPIC_SIZE];
    
    snprintf(topic, MAX_TOPIC_SIZE, HOMIE_DEVICE_HOMIE_TOPIC, CONFIG_ROOT_TOPIC, _sofaDevice->deviceID);
    mqtt_publish(topic, _sofaDevice->version);

    char state[15];
    HomieDevice::getHomieState(HOMIE_STATE_INIT, state, 15);
    snprintf(topic, MAX_TOPIC_SIZE, HOMIE_DEVICE_STATE_TOPIC, CONFIG_ROOT_TOPIC, _sofaDevice->deviceID);
    mqtt_publish(topic, state);   

    snprintf(topic, MAX_TOPIC_SIZE, HOMIE_DEVICE_NAME_TOPIC, CONFIG_ROOT_TOPIC, _sofaDevice->deviceID);
    mqtt_publish(topic, _sofaDevice->name);

    snprintf(topic, MAX_TOPIC_SIZE, HOMIE_DEVICE_EXTENSIONS_TOPIC, CONFIG_ROOT_TOPIC, _sofaDevice->deviceID);
    mqtt_publish(topic, "");

    char nodes[MAX_TOPIC_SIZE];
    _sofaDevice->getNodes(nodes, MAX_TOPIC_SIZE);
    snprintf(topic, MAX_TOPIC_SIZE, HOMIE_DEVICE_NODES_TOPIC, CONFIG_ROOT_TOPIC, _sofaDevice->deviceID);
    mqtt_publish(topic, nodes);

    int length = sizeof(_sofaDevice->nodes) / sizeof(_sofaDevice->nodes[0]);
    int i = 0;
    for(i=0; i<length; i++) {
        publishNode(&(_sofaDevice->nodes[i]));
    };

    subscribe();
};

void Sofa::subscribe(){
    char topic[MAX_TOPIC_SIZE];
    snprintf(topic, 256, HOMIE_DEVICE_SUBSCRIBE_TOPIC, CONFIG_ROOT_TOPIC, _sofaDevice->deviceID);
    mqtt_subscribe(topic);
};

void Sofa::publishConnected(){
    char state[15];
    char topic[MAX_TOPIC_SIZE];
    HomieDevice::getHomieState(HOMIE_STATE_READY, state, 15);
    snprintf(topic, MAX_TOPIC_SIZE, HOMIE_DEVICE_STATE_TOPIC, CONFIG_ROOT_TOPIC, _sofaDevice->deviceID);
    mqtt_publish(topic, state);   
}
