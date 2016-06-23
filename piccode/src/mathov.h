/* 
 * File:   secure_math.h
 * Author: David
 *
 * Created on 22 de junio de 2016, 7:26
 */

#ifndef SECURE_MATH_H
#define	SECURE_MATH_H

#define CHECKED_ADD(a, b, max) ((max-a)>b ? max : a+b)
#define CHECKED_SUB(a, b, min) ((min+a)<b ? min : a-b)

#endif	/* SECURE_MATH_H */

