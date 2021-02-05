#include <esp_types.h>
#include <driver/periph_ctrl.h>
#include <driver/timer.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/queue.h>

#define TIMER_DIVIDER 8000
#define TIMER_SCALE (TIMER_BASE_CLK / TIMER_DIVIDER)

typedef struct
{
	uint32_t group;
	uint32_t idx;
	uint64_t value;
} hdr_evt_t;

typedef struct
{
	xQueueHandle queue;
	timer_config_t timer0, timer1;	
} hdr_t;

extern hdr_t HDR;

int hdr_init();
int hdr_deinit();


