/*
    Vignetting Correction

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

#ifndef _VIG
#define _VIG

#include <iostream>
#include <stdio.h>
#include <cstring>

#include <omp.h>
#include <opencv2/opencv.hpp>
#include <vector>

using namespace std;
using namespace cv;

bool vig_correction(Mat& raw, Mat& raw_cali){//modify raw based on raw_cali

Mat img_c(IMG_H,IMG_W,CV_32FC1);

for (int j = 0; j <raw_cali.rows; j++){
	for (int i = 0; i <raw_cali.cols; i++){
		img_c.at<float>(j,i)=(raw_cali.at<float>(j,i)-168.0) /(4095.0-168.0);		

        float temp=img_c.at<float>(j,i);
		if (temp>1)   img_c.at<float>(j,i)=1;
		if (temp<0.1) img_c.at<float>(j,i)=0.1;
	}
}

for (int j = 0; j <raw_cali.rows; j++){
	for (int i = 0; i <raw_cali.cols; i++){
		raw.at<float>(j,i)=(raw.at<float>(j,i)-168.0)/img_c.at<float>(j,i);
        float temp=raw.at<float>(j,i);
		if (temp>4095)   raw.at<float>(j,i)=4095;
		if (temp<32) raw.at<float>(j,i)=32;		
	}
}





return true;
}


#endif
