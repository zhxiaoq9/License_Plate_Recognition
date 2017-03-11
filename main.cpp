#include "Extra_Fun.h"


//save sorce picture
Mat src_img;
// flag for canny detect directly
bool is_canny_detected = false;
// be true if roi regoin at bottom half of picture
bool bottom_part = true;
//detected ROI Region
Rect Rect_ROI;


int main(int argc, char** argv)
{

	Mat src_gray, img;

//---------------------------step1 pre process---------------------------//
	//Read image and convert it to gray image if it is RGB
	img = imread("./picture/car7.bmp");

	img.copyTo(src_img);
	//namedWindow("src_img", 1);
	//imshow("src_img", src_img);

	// select a blue seedPoint and floodFill the source picture
	FloodFill(img);
	//namedWindow("floodFill", 1);
	//imshow("floodFill", img);
	// change to gray
	if (!img.data) { printf("Image read failure!\n"); return -1; }

	if (img.channels() == 3)
		cvtColor(img, src_gray, CV_RGB2GRAY);
	else
		src_gray = img;

	//namedWindow("source gray image", 1);
	//imshow("source gray image", src_gray);

	///median Blur
	medianBlur(src_gray, src_gray, 5);
	//namedWindow("median Blur", 1);
	//imshow("median Blur", src_gray);

	///Canny Detect
	Canny(src_gray, src_gray, 100, 150,5);
	//namedWindow("Canny", 1);
	//imshow("Canny", src_gray);
	Mat Canny_Tmp(src_gray);
	src_gray.copyTo(Canny_Tmp);

//---------------------------step2 find position---------------------------//
	// detect directly after canny process
	Rect_Detect(src_gray);

	///if directly detect failed , process with morphology operation after canny detect
	if (is_canny_detected == false)
	{
		mor_process(Canny_Tmp, Canny_Tmp);
		//namedWindow("morphology", 1);
		//imshow("morphology", Canny_Tmp);
		Rect_Detect(Canny_Tmp);
		is_canny_detected = false;
	}

	/// after this we can get position of ROI(Rect_ROI)
	Mat img_roi;
	src_img(Rect_ROI).copyTo(img_roi);
	//namedWindow("img_roi", 1);
	//imshow("img_roi", img_roi);

#ifdef __DEBUG__
	if (is_canny_detected)
		printf("Detected directly after canny detect!\n");
	else
		printf("Detected with morphology after canny detect!\n");
	printf("Position of Rect_ROI is x = %d, y = %d, width = %d, height = %d\n", Rect_ROI.x, Rect_ROI.y, Rect_ROI.width, Rect_ROI.height);
	printf("Area of Rect_ROI is : %d.\n", Rect_ROI.area());
#endif
//---------------------------step3 pre process roi region(img_roi)---------------------------//
	Mat roi_binary;
	Mat roi_canny;
	int row_begin = 0;
	int row_end = 0;
	int col_begin = 0;
	int col_end = 0;
	cvtColor(img_roi, img_roi, CV_RGB2GRAY);
	medianBlur(img_roi, img_roi, 3);
	threshold(img_roi, roi_binary, 0, 255, CV_THRESH_OTSU);

	is_reverse(roi_binary, roi_binary);
	namedWindow("inv_binary", 1);
	imshow("inv_binary", roi_binary);

	//medianBlur(img_roi, img_roi, 3);

	roi_binary.copyTo(roi_canny);
	Canny(roi_canny, roi_canny, 100, 150, 3);
	namedWindow("canny", 1);
	imshow("canny", roi_canny);

	get_row_position(roi_canny, row_begin, row_end);
	//cout << row_begin << endl;
	//cout << row_end << endl;

	Mat row_roi = roi_canny.rowRange(row_begin, row_end + 1).clone();
	//namedWindow("row_roi", 1);
	//imshow("row_roi", row_roi);

	get_col_position(row_roi, col_begin, col_end);
	//cout << col_begin << endl;
	//cout << col_end << endl;
	Mat col_roi = row_roi.colRange(col_begin, col_end).clone();
	//namedWindow("col_roi", 1);
	//imshow("col_roi", col_roi);

	Mat ROI = roi_binary(Range(row_begin, row_end + 1), Range(col_begin, col_end)).clone();
	namedWindow("ROI", 1);
	imshow("ROI", ROI);
	/*
	Mat ROI_Gray = img_roi(Range(row_begin, row_end + 1), Range(col_begin, col_end)).clone();
	namedWindow("ROI_Gray", 1);
	imshow("ROI_Gray", ROI_Gray);
	threshold(ROI_Gray, ROI_Gray, 0, 255, CV_THRESH_OTSU);
	is_reverse(ROI_Gray, ROI_Gray);
	*/
	//---------------------------step4 Split character(ROI)---------------------------//

	vector<int> Position = get_char_position(ROI);
	//int ave_width = get_char_width(ROI);
	vector<Mat> char_all = get_char(ROI, Position);
	cout << char_all.size() << endl;

	namedWindow("Char1", 1);
	imshow("Char1", char_all[0]);
	namedWindow("Char2", 1);
	imshow("Char2", char_all[1]);
	namedWindow("Char3", 1);
	imshow("Char3", char_all[2]);
	namedWindow("Char4", 1);
	imshow("Char4", char_all[3]);
	namedWindow("Char5", 1);
	imshow("Char5", char_all[4]);
	namedWindow("Char6", 1);
	imshow("Char6", char_all[5]);
	namedWindow("Char7", 1);
	imshow("Char7", char_all[6]);
	//namedWindow("Char8", 1);
	//imshow("Char8", char_all[7]);
	waitKey(0);
	return 0;
}

