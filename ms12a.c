#include <ms12a.h>

#define TAG "MS12A"
#define SELF MS12A

inline void ms12a_store_7bit(uint8_t* data, size_t len, uint32_t v)
{
        while (len--)
        {
                *(data++) = v & 0x7F;
                v >>= 7;
        }
}

inline uint32_t ms12a_load_7bit(uint8_t* data, size_t len)
{
        uint32_t result = 0;
        while (len--)
        {
                result <<= 7;
                result |= data[len] & 0x7F;
        }
        return result;
}

static int ms12a_write_message(uint8_t dst, const uint8_t* message,int len)
{
        uint8_t data[32];
        uint8_t hash = 0;
        data[0] = MS12A_SYSEX_START;
        data[1] = hash = dst;
        for (int i=0; i<len; i++) hash += data[2+i] = message[i];
        data[len+2] = hash & 0x7F;
        data[len+3] = MS12A_SYSEX_STOP;
        uart_write_bytes(SELF.uart.port, (const char*) data, len+4);
        ESP_LOG_BUFFER_HEX(__func__, data, len+4);
	vTaskDelay(1);
	return ESP_OK;
}
#define _write(dst, message) \
        ms12a_write_message(dst, (uint8_t*) message, sizeof(message))

#define _load7(message, offset, nbytes) \
        ms12a_load_7bit(((uint8_t*)message) + offset, nbytes)

#define _store7(message, offset, nbytes, value) \
        ms12a_store_7bit(((uint8_t*)message)+offset, nbytes, (uint32_t) value)



#define PLACEHOLDER_U32 0x00, 0x00, 0x00, 0x00, 0x00
#define PLACEHOLDER_U16 0x00, 0x00, 0x00
#define PLACEHOLDER_U14 0x00, 0x00
#define PLACEHOLDER_U8  0x00, 0x00
#define PLACEHOLDER_U7  0x00

#define PLACEHOLDER_I32 PLACEHOLDER_U32
#define PLACEHOLDER_I16 PLACEHOLDER_U16
#define PLACEHOLDER_I14 PLACEHOLDER_U14
#define PLACEHOLDER_I8  PLACEHOLDER_U8
#define PLACEHOLDER_I7  PLACEHOLDER_U7

int ms12a_assign_ids()
{
	for (int i=0; i<SELF.count; i++) SELF.servos[i].id = i + 1;
        uint8_t message[] = { MS12A_CTL_ASSIGN_ID, 0x00 };
        int res = _write(MS12A_ALL, message);
	return res;
}

int ms12a_set_angle_abs(uint8_t servo, int32_t angle, uint16_t speed)
{
        uint8_t message[] = { MS12A_SERVO, MS12A_CMD_SET_ANGLE_ABS_LONG,
                                PLACEHOLDER_I32, // angle
                                PLACEHOLDER_U14, // speed
        };

        _store7(message, 2, +5, angle);
        _store7(message, 7, +2, speed);
        return _write(servo, message);
}


int ms12a_set_angle_rel(uint8_t servo, int32_t angle, uint16_t speed)
{
        uint8_t message[] = { MS12A_SERVO, MS12A_CMD_SET_ANGLE_REL_LONG,
                                PLACEHOLDER_I32, // angle
                                PLACEHOLDER_U14, // speed
        };

        _store7(message, 2, +5, angle);
        _store7(message, 7, +2, speed);
        return _write(servo, message);
}

int ms12a_set_angle_tare(uint8_t servo)
{
        uint8_t message[] = { MS12A_SERVO, MS12A_CMD_SET_ANGLE_TARE };
        return _write(servo, message);
}
int ms12a_set_break(uint8_t servo, uint7_t value)
{
        uint8_t message[] = { MS12A_SERVO, MS12A_CMD_SET_BREAK,
                                PLACEHOLDER_U7, // value
        };
        _store7(message, 2, +1, value);
        return _write(servo, message);
}

int ms12a_set_led(uint8_t servo, uint8_t r, uint8_t g, uint8_t b)
{
        uint8_t message[] = { MS12A_SERVO, MS12A_CMD_SET_LED,
                                PLACEHOLDER_U8, // red
                                PLACEHOLDER_U8, // green
                                PLACEHOLDER_U8, // blue
        };

        _store7(message, 2, +2, r);
        _store7(message, 4, +2, g);
        _store7(message, 6, +2, b);

	return _write(servo, message);
}

int ms12a_handshake(uint8_t servo)
{
        if (servo >= MS12A_NUM_SERVOS && servo != MS12A_ALL) return ESP_FAIL;
        uint8_t message[] = { MS12A_SERVO, MS12A_CMD_HANDSHAKE };
      	int res =_write(servo, message);
	return res;
}

