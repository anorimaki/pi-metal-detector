#include "main.h"
#include "battery.h"
#include "mathutil.h"
#include "adconvert.h"
#include "picconfig.h"


	
int16 battery_read_adc()
{
	disable_interrupts(GLOBAL);

	int16 ret = adc_read(PI_BATTERY_VOLTAGE_CH);
	
	enable_interrupts(GLOBAL);
	
	return COIL_MAX_ADC_VALUE-ret;
}


int16 battery_read_volts()
{
	int16 battery_sample = battery_read_adc();
	return math_change_range( battery_sample, 4095, 1272 );
}
