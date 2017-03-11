
#ifndef __EXTRA_FUN_H__
#define __EXTRA_FUN_H__
#include <iostream>
#include <fstream>
#include <cv.h>
#include <highgui.h>
#include <opencv2/ml/ml.hpp>
#include <opencv2/opencv.hpp>


#define __DEBUG__


using namespace std;
using namespace cv;


//input a color picture, this function will change input picture
void FloodFill(Mat src);
//line transiation for gray images
void Line_Trans(Mat src, Mat& des);
// detect Rect region after pre process
void Rect_Detect(Mat src);
//used after canny detect
void mor_process(Mat src, Mat& des);


//for a binary picture, if the number of pixal value of 0
//more than half of picture, then reverse picture
void is_reverse(Mat src, Mat& des);

//After canny detect, we should detect hope times both in row
//and col to remove frame and rivets
void get_row_position(const Mat src, int& row_begin, int& row_end);
void get_col_position(const Mat src, int& col_begin, int& col_end);

//intput the binary ROI region picture, then we can get
//start and end position for every character
vector<int> get_char_position(const Mat src);
//get average width of character
int get_char_width(const Mat src);

vector<Mat> get_char(const Mat src, vector<int> Position);



//---------------------------- for ANN----------------------------//
//calculate sum gray value of a picture
float SumMatValue(const Mat& image);
//get picture feature1: resize picture to 16*8, and detect sobelX and
//sobelY picture, then divide picture to 8 subpicture as 4*4, then calculate
//sum of gray value for every picture, finally get persentage of this sum value
//in total picture gray summation
void SalcGradientFeat(const Mat& src, vector<float>& feat);

// calculate total feature of picture
vector<float> GetImgFeat(const Mat& src, vector<float>& feat);

// read picture from filename and get feature vector and save to train_inputs
void GetInput(string filename, vector<vector<float>>& inputs);


//
void MyAnnTrain(string train_file, string test_file);
#endif
