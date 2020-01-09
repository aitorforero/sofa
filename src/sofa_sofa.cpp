#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_log.h"

#include "sofa_asiento.hpp"
#include "sofa_sofa.hpp"

#define ESP_INTR_FLAG_DEFAULT 0
static const char *TAG = "SOFA_SOFA";


static void IRAM_ATTR button_isr_handler(void* arg){
    BaseType_t woken = pdFALSE;
    SofaEventArgs* sofaEventArgs = (SofaEventArgs*) arg;
    sofaEventArgs->sofa->onButtonPressed(sofaEventArgs->pin, &woken);

    if(woken){
      portYIELD_FROM_ISR();
    }
}

static void button_task( void * pvParameters ){
  Sofa* sofa = (Sofa*) pvParameters;
  sofa->buttonTask();
}

Sofa::Sofa(Asiento* derecha,Asiento* centro,Asiento* izquierda, QueueHandle_t state_machine_event_queue): _derecha(derecha), _centro(centro), _izquierda(izquierda), _state_machine_event_queue(state_machine_event_queue){
  ESP_LOGI(TAG, "Inicializando sofa...");

  event_queue = xQueueCreate(10,sizeof(int));

  this->_derecha_abrir_event_args = new SofaEventArgs(this, this->getDerecha()->getPinBotonAbrir());
  this->_derecha_cerrar_event_args = new SofaEventArgs(this, this->getDerecha()->getPinBotonCerrar());
  this->_centro_abrir_event_args = new SofaEventArgs(this, this->getCentro()->getPinBotonAbrir());
  this->_centro_cerrar_event_args = new SofaEventArgs(this, this->getCentro()->getPinBotonCerrar());
  this->_izquierda_abrir_event_args = new SofaEventArgs(this, this->getIzquierda()->getPinBotonAbrir());
  this->_izquierda_cerrar_event_args = new SofaEventArgs(this, this->getIzquierda()->getPinBotonCerrar());
  

  this->inicializa_entradas();
  this->inicializa_salidas();

  ESP_LOGI(TAG, "Sofa inicializado");

};

Sofa::~Sofa() {
  delete this->_derecha_abrir_event_args;
  delete this->_derecha_cerrar_event_args;
  delete this->_centro_abrir_event_args;
  delete this->_centro_cerrar_event_args;
  delete this->_izquierda_abrir_event_args;
  delete this->_izquierda_cerrar_event_args;
}

void Sofa::onButtonPressed(gpio_num_t pin, BaseType_t* pWoken){

    xQueueSendToBackFromISR(this->event_queue, (void*)pin, pWoken);
}

void Sofa::buttonTask(){
  for( ;; ) {
      int pin;
      if(xQueueReceive(this->event_queue, &pin, 0)){
         if(this->getDerecha()->getPinBotonAbrir()==pin) {
           // Boton derecha abrir pulsado
         } else if(this->getDerecha()->getPinBotonCerrar()==pin) {
           // Boton derecha cerrar pulsado
         } else if(this->getCentro()->getPinBotonAbrir()==pin) {
           // Boton derecha cerrar pulsado
         } else if(this->getCentro()->getPinBotonCerrar()==pin) {
           // Boton derecha cerrar pulsado
         } else if(this->getIzquierda()->getPinBotonAbrir()==pin) {
           // Boton derecha cerrar pulsado
         } else if(this->getIzquierda()->getPinBotonCerrar()==pin) {
           // Boton derecha cerrar pulsado
         }

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
                          (1ULL<<this->getIzquierda()->getPinMotorCerrar()) ;

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


  // Start Task that will handle the Pushbutton
  xTaskCreate(button_task, "button task", 2048, NULL, 10, NULL);

  //install gpio isr service
  gpio_install_isr_service(ESP_INTR_FLAG_DEFAULT);
  //hook isr handler for specific gpio pin

  SofaEventArgs(this, this->getDerecha()->getPinBotonAbrir());

  gpio_isr_handler_add(this->getDerecha()->getPinBotonAbrir(), button_isr_handler, (void*)this->_derecha_abrir_event_args);
  gpio_isr_handler_add(this->getDerecha()->getPinBotonCerrar(), button_isr_handler, (void*)this->_derecha_cerrar_event_args);
  gpio_isr_handler_add(this->getCentro()->getPinBotonAbrir(), button_isr_handler, (void*)this->_centro_abrir_event_args);
  gpio_isr_handler_add(this->getCentro()->getPinBotonCerrar(), button_isr_handler, (void*)this->_centro_cerrar_event_args);
  gpio_isr_handler_add(this->getIzquierda()->getPinBotonAbrir(), button_isr_handler, (void*)this->_izquierda_abrir_event_args);
  gpio_isr_handler_add(this->getIzquierda()->getPinBotonCerrar(), button_isr_handler, (void*)this->_izquierda_cerrar_event_args);
  ESP_LOGI(TAG, "Entradas inicializadas");
}