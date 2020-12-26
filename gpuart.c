#include <gpuart.h>

#define TAG "GPUART"

        //uart_write_bytes(SELF.uart.port, (const char*) data, len+4);
        //ESP_LOG_BUFFER_HEX(__func__, data, len+4);
	//vTaskDelay(1);
	//return ESP_OK;

void gpuart_rx_task(void* param)
{
        uart_event_t event;
	gpuart_t* self = (gpuart_t*)param;
        for (;;)
        {
                if(xQueueReceive(self->uart.queue, &event, (portTickType) portMAX_DELAY))
                {
                        switch (event.type)
                        {
                                case UART_DATA:
                                        uart_read_bytes(self->uart.port,
                                                        self->uart.rx_buffer, event.size,
                                                        portMAX_DELAY);
					ESP_LOGI(TAG, "%s", self->name);
                                        ESP_LOG_BUFFER_HEX(__func__, self->uart.rx_buffer, event.size);
                                        break;
                                case UART_FIFO_OVF:
					ESP_LOGI(TAG, "%s", self->name);
                                        ESP_LOGE(__func__, "UART FIFO OVF");
                                        uart_flush_input(self->uart.port);
                                        xQueueReset(self->uart.queue);
                                        break;
                                case UART_BUFFER_FULL:
					ESP_LOGI(TAG, "%s", self->name);
                                        ESP_LOGE(__func__, " UART_BUFFER FULL");
                                        uart_flush_input(self->uart.port);
                                        xQueueReset(self->uart.queue);
                                        break;
                                case UART_BREAK:
					ESP_LOGI(TAG, "%s", self->name);
                                        ESP_LOGW(__func__, "UART BREAK");
                                        break;
                                case UART_PARITY_ERR:
                                case UART_FRAME_ERR:
                                case UART_PATTERN_DET:
                                default:
					ESP_LOGI(TAG, "%s", self->name);
                                        ESP_LOGW(TAG, "Unhandled event %d of size %d",
                                                        event.type, event.size);
                                        break;
                        }
                }
        }
}

static int s_init(gpuart_t* self)
{
        ESP_ERROR_CHECK(uart_param_config(self->uart.port, &self->uart.config));

        ESP_ERROR_CHECK(uart_set_pin(self->uart.port,
                                self->uart.pins.TX,
                                self->uart.pins.RX,
                                UART_PIN_NO_CHANGE,
                                UART_PIN_NO_CHANGE));

        ESP_ERROR_CHECK(uart_driver_install(self->uart.port,
                                self->uart.rx_buffer_size,
                               	self->uart.tx_buffer_size,
                                self->uart.task_queue_size, 
				&self->uart.queue, 0));

	if (self->uart.rx_buffer_size > 0)
		self->uart.rx_buffer = malloc(self->uart.rx_buffer_size);

	if (self->uart.tx_buffer_size > 0)
		self->uart.tx_buffer = malloc(self->uart.tx_buffer_size);

	
        xTaskCreate(gpuart_rx_task, self->name,
			self->uart.task_stack_size, 
			self, 12, &self->uart.task);
	return ESP_OK;
}

static int s_deinit(gpuart_t* self)
{
        vTaskDelete(self->uart.task);
        uart_driver_delete(self->uart.port);
	if (self->uart.rx_buffer) free(self->uart.rx_buffer);
	if (self->uart.tx_buffer) free(self->uart.tx_buffer);
        return ESP_OK;
}

static int s_write(gpuart_t* self, void* buffer, int len)
{
        return uart_write_bytes(self->uart.port, (const char*) buffer, len);
}


static int s_init1()
{
	return s_init(&GPUART1);
}

static int s_init2()
{
	return s_init(&GPUART2);
}

static int s_deinit1()
{
	return s_deinit(&GPUART1);
}

static int s_deinit2()
{
	return s_deinit(&GPUART2);
}

static int s_write1(void* buffer, int len) 
{
	return s_write(&GPUART1, buffer, len);
}

static int s_write2(void* buffer, int len)
{
	return s_write(&GPUART2, buffer, len);
}

#if CONFIG_LMTZ_GPUART1_ENABLE
gpuart_t GPUART1 = { 
	.name = "GPUART1",
	.uart = {
		.port = GPUART1_UART_PORT,
		.config = {
			.baud_rate = GPUART1_BAUD_RATE,
               		.data_bits = UART_DATA_8_BITS,
                	.parity = UART_PARITY_DISABLE,
                	.stop_bits = UART_STOP_BITS_1,
                	.flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
		},
		.pins = {
			.TX = GPUART1_PIN_TX,
			.RX = GPUART1_PIN_RX,
		},
		.rx_buffer_size = GPUART1_RX_BUFFER_SIZE,
		.tx_buffer_size = GPUART1_TX_BUFFER_SIZE,
		.task_stack_size = GPUART1_TASK_STACK_SIZE,
		.task_queue_size = GPUART1_TASK_QUEUE_SIZE,
	},

	.init   = s_init1,
	.write = s_write1,
	.deinit = s_deinit1,
};
#endif
#if CONFIG_LMTZ_GPUART2_ENABLE
gpuart_t GPUART2 = { 
	.name = "GPUART2",
	.uart = {
		.port = GPUART2_UART_PORT,
		.config = {
			.baud_rate = GPUART2_BAUD_RATE,
               		.data_bits = UART_DATA_8_BITS,
                	.parity = UART_PARITY_DISABLE,
                	.stop_bits = UART_STOP_BITS_1,
                	.flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
		},
		.pins = {
			.TX = GPUART2_PIN_TX,
			.RX = GPUART2_PIN_RX,
		},
		.rx_buffer_size = GPUART2_RX_BUFFER_SIZE,
		.tx_buffer_size = GPUART2_TX_BUFFER_SIZE,
		.task_stack_size = GPUART2_TASK_STACK_SIZE,
		.task_queue_size = GPUART2_TASK_QUEUE_SIZE,
	},
	.init   = s_init2,
	.write = s_write2,
	.deinit = s_deinit2,
};
#endif
