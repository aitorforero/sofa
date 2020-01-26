#pragma once

#include "freertos/FreeRTOS.h"
#include "driver/gpio.h"

#include "asiento_state_machine.hpp"


class Asiento {
    private:
        gpio_num_t _pin_boton_abrir;
        gpio_num_t _pin_boton_cerrar;
        gpio_num_t _pin_motor_abrir;
        gpio_num_t _pin_motor_cerrar;

        AsientoStateMachine* state_machine;


    public:
        Asiento(gpio_num_t pin_boton_abrir, gpio_num_t pin_boton_cerrar, gpio_num_t pin_motor_abrir, gpio_num_t pin_motor_cerrar);
        void abrir();
        void cerrar();
        void parar();

        AsientoStateMachine* getStateMachine();

        gpio_num_t getPinBotonAbrir();
        gpio_num_t getPinBotonCerrar();
        gpio_num_t getPinMotorAbrir();
        gpio_num_t getPinMotorCerrar();

        void initializeStateMachine(Sofa * sofa);
};