#include <axp192.h>
#include <i2cbus.h>
#include <esp_system.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

static uint8_t read_u8(uint8_t reg)
{
	uint8_t u8;
	i2c_read_reg(0, 0x34, reg, &u8, 1);
	return u8;
}

static uint16_t read_u12(uint8_t reg)
{
	uint8_t v[2];
	i2c_read_reg(0, 0x34, reg, v, 2);
	return (v[0]<<4) | (v[1] & 0x0F);
}

static uint16_t read_u13(uint8_t reg)
{
	uint8_t v[2];
	i2c_read_reg(0, 0x34, reg, v, 2);
	return (v[0]<<5) | (v[1] & 0x1F);
}

static uint16_t read_u16(uint8_t reg)
{
	uint8_t v[2];
	i2c_read_reg(0, 0x34, reg, v, 2);
	return (v[0]<<8) | (v[1]);
}

static uint32_t read_u24(uint8_t reg)
{
	uint8_t v[3];
	i2c_read_reg(0, 0x34, reg, v, 3);
	return (v[0]<<16) | (v[1]<<8) | v[2];
}

static uint32_t read_u32(uint8_t reg)
{
	uint8_t v[4];
	i2c_read_reg(0, 0x34, reg, v, 4);
	return (v[0]<<24) | (v[1]<<16) | (v[2]<<8) | v[3];
}

static int write_u8(uint8_t reg, uint8_t v)
{
	return i2c_write_reg(0, 0x34, reg, &v, 1);
}

int axp192_init()
{
	uint8_t mode = 0; // 1 for outside input

	// turn of vbus limit
	write_u8(0x30, (read_u8(0x30) & 0x04) | 0x02);

	// GPIO1 ouput
	write_u8(0x92, (read_u8(0x92) & 0xF8));

	// GPIO2 ouput
	write_u8(0x93, (read_u8(0x92) & 0xF8));

	// RTC charging on
	write_u8(0x35, (read_u8(0x35) & 0x1C) | 0xA2);

	axp192_set_esp_voltage(3350);
	axp192_set_lcd_voltage(2800);
	axp192_set_ldo_voltage(2, 3300);
	axp192_set_ldo_voltage(3, 2000);
	axp192_set_ldo(1, 0);
	axp192_set_ldo(2, 0);
	axp192_set_ldo(3, 0);
	axp192_set_led(1);
	axp192_set_charge_current(AXP_CHG_700);

	write_u8(0x95, (read_u8(0x95) & 0x72) | 0x84);
	write_u8(0x36, 0x4C);
	write_u8(0x82, 0xFF);
	write_u8(0x84, 25);

	axp192_set_lcdrset(0);
	vTaskDelay(10);
	axp192_set_lcdrset(1);
	vTaskDelay(10);
	axp192_set_bus_power_mode(mode);
	
	return ESP_OK;
}

int axp192_get_adc_lsb()
{
	return read_u8(0x84);
}

int axp192_power_off()
{
	return write_u8(0x32, read_u8(0x32) | 0x80);
}

int axp192_breathe(int brightness)
{
	if (brightness > 12) brightness = 12;
	return write_u8(0x28, (read_u8(0x28) & 0x0F) | brightness << 4);
}

int axp192_get_battery_state()
{
	if (read_u8(0x01) & 0x20) return 1;
	return 0;
}

int axp192_set_adc(int state)
{
	return write_u8(0x82, state ? 0xFF: 0x00);
}

int axp192_prepare_to_sleep()
{
	axp192_set_adc(0);
	axp192_set_led(0);
	axp192_set_dcdc3(0);
	return ESP_OK;
}

int axp192_restore_from_sleep()
{
	axp192_set_dcdc3(1);
	axp192_set_led(1);
	axp192_set_adc(1);
	return ESP_OK;
}

int axp192_get_warning_level()
{
	return ESP_OK;
}

int axp192_get_battery_voltage()
{
	return read_u12(0x78) * 11 / 10;
}

int axp192_get_battery_current()
{
	uint16_t in = read_u13(0x7A);
	uint16_t out = read_u13(0x7C);
	return (in - out) / 2;
}

int axp192_get_vin_voltage()
{
	return read_u12(0x56) * 17 / 10;
}

int axp192_get_vin_current()
{
	return read_u12(0x58) * 5 / 8;
}

int axp192_get_vbus_voltage()
{
	return read_u12(0x5A) * 17 / 10;
}

int axp192_get_vbus_current()
{
	return read_u12(0x5C) * 3 / 8;
}

int axp192_get_battery_power()
{
	return read_u24(0x70) * 11 / 20;
}

