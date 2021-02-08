#ifndef LIBC_H_included
#define LIBC_H_included

/*
 * NULL
 */

#define NULL 0x0

/*
 * Boolean
 */

typedef unsigned char bool; 

#define false 	0x0
#define true 	0x1

/*
 * Integers
 */

typedef char 		int8_t;
typedef short 		int16_t;
typedef long 		int32_t;
typedef long long 	int64_t;

typedef unsigned char 		uint8_t;
typedef unsigned short 		uint16_t;
typedef unsigned long 		uint32_t;
typedef unsigned long long 	uint64_t;

typedef uint32_t     	    size_t;
typedef uint32_t			off_t;

#endif
