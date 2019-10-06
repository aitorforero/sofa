#ifndef SOFAIO_DEFINED
#define SOFAIO_DEFINED

    typedef struct {
        int boton_abrir;      /**< Pin pulsador abrir */
        int boton_cerrar;     /**< Pin pulsador cerrar*/
        int motor_abrir;      /**< Pin motor abrir */
        int motor_cerrar;     /**< Pin motor cerrar */
    } sofaio_asiento_t;

    typedef struct {
        sofaio_asiento_t asiento_derecha; /**< asiento derecha */
        sofaio_asiento_t asiento_centro; /**< asiento centro */
        sofaio_asiento_t asiento_izquierda; /**< asiento izquierda */
    } sofaio_sofa_t;

    void sofaIO_init(sofaio_sofa_t* sofa);
#endif