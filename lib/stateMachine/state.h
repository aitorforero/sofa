#ifndef STATE_DEFINED
#define STATE_DEFINED

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "homieDevice.h"

class State
{
	protected:
        QueueHandle_t machineEvents;
        homie_device *machineDevice;
	public:
		State(QueueHandle_t machine_events, homie_device* machine_device):machineEvents(machine_events),machineDevice(machine_device){};
		
		virtual void enter()=0;
		virtual void execute()=0;
		virtual void exit()=0;
		virtual const char * getName()=0;
		
		virtual ~State(){};
};

#endif