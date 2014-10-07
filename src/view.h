/*
    Extract views from microlens array raw images indcluding 4D demosaicing

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


#ifndef _VIEW
#define _VIEW

#include <iostream>
#include <stdio.h>
#include <cstring>

#include <omp.h>
#include <opencv2/opencv.hpp>
#include <vector>
#include "tool.h"
#include "config.h"
#include "h5_io.h"
#include "pipe.h"
using namespace std;
using namespace cv;
clock_t endt,start;

inline bool IsValidRange(int s, int range_min, int range_max)
{
    bool isValid = true;
    if (!((s>range_min)&&(s<range_max)))
    {
        isValid = false;
    }
    return isValid;
}


typedef struct
{
	int order; // orders of polynomial
	int type;  // 0: ckr 1: skr
	int wsize;//  windows size
	double sigma_x;//gaussian variance
	double sigma_y;//gaussian variance
	double sigma_u;//gaussian variance
	double sigma_v;//gaussian variance
	double alpha;//structure sensitivity
    double lambda;//regularization parameter
}Options;

typedef struct
{
	vector<vector<Point2f > > xy;
	vector<vector<Point2f > > uv;
	vector<vector<int > >     color;//0 B, 1 G, 2 R 3 border not considered

}LF_REG_MAP;
 
typedef struct
{
	vector<vector<Mat > > C;
	vector<vector<float > > gx; 
	vector<vector<float > > gy;
	vector<vector<float > > gu;
	vector<vector<float > > gv;

}LF_STRUCTURE;
 
bool registration_map(Mat& raw,
					  const vector<Point2d>& pos,
					  LF_REG_MAP& lf_map){
	//initilazition the 4d light field registration map
	(lf_map.xy).resize(raw.rows);
	(lf_map.uv).resize(raw.rows);
	(lf_map.color).resize(raw.rows);
	for (int j = 0; j < raw.rows; j++) {
		(lf_map.xy)[j].resize(raw.cols);
		(lf_map.uv)[j].resize(raw.cols);
		(lf_map.color)[j].resize(raw.cols);
		for (int i = 0; i< raw.cols; i++) {
			(lf_map.xy)[j][i]=Point2f(0.f,0.f);
			(lf_map.uv)[j][i]=Point2f(0.f,0.f);
			(lf_map.color)[j][i]=3;
		}
	}

	//scan the raw light field
    for (size_t j=0; j<ML_H; j++){//every microlens y axis
        for (size_t i=0; i<ML_W; i++){  //every microlens x axis
			for (int v=-6; v<7; v++){  //every vertical views
				for (int u=-6; u<7; u++){  //every horizontal views
					Point2f    center=pos[j*ML_W+i];
					Point      pt    =Point(round(center.x),round(center.y))+Point(u,v);
					//double     dist  = cv::norm(Point2f(pt.x,pt.y)-center);
					if ((IsValidRange(pt.x,0,raw.cols))&&(IsValidRange(pt.y,0,raw.rows))){
						if ((abs(pt.x-center.x)<4.9)&&(abs(pt.y-center.y)<4.9)){//within microlens image
							//euclidean distance
	 						(lf_map.xy)[pt.y][pt.x]=center;
							(lf_map.uv)[pt.y][pt.x]=Point2f(pt.x,pt.y)-center;
							//color
							if((pt.y+pt.x)%2==0){
								if((pt.x%2)==0){
									(lf_map.color)[pt.y][pt.x]=0;//B
								}
								else
									(lf_map.color)[pt.y][pt.x]=2;//R
							}
							else 	(lf_map.color)[pt.y][pt.x]=1;//G
						}
					}
				}
			}
		}
	}
	
return true;
}


bool	lf_struc_initialize(Mat& raw,LF_STRUCTURE& lf_struc){

	//initilazition the 4d light field structure map
	(lf_struc.gx).resize(raw.rows);
	(lf_struc.gy).resize(raw.rows);
	(lf_struc.gu).resize(raw.rows);
	(lf_struc.gv).resize(raw.rows);
	(lf_struc.C).resize(raw.rows);
	for (int j = 0; j < raw.rows; j++) {
		(lf_struc.gx)[j].resize(raw.cols);
		(lf_struc.gy)[j].resize(raw.cols);
		(lf_struc.gu)[j].resize(raw.cols);
		(lf_struc.gv)[j].resize(raw.cols);
		(lf_struc.C)[j].resize(raw.cols);
		for (int i = 0; i< raw.cols; i++) {
			(lf_struc.gx)[j][i]=0;
			(lf_struc.gy)[j][i]=0;
			(lf_struc.gu)[j][i]=0;
			(lf_struc.gv)[j][i]=0;
			(lf_struc.C)[j][i] =Mat::eye(4,4,CV_32F);
		}
	}
return true;
}


bool crk( Mat& raw, Mat& raw2,
		  LF_REG_MAP& lf_reg_map,
		  //LF_STRUCTURE& lf_struc,
		  Options option,
		  int U, int V){ //view to be interpolated

	float i_ratio=float (raw.cols)/float (raw2.cols);
	float j_ratio=float (raw.rows)/float (raw2.rows);

	//int        r=option.wsize;//1;
	//float lambda=option.lambda;//1
	//float alpha =option.alpha;//0.5;
	//int size=2*r+1;
	int L=12;//need to improve

	int poly=1;
	if (option.order==1)  poly=5;
	//else               num=6;
	Mat vec_4d= Mat::zeros(4,1, CV_64F);//4d coordinates
	Mat y  =Mat::zeros(2*L*L,1,   CV_64F);
	Mat Xx =Mat::zeros(2*L*L,poly,CV_64F);//contain data exist and weight
	Mat Xx0=Mat::zeros(2*L*L,poly,CV_64F);//only contain data exist or not (1,0)

    for (int j=0; j< (raw2.rows); j++){ // scan every pixels of the raw images 
		//cout<<"gkr"<<j<<" "<<raw2.rows<<endl;
        for (int i=0; i<(raw2.cols); i++){ //
		//cout<<"gkr"<<j<<" "<<i<<endl;
			float jj= (j*j_ratio);
			float ii= (i*i_ratio);
			int   jjj= round(jj);
			int   iii= round(ii);
			for (int k=0;k<3;k++){
				int idx=0;
				for (int m=-L; m<(L+1); m++){
					for (int n=-L; n<(L+1); n++){
						//cout<<m<<" "<<n<<" "<<k<<endl;
						if ((IsValidRange(jjj+m,0,raw.rows))&&(IsValidRange(iii+n,0,raw.cols))){
							//if ((i==0)&&(j==1020)) cout<<k<<" "<<m<<" "<<n<<endl;
							if (lf_reg_map.color[jjj+m][iii+n]==k){
 
								float dist_x=(lf_reg_map.xy[jjj+m][iii+n].x-ii);
								float dist_y=(lf_reg_map.xy[jjj+m][iii+n].y-jj);
								float dist_u=(lf_reg_map.uv[jjj+m][iii+n].x-U);
								float dist_v=(lf_reg_map.uv[jjj+m][iii+n].y-V);
								//float tt2= dist_x*dist_x+dist_y*dist_y+dist_u*dist_u+dist_v*dist_v; 
								if ((abs(dist_x)<22)&&(abs(dist_y)<22)&&(abs(dist_u)<3.2)&&(abs(dist_v)<3.2))
								{

									//Mat KC=Mat::eye(4, 4, CV_64F);//lf_struc.C[jjj+m][iii+n];
									double xx=dist_x/option.sigma_x; 
									double yy=dist_y/option.sigma_y;
									double uu=dist_u/option.sigma_u;
									double vv=dist_v/option.sigma_v;

									double tt=xx*xx+yy*yy+uu*uu+vv*vv;

									double w =exp(-(0.5*(tt)));
									
									y.at<double>  (idx,0) = raw.at<float>(jjj+m,iii+n);
									Xx0.at<double>(idx,0) = 1;
									Xx.at<double>(idx,0) =  w;
		
									if (option.order==1){			
											Xx0.at<double>(idx,1) =    dist_x;
											Xx0.at<double>(idx,2) =    dist_y;
											Xx0.at<double>(idx,3) =    dist_u;
											Xx0.at<double>(idx,4) =    dist_v;
											Xx.at<double>(idx,1) =    w*dist_x;
											Xx.at<double>(idx,2) =    w*dist_y;
											Xx.at<double>(idx,3) =    w*dist_u;
											Xx.at<double>(idx,4) =    w*dist_v;
									}
									idx++;	
								}
							}	
						}
					}
				}
				//cout<<"---"<<endl;
				if (idx>4){
					Mat Xxtr  = Xx (Rect(0,0,poly,idx));
					Mat Xx0tr = Xx0(Rect(0,0,poly,idx));
					Mat ytr   = y  (Rect(0,0,1,   idx));
					Mat M_reg = Mat::eye(poly, poly, CV_64F)*0.00001;
					Mat p=(Xx0tr.t()*Xxtr+M_reg).inv()*Xxtr.t()*ytr;

					raw2.at<Vec3f>(j,i)[k]=(float) p.at<double>(0,0);

				}

			}


		}
	}


return true;
}

bool gkr_color( Mat& raw,
				const vector<Point2d>& pos,
				Options option,
				String filename){

	LF_REG_MAP lf_reg_map;
	LF_STRUCTURE lf_struc;
	registration_map(raw,pos,lf_reg_map);
	lf_struc_initialize(raw,lf_struc);
	stringstream convert;

    start=clock();
	Mat raw3(VIEW_H+10,VIEW_W+10,CV_32FC3);
	Mat multi_view(VIEW_H*(2*TT+1),VIEW_W*(2*SS+1),CV_32FC3);

	for (int j=-TT; j<(TT+1); j++){
		for (int i=-SS; i<(SS+1); i++){
			//cout<<i<<" "<<j<<endl;
			crk(raw,raw3,lf_reg_map,option,j,i);//lf_struc
			Mat raw_ROI=raw3(Rect(5,5,VIEW_H,VIEW_W)).clone();
			image_adjust(raw_ROI);
			raw_ROI.copyTo(multi_view (Rect((j+TT)*VIEW_H,(i+SS)*VIEW_W,VIEW_H,VIEW_W)));
			convert <<"../out/"<<filename<<"/"<<"views/"<<"view_"<<filename<<"_"<<j<<"_"<<i<<".png";		
			imwrite(convert.str(),raw_ROI);
			convert.str("");
		}
	}

	endt=clock();
	cout<<(endt-start)/1000000<<" S"<<endl;
	convert.str("");
    convert <<"../out/"<<filename<<"/"<<"views/"<<"multiview_"<<filename<<".png";		
	imwrite(convert.str(),multi_view);
	return true;
}

#endif
