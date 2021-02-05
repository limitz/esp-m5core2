#include <gpuart.h>
#include <hdr.h>
#include <i2cbus.h>
#include <axp192.h>
#include <ft6336.h>

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
	//axp192_set_ldo(1,1);
	axp192_set_ldo(2,1);
	//axp192_set_ldo(3,1);

	int is_vbus = axp192_is_vbus();
	int is_acin = axp192_is_acin();
	int is_chrg = axp192_is_charging();

	ESP_LOGW(__func__, "%d %d %d", is_vbus, is_acin, is_chrg);


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
