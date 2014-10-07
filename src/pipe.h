/*
    Light field image processing pipeline

    Copyright (C) 2014 Shan Xu and Nicholas Devaney

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions are
    met:

    - Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.

    - Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
    IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
    TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
    PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
    HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
    SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
    LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
    DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
    THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
    (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
    OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

 */



#ifndef _IMG_PIPE
#define _IMG_PIPE

#include <iostream>
#include <stdio.h>
#include <cstring>

#include <omp.h>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <vector>

using namespace std;
using namespace cv;

bool image_adjust(Mat& raw){

Mat raw2=Mat(raw.rows,raw.cols,CV_32FC3);

//linearize intensity 
Mat gray_image;
cvtColor(raw, gray_image, 6);// CV_BGR2GRAY=6

float pixel_max= precent ( gray_image,0.995);
float pixel_min= precent ( gray_image,0.005);

//cout<<pixel_max<<" "<<pixel_min<<" "<<raw2.size()<<endl;

for (int j=0; j<raw.rows; j++){
	for (int i=0; i<raw.cols; i++){ 

		for (int k=0; k<3; k++){
			if (raw.at<Vec3f>(j,i)[k]>pixel_max)
				raw2.at<Vec3f>(j,i)[k]=1;
			else if (raw.at<Vec3f>(j,i)[k]<pixel_min)
				raw2.at<Vec3f>(j,i)[k]=0;
			else raw2.at<Vec3f>(j,i)[k]= (raw.at<Vec3f>(j,i)[k]-pixel_min)/(pixel_max-pixel_min);
		}
	}
}

raw=256*raw2;
return true;
}



bool gammac( Mat& img, double gamma ) {
 
	img=img/256;
	for (int j=0; j<img.rows; j++){
		for (int i=0; i<img.cols; i++){
			if (img.at<Vec3f>(j,i)[0]>0)
			img.at<Vec3f>(j,i)[0]= pow( (double) img.at<Vec3f>(j,i)[0], gamma ) ;
			else img.at<Vec3f>(j,i)[0]= 0;
			if (img.at<Vec3f>(j,i)[1]>0)
			img.at<Vec3f>(j,i)[1]= pow( (double) img.at<Vec3f>(j,i)[1], gamma ) ;
			else img.at<Vec3f>(j,i)[1]= 0;
			if (img.at<Vec3f>(j,i)[2]>0)
			img.at<Vec3f>(j,i)[2]= pow( (double) img.at<Vec3f>(j,i)[2], gamma ) ;
			else img.at<Vec3f>(j,i)[2]= 0;
		}
	}
	
	for (int j=0; j<img.rows; j++){
		for (int i=0; i<img.cols; i++){

			img.at<Vec3f>(j,i)=img.at<Vec3f>(j,i)*256;

			if (img.at<Vec3f>(j,i)[0]>4095) img.at<Vec3f>(j,i)[0]=4095;
			if (img.at<Vec3f>(j,i)[1]>4095) img.at<Vec3f>(j,i)[1]=4095;	
			if (img.at<Vec3f>(j,i)[2]>4095) img.at<Vec3f>(j,i)[2]=4095;

		}
	}

return true;
}



#endif
