#pragma once
#include <stdint.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include <esp_system.h>
#include <esp_log.h>
#include "driver/uart.h"

#define GPUART1_RX_BUFFER_SIZE CONFIG_LMTZ_GPUART1_RX_BUFFER_SIZE
#define GPUART1_TX_BUFFER_SIZE CONFIG_LMTZ_GPUART1_TX_BUFFER_SIZE
#define GPUART1_TASK_QUEUE_SIZE CONFIG_LMTZ_GPUART1_TASK_QUEUE_SIZE
#define GPUART1_TASK_STACK_SIZE CONFIG_LMTZ_GPUART1_TASK_STACK_SIZE

#define GPUART1_UART_PORT CONFIG_LMTZ_GPUART1_UART
#define GPUART1_PIN_TX CONFIG_LMTZ_GPUART1_PIN_TX
#define GPUART1_PIN_RX CONFIG_LMTZ_GPUART1_PIN_RX
#define GPUART1_BAUD_RATE CONFIG_LMTZ_GPUART1_BAUDRATE
#define GPUART1_BUFFER_SIZE 64

#define GPUART2_RX_BUFFER_SIZE CONFIG_LMTZ_GPUART2_RX_BUFFER_SIZE
#define GPUART2_TX_BUFFER_SIZE CONFIG_LMTZ_GPUART2_TX_BUFFER_SIZE
#define GPUART2_TASK_QUEUE_SIZE CONFIG_LMTZ_GPUART2_TASK_QUEUE_SIZE
#define GPUART2_TASK_STACK_SIZE CONFIG_LMTZ_GPUART2_TASK_STACK_SIZE

#define GPUART2_UART_PORT CONFIG_LMTZ_GPUART2_UART
#define GPUART2_PIN_TX CONFIG_LMTZ_GPUART2_PIN_TX
#define GPUART2_PIN_RX CONFIG_LMTZ_GPUART2_PIN_RX
#define GPUART2_BAUD_RATE CONFIG_LMTZ_GPUART2_BAUDRATE
#define GPUART2_BUFFER_SIZE 64

// DRIVER
typedef struct
{
	struct
	{
		uint8_t port;
		struct { uint8_t TX, RX; } pins;
		uart_config_t config;
        	uint8_t* rx_buffer;
		uint8_t* tx_buffer;
        	TaskHandle_t task;
        	QueueHandle_t queue;

		int rx_buffer_size;
		int tx_buffer_size;
		int task_queue_size;
		int task_stack_size;
	} uart;


	const char* name;
	int (*init)();
	int (*deinit)();

	int (*write)(void* buffer, int len);
	
} gpuart_t;

#if CONFIG_LMTZ_GPUART1_ENABLE
extern gpuart_t GPUART1;
#endif

#if CONFIG_LMTZ_GPUART2_ENABLE
extern gpuart_t GPUART2;
#endif

