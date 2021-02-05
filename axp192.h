#pragma once

#include <stdint.h>

enum
{
	AXP_CHG_100,
	AXP_CHG_190,
	AXP_CHG_280,
	AXP_CHG_360,
	AXP_CHG_450,
	AXP_CHG_550,
	AXP_CHG_630,
	AXP_CHG_700,
	AXP_CHG_780,
	AXP_CHG_880,
	AXP_CHG_960,
	AXP_CHG_1000,
	AXP_CHG_1080,
	AXP_CHG_1160,
	AXP_CHG_1240,
	AXP_CHG_1320,

} axp192_charge_current_t;

int axp192_init();
int axp192_get_adc_lsb();
int axp192_power_off();
int axp192_breathe(int brightness);
int axp192_get_battery_state();
int axp192_set_adc(int state);
int axp192_prepare_to_sleep();
int axp192_restore_from_sleep();
int axp192_get_warning_level();
int axp192_get_battery_voltage();
int axp192_get_battery_current();
int axp192_get_vin_voltage();
int axp192_get_vin_current();
int axp192_get_vbus_voltage();
int axp192_get_vbus_current();
int axp192_get_battery_power();
int axp192_get_battery_charge_current();
int axp192_get_aps_voltage();
int axp192_set_ldo2(int state);
int axp192_set_dcdc3(int state);
int axp192_state();
int axp192_is_acin();
int axp192_is_charging();
int axp192_is_vbus();
int axp192_set_ldo_voltage(uint8_t num, uint16_t voltage);
int axp192_set_dc_voltage(uint8_t num, uint16_t voltage);
int axp192_set_esp_voltage(uint16_t voltage);
int axp192_set_lcd_voltage(uint16_t voltage);
int axp192_set_ldo(uint8_t num, int state);
int axp192_set_lcdrset(int state);
int axp192_set_bus_power_mode(int state);
int axp192_set_led(int state);
int axp192_set_speaker(int state);
int axp192_set_charge_current(int state);

int axp192_enable_coulomb_counter();
int axp192_disable_coulomb_counter();
int axp192_stop_coulomb_counter();
int axp192_clear_coulomb_counter();
int axp192_get_coulomb_charge_data();
int axp192_get_coulomb_discharge_data();
int axp192_get_coulomb_data();