int ms12a_set_pwm(uint8_t servo, uint14_t pwm)
{
        uint8_t message[] = { MS12A_SERVO, MS12A_CMD_SET_PWM,
                                PLACEHOLDER_U16, // pwm
        };

        _store7(message, 2, +3, pwm);
        return _write(servo, message);
}

int ms12a_return_to_zero(uint8_t servo, uint7_t mode, uint16_t speed)
{
        uint8_t message[]= { MS12A_SERVO, MS12A_CMD_RETURN_TO_ZERO,
                                PLACEHOLDER_U7,  // mode
                                PLACEHOLDER_U14, // speed
        };

        _store7(message, 2, +1, mode);
        _store7(message, 3, +2, speed);
        return _write(servo, message);
}

int ms12a_get_angle(uint8_t servo)
{
        uint8_t message[] = { MS12A_SERVO, MS12A_CMD_GET_ANGLE, 0x00 };
        return _write(servo, message);
}

int ms12a_get_speed(uint8_t servo)
{
        uint8_t message[] = {MS12A_SERVO, MS12A_CMD_GET_SPEED, 0x00 };
        return _write(servo, message);
}

int ms12a_get_voltage(uint8_t servo)
{
        uint8_t message[] = { MS12A_SERVO, MS12A_CMD_GET_VOLTAGE, 0x00 };
        return _write(servo, message);
}

int ms12a_get_current(uint8_t servo)
{
        uint8_t message[] = { MS12A_SERVO, MS12A_CMD_GET_CURRENT, 0x00 };
        return _write(servo, message);
}

int ms12a_get_temperature(uint8_t servo)
{
        uint8_t message[] = { MS12A_SERVO, MS12A_CMD_GET_TEMPERATURE, 0x00 };
        return _write(servo, message);
}

int ms12a_get_pid(uint8_t servo)
{
        uint8_t message[] = { MS12A_SERVO, MS12A_CMD_GET_PID, 0x00 };
        return _write(servo, message);
}

int ms12a_get_compensation(uint8_t servo)
{
        uint8_t message[] = { MS12A_SERVO, MS12A_CMD_GET_COMPENSATION, 0x00 };
        return _write(servo, message);
}

int ms12a_get_position(uint8_t servo)
{
        uint8_t message[] = { MS12A_SERVO, MS12A_CMD_GET_POSITION, 0x00 };
        return _write(servo, message);
}

int ms12a_set_mode(uint8_t servo, uint7_t mode)
{
        uint8_t message[] = { MS12A_SERVO, MS12A_CMD_SET_MODE, mode };
        return _write(servo, message);
}

int ms12a_report_when_position(uint8_t servo, int32_t position)
{
        return ESP_FAIL;
}

int ms12a_set_position_abs(uint8_t servo, int32_t position, uint14_t speed)
{
        return ESP_FAIL;
}

int ms12a_set_position_rel(uint8_t servo, int32_t position, uint14_t speed)
{
        uint8_t message[] = { MS12A_SERVO, MS12A_CMD_SET_POSITION_REL,
                                PLACEHOLDER_I32, // angle
                                PLACEHOLDER_U16, // speed
        };

        _store7(message, 2, +5, position);
        _store7(message, 7, +3, speed);
        return _write(servo, message);
        //return ESP_FAIL;
}


void ms12a_rx_task(void* param)
{
        uart_event_t event;

        for (;;)
        {
                if(xQueueReceive(SELF.uart.queue, &event, (portTickType) portMAX_DELAY))
                {
                        switch (event.type)
                        {
                                case UART_DATA:
                                        uart_read_bytes(SELF.uart.port,
                                                        SELF.uart.rx_buffer, event.size,
                                                        portMAX_DELAY);
                                        ESP_LOG_BUFFER_HEX(__func__, SELF.uart.rx_buffer, event.size);
                                        break;
                                case UART_FIFO_OVF:
                                        ESP_LOGE(TAG, "UART FIFO OVF");
                                        uart_flush_input(SELF.uart.port);
                                        xQueueReset(SELF.uart.queue);
                                        break;
                                case UART_BUFFER_FULL:
                                        ESP_LOGE(TAG, " UART_BUFFER FULL");
                                        uart_flush_input(SELF.uart.port);
                                        xQueueReset(SELF.uart.queue);
                                        break;
                                case UART_BREAK:
                                        ESP_LOGW(TAG, "UART BREAK");
                                        break;
                                case UART_PARITY_ERR:
                                case UART_FRAME_ERR:
                                case UART_PATTERN_DET:
                                default:
                                        ESP_LOGW(TAG, "Unhandled event %d of size %d",
                                                        event.type, event.size);
                                        break;
                        }
                }
        }
}

static int s_tare()
{
	for (int i=0; i<SELF.count; i++)
	{
		ms12a_set_angle_tare(SELF.servos[i].id);
	}
	return ESP_OK;
}

