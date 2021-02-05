#include <stdint.h>

typedef struct
{
	int int_pin;
	int address;
	int num_points;
	int interval;
	int width;
	int height;
	struct { int x, y; } point[2];
} ft6336_t;

int ft6336_init();
int ft6336_is_touched();
int ft6336_update();
int ft6336_deinit();

extern ft6336_t FT6336;
