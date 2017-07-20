#include <stdlib.h>
#include <sys/time.h>
#include <math.h>
#include <inttypes.h>
#include <stddef.h>


typedef signed char s8;
typedef unsigned char u8;

typedef struct {
	u8 y1;
	u8 u;
	u8 y2;
	u8 v;
} __attribute__((packed)) pixel2_t;
/* Defines a couple of 2 consecutive pixels in YUV422 interlaced format */

/*
 * helper functions
 */
int get_location();

