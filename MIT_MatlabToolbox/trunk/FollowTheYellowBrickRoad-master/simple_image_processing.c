#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>
#include <math.h>
#include <inttypes.h>
#include <stddef.h>
#include "image_processing.h"
int main() {


long   nx =0, ny=0;          /* image size in x, y direction */

char filename[20]="image.uyvy";
pixel2_t fileimage[80*120];

//helper variables
long   i, j;                 /* loop variables */
float x_avg = 0;
int hits = 0;
int y;

	 FILE* data;
		
		  if ((data = fopen(filename, "rb")) == NULL)
			{
				printf("ERROR opening file!\n");
				return 1;
			}
		
		nx=80;
		ny=120;


		fread(fileimage, sizeof(pixel2_t) *nx*ny, 1, data);
		
		fclose(data);
		

		//read image data
		for (j=0; j<ny; j++)
		{
		 for (i=0; i<nx; i++)
		   {
			 y = (int)fileimage[nx*j+i].y1; //noneg yuv! transform to 0 centerd uav by (-16,-128,-128)

            if (y > 240){
                 x_avg += i;
                 hits++;
             }

		   }

		}

    if (hits > 0){
        x_avg =(int) (100*(x_avg/hits) / 80.0) - 50;
        printf("x average is %f\n", x_avg);
    }
    return x_avg;
	}
    

