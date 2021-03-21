#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>
#include <freertos/task.h>

#include <esp_system.h>
#include <esp_log.h>

#include <i2cbus.h>
#include <axp192.h>
#include <ft6336.h>
#include <ws2812.h>
#include <lvgl.h>
#include <lvgl_helpers.h>

#define LV_TICK_PERIOD_MS 1

static struct
{
	TaskHandle_t task;
	lv_obj_t* root;
	struct
	{
		char text[32];
		lv_obj_t* handle;
	} voltage, percentage;
} UI;

static void buzzer(int ms)
{
	axp192_set_ldo(3,1);
	vTaskDelay(ms / portTICK_PERIOD_MS);
	axp192_set_ldo(3,0);
}

static void blink(int ms)
{
	axp192_set_led(1);
	vTaskDelay(ms / portTICK_PERIOD_MS); 
	axp192_set_led(0);
}

static void gui_timer_tick(void* arg)
{
	(void)arg;
	lv_tick_inc(LV_TICK_PERIOD_MS);
}

static void gui_task(void* arg)
{
	(void)arg;

	static lv_color_t buffer[2][DISP_BUF_SIZE];
	static lv_disp_buf_t display_buffer;
	lv_disp_buf_init(&display_buffer, &buffer[0], &buffer[1], DISP_BUF_SIZE);

	lv_disp_drv_t display_driver;
	lv_disp_drv_init(&display_driver);
	display_driver.flush_cb = disp_driver_flush;
	display_driver.buffer = &display_buffer;
	lv_disp_drv_register(&display_driver);

#if 0
	lv_indev_drv_t touch_driver;
	lv_index_drv_init(&touch_driver);
	touch_driver.read_cb = touch_driver_read;
	touch_driver.type = LV_INDEV_TYPE_POINTER;
	lv_indev_drv_register(&touch_driver);
#endif

	const esp_timer_create_args_t periodic_args = {
		.callback = &gui_timer_tick,
		.name = "Periodic GUI",
	};
	esp_timer_handle_t periodic_timer;
	ESP_ERROR_CHECK( esp_timer_create(&periodic_args, &periodic_timer) );
	ESP_ERROR_CHECK( esp_timer_start_periodic(periodic_timer, LV_TICK_PERIOD_MS * 1000) );

	UI.root = lv_cont_create(lv_scr_act(), NULL);
	lv_obj_set_size(UI.root, 320, 240);
	lv_obj_set_click(UI.root, false);

	UI.voltage.handle = lv_label_create(UI.root, NULL);
	lv_label_set_text(UI.voltage.handle, "SPLENDO");
	lv_obj_set_pos(UI.voltage.handle, 10, 10);

	for (;;)
	{
		vTaskDelay(1000 / portTICK_PERIOD_MS);
		int voltage = axp192_get_battery_voltage();
		int charging= axp192_is_charging();

		sprintf(UI.voltage.text, "%s %d", charging ? "CHG" : "BAT", voltage);
		lv_label_set_text(UI.voltage.handle, UI.voltage.text);
		lv_task_handler();	
	}
}

void app_main()
{
	i2cbus_init(0, 21, 22, 1000000);
	axp192_init();
	ft6336_init();
	ws2812_init();

	axp192_set_led(1);
	axp192_set_ldo(1,1);
	axp192_set_ldo(2,1);

	buzzer(100);

	int is_vbus = axp192_is_vbus();
	int is_acin = axp192_is_acin();
	int is_chrg = axp192_is_charging();

	ESP_LOGW(__func__, "%d %d %d", is_vbus, is_acin, is_chrg);

	lv_init();
	lvgl_driver_init();

	UI.task = xTaskCreatePinnedToCore(gui_task, "GUI", 8192, NULL, 0, NULL, 1);

	rgbVal pixel[10];

	for (int i=0; i<10; i++)
	{
		pixel[i].r =   0 + i * 25;
		pixel[i].g =  80 + i * 25;
		pixel[i].b = 160 + i * 25;
	}

	int isTouching = 0;
	for(int i=0; 1; i=1-i) 
	{
		axp192_set_led(i);
		ft6336_update();

		for (int i=0; i<10; i++)
		{
			pixel[i].r += 10;
			pixel[i].g += 10;
			pixel[i].b += 10;
		}
		ws2812_update(pixel, 10);

		ESP_LOGW(__func__, "Touches:%d (%d,%d) (%d,%d)",
			FT6336.num_points, 
			FT6336.point[0].x, FT6336.point[0].y, 
			FT6336.point[1].x, FT6336.point[1].y);
		vTaskDelay(100 / portTICK_PERIOD_MS); 
	}
	
}
