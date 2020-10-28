#include <ms12a.h>

void app_main()
{
	MS12A.init();
	MS12A.tare();

	int times = 3;
	while (times-->0)
	{
		MS12A.servos[0].led = 
		MS12A.servos[1].led = 
		MS12A.servos[2].led = 0xFF8800;
		
		MS12A.servos[0].target.speed =
		MS12A.servos[1].target.speed = 
		MS12A.servos[2].target.speed = 0;

		for (int i=0; i<30; i++)
		{
			MS12A.servos[0].target.angle += 2;
			MS12A.servos[1].target.angle -= 1;
			MS12A.servos[2].target.angle += 1;
			MS12A.update();
			vTaskDelay(20);
		}

		ESP_LOGI("MS12A EXAMPLE", "");
		MS12A.servos[0].target.angle =
		MS12A.servos[1].target.angle = 
		MS12A.servos[2].target.angle = 0;
		MS12A.update();

		vTaskDelay(100);
	}

	MS12A.deinit();
}
