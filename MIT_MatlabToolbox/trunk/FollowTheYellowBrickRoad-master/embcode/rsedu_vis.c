//File: rsedu_vis.c
/*
* AUTHOR Fabian Riether
* CREATE DATE 2015/08/25
* PURPOSE This module takes care of processing images to reconstruct the drone's pose (needs specific landmark setup)
* SPECIAL NOTES
* ===============================
* Change History
* 2015/08/25 created
* ==================================
*/
#include "rsedu_vis.h"

/*
 * reconstruct camera pose
 */
void reconstructCameraPose(float camerapos[3], float *camerayaw, float feature_pps[3][4], double reconrightMatrix[4][4], double intrMatrx_inv[3][3])
{
    double LiMatrix[3][4];
    double LiiMatrix[3][4];
    double scalefactor;
    int c, d, k;
    int m = 3;
    int q = 4;
    int p = 4;
    double sum = 0.0;

    for(c = 0; c < m; c++)
    {
        for(d = 0; d < q; d++)
        {
            for(k = 0; k < p; k++)
            {
                sum = sum + feature_pps[c][k] * reconrightMatrix[k][d];
            }
            LiMatrix[c][d] = sum;
            sum = 0;
        }
    }

    for(c = 0; c < m; c++)
    {
        for(d = 0; d < q; d++)
        {

            for(k = 0; k < m; k++)
            {
                sum = sum + intrMatrx_inv[c][k] * LiMatrix[k][d];
            }
            LiiMatrix[c][d] = sum;
            sum = 0;
        }
    }



    scalefactor = 1 / sqrt(pow(LiiMatrix[0][0], 2) + pow(LiiMatrix[0][1], 2));

    camerapos[0] = (float)(LiiMatrix[1][3] * scalefactor);
    camerapos[1] = (float)(-LiiMatrix[0][3] * scalefactor);
    camerapos[2] = (float)(LiiMatrix[2][3] * scalefactor);

    //Yaw: correct to output x-axis(RS)alignment iwth x-axis landmarkfield. z-axis facing down, rotation +-pi
    float cosa = (LiiMatrix[0][0]) * scalefactor;
    float sina = -(LiiMatrix[0][1]) * scalefactor;
    if(cosa > 0)
    {
        if(sina < 0)
        {
            *camerayaw    = (float)(acos(cosa) - 1.571);
        }
        else
        {
            *camerayaw    = (float)(-acos(cosa) - 1.571);
        };

    }
    else
    {
        if(sina < 0)
        {
            *camerayaw    = (float)(acos(cosa) - 1.571);
        }
        else
        {
            *camerayaw    = (float)(-acos(cosa) + 1.571 * 3);
        };
    };


}


//----------------------------------
// Image processing / Vision-based pose estimation
//----------------------------------

/*
 * @input buffer Pointer to the current picture seen by the vertical camera
 * Picture is 160x120 pixels in YUYV format, ie 80x120 elements of type 'pixel2_t'
 * This functions then dumps a planar camera position estimate relative to a colored landmark setup into a fifo to make them available to the control code
 *
 * Called 60 times per second.
 */

void RSEDU_image_processing(void * buffer)
{


    //process control
    static int counter = 0;

    //communication
    static float vis_data[4];
    int status;
    static int vis_fifo;

    //image variables
    int row, col;
    float yuv[3];
    float feature_pps[3][4];
    static unsigned char matchLookup[128][128][128];

    //camera variables
    float camerapos[3]; //@TODO static?
    float camerayaw; 	//@TODO static?

    //Image and Matching Thresholds
    static int filtersize = 5; 			//odd numbered! dummy for potential gaussian filter mask, etc.
    int pxls_ftr_min 	  = 5; 			//minimum required nr of detected pixels per landmark

    //Landmarks, Pose Estimation Matrices
    int lndmrk_nr = 5, lndmrk_best = 0;
    static lndmrk_t lndmrks[5];
    //matrices for reconstructing camera pose by intrMatrx_inv*landmark-pixellocation*ldnmrk_pinv
    static double ldnmrk_pinv[4][4] =
    {
        { -3.093396787626414,   2.082093991671625                   , 0,   0.766805472932779},
        {2.320047590719810 ,  3.438429506246282    ,               0  , 0.174895895300416},
        { -1.737061273051754 , -2.676977989292088    ,               0  , 0.299821534800714},
        {2.510410469958359 , -2.843545508625819     ,              0 , -0.241522903033909}
    };

    static double intrMatrx_inv[3][3] =       {{ 0.006551831768882                   , 0,  -0.550082487527771},
        {0,   0.006546559888686,  -0.399495805347318},
        {0,                   0,   1.000000000000000}
    };

    //wait on first call
    if(counter == 1)
    {
        usleep(20000);
    }

    //Init Communication, Streaming
    int fifo;
    if(FEAT_IMSAVE == 2)
    {
        u8 * raw = buffer;
    };
    pixel2_t *image = buffer;  /* Picture is a 160x120 pixels, stored in YUV422 interlaced format - TO BE CHECKED */


    /*
     * PROGRAM
     */


    //ptiming - declare and start
    //------------
    long long start;
    static FILE *ptfile;
    ptimer_start(FEAT_TIME, counter, &(start));
    //------------


    //process control
    counter++;

    if(counter == 1)
    {
        //ptiming - init file
        //------------
        ptimer_init(FEAT_TIME, __func__, &(ptfile), NULL);
        //------------

        printf("rsedu_vis(): Init fifo-communication...\n");

        //open fifo to dump visual position estimates to control code

        if(access("/tmp/vis_fifo", F_OK) != -1)
        {
            printf("rsedu_vis(): SUCCESS POSVIS FIFO exists! \n");

            vis_fifo = open("/tmp/vis_fifo", O_WRONLY);
            if(vis_fifo)
            {
                vis_data[0] = -99.0;
                write(vis_fifo, (float*)(&vis_data), sizeof(vis_data));
                close(vis_fifo);
                printf("rsedu_vis(): SUCCESS opening POSVIS-fifo!\n");
            }
            else
            {
                printf("rsedu_vis(): ERROR opening POSVIS-fifo!\n");
            }
        }
        else
        {
            printf("rsedu_vis(): ERROR opening POSVIS-fifo!\n");
        }



    }




    long   nx =0, ny=0;          /* image size in x, y direction */

    pixel2_t fileimage[80*120];

    //helper variables
    long   i, j;                 /* loop variables */
    float x_avg = 0;
    int hits = 0;
    int y;

		
		nx=80;
		ny=120;


		//read image data
		for (j=0; j<ny; j++)
		{
		 for (i=0; i<nx; i++)
		   {
			 y = (int)image[nx*j+i].y1; //noneg yuv! transform to 0 centerd uav by (-16,-128,-128)

            if (y > 240){
                 x_avg += i;
                 hits++;
             }

		   }

		}

    if (hits > 0){
        x_avg =(int) (100*(x_avg/hits) / 80.0) - 50;
        //XXX printf("x average is %f\n", x_avg);
    }

            /*
             * dump pose into FIFO to make available to controls code
             */
            //compile data
            vis_data[1] = (float)x_avg;
            /*vis_data[1] = 0.0;
            vis_data[2] = 0.0;
            vis_data[3] = 0.0;*/

            vis_fifo = open("/tmp/vis_fifo", O_WRONLY);
            if(vis_fifo)
            {
                write(vis_fifo, (float*)(&vis_data), sizeof(vis_data));
                close(vis_fifo);
            }

    

    usleep(4000);

    //ptiming - store
    //----------
    ptimer_stopstore(FEAT_TIME, counter, start, ptfile);
    //----------


}
