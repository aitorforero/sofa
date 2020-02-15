#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "nvs_flash.h"
#include "esp_log.h"

#include "sofa_sofa.hpp"
#include "sofa_asiento.hpp"
#include "sofa_homie_device.hpp"


static const char *TAG = "SOFA_MAIN";

Asiento derecha(GPIO_NUM_16, GPIO_NUM_4, GPIO_NUM_27, GPIO_NUM_14);
Asiento centro(GPIO_NUM_18, GPIO_NUM_17, GPIO_NUM_25, GPIO_NUM_26);
Asiento izquierda(GPIO_NUM_21, GPIO_NUM_19, GPIO_NUM_32, GPIO_NUM_33);  
Sofa* sofa;

HomieProperty* stateProperty;
HomieNode* asientoDerechaNode;
HomieNode* asientoCentroNode;
HomieNode* asientoIzquierdaNode;
HomieDevice* sofaDevice;


void inicializarHomieDevice() {
    stateProperty = new HomieProperty();
    stateProperty->propertyID = "state";
    stateProperty->name = "State";
    stateProperty->datatype = HOMIE_DATATYPE_ENUM;
    stateProperty->format = "UP,DOWN,STOP";
    stateProperty->settable = true;
    stateProperty->retained = true;
    stateProperty->unit = "";
    stateProperty->initialValue = "STOP";

    asientoDerechaNode = new HomieNode();
    asientoDerechaNode->nodeID = "ad",
    asientoDerechaNode->name = "AsientoDerecha",
    asientoDerechaNode->nodetype = "asiento",
    asientoDerechaNode->properties.push_back(*stateProperty);

    asientoCentroNode = new HomieNode();
    asientoCentroNode->nodeID = "ac",
    asientoCentroNode->name = "AsientoCentro",
    asientoCentroNode->nodetype = "asiento",
    asientoCentroNode->properties.push_back(*stateProperty);

    asientoIzquierdaNode = new HomieNode();
    asientoIzquierdaNode->nodeID = "ai",
    asientoIzquierdaNode->name = "AsientoIzquierda",
    asientoIzquierdaNode->nodetype = "asiento",
    asientoIzquierdaNode->properties.push_back(*stateProperty);

    sofaDevice = new HomieDevice();
    sofaDevice->deviceID = "A4CF126C25BC";
    sofaDevice->name = "SofaSala";
    sofaDevice->version = "4.0";
    sofaDevice->nodes.push_back(*asientoDerechaNode);
    sofaDevice->nodes.push_back(*asientoCentroNode);
    sofaDevice->nodes.push_back(*asientoIzquierdaNode);
}

extern "C" void app_main(void){
    ESP_LOGI(TAG, "Empiezo!");

    nvs_flash_init();

    inicializarHomieDevice();

    sofa = new Sofa(&derecha, &centro, &izquierda, GPIO_NUM_23, GPIO_NUM_22, sofaDevice);
}