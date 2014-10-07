/*
    Debug programme.

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

#ifndef _DEBUG
#define _DEBUG

#include <iostream>
#include <stdio.h>
#include <cstring>

#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

bool lable_center(const Mat& lf_raw,  vector<Point2d> pos_vec, string dir){

double iy[6]={1.0f/3.0f, -1.0f/3.0f, 1.0f/6.0f,  1.0f/6.0f, -1.0f/6.0f, -1.0f/6.0f}; 
double ix[6]={        0,          0,       0.5,       -0.5,        0.5,       -0.5};

Mat temp(lf_raw.cols,lf_raw.rows,CV_8UC1);

    for(int j=0;j<lf_raw.rows;j++){
        for(int i=0;i<lf_raw.cols;i++){
			temp.at<uchar>(j,i)=uchar(lf_raw.at<float>(j,i)/16);
		}
	}

Mat t;
resize(temp, t, Size(), 2, 2, INTER_CUBIC);

    for (size_t i = 0; i <pos_vec.size(); i++){
		//cout<<"  "<<i<<" "<<pos_vec[i]<<endl;
        int idx=int(pos_vec[i].x*2+0.5);
        int idy=int(pos_vec[i].y*2+0.5);
		//if ((idx<0)&&(idy<0)&&(idx>3279)&(idy>3279)) cout<<idx<<" "<<idy<<endl;
		for (int k=0;k<6;k++){
		    Point p= Point(round(idx+2*D*ix[k]),round(idy+2*D*sqrt(3)*iy[k]));	
			t.at<uchar>(p)=255;
		}

     t.at<uchar>(idy,idx)=255;
	}

imwrite(dir, t);
return true;
} 



#endif

