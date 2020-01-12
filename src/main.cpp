#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "nvs_flash.h"
#include "esp_log.h"

#include "sofa_asiento.hpp"
#include "sofa_sofa.hpp"
#include "sofa_homie_device.hpp"

// #include "homieDevice.h"
// #include "Wifi.h"
// #include "mqtt.h" 
// #include "stateMachine.h"
// #include "initWifiState.h"

static const char *TAG = "SOFA_MAIN";



// homie_property asiento_properties[1] = 
// {
//     {
//         .propertyID = "state",
//         .name = "State",
//         .datatype = Enum,
//         .format = "UP,DOWN,STOP",
//         .settable = true,
//         .retained = true,
//         .unit = "",
//         .initialValue = "STOP"
//     }
// };

// homie_node asientos[3] =
// {
//     {
//         .nodeID = "ad",
//         .name = "Asiento derecha",
//         .nodetype = "asiento",
//         .properties =  asiento_properties
//     },
//     {
//         .nodeID = "ac",
//         .name = "Asiento centro",
//         .nodetype = "asiento",
//         .properties =  asiento_properties
//     },
//     {
//         .nodeID = "ai",
//         .name = "Asiento izquierda",
//         .nodetype = "asiento",
//         .properties =  asiento_properties
//     }
// };

// homie_device device = 
// {
//     .version = "4.0",
//     .deviceID = "A4CF126C25BC",
//     .name = "Sofa sala",
//     .nodes = asientos
// };


// state_t *currentState;

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
    asientoDerechaNode->name = "Asiento derecha",
    asientoDerechaNode->nodetype = "asiento",
    asientoDerechaNode->properties = new HomieProperty[1];
    asientoDerechaNode->properties[0] =  *stateProperty;

    asientoCentroNode = new HomieNode();
    asientoCentroNode->nodeID = "ac",
    asientoCentroNode->name = "Asiento centro",
    asientoCentroNode->nodetype = "asiento",
    asientoCentroNode->properties = new HomieProperty[1];
    asientoCentroNode->properties[0] =  *stateProperty;

    asientoIzquierdaNode = new HomieNode();
    asientoIzquierdaNode->nodeID = "ai",
    asientoIzquierdaNode->name = "Asiento izquierda",
    asientoIzquierdaNode->nodetype = "asiento",
    asientoIzquierdaNode->properties = new HomieProperty[1];
    asientoIzquierdaNode->properties[0] =  *stateProperty;

    sofaDevice = new HomieDevice();
    sofaDevice->deviceID = "A4CF126C25BC";
    sofaDevice->name = "Sofa sala";
    sofaDevice->version = "4.0";
    sofaDevice->nodes = new HomieNode[3]; 
    sofaDevice->nodes[0] = *asientoDerechaNode;
    sofaDevice->nodes[1] = *asientoCentroNode;
    sofaDevice->nodes[2] = *asientoIzquierdaNode;
}

extern "C" void app_main(void){
    ESP_LOGI(TAG, "Empiezo!");

    nvs_flash_init();

    inicializarHomieDevice();

    sofa = new Sofa(&derecha, &centro, &izquierda, GPIO_NUM_23, sofaDevice);
    sofa->encenderOK();
}