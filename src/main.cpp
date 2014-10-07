/*
    Multi-view Image Restoration
    This software library implements the maxflow algorithm described in

	"Multi-view Image Restoration From Plenoptic Raw Images"
	 Shan Xu, Zhi-Liang Zhou and Nicholas Devaney
	In Asian Conference on Computer Vision 2014
    Emerging Topics In Image Restoration and Enhancement workshop

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

#include <omp.h>
#include <opencv2/opencv.hpp>
#include <iostream>
#include <stdio.h>
#include <vector>
#include <cstring>
#include <stdlib.h>
#include "config.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "raw_loader.h"
#include "h5_io.h"
#include "grid.h"
#include "view.h"
#include "pipe.h"
#include "vig.h"

using namespace std;
using namespace cv;


//-------main--------------
int main(int argc, char **argv){

ostringstream file_name;

String dir,filename;
if (argc > 1) {
  dir  = String(argv[1]);
  filename = String(argv[2]);
}
else{
  cout<<"usage: path + raw_file_name "<<endl;
}


if(!filename.empty()){

	//creat sub folder
	struct stat sb;
    file_name<<"../out/"<< filename;
	//cout<<file_name.str()<<endl;
	if ( stat((file_name.str()).c_str(), &sb) == -1) {
		 mkdir((file_name.str()).c_str(), 0700);
	     ostringstream subdir1,subdir2;
         subdir1<<file_name.str()<<"/views";
		 mkdir((subdir1.str()).c_str(), 0700);
         subdir2<<file_name.str()<<"/debug";
		 mkdir((subdir2.str()).c_str(), 0700);			
	}

	//-----load raw file
	Mat raw(IMG_H,IMG_W,CV_32FC1);
	//"../../LYTRO_DATABASE/tsinghua/iso_chart_3/7_imageRef0.raw"
	file_name.str("");
	file_name<< dir << filename << ".raw" ;
	raw2buf(file_name.str(),raw);

	file_name.str("");

	//-----microlens image centers registration
	file_name<< "../out/" << filename<<"/debug/" << filename <<"_pt.h5" ;
	cout<<file_name.str()<<endl;
	FILE *fp=fopen((file_name.str()).c_str(),"r");
	vector<Point2d> grid_array;

	if (fp){
		cout<<"Loading previoius result"<<endl;
		grid_array.resize(ML_W*ML_H);
		string file=(file_name.str()).c_str();
		hdf5_read_2d_pt((file_name.str()).c_str(), grid_array);
		//cout<<grid_array.size()<<endl;
	}
	else {
		cout<<"Building the the grid model ..."<<endl;
    	grid_array_gen(raw,grid_array,String(argv[2]));
		hdf5_writer_2d_pt((file_name.str()).c_str(), grid_array);
	}


	//-----4d demosaicing
	raw=raw/16;

	Mat raw2(raw.rows,raw.cols,CV_32FC3);//this is debayered and fine interpolate
	Options option;
	option.order=1; 
	option.wsize=3;

	float cc=0.4;

	option.sigma_x=2*cc*20; //5, 10
	option.sigma_y=2*cc*20;
	option.sigma_u=2*cc;
	option.sigma_v=2*cc;
	cout<<"Rendering views"<<endl;
	gkr_color(raw,grid_array,option,String(argv[2]));//current is designed for 0-256 range

}	

cout<<"Finish !"<<endl;
return true;

}
