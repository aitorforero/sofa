#include "freertos/FreeRTOS.h"
#include "driver/gpio.h"

#pragma once


class Asiento {
    private:
        gpio_num_t _pin_boton_abrir;
        gpio_num_t _pin_boton_cerrar;
        gpio_num_t _pin_motor_abrir;
        gpio_num_t _pin_motor_cerrar;

    public:
        Asiento(gpio_num_t pin_boton_abrir, gpio_num_t pin_boton_cerrar, gpio_num_t pin_motor_abrir, gpio_num_t pin_motor_cerrar);
        void abrir();
        void cerrar();
        void parar();

        gpio_num_t getPinBotonAbrir();
        gpio_num_t getPinBotonCerrar();
        gpio_num_t getPinMotorAbrir();
        gpio_num_t getPinMotorCerrar();
};