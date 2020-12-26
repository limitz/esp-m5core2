#include <gpuart.h>

void app_main()
{
	GPUART1.init();
	GPUART2.init();

	int times = 3;
	while (times-->0)
	{
		GPUART1.write("hello\n", 6);
		vTaskDelay(100);
		
		GPUART2.write("byebye\n", 7);
		vTaskDelay(100);

	}

	GPUART1.deinit();
	GPUART2.deinit();
}
