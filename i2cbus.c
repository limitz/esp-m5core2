#include "i2cbus.h"

#include <esp_log.h>
#include <driver/i2c.h>

int i2cbus_init(int bus, int sda, int scl, int speed)
{
	int rc;

	i2c_config_t config = 
	{
		.mode = I2C_MODE_MASTER,
		.sda_io_num = sda,
		.scl_io_num = scl,
		.master.clk_speed = speed,
		.sda_pullup_en = 1,
		.scl_pullup_en = 1
	};

	rc = i2c_param_config(bus, &config);
	if (ESP_OK != rc) return rc;

	rc = i2c_driver_install(bus, I2C_MODE_MASTER, 0, 0, 0);
	if (ESP_OK != rc) return rc;

	return ESP_OK;
}

int i2cbus_deinit(int bus)
{
	return i2c_driver_delete(bus);
}

int i2c_write_reg(int bus, int addr, int reg, const uint8_t* data, int len)
{
	int rc;
	i2c_cmd_handle_t cmd = i2c_cmd_link_create();
	rc = i2c_master_start(cmd)
	  || i2c_master_write_byte(cmd, (addr<<1)|I2C_MASTER_WRITE, 0x01)
	  || i2c_master_write_byte(cmd, reg, 0x01)
	  || (len && i2c_master_write(cmd, data, len, 0x01))
	  || i2c_master_stop(cmd)
	  ;
	
	if (ESP_OK != rc)
	{
		i2c_reset_tx_fifo(bus);
		i2c_cmd_link_delete(cmd);
		return rc;
	}

	i2c_master_cmd_begin(bus, cmd, 100);
	i2c_cmd_link_delete(cmd);
	
	return ESP_OK;
}

int i2c_read_reg(int bus, int addr, int reg, uint8_t* data, int len)
{
	int rc;
	i2c_cmd_handle_t cmd = i2c_cmd_link_create();
	rc = i2c_master_start(cmd)
	  || i2c_master_write_byte(cmd, (addr<<1)|I2C_MASTER_WRITE, 0x01)
	  || i2c_master_write_byte(cmd, reg, 0x01)
	  || i2c_master_start(cmd)
	  || i2c_master_write_byte(cmd, (addr<<1)|I2C_MASTER_READ, 0x01)
	  || (len > 1 && i2c_master_read(cmd, data, len-1, 0x00))
	  || (len > 0 && i2c_master_read(cmd, data +len-1, 1, 0x01))
	  || i2c_master_stop(cmd)
	  ;
	
	if (ESP_OK != rc)
	{
		i2c_reset_tx_fifo(bus);
		i2c_cmd_link_delete(cmd);
		return rc;
	}

	i2c_master_cmd_begin(bus, cmd, 100);
	i2c_cmd_link_delete(cmd);
	return ESP_OK;
}
