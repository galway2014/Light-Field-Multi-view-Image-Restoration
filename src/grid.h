/*
    Generate the real centers grid array by solving the global optimization problem.

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


#ifndef _GRID
#define _GRID

#include <omp.h>
#include <opencv2/opencv.hpp>

#include <iostream>
#include <stdio.h>
#include <vector>
#include <cstring>
#include <stdlib.h>

#include "config.h"
#include "debug.h"
//#include "hdf5_file_2d.h"
//#include "debug.h"
using namespace std;
using namespace cv;

/**
 *  gen_score_map()
 *  Generate the score map based on the local estimator
 *
 */


bool gen_score_map(Mat& img, Mat& score){

double idy[6]={1.0f/3.0f, -1.0f/3.0f, 1.0f/6.0f,  1.0f/6.0f, -1.0f/6.0f, -1.0f/6.0f}; 
double idx[6]={        0,          0,       0.5,       -0.5,        0.5,       -0.5};

for(int j=0;j<IMG_H*MAG;j++){
	for(int i=0;i<IMG_W*MAG;i++){

		if ((j>D*MAG)&&(i>D*MAG)&&(j<(IMG_H-D)*MAG)&&(i<(IMG_W-D)*MAG)){
			double avg=0;
			double v;
			for (int t=0;t<6;t++){
		    	Point pt= Point(round(i+MAG*D*idx[t]),round(j+MAG*D*sqrt(3)*idy[t]));
				v=img.at<float>(pt);
				avg=avg+v;				
			}

 		    score.at<float>(j,i)=avg/6;
		}
	}
	//cout<<j<<endl;
}

//imwrite("../out/score_max.png",score(Rect(0,0,IMG_W,IMG_H))/16);
return true;

}


/**
 *  gen_pos()
 *  @flag 0. center region 1. outer regreion
 *        2. entire region  3. entir real postion grid array
 */

bool gen_pos(vector<Point2d>& pos, int flag, double* t){ //no maginification

    float d1=D*MAG;
	float d2=D*sqrt(3)*MAG;

	if (flag==0){
		for(int j=-5;j<6; j++){		
			for(int i=-10;i<11;i++){
				pos.push_back(Point2d(i*d1,j*d2)); 
				pos.push_back(Point2d(i*d1-d1/2,j*d2-d2/2));        
			}
		}
	}

	else if (flag==1){
		for(int j=80;j>-81; j=j-2){		
			for(int i=-135;i<136;i=i+2){
				if ((abs(j)>70)||(abs(i)>125)){
					pos.push_back(Point2d(i*d1,j*d2)); 
					pos.push_back(Point2d(i*d1-d1/2,j*d2-d2/2));  
				} 
			}
		}
	}

	else if (flag>1){ //entire perfrect array
		int hh=(ML_H-2)/4;
		int ww=(ML_W-1)/2;
		for(int j=hh;j>-(hh-1); j--){		
			for(int i=-ww;i<(ww+1);i++){
				pos.push_back(Point2d(i*d1,j*d2)); 
				pos.push_back(Point2d(i*d1-d1/2,j*d2-d2/2));       
			}
		}
	}
	
	if (flag==3){ //real grid array

		for(size_t c=0;c<pos.size();c++){	

			float xx=pos[c].x +t[0];
			float yy=pos[c].y +t[1];

			pos[c].x =(IMG_W*MAG-1)/2+ (t[2]*xx +t[3]*yy);
			pos[c].y =(IMG_H*MAG-1)/2- (t[4]*xx +t[5]*yy);
		}
	}
	return true;
}


bool save_para(double* src, double* dst){

for (int i=0;i<6;i++){
	dst[i]=src[i];
}

return true;
}


/**
 *  brute_force_search_2d()
 *  Brue Force Search in 2D
 *  return 2 updated parameters
 */


bool brute_force_search_2d (int* itn,
							double* step,
							double* tran_vec,
							vector<Point2d> pos,
							const Mat& img){
double tran_vec_temp[6];
double tran_vec_optimum[6];
save_para(tran_vec,tran_vec_temp);

double current=100000;

for(int g=-itn[0];g<(itn[0]+1);g++){
	for(int h=-itn[1];h<(itn[1]+1);h++){
	
		tran_vec_temp[0]=tran_vec[0]+g*step[0];
		tran_vec_temp[1]=tran_vec[1]+h*step[1];

		double next=0; 
		vector<Point2d> pos_2d;
		pos_2d.resize(pos.size());
		//#pragma omp parallel for 
		for(size_t kk=0;kk<pos.size();kk++){	    
		
		  	float xx=pos[kk].x + tran_vec_temp[0];
			float yy=pos[kk].y + tran_vec_temp[1];	

		  	pos_2d[kk].x=(IMG_W*MAG-1)/2+(xx*tran_vec[2] + yy*tran_vec[3]);
			pos_2d[kk].y=(IMG_H*MAG-1)/2-(xx*tran_vec[4] + yy*tran_vec[5]);	

			Point p=Point(round(pos_2d[kk].x),round(pos_2d[kk].y));

			next=next+img.at<float>(p);
		}
			next=next/pos.size();
			if (next<current){
				current=next;
				save_para(tran_vec_temp,tran_vec_optimum);
			}
			vector<Point2d>().swap(pos_2d);
	}
}
save_para(tran_vec_optimum,tran_vec);

//cout<<"---2d---"<<endl;
//cout<<current<<"  "<<tran_vec[0]<<"  "<<tran_vec[1]<<endl;
return true;
}

/**
 *  brute_force_search_4d()
 *  Brue Force Search in 4D
 *  return 4 update parameters
 */

