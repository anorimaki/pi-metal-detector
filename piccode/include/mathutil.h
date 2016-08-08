#ifndef MATHUTIL_H
#define	MATHUTIL_H

#define CHECKED_ADD(a, b, max) ((max-a)>b ? a+b : max)
#define CHECKED_SUB(a, b, min) ((min+a)<b ? min : a-b)

signed int32 math_change_range(signed int16 value, int16 max_value, 
								int16 new_max_value);

signed int32 math_change_log_range(signed int16 value, int16 max_value, 
								int16 new_max_value);

#endif

