#ifndef SOFAIO_DEFINED
#define SOFAIO_DEFINED

    typedef struct {
        int pin;      /**< Pin pulsador abrir */
        bool pulsado;     /**< Pin pulsador cerrar*/
    } sofaio_boton_t;

    typedef struct {
        sofaio_boton_t boton_abrir;      /**< Pin pulsador abrir */
        sofaio_boton_t boton_cerrar;     /**< Pin pulsador cerrar*/
        int motor_abrir;      /**< Pin motor abrir */
        int motor_cerrar;     /**< Pin motor cerrar */
    } sofaio_asiento_t;

    typedef struct {
        sofaio_asiento_t asiento_derecha; /**< asiento derecha */
        sofaio_asiento_t asiento_centro; /**< asiento centro */
        sofaio_asiento_t asiento_izquierda; /**< asiento izquierda */
    } sofaio_sofa_t;

    void sofaIO_init(sofaio_sofa_t* sofa, QueueHandle_t events);
#endif