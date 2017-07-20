#include <stdlib.h>
#include <stdio.h>
#include <sys/time.h>
#include <math.h>
#include <inttypes.h>
#include <stddef.h>
#include "image_processing.h"
#define PI 3.14159265

int main(int argc, char *argv[]) {

	if( argc == 2 ) {
		printf("The image file is %s\n", argv[1]);
	}
	else {
		printf("Image filename expected.\n");
	}

	int nx = 80;
	int ny = 120;          		//image size in x, y direction 
	char *filename;
	filename = argv[1]; 		//"test_imgs/left.uyvy";
	pixel2_t fileimage[nx*ny];


	//helper variables
	int i,j;                
	float x_sum = 0;
	int hits = 0;
	int y;
	float top_x_sum = 0;
	float bot_x_sum = 0;
	int top_hits = 0;
	int bot_hits = 0;
	int offset = 20;

	FILE* data;
  	if ((data = fopen(filename, "rb")) == NULL)
	{
		printf("ERROR opening file!\n");
		return 1;
	}

	fread(fileimage, sizeof(pixel2_t)*nx*ny, 1, data);
	fclose(data);


	//read image data
	for (j=0; j<ny; j++) {
		for (i=0; i<nx; i++) {
			y = (int) fileimage[nx*j+i].y1; // y value of current pixel

	    	if (y > 240) {
	        	x_sum += i;
	        	hits++;

	        	// this simple calc assumes tape passes through entire height of image
		    	// top row
		    	if (j==offset) {
					top_x_sum += i;
					top_hits++;
				}
				// bottom row
				if (j==ny-offset) {
					bot_x_sum += i;
					bot_hits++;
				}
	    	}
		}
	}

	float top_x_avg = 100*(top_x_sum/top_hits)/nx;
	float bot_x_avg = 100*(bot_x_sum/bot_hits)/nx;

	float delta_y = ny-2*offset;
	float delta_x = top_x_avg-bot_x_avg;
	float angle = atan(delta_x/delta_y);

	printf("Top x_avg is %f\n", top_x_avg);
	printf("Bottom x_avg is %f\n", bot_x_avg);
	printf("Angle from center: %f radians (%f degrees) (positive - clockwise)\n", angle, angle*180/PI);

	float x_avg = 0;
	if (hits > 0) {
		x_avg = (int) (100*(x_sum/hits)/80.0)-50;
		printf("x average from center: %f\n", x_avg);
	}

	// add code to return angle as well
	return x_avg;
}
    

