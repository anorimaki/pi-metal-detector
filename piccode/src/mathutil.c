#include "main.h"
#include "mathutil.h"
#include "math.h"


signed int32 math_change_range(signed int16 value, int16 max_value, 
								int16 new_max_value)
{
	int32 ret = abs(value);
	
	if (ret > max_value)
		ret = max_value;

	ret *= new_max_value;        //Can't overlay (12bits * 16bits = 28bits)
	ret <<= 3;                   //Multiply x8 to round final value (31 bits)
	ret /= max_value;

	int8 remainder = ret & 0x07;
	ret >>= 3;

	if ( remainder > 0x03 )
		++ret;
	
	return (value & 0x8000) ? -(signed int32)ret : ret;
}

signed int32 math_change_log_range(signed int16 value, int16 max_value, 
								int16 new_max_value)
{
	static signed int16 cached_value = 0;
	static float32 cached_log_value;
	static int16 cached_max_value = 0;
	static float32 cached_log_max_value;
	
	if ( value == 0 )
		return 0;
	
	if ( cached_value!=value ) {
		cached_value = value;
		cached_log_value = sqrt( abs(value) );
	}
	float32 value_log = cached_log_value;
	
	if ( cached_max_value!=max_value ) {
		cached_max_value = max_value;
		cached_log_max_value = sqrt( max_value );
	}
	float32 max_log = cached_log_max_value;
	
	int32 ret = (value_log*new_max_value) / max_log;
	
//	printf( "v: %Ld, m: %Lu, lv=%f, lm=%f, r=%Ld\r\n", value, 
//			max_value, value_log, max_log, ret );
	
	return (value<0) ? -ret : ret;
}