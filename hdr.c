#include "hdr.h"
#include <driver/gpio.h>
#include <esp_log.h>

hdr_t HDR = {
	.timer0 = {
		.divider = TIMER_DIVIDER,
		.counter_dir = TIMER_COUNT_UP,
		.counter_en = TIMER_PAUSE,
		.alarm_en = TIMER_ALARM_EN,
		.auto_reload = 1,
	},
	.timer1 = {
		.divider = TIMER_DIVIDER,
		.counter_dir = TIMER_COUNT_UP,
		.counter_en = TIMER_PAUSE,
		.alarm_en = TIMER_ALARM_EN,
		.auto_reload = 1,
	},
};

void IRAM_ATTR timer_group0_isr(void* param)
{
	static uint64_t exp = 10000;

	timer_spinlock_take(TIMER_GROUP_0);
	int timer = (int) param;

	uint32_t intr = timer_group_get_intr_status_in_isr(TIMER_GROUP_0);
	uint64_t cval = timer_group_get_counter_value_in_isr(TIMER_GROUP_0, timer);

	if (intr & TIMER_INTR_T0)
	{
		// GPIO 0
		gpio_set_level(19, 0);
		timer_group_clr_intr_status_in_isr(TIMER_GROUP_0, TIMER_0);
		timer_group_set_counter_enable_in_isr(TIMER_GROUP_0, TIMER_0, TIMER_PAUSE);
		exp += 10000;
		if (exp > 40000) exp = 10000;
	}
	else if (intr & TIMER_INTR_T1)
	{
		//GPIO 1
		gpio_set_level(19, 1);
		timer_group_clr_intr_status_in_isr(TIMER_GROUP_0, TIMER_1);
		timer_group_set_alarm_value_in_isr(TIMER_GROUP_0, TIMER_0, exp);
		timer_group_set_counter_enable_in_isr(TIMER_GROUP_0, TIMER_0, TIMER_START);
		timer_group_enable_alarm_in_isr(TIMER_GROUP_0, TIMER_0);
		timer_group_enable_alarm_in_isr(TIMER_GROUP_0, TIMER_1);
	}
	hdr_evt_t evt = { .group = 0, .idx = timer, .value = cval };
	timer_spinlock_give(TIMER_GROUP_0);
	
	//bool woken = 0
	xQueueSendFromISR(HDR.queue, &evt, NULL);
	//return woken == pdTRUE ? 1 : 0;
}

static void hdr_task(void* arg)
{
	for (;;)
	{
		hdr_evt_t evt;
		xQueueReceive(HDR.queue, &evt, portMAX_DELAY);
		ESP_LOGI(__func__, "G:%d T:%d V:%llu", evt.group, evt.idx, evt.value);
	}
}

int hdr_init()
{
	HDR.queue = xQueueCreate(10, sizeof(hdr_evt_t));
	xTaskCreate(hdr_task, "hdr task", 4096, NULL, 5, NULL);

	gpio_config_t gpio = {
		.pin_bit_mask = 1ULL << 19,
		.mode = GPIO_MODE_OUTPUT,
	};

	gpio_config(&gpio);
	
	timer_init(TIMER_GROUP_0, TIMER_0, &HDR.timer0);
	timer_init(TIMER_GROUP_0, TIMER_1, &HDR.timer1);
	timer_set_counter_value( TIMER_GROUP_0, TIMER_0, 0x0ULL);
	timer_set_counter_value( TIMER_GROUP_0, TIMER_1, 0x0ULL);
	timer_set_alarm_value( TIMER_GROUP_0, TIMER_0, 10000); //2 * TIMER_SCALE);
	timer_set_alarm_value( TIMER_GROUP_0, TIMER_1, 50000); //3 * TIMER_SCALE);

	timer_enable_intr(TIMER_GROUP_0, TIMER_0);
	timer_enable_intr(TIMER_GROUP_0, TIMER_1);
	timer_isr_register(TIMER_GROUP_0, TIMER_0, timer_group0_isr, (void*) TIMER_0, ESP_INTR_FLAG_IRAM, NULL);
	timer_isr_register(TIMER_GROUP_0, TIMER_1, timer_group0_isr, (void*) TIMER_1, ESP_INTR_FLAG_IRAM, NULL);

	gpio_set_level(19, 1);

	timer_start(TIMER_GROUP_0, TIMER_0);
	timer_start(TIMER_GROUP_0, TIMER_1);
	return ESP_OK;
}

int hdr_deinit()
{
	return ESP_OK;
}
