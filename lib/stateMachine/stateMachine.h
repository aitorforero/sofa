#ifndef STATE_MACHINE_DEFINED
#define STATE_MACHINE_DEFINED

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"

#include "homieDevice.h"

typedef enum
{
    boton_abrir_pulsado,
    boton_abrir_levantado,
    boton_cerrar_pulsado,
    boton_cerrar_levantado,
    wifi_conectado,
    wifi_desconectado,
    mqtt_conectado,
    mqtt_desconectado,
    mqtt_mensaje_abrir,
    mqtt_mensaje_cerrar,
    mqtt_mensaje_parar,
    mqtt_suscrito,
    last_event
} event_t;

typedef enum
{
    init_wifi,
    init_mqtt,
    init_device,
    idle,
    abrir_manual,
    cerrar_manual,
    abrir_auto,
    cerrar_auto,
    last_state
} state_name_t;


QueueHandle_t machineEvents;
homie_device *machineDevice;

struct state_s;

typedef void (*handle_function_t)(event_t, struct state_s**);
typedef void (*enter_function_t)(void);
typedef void (*exit_function_t)(void);

typedef struct state_s
{
    state_name_t nombre;
    handle_function_t handle;
    enter_function_t enter;
    exit_function_t exit;
} state_t;


void stateMachine_init(QueueHandle_t eventQueue, homie_device *device);
void raise(event_t event);
void raise_mqtt(char *topic, char *data);

#endif