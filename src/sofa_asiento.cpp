#include "freertos/FreeRTOS.h"
#include "driver/gpio.h"
#include "sofa_sofa.hpp"
#include "sofa_asiento.hpp"
#include "asiento_state_machine.hpp"

Asiento::Asiento(gpio_num_t pin_boton_abrir, gpio_num_t pin_boton_cerrar, gpio_num_t pin_motor_abrir, gpio_num_t pin_motor_cerrar):
    _pin_boton_abrir(pin_boton_abrir),    
    _pin_boton_cerrar(pin_boton_cerrar),    
    _pin_motor_abrir(pin_motor_abrir),    
    _pin_motor_cerrar(pin_motor_cerrar){};

void Asiento::abrir(){
    this->parar();
    gpio_set_level(this->_pin_motor_abrir, 0);  
};

void Asiento::cerrar(){
    this->parar();
    gpio_set_level(this->_pin_motor_cerrar, 0);
};

void Asiento::parar(){
    gpio_set_level(this->_pin_motor_abrir, 1);  
    gpio_set_level(this->_pin_motor_cerrar, 1);
};

gpio_num_t Asiento::getPinBotonAbrir(){ return this->_pin_boton_abrir;};

gpio_num_t Asiento::getPinBotonCerrar(){ return this->_pin_boton_cerrar;};

gpio_num_t Asiento::getPinMotorAbrir(){ return this->_pin_motor_abrir;};

gpio_num_t Asiento::getPinMotorCerrar(){ return this->_pin_motor_cerrar;};

void Asiento::initializeStateMachine(Sofa * sofa){
  state_machine = new AsientoStateMachine(sofa, this);
  state_machine->start();
}

AsientoStateMachine* Asiento::getStateMachine(){
  return state_machine;
}