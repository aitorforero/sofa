#ifndef STATE_MACHINE_DEFINED
#define STATE_MACHINE_DEFINED

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "homieDevice.h"

typedef enum
{
    asiento_derecha = 0x000,
    asiento_centro = 0x010,
    asiento_izquierda = 0x020,

    boton_abrir_pulsado = 0x000,
    boton_abrir_levantado = 0x001,
    boton_cerrar_pulsado = 0x002,
    boton_cerrar_levantado = 0x003,
    mqtt_mensaje_abrir = 0x004,
    mqtt_mensaje_cerrar = 0x005,
    mqtt_mensaje_parar = 0x006,

    wifi_desconectado = 0x100,
    wifi_conectado = 0x101,

    mqtt_desconectado = 0x200,
    mqtt_conectado = 0x201,
    mqtt_suscrito = 0x202,

    evento_no_valido = 0xFFF
} event_t;

typedef enum
{
    init_wifi,
    init_mqtt,
    init_device,
    conectado,
    last_state
} state_name_t;


typedef enum
{
    idle,
    abrir_manual,
    cerrar_manual,
    abrir_auto,
    cerrar_auto,
    last_asiento_state
} asiento_state_name_t;


QueueHandle_t machineEvents; 
homie_device *machineDevice;

struct asiento_state_s;

typedef void (*asiento_handle_function_t)(event_t, asiento, struct asiento_state_s**);
typedef void (*asiento_enter_function_t)(struct asiento_state_s*);
typedef void (*asiento_exit_function_t)(struct asiento_state_s*);


typedef struct asiento_state_s
{
    asiento_state_name_t nombre;
    asiento_handle_function_t handle;
    asiento_enter_function_t enter;
    asiento_exit_function_t exit;
} asiento_state_t;

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