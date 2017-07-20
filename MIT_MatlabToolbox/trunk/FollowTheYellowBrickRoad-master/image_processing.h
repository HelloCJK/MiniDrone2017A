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
} __attribute__((packed)) pixel2_t;  /* Defines a couple of 2 consecutive pixels in YUV422 interlaced format */


/*
 * helper functions
 */

//allocates storage for matrix of size nx * ny
void alloc_matrix

     (float ***matrix,  /* matrix */
      long  nx,         /* size in x-direction */
      long  ny)         /* size in y-direction */

{
long i;

*matrix = (float **) malloc (nx * sizeof(float *));
if (*matrix == NULL)
   {
   printf("alloc_matrix: not enough storage available\n");
   exit(1);
   }
for (i=0; i<nx; i++)
    {
    (*matrix)[i] = (float *) malloc (ny * sizeof(float));
    if ((*matrix)[i] == NULL)
       {
       printf("alloc_matrix: not enough storage available\n");
       exit(1);
       }
    }
return;
}

//disallocates storage for matrix of size nx * ny
void disalloc_matrix

     (float **matrix,   /* matrix */
      long  nx,         /* size in x-direction */
      long  ny)         /* size in y-direction */

{
long i;
for (i=0; i<nx; i++)
    free(matrix[i]);
free(matrix);
return;
}


