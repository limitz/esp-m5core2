#include "i2cbus.h"
#include "ft6336.h"
#include <esp_system.h>
#include <esp_log.h>
#include <driver/gpio.h>

ft6336_t FT6336 = {
	.address = 0x38,
	.int_pin = 39,
	.width = 320,
	.height = 280,
	.interval = 13,
};

static uint8_t read_u8(uint8_t reg)
{
	uint8_t u8;
	i2c_read_reg(0, FT6336.address, reg, &u8, 1);
	return u8;
}

static int write_u8(uint8_t reg, uint8_t v)
{
	return i2c_write_reg(0, FT6336.address, reg, &v, 1);
}

static uint16_t read_u16(uint8_t reg)
{
	uint8_t v[2];
	i2c_read_reg(0, FT6336.address, reg, v, 2);
	return (v[0]<<8) | (v[1]);
}

int ft6336_init()
{
	// TODO set interrupt for touch
	gpio_config_t gpio = {
		.pin_bit_mask = 1ULL << FT6336.int_pin,
		.mode = GPIO_MODE_INPUT,
	};
	gpio_config(&gpio);

	// Set INT wire to polled mode, low as long as one touch is present
	write_u8(0xA4, 0x00);

	// interval
	write_u8(0x88, FT6336.interval);
	uint8_t interval = read_u8(0x88);
	if (interval != FT6336.interval)
	{
		ESP_LOGE(__func__, "FT6336 init failed");
		return ESP_FAIL;
	}
	
	int fwid = read_u8(0xA6);
	int rel  = read_u8(0xAF);
	int lib  = read_u16(0xA1);

	ESP_LOGI(__func__, "FT6336 init success (fw:0x%02x rel:0x%02x lib:0x%04x)", fwid, rel, lib);
	return ESP_OK;
}

int ft6336_is_touched()
{
	return gpio_get_level(FT6336.int_pin);
}

int ft6336_update()
{
	if (true || ft6336_is_touched())
	{
		uint8_t data[11];
		i2c_read_reg(0, FT6336.address, 0x02, data, 11);
		FT6336.num_points = data[0] > 2 ? 0 : data[0];

		if (FT6336.num_points > 0)
		{
			FT6336.point[0].x = ((data[1] << 8) | data[2]) & 0x0FFF;
			FT6336.point[0].y = ((data[3] << 8) | data[4]) & 0x0FFF;
		
			if (FT6336.num_points > 1)
			{
				FT6336.point[1].x = ((data[7] << 8) | data[8]) & 0x0FFF;
				FT6336.point[1].y = ((data[9] << 8) | data[10]) & 0x0FFF;
			}
		}
	}
	else
	{
		FT6336.num_points = 0;
	}
	return ESP_OK;
}