static int s_handshake()
{
	ms12a_handshake(MS12A_ALL);
	/*
	for (int i=0; i<SELF.count; i++)
	{
		ms12a_handshake(SELF.servos[i].id);
//		ms12a_set_mode(SELF.servos[i].id, 0x00);
//		ms12a_set_led(SELF.servos[i].id, 0x00, 0xFF, 0x00);
	}
	*/
	return ESP_OK;
}

static int s_read()
{
	for (int i=0; i<SELF.count; i++)
	{
		ms12a_servo_t *s = &SELF.servos[i];
		ms12a_servo_state_t *cur = &SELF.servos[i].current;
		
		cur->position = ms12a_get_position(s->id);
		cur->angle = ms12a_get_angle(s->id);
		cur->speed = ms12a_get_speed(s->id);
		cur->compensation = ms12a_get_compensation(s->id);

		s->voltage = ms12a_get_voltage(s->id);
		s->milliamp = ms12a_get_current(s->id);
		s->temperature = ms12a_get_temperature(s->id);
		s->pid = ms12a_get_pid(s->id);
		
	}
	return ESP_OK;
}

static int s_init()
{
        ESP_ERROR_CHECK(uart_param_config(SELF.uart.port, &SELF.uart.config));

        ESP_ERROR_CHECK(uart_set_pin(SELF.uart.port,
                                SELF.uart.pins.TX,
                                SELF.uart.pins.RX,
                                UART_PIN_NO_CHANGE,
                                UART_PIN_NO_CHANGE));

        ESP_ERROR_CHECK(uart_driver_install(SELF.uart.port,
                                MS12A_RX_BUFFER_SIZE,
                                MS12A_TX_BUFFER_SIZE,
                                MS12A_TASK_QUEUE_SIZE, &SELF.uart.queue, 0));

	SELF.count = MS12A_NUM_SERVOS;
	if (SELF.count)
	{
		SELF.servos = calloc(SELF.count,sizeof(ms12a_servo_t));
	}
	if (MS12A_RX_BUFFER_SIZE > 0)
		SELF.uart.rx_buffer = malloc(MS12A_RX_BUFFER_SIZE);

	if (MS12A_TX_BUFFER_SIZE > 0)
		SELF.uart.tx_buffer = malloc(MS12A_TX_BUFFER_SIZE);

        xTaskCreate(ms12a_rx_task, "ms12a uart event",MS12A_TASK_STACK_SIZE, NULL, 12, &SELF.uart.task);

        vTaskDelay(500 / portTICK_PERIOD_MS);
        ms12a_assign_ids();
	vTaskDelay(1000 / portTICK_PERIOD_MS);
	s_handshake();
	vTaskDelay(5000 / portTICK_PERIOD_MS);
	ms12a_set_led(MS12A_ALL, 0x00, 0xFF, 0x00);
	return ESP_OK;
}

static int s_deinit()
{
        vTaskDelete(SELF.uart.task);
        uart_driver_delete(SELF.uart.port);
	if (SELF.servos) free(SELF.servos);
	if (SELF.uart.rx_buffer) free(SELF.uart.rx_buffer);
	if (SELF.uart.tx_buffer) free(SELF.uart.tx_buffer);
        return ESP_OK;
}

static int s_update() 
{
	
	for (int i=0; i<SELF.count; i++)
	{
		ms12a_servo_t *s = &SELF.servos[i];
		ms12a_servo_state_t t = SELF.servos[i].target;
		ms12a_servo_state_t _t = SELF.servos[i]._target;
		SELF.servos[i]._target = t;

		if (t.angle != _t.angle) ms12a_set_angle_abs(s->id, t.angle, t.speed);
		//if (t.position != _t.position) 
		//ms12a_set_position_abs(s->id, t.position, t.speed);
		//if (t.compensation != _t.compensation) ms12a_set_compensation(s->id, t.compensation);
	
		/*
		ms12a_set_led(s->id, 
				(s->led >> 0x10) & 0xFF,
				(s->led >> 0x08) & 0xFF,
				(s->led >> 0x00) & 0xFF);
		vTaskDelay(5);
		*/

	}
	return ESP_OK;
}

ms12a_driver_t MS12A = { 
	.uart = {
		.port = MS12A_UART_PORT,
		.config = {
			.baud_rate = MS12A_BAUD_RATE,
               		.data_bits = UART_DATA_8_BITS,
                	.parity = UART_PARITY_DISABLE,
                	.stop_bits = UART_STOP_BITS_1,
                	.flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
		},
		.pins = {
			.TX = MS12A_PIN_TX,
			.RX = MS12A_PIN_RX,
		},
	},
	.init   = s_init,
	.update = s_update,
	.tare   = s_tare,
	.read   = s_read,
	.deinit = s_deinit,
};

