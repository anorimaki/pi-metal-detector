#include "main.h"
#include "battery.h"
#include "mathutil.h"
#include "adconvert.h"
#include "picconfig.h"

//This value is manually calibrated. 
//With pefect resistors (15K and 10K) and perfect negative regulator (-5V)
// it should be 1250 (12.5V).
#define BATERY_DIVIDER_MAX_VALUE 1280
	
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
	return math_change_range( battery_sample, 4095, BATERY_DIVIDER_MAX_VALUE );
}