bool brute_force_search_4d (int* itn,
							double* step,
							double* tran_vec,
							vector<Point2d> pos,
							const Mat& img,
							double& previous){
double tran_vec_temp[6];
double tran_vec_optimum[6];
save_para(tran_vec,tran_vec_temp);

double current=100000;

		for(int m=-itn[2];m<(itn[2]+1);m++){
			for(int n=-itn[3];n<(itn[3]+1);n++){
				for(int j=-itn[4];j<(itn[4]+1);j++){
					for(int i=-itn[5];i<(itn[5]+1);i++){

						tran_vec_temp[2]=tran_vec[2]+m*step[2];
						tran_vec_temp[3]=tran_vec[3]+n*step[3];
						tran_vec_temp[4]=tran_vec[4]+j*step[4];
						tran_vec_temp[5]=tran_vec[5]+i*step[5];

						double next=0; 
						vector<Point2d> pos_2d;
						pos_2d.resize(pos.size());
						//#pragma omp parallel for 
						for(size_t kk=0;kk<pos.size();kk++){	    
		
							double xx=pos[kk].x+tran_vec[0];
							double yy=pos[kk].y+tran_vec[1];

		  					pos_2d[kk].x=(IMG_W*MAG-1)/2+(xx*tran_vec_temp[2] + yy*tran_vec_temp[3]);
							pos_2d[kk].y=(IMG_H*MAG-1)/2-(xx*tran_vec_temp[4] + yy*tran_vec_temp[5]);	
							Point p=Point(round(pos_2d[kk].x),round(pos_2d[kk].y));
							
							next=next+img.at<float>(p);									

						}
						next=next/pos.size();
						if (next<current){
							current=next;	
							save_para(tran_vec_temp,tran_vec_optimum);
						}
						vector<Point2d>().swap(pos_2d);

					}
				}
			}	
		}

if (current<previous){
	previous=current;
	save_para(tran_vec_optimum,tran_vec);
}

//cout<<"---4d----"<<endl;
//cout<<current<<"  "<<tran_vec[2]<<"  "<<tran_vec[3]<<" "<<tran_vec[4]<<"  "<<tran_vec[5]<<endl;
//cout<<" value  is "<<current/16<<" "<<pos.size()<<endl;

return true;
}


/**
 * grid_optimization()
 * Find the opimized paramters by complete search
 * return a six paramters
 */
bool grid_optimization(const Mat& img, double* tran_mat_vec){

//initial condition
int    itn [6]    ={10,10,     4,   4,    4,    4};
double step[6]    ={ 3, 3,0.002,0.002,0.002,0.002};

cout<<"Optimization Step 1..."<<endl;
vector<Point2d> pos_vec;
gen_pos(pos_vec,0,NULL);//centeral region
for(int j=0;j<5;j++){
	brute_force_search_2d(itn,step,tran_mat_vec,pos_vec,img);
	for(int i=0;i<2;i++)
	step[i]=step[i]*0.6;
}
vector<Point2d>().swap(pos_vec);

cout<<"Optimization Step 2..."<<endl;

gen_pos(pos_vec,1,NULL);//outer region

double value=10000;
int cnt=0;

for(int j=0;j<20;j++){
	double pre=value;
	brute_force_search_4d(itn,step,tran_mat_vec,pos_vec,img,value);

	if ((pre-value)<0.1) cnt=cnt+1; //stop criterion
	else cnt=0;

	if (cnt>3){
		break;
	}

	for (int i=2;i<6;i++)
	step[i]=step[i]*0.6;
}

vector<Point2d>().swap(pos_vec);
cout<<"Optimization Step 3..."<<endl;

gen_pos(pos_vec,2,NULL);//entire region

itn[0]=8;
itn[1]=8;
step[0]=0.4;
step[1]=0.4;
brute_force_search_2d(itn,step,tran_mat_vec,pos_vec,img);
vector<Point2d>().swap(pos_vec);

return true;
}


/**
 * grid_array_gen()
 * Generate the real grid array
 * return a vector of points
 */


bool grid_array_gen(const Mat& img, vector<Point2d>& pos_array, String name){

Mat img_upsample(IMG_H*MAG,IMG_W*MAG,CV_32FC1);
Mat score_map   (IMG_H*MAG,IMG_W*MAG,CV_32FC1);
Mat img_blur;
GaussianBlur(img, img_blur, Size(5, 5), 0, 0);

//CPU code
resize(img_blur, img_upsample, Size(), MAG, MAG, INTER_CUBIC);

/*GPU code
cv::cuda::GpuMat img_gpu,image_resized;
img_gpu.upload(img_blur);
cv::cuda::resize(img_gpu, image_resized, Size(0,0),2,2,INTER_CUBIC);
image_resized.download(img_upsample);
*/
gen_score_map(img_upsample,score_map);
//cout<<"Finish generating the score map ... "<<endl;
                          //tx    ty
double tran_mat_vec[6]={0,0,1,0,0,1};

grid_optimization(score_map,tran_mat_vec);
//grid generation
gen_pos(pos_array,3,tran_mat_vec);//entire region


for (size_t i=0; i<pos_array.size(); i++){
	pos_array[i].y=pos_array[i].y/MAG;
	pos_array[i].x=pos_array[i].x/MAG;
	//cout<<pos_array[i]<<endl;
}

stringstream debug_file;
debug_file<<"../out/"<<name<<"/debug/"<<name<<"_center.png";		
lable_center(img, pos_array, debug_file.str());
//hdf5_writer_2d_pt("../out/pos.h5",pos_array);
img_upsample.release();
img_blur.release();
score_map.release();

return true;
}



#endif
