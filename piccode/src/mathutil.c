#include "main.h"
#include "mathutil.h"


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

