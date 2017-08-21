#include <iostream>  
#include <string>  
#include "PreProcess.hpp"
#include "PlateLocate.hpp"
#include "CommonHead.hpp"
#include "HelpFun.hpp"
#include "Extra_Fun.h"
using namespace std;
using namespace cpr;

int main()
{
	Mat img = imread("./imgtrain/isplate/img_lib/Level_3/001.jpg");

	
	
	if (img.empty())
		return -2;
	namedWindow("source image", CV_WINDOW_AUTOSIZE);
	imshow("source image", img);
	
	PreProcess PP(img);
	PP.setIsResize(false);
	PP.setResizeFactor(1.0);
	PP.startPreProcess();
	

	
	PlateLocate PL(PP.getMat());
	PL.setMorpySize(9, 5);
	PL.setPlateRatio(1.0, 4.0);
	PL.setPlateWidth(30, 100);
	PL.setPlateHeight(15, 50);
	PL.setPlateMaxAngle(30);	
	PlateQuality Q = HARD;
	//PL.setDefaultPlateParam(Q);
	vector<Mat> plates = PL.startPlateLocate(PP);
	PL.showImg();
	PP.showImg();
	if (plates.size() > 0){	
		namedWindow("aa", CV_WINDOW_AUTOSIZE);
		imshow("aa", plates[0]);
		if (PL.getDetectedProcess()){
			cout << "the plate is detected in first process.\n";
		}
		else{
			cout << "the plate is detected in second process.\n";
		}	
	}

	
	waitKey();
	return 0;
}
