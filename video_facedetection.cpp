//2017.2.6

#include <stdio.h>
#include <opencv2/opencv.hpp>
#include "facedetect-dll.h"

#pragma comment(lib,"libfacedetect.lib")
//#pragma comment(lib,"libfacedetect-x64.lib")

//define the buffer size. Do not change the size!
#define DETECT_BUFFER_SIZE 0x20000
using namespace cv;

void OnMouseAction(int event, int x, int y, int falgs, void *ustc);
bool stime = false;

int main(int argc, char* argv[])
{

	Mat gray;
	Mat frame;
	Mat new_frame;
	int resize_height = 480;
	int resize_width = 640;

	int * pResults = NULL; 

    unsigned char * pBuffer = (unsigned char *)malloc(DETECT_BUFFER_SIZE);
    if(!pBuffer)
    {
        fprintf(stderr, "Can not alloc buffer.\n");
        return -1;
    }
	VideoCapture capture(0);

	while (stime==false)
	{
		capture >> frame;
		cv::resize(frame, new_frame, cv::Size(resize_width, resize_height), (0.0), (0.0), cv::INTER_LINEAR);
		cvtColor(new_frame, gray, CV_BGR2GRAY);
		int doLandmark = 1;

		pResults = facedetect_multiview_reinforce(pBuffer, (unsigned char*)(gray.ptr(0)), gray.cols, gray.rows, (int)gray.step,
			1.2f, 3, 24, 0, doLandmark);

		//printf("%d faces detected.\n", (pResults ? *pResults : 0));
		Mat result_multiview_reinforce = new_frame.clone();;
		//print the detection results
		for (int i = 0; i < (pResults ? *pResults : 0); i++)
		{
			short * p = ((short*)(pResults + 1)) + 142 * i;
			int x = p[0];
			int y = p[1];
			int w = p[2];
			int h = p[3];
			int neighbors = p[4];
			int angle = p[5];

			printf("face_rect=[%d, %d, %d, %d], neighbors=%d, angle=%d\n", x, y, w, h, neighbors, angle);
			rectangle(result_multiview_reinforce, Rect(x, y, w, h), Scalar(0, 255, 0), 2);
			if (doLandmark)
			{
				for (int j = 0; j < 68; j++)
					circle(result_multiview_reinforce, Point((int)p[6 + 2 * j], (int)p[6 + 2 * j + 1]), 1, Scalar(0, 255, 0));
			}
		}
		imshow("Results_multiview_reinforce", result_multiview_reinforce);
		waitKey(30);
		setMouseCallback("Results_multiview_reinforce",OnMouseAction);
	}
    //release the buffer
    free(pBuffer);

	return 0;
}

void OnMouseAction(int event, int x, int y, int falgs, void *ustc)
{
	if (event==CV_EVENT_LBUTTONDOWN)
	{
		stime = true;
	} 
	else
	{
		stime = false; 
	}
}