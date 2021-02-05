#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>
#include <freertos/task.h>

#include <lvgl.h>
#include <lvgl_helpers.h>

#include <gpuart.h>
#include <hdr.h>
#include <i2cbus.h>
#include <axp192.h>
#include <ft6336.h>

#define LV_TICK_PERIOD_MS 20

static struct
{
	SemaphoreHandle_t lock;
	TaskHandle_t main_task;
	esp_timer_handle_t periodic_timer;
	lv_style_t background;
	lv_obj_t* screen;
	void (*update)();

	lv_color_t buffer[DISP_BUF_SIZE][2];
	lv_disp_buf_t display_buffer;
} UI = {0};

static void ui_proc_tick(void* args)
{
	lv_tick_inc(LV_TICK_PERIOD_MS);
}

static void ui_task(void* args)
{
	for (;;)
	{
		vTaskDelay(1);
		if (xSemaphoreTake(UI.lock, (TickType_t) 10) == pdTRUE)
		{
			if (UI.update) UI.update();
			lv_task_handler();
			xSemaphoreGive(UI.lock);
		}
	}
}

int ui_init()
{
	UI.lock = xSemaphoreCreateMutex();

	lv_init();
	lvgl_driver_init();
	lv_disp_buf_init(&UI.display_buffer, UI.buffer[0], UI.buffer[1], DISP_BUF_SIZE);

	lv_disp_drv_t driver;
	lv_disp_drv_init(&driver);
	driver.flush_cb = disp_driver_flush;
	driver.buffer = &UI.display_buffer;
	lv_disp_drv_register(&driver);

	lv_style_t* s = &UI.background;
	lv_style_init(s);
	lv_style_set_border_side(s, LV_STATE_DEFAULT, LV_BORDER_SIDE_NONE);
	lv_style_set_outline_width(s, LV_STATE_DEFAULT, 0);
	lv_style_set_radius(s, LV_STATE_DEFAULT, 0);
	lv_style_set_bg_opa(s, LV_STATE_DEFAULT, LV_OPA_100);
	lv_style_set_bg_color(s, LV_STATE_DEFAULT, LV_COLOR_MAKE(0xFF, 0x00, 0x00));
	lv_style_set_text_font(s, LV_STATE_DEFAULT, &lv_font_unscii_8);
	lv_style_set_text_color(s, LV_STATE_DEFAULT, LV_COLOR_MAKE(0xFF, 0xFF, 0xFF));
	lv_style_set_line_rounded(s, LV_STATE_DEFAULT, true);

	UI.screen = lv_scr_act();
	lv_obj_add_style(UI.screen, LV_OBJ_PART_MAIN, &UI.background);
	lv_obj_set_size(UI.screen, 240, 320);
	lv_obj_set_pos(UI.screen, 0, 0);

	return ESP_OK;
}

int ui_start()
{
	if (!UI.periodic_timer)
	{
		const esp_timer_create_args_t args = {
			.callback = &ui_proc_tick,
			.name = "periodic ui timer",
		};
		ESP_ERROR_CHECK( esp_timer_create(&args, &UI.periodic_timer) );
		ESP_ERROR_CHECK( esp_timer_start_periodic(UI.periodic_timer, LV_TICK_PERIOD_MS * 1000) );
	}
	else
	{
		ESP_ERROR_CHECK(esp_timer_start_periodic(UI.periodic_timer, LV_TICK_PERIOD_MS * 1000) );
		lv_task_handler();
		lv_task_handler();
		ui_proc_tick(NULL);
	}
	xTaskCreatePinnedToCore(ui_task, "UI TASK", 4096, NULL, 0, &UI.main_task, 0);
	return ESP_OK;
}

int ui_stop()
{
	vTaskDelete(UI.main_task);
	return ESP_OK;
}

int ui_deinit()
{
	ui_stop();
	lv_deinit();
	vSemaphoreDelete(UI.lock);
	return ESP_OK;
}

void app_main()
{
#if 0
	GPUART1.init();
	GPUART2.init();

	int times = 1;
	while (times-->0)
	{
		GPUART1.write("hello\n", 6);
		vTaskDelay(10);
		
		GPUART2.write("byebye\n", 7);
		vTaskDelay(10);

	}

	GPUART1.deinit();
	GPUART2.deinit();

	//hdr_init();
#endif

	i2cbus_init(0, 21, 22, 1000000);
	axp192_init();
	ft6336_init();


	axp192_set_led(1);
	axp192_set_ldo(1,1);
	axp192_set_ldo(2,1);
	//axp192_set_ldo(3,1);

	int is_vbus = axp192_is_vbus();
	int is_acin = axp192_is_acin();
	int is_chrg = axp192_is_charging();

	ESP_LOGW(__func__, "%d %d %d", is_vbus, is_acin, is_chrg);

	// Initialize User Interface
	ui_init();
	ui_start();

	for(int i=0; 1; i=1-i) 
	{
		axp192_set_led(i);
		ft6336_update();

		int vbat = axp192_get_battery_voltage();
		int vin  = axp192_get_vin_voltage();
		int vbus = axp192_get_aps_voltage();

		ESP_LOGW(__func__, "Batt:%d  Vin:%d  Vbus:%d Touches:%d (%d,%d) (%d,%d)", vbat, vin, vbus, 
			FT6336.num_points, FT6336.point[0].x, FT6336.point[0].y, FT6336.point[1].x, FT6336.point[1].y);
		vTaskDelay(100); 
	}
	
}