int axp192_get_battery_charge_current()
{
	return read_u12(0x7A) / 2;
}

int axp192_get_aps_voltage()
{
	return read_u12(0x7E) * 14 / 10;
}

int axp192_set_ldo2(int state)
{
	uint8_t v = read_u8(0x12);
	v = state ? (v | 4) : (v & ~4);
	return write_u8(0x12, v);
}

int axp192_set_dcdc3(int state)
{

	uint8_t v = read_u8(0x12);
	v = state ? (v | 2) : (v & ~2);
	return write_u8(0x12, v);
}

int axp192_state()
{
	return read_u8(0x00);
}

int axp192_is_acin()
{
	return read_u8(0x00) & 0x80;
}

int axp192_is_charging()
{
	return read_u8(0x00) & 0x04;
}

int axp192_is_vbus()
{
	return read_u8(0x00) & 0x20;
}

int axp192_set_ldo_voltage(uint8_t num, uint16_t voltage)
{
	voltage = (voltage > 3300) ? (15) : (voltage / 100 - 18);
	switch (num)
	{
		case 2:
			return write_u8(0x28, (read_u8(0x28) & 0x0F) | (voltage << 4));
		case 3:
			return write_u8(0x28, (read_u8(0x28) & 0xF0) | (voltage));
		default: 
			return ESP_FAIL;
	}
}

int axp192_set_dc_voltage(uint8_t num, uint16_t voltage)
{
	voltage = (voltage < 700) ? 0 : (voltage - 700) / 25;
	switch (num)
	{
		case 0: return write_u8(0x26, (read_u8(0x26) & 0x80) | (voltage & 0x7F));
		case 1: return write_u8(0x25, (read_u8(0x25) & 0x80) | (voltage & 0x7F));
		case 2: return write_u8(0x27, (read_u8(0x27) & 0x80) | (voltage & 0x7F));
		default: return ESP_FAIL;
	}
}

int axp192_set_esp_voltage(uint16_t voltage)
{
	if (voltage >= 3000 && voltage <= 3400)
	{
		return axp192_set_dc_voltage(0, voltage);
	}
	return ESP_FAIL;
}

int axp192_set_lcd_voltage(uint16_t voltage)
{
	if (voltage >= 2500 && voltage <= 3300)
	{
		return axp192_set_dc_voltage(2,voltage);
	}
	return ESP_FAIL;
}

int axp192_set_ldo(uint8_t num, int state)
{
	int b = 1 << num;
	int v = read_u8(0x12);
	v = state ? (v | b) : (v & ~b);
	return write_u8(0x12, v);
}

int axp192_set_lcdrset(int state)
{
	int v = read_u8(0x96);
	v = state ? (v | 2) : (v & ~2);
	return write_u8(0x96, v);
}

int axp192_set_bus_power_mode(int state)
{
	if (!state)
	{
		write_u8(0x91, (read_u8(0x91) & 0x0F) | 0xF0);
		write_u8(0x90, (read_u8(0x90) & 0xF8) | 0x02);
		read_u8(0x91);
		write_u8(0x12, read_u8(0x12) | 0x40);
	}
	else
	{
		write_u8(0x12, read_u8(0x12) & 0xBF);
		write_u8(0x90, (read_u8(0x90) & 0xF8) | 0x01);
	}
	return ESP_OK;
}

int axp192_set_led(int state)
{
	int v = read_u8(0x94);
	v = state ? v & 0xFD : v | 0x02;
	return write_u8(0x94, v);
}

int axp192_set_speaker(int state)
{
	int v = read_u8(0x94);
	v = state ? v | 4 : v & ~4;
	return write_u8(0x94, v);
}

int axp192_set_charge_current(int state)
{
	return write_u8(0x33, (read_u8(0x33) & 0xF0) | (state & 0x0F));
}

int axp192_enable_coulomb_counter()
{
	return write_u8(0xB8, 0x80);
}

int axp192_disable_coulomb_counter()
{
	return write_u8(0xB8, 0x00);
}

int axp192_stop_coulomb_counter()
{
	return write_u8(0xB8, 0xC0);
}

int axp192_clear_coulomb_counter()
{
	return write_u8(0xB8, 0xA0);
}

int axp192_get_coulomb_charge_data()
{
	return read_u32(0xB0);
}

int axp192_get_coulomb_discharge_data()
{
	return read_u32(0xB4);
}

int axp192_get_coulomb_data()
{
	int cin = axp192_get_coulomb_charge_data();
	int cout = axp192_get_coulomb_discharge_data();

	return (((cin - cout) << 15) / 3600) / 25;
}

