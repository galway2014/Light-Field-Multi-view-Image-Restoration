/*
    Tools

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

#ifndef _TOOLS
#define _TOOLS

#include <iostream>
#include <stdio.h>
#include <cstring>

#include <omp.h>
#include <opencv2/opencv.hpp>
#include <vector>


using namespace std;

//return the value that is percentage of Mat
float precent (const Mat& img, float percent)
{
	double min, max;
	minMaxLoc(img, &min, &max);
	int size=img.cols*img.rows;
    int number_bin=100;
    float* cdf_ptr=new float [number_bin]; 
    memset(cdf_ptr,0,(100)*sizeof(float));

    float inv_size=1/float(size);
    float min_value= float(min);//*min_element(data_ptr,data_ptr+size);
    float max_value= float(max);//*max_element(data_ptr,data_ptr+size);	
    float value;	
    float bin_interval= (max_value - min_value) / (float (number_bin) - 1);// range of a bin
    if (bin_interval!=0){ 
    float first_value = min_value - 0.5*bin_interval;// start of bin value
    //float last_value  = first_value + bin_interval * number_bin;// end of bin value
    //cout<<first_value<<" "<<last_value<<" "<<bin_interval<<endl;
    for(int j=0;j<img.rows;j++)
    	for(int i=0;i<img.cols;i++)
		{
		int index=int((img.at<float>(j,i) - first_value)/bin_interval);  
		//cout<<index<<endl;
		(*(cdf_ptr+index))++;

		}
	
	*(cdf_ptr)=*(cdf_ptr)*inv_size;

    float* temp_ptr=cdf_ptr+1;	
    for(int i=1;i<number_bin;i++)
	{
		*(temp_ptr)=(*(temp_ptr)*inv_size+*(temp_ptr-1));
		temp_ptr++;    
	}
	int index=0;
    while (*(cdf_ptr+index)<percent)
   {index=index+1;}
   value=index*((max_value - min_value) / (float (number_bin) - 1))+first_value;
   }
   else value=min_value;

   return value;
}
#endif
