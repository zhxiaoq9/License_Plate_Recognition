#include "Extra_Fun.h"


void Line_Trans(Mat src, Mat& des)
{
	assert(src.rows == des.rows &&src.cols == des.cols);
	int img_rows, img_cols;
	int level_low, level_high;
	
	int gray_times[256] = { 0 };
	float gray_rat[256] = { 0 };
	float gray_acc[256] = { 0 };

	img_rows = src.rows;
	img_cols = src.cols;


	//calculate every gray level times
	for (int i = 0; i < img_rows; i++)
	{
		uchar* ptr = (uchar*)(src.data + i*img_cols);
		//uchar* data = src.ptr<uchar>(i);
		for (int j = 0; j < img_cols; j++)
		{
			gray_times[ptr[j]]++;
		
		}

	}
	//calculate ratio of every gray level
	for (int i = 0; i < 256; i++)
	{
		gray_rat[i] = (float) gray_times[i] / float(img_rows*img_cols);

	}
	//calculate accumulation  of every gray level
	for (int i = 0; i < 256; i++)
	{
		if (i == 0)
			gray_acc[i] = gray_rat[i];
		else
			gray_acc[i] = gray_acc[i-1] + gray_rat[i];
	}
	//find two gray level
	for (int i = 0; i < 256; i++)
	{
		if (gray_acc[i] < 0.1)
			level_low = i;	
	}
	for (int i = 255; i >= 0; i--)
	{
		if (gray_acc[i] > 0.9)
			level_high = i;
	}


	//gray level streach src image
	for (int i = 0; i < img_rows; i++)
	{
		uchar *src_ptr = (uchar*)(src.data + i*img_cols);
		uchar *des_ptr = (uchar*)(src.data + i*img_cols);
		float tmp = 255 / (float(level_high - level_low));

		for (int j = 0; j < img_cols; j++)
		{
			uchar v = (uchar)(src_ptr[j]);
			if (v < level_low)
			{
				v = (uchar)0;
			}
			if (v > level_high)
			{
				//v = (uchar)255;
				v = (uchar)255;
			}
			v = (uchar)(tmp*(v - level_low));
			des_ptr[j] = v;
		}

	}
	return;
}

void Rect_Detect(Mat src)
{
	vector<vector<Point>> contours;
	Mat result(src.size(), CV_8U, Scalar(0));
	Mat Rect_show(src.size(), CV_8U, Scalar(0));
	int area_down = 2900, area_up = 9000;
	int ratio_down = 2, ratio_up = 5;

	//find and draw outline
	//findContours(src, contours, CV_RETR_LIST, CV_CHAIN_APPROX_NONE);
	findContours(src, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);
	drawContours(result, contours, -1, Scalar(255), 1,8);
	
	//extract outline and draw the Rect 
	for (int i = 0; i < contours.size(); i++)
	{
		float ratio=0;
		Rect R0 = boundingRect(contours[i]);
		int R0_y = R0.y;
		extern bool bottom_part;
		extern bool is_canny_detected;
	
		if ((bottom_part == true) && (R0_y >= (src.rows*0.25)))
		{
			if ((R0.area() > area_down) && (R0.area() < area_up))
			{
				ratio = (float(R0.width)) / (float(R0.height));
				
			}
		}
		else if ((bottom_part == false) && (R0_y <= (src.rows*0.75)))
		{
			if ((R0.area() > area_down) && (R0.area() < area_up))
			{
				ratio = (float(R0.width)) / (float(R0.height));
			}
		}
		else
		{
			ratio = 0;
		}

		if (ratio > ratio_down && ratio < ratio_up)
		{
			is_canny_detected = true;
			extern Rect Rect_ROI;
			Rect_ROI = R0;
			//extern Mat src_img;
			//rectangle(src_img, R0, Scalar(255), 2);
		}	
	}
	//namedWindow("contours",1);
	//imshow("contours", result);
	namedWindow("result image", 1);
	extern Mat src_img;
	imshow("result image", src_img);
	return;
}

//input must be color picture
//select a blue seedPoint and floodFill the source picture
//if can not find any blue seed Point, then we select center
//point of the picture as seed Point
//blut seed Pint needs to be
// 0<= R <= 20 0<= G <= 20 230<= B <= 255
void FloodFill(Mat src)
{
	uchar H = 0, S = 0, V = 0;
	int max_x = int(float(src.rows) * 0.85);
	int max_y = int(float(src.cols) * 0.5);
	int H_high = 125, H_low = 110;
	int S_high = 200, S_low = 50;
	int V_high = 200, V_low = 50;
	Mat hsv;
	vector<Mat> split_hsv;
	vector<int> pos_x(src.rows,0);
	vector<int> pos_y(src.cols,0);
	bool seed_flag = false;

	//H:0-180(Standard is 0-360) S:0-255 V:0-255
	//H->hsv[0] S->hsv[1] V->hsv[2]
	//for blue H>220 && H <250
	cvtColor(src, hsv, COLOR_BGR2HSV);
	split(hsv, split_hsv);
	Mat color_h(split_hsv[0]);
	Mat color_s(split_hsv[1]);
	Mat color_v(split_hsv[2]);

	// get all blue pixel position
	for (int i = 0; i < color_h.rows; i++)
	{
		for (int j = 0; j < color_h.cols; j++)
		{
			uchar* ptr_h = (uchar*)(color_h.data + i*color_h.cols);
			uchar tmp_h = ptr_h[j];
			uchar* ptr_s = (uchar*)(color_s.data + i*color_s.cols);
			uchar tmp_s = ptr_s[j];
			uchar* ptr_v = (uchar*)(color_v.data + i*color_v.cols);
			uchar tmp_v = ptr_v[j];
			if (((tmp_h>H_low) && (tmp_h<H_high)) && ((tmp_s>S_low) && (tmp_s<S_high)) && ((tmp_v>V_low) && (tmp_v<V_high)))
			{
				if (((i < (color_h.rows *0.75)) && (i >(color_h.rows *0.25))) &&
					((j < (color_h.cols *0.75)) && (j >(color_h.cols *0.25))))
				{
					max_x = i;
					max_y = j;
					H = tmp_h;
					S = tmp_s;
					V = tmp_v;
					#ifdef __DEBUG__
						seed_flag = true;
					#endif
				}
				
			}
		}
	}
	Point seed(max_x, max_y);
	
#ifdef __DEBUG__

	if (seed_flag == false)
		cout << "In FloodFill function, blue seed Point can not be found, default seed was choosen!\n";
	else
		cout << "In FloodFill function, blue seed Point was found!\n";
	cout << "Seed Point Position is:\n";
	cout << "x = " << seed.x << endl;
	cout << "y = " << seed.y << endl;
	printf("H = %d,S = %d, V = %d\n", H, S, V);
#endif

	Rect ccomp;
	floodFill(src, seed, Scalar(0, 0, 0), &ccomp, Scalar(20, 20, 20), Scalar(20, 20, 20));
	
	return;
}

//input must be gray or binary image
void mor_process(Mat src, Mat& des)
{
	int an1 = 2;
	int an2 = 1;
	Mat element1 = getStructuringElement(MORPH_RECT, Size(an1 * 2 + 1, an1 * 2 + 1), Point(an1, an1));
	Mat element2 = getStructuringElement(MORPH_RECT, Size(an2 * 2 + 1, an2 * 2 + 1), Point(an2, an2));
	morphologyEx(src, src, CV_MOP_CLOSE, element2);
	morphologyEx(src, src, CV_MOP_CLOSE, element2);
	//morphologyEx(src, src, CV_MOP_CLOSE, element2);
	dilate(src, src, element1);
	dilate(src, src, element1);
	dilate(src, src, element1);
	morphologyEx(src, src, CV_MOP_CLOSE, element2);
	morphologyEx(src, src, CV_MOP_OPEN, element1);
	des = src;
	return;
}

void is_reverse(Mat src, Mat& des)
{
	float zero_rate = 0;
	int zero_num = 0;
	for (int i = 0; i < src.rows; i++)
	{
		uchar* ptr = (uchar*)(src.data + i*src.cols);

		for (int j = 0; j < src.cols; j++)
		{
			if (ptr[j] == 0)
			{
				zero_num++;
			}
		}

	}

	zero_rate = ((float) zero_num) / (float (src.rows * src.cols));
	
	if (zero_rate > 0.55)
	{
		des = src;
		return;
	}

	for (int i = 0; i < src.rows; i++)
	{
		uchar* src_ptr = (uchar*)(src.data + i*src.cols);
		uchar* des_ptr = (uchar*)(des.data + i*des.cols);
		for (int j = 0; j < src.cols; j++)
		{
			if (src_ptr[j] == 0)
			{
				des_ptr[j] = 255;
			}
			else
			{
				des_ptr[j] = 0;
			}
		}

	}

	return;
}

void get_row_position(const Mat src, int& row_begin, int& row_end)
{
	int valid_hop_times = 13;
	int row = src.rows;
	int col = src.cols;
	vector<int> hop_times(0);
	uchar next_value = 0;
	uchar current_value = 0;
	int times = 0;

	for (int i = 0; i < row; i++)
	{
		times = 0;
		uchar* ptr = (uchar*)(src.data + i * col);
		for (int j = 0; j < col - 1; j++)
		{
			current_value = ptr[j];
			next_value = ptr[j + 1];
			if (current_value != next_value)
			{
				times++;
			}
		}
		hop_times.push_back(times);
	}

	/*
	for (int i = 0; i < hop_times.size(); i++)
	{
		if (hop_times[i] > valid_hop_times)
			row_end = i;
	}
	for (int i = hop_times.size() - 1; i >= 0; i--)
	{
		if (hop_times[i] > valid_hop_times)
			row_begin = i;
	}
	*/
	for (int i = 0; i < hop_times.size() - 2; i++)
	{
		if (hop_times[i] < valid_hop_times)
		{
			continue;
		}
		else if (hop_times[i + 1] < valid_hop_times)
		{
			continue;
		}
		else if (hop_times[i + 2] < valid_hop_times)
		{
			continue;
		}
		else
		{
			row_end = i + 2;
		}

	}

	for (int i = hop_times.size() - 1; i > 1; i--)
	{
		if (hop_times[i] < valid_hop_times)
		{
			continue;
		}
		else if (hop_times[i - 1] < valid_hop_times)
		{
			continue;
		}
		else if (hop_times[i - 2] < valid_hop_times)
		{
			continue;
		}
		else
		{
			row_begin = i - 2;
		}

	}
	if (row_begin < 0)
	{
		cout << "Row_begin is less than or equal to 0 in function get_row_position" << endl;
		exit(1);
	}
	if (row_begin >= row_end)
	{
		cout << "Row_begin is larger than or equal to Row_end in function get_row_position" << endl;
		exit(1);
	}
	if (row_end >= row)
	{
		cout << "Row_end is larger than or equal to maximun row in function get_row_position" << endl;
		exit(1);
	}
	return;
}

void get_col_position(const Mat src, int& col_begin, int& col_end)
{
	int valid_hop_times = 2;
	int row = src.rows;
	int col = src.cols;
	vector<int> hop_times(0);
	uchar next_value = 0;
	uchar current_value = 0;
	int times = 0;

	for (int j = 0; j < col; j++)
	{
		times = 0;		
		for (int i = 0; i < row - 1; i++)
		{
			uchar* current_ptr = (uchar*)(src.data + i * col);
			uchar* next_ptr = (uchar*)(src.data + (i + 1) * col);

			current_value = current_ptr[j];
			next_value = next_ptr[j];

			if (current_value != next_value)
			{
				times++;
			}
		}
		hop_times.push_back(times);
	}

	for (int i = 0; i < hop_times.size(); i++)
	{
		if (hop_times[i] > valid_hop_times)
			col_end = i;
	}
	for (int i = hop_times.size() - 1; i >= 0; i--)
	{
		if (hop_times[i] > valid_hop_times)
			col_begin = i;
	}


	
	if (col_begin < 0)
	{
		cout << "Col_begin is less than or equal to 0 in function get_col_position" << endl;
		exit(1);
	}
	if (col_begin >= col_end)
	{
		cout << "Col_begin is larger than or equal to Col_end in function get_col_position" << endl;
		exit(1);
	}
	if (col_end >= col)
	{
		cout << "Col_end is larger than or equal to maximun col in function get_col_position" << endl;
		exit(1);
	}
	return;


}

vector<int> get_char_position(const Mat src)
{
	int valid_dots = 1;
	float low_ratio = 1.5;
	float high_ratio = 5;
	vector<int> Position;
	vector<int> nCount;
	vector<int> nCharacter;
	int start = 0;
	int end = 0;
	int row = src.rows;
	int col = src.cols;
	int num = 0;

	//get none zero number for every colum
	for (int j = 0; j < col; j++)
	{
		num = 0;
		for (int i = 0; i < row; i++)
		{
			uchar* ptr = (uchar*)(src.data + i*col);
			if (ptr[j] != 0)
			{
				num++;
			}
		}
		nCount.push_back(num);
	}
	for (int i = 0; i < nCount.size(); i++)
	{
		if (nCount[i] <= valid_dots)
			nCount[i] = 0;
		else
			nCount[i] = 1;
	}
	//extract possible character position, save start and stop position
	int current = 0;
	for (int i = 0; i < nCount.size(); i++)
	{
		int next = nCount[i];
		if ((current == 0) && (next == 1))
			start = i;
		if ((current == 1) && (next == 0))
		{
			end = i;
			nCharacter.push_back(start);
			nCharacter.push_back(end);
			start = 0;
			end = 0;
		}
		
		current = next;
	}

	if (nCount[nCount.size() - 1] != 0)
	{
		end = nCount.size();
		nCharacter.push_back(start);
		nCharacter.push_back(end);
	}

	//determine if position is a real position		
	for (int i = 0; i < nCharacter.size(); i = i + 2)
	{
		int nStart = nCharacter[i];
		int nEnd = nCharacter[i+1];
		
		float ratio = (float (src.rows))/(float(nEnd - nStart));
		/*
		if ((ratio > low_ratio) && (ratio < high_ratio))
		{
			Position.push_back(nStart);
			Position.push_back(nEnd);
		}
		*/
		if ((ratio > low_ratio))
		{
			if (ratio < high_ratio)
			{
				Position.push_back(nStart);
				Position.push_back(nEnd);
			}
			else if ((nEnd - nStart) > 2) //special test for character '1'
			{
				int front = 0;
				float percent = 0;
				for (int k = 0; k < row; k++)
				{
					uchar *ptr = (uchar*)src.data + k * col;
					if (ptr[nStart+2] > 0)
					{
						front++;
					}
				}
				percent = ((float) front) / ((float)row);
				if (percent > 0.9)
				{
					Position.push_back(nStart);
					Position.push_back(nEnd);
				}

			}
		}
	}

	return Position;

}

int get_char_width(const Mat src)
{
	int valid_dots = 1;
	float low_ratio = 1.5;
	float high_ratio = 5;
	vector<int> Width;
	vector<int> nCount;
	vector<int> nCharacter;
	int start = 0;
	int end = 0;
	int row = src.rows;
	int col = src.cols;
	int num = 0;

	//get none zero number for every colum
	for (int j = 0; j < col; j++)
	{
		num = 0;
		for (int i = 0; i < row; i++)
		{
			uchar* ptr = (uchar*)(src.data + i*col);
			if (ptr[j] != 0)
			{
				num++;
			}
		}
		nCount.push_back(num);
	}
	for (int i = 0; i < nCount.size(); i++)
	{
		if (nCount[i] <= valid_dots)
			nCount[i] = 0;
		else
			nCount[i] = 1;
	}
	//extract possible character position, save start and stop position
	int current = 0;
	for (int i = 0; i < nCount.size(); i++)
	{
		int next = nCount[i];
		if ((current == 0) && (next == 1))
			start = i;
		if ((current == 1) && (next == 0))
		{
			end = i;
			nCharacter.push_back(start);
			nCharacter.push_back(end);
			start = 0;
			end = 0;
		}

		current = next;
	}

	if (nCount[nCount.size() - 1] != 0)
	{
		end = nCount.size();
		nCharacter.push_back(start);
		nCharacter.push_back(end);
	}

	//determine if position is a real position		
	for (int i = 0; i < nCharacter.size(); i = i + 2)
	{
		int nStart = nCharacter[i];
		int nEnd = nCharacter[i + 1];

		float ratio = (float(src.rows)) / (float(nEnd - nStart));

		if ((ratio > low_ratio) && (ratio < high_ratio))
		{
			Width.push_back(nEnd - nStart);
		}
	}
	
	sort(Width.begin(), Width.end());
	int index = (Width.size() + 1) / 2;

	return Width[index];

}

vector<Mat> get_char(const Mat src, vector<int> Position)
{
	Mat character;
	vector<Mat> char_all;

	int left = 0;
	int right = 0;
	/*
	right = Width;
	character = src.colRange(left, right).clone();
	char_all.push_back(character);

	left = (Width * 57) / 45;
	right = (Width * 102) / 45;
	character = src.colRange(left, right).clone();
	char_all.push_back(character);

	left = (Width * 136) / 45;
	right = (Width * 181) / 45;
	character = src.colRange(left, right).clone();
	char_all.push_back(character);

	left = (Width * 193) / 45;
	right = (Width * 238) / 45;
	character = src.colRange(left, right).clone();
	char_all.push_back(character);

	left = (Width * 250) / 45;
	right = (Width * 295) / 45;
	character = src.colRange(left, right).clone();
	char_all.push_back(character);

	left = (Width * 307) / 45;
	right = (Width * 352) / 45;
	character = src.colRange(left, right).clone();
	char_all.push_back(character);

	left = (Width * 364) / 45;
	right = (Width * 409) / 45;
	character = src.colRange(left, right).clone();
	char_all.push_back(character);
	*/
	
	for (int i = 0; i < Position.size(); i = i + 2)
	{
		left = Position[i];
		right = Position[i+1];
		character = src.colRange(left, right).clone();
		char_all.push_back(character);
	}
	
	return char_all;

}










//----------------------for ANN----------------------//
float SumMatValue(const Mat& image)
{
	float sum = 0;
	int row = image.rows;
	int col = image.cols;

	for (int i = 0; i < row; i++)
	{
		const float* linePtr = image.ptr<float>(i);
		for (int j = 0; j < col; j++)
		{
			sum += linePtr[j];
		}
	}

	return sum;
}

void SalcGradientFeat(const Mat& src, vector<float>& feat)
{
	Mat image;
	resize(src, image, Size(8,16));
	
	//sobel x,y detect
	float mask[3][3] = { { 1, 2, 1 }, {0, 0, 0}, { -1, -2, -1 } };
	Mat y_mask = Mat(3, 3, CV_32F, mask) / 8;
	Mat x_mask = y_mask.t();
	Mat sobelX, sobelY;

	filter2D(image, sobelX, CV_32F, x_mask);
	filter2D(image, sobelY, CV_32F, y_mask);

	sobelX = abs(sobelX);
	sobelY = abs(sobelY);

	float totalValueX = SumMatValue(sobelX);
	float totalValueY = SumMatValue(sobelY);
	
	for (int i = 0; i < image.rows; i = i + 4)
	{
		for (int j = 0; j < image.cols; j = j + 4)
		{
			Mat subX = sobelX(Rect(j, i, 4, 4));
			feat.push_back(SumMatValue(subX) / totalValueX);
			Mat subY = sobelY(Rect(j, i, 4, 4));
			feat.push_back(SumMatValue(subY) / totalValueY);
		}
	}

}


vector<float> GetImgFeat(const Mat& src)
{
	Mat img;
	vector<float> feat;

	SalcGradientFeat(src, feat);

	// resize src picture to 8*4 then reshape to 1*32 and add to feat 
	resize(src, img, Size(4, 8));
	Mat img_reshape = img.reshape(0, 1);
	for (int i = 0; i < img_reshape.cols; i++)
	{
		uchar value = img_reshape.at<uchar>(0, i);
		feat.push_back(value);
	}
	return feat;
}

void GetInput(string filename, vector<vector<float>>& inputs)
{
	string line;
	vector<float> tmp_feat;

	ifstream IN(filename);

	if (!IN)
	{
		cout << "Failed to read file " << filename  << endl;
		return;
	}

	while (getline(IN, line))
	{
		Mat img = imread(line);
		if (!img.data) 
		{
				printf("Image read failure!\n");
				return ; 
		}
		tmp_feat = GetImgFeat(img);
		inputs.push_back(tmp_feat);
	}
	IN.close();
	return;
}


void MyAnnTrain(string train_file, string test_file)
{

	int feat_num = 48;
	int kinds = 34;
	int train_num_every_kind = 40;
	int sample_num = kinds * train_num_every_kind;

	vector<vector<float>> train_inputs;
	Mat Train_inputs(sample_num, feat_num, CV_32FC1);
	Mat Train_labels(sample_num, kinds, CV_32FC1, Scalar::all(0));


	//----------get all image input feature vector----------//
	GetInput(train_file, train_inputs);
	for (int i = 0; i < Train_inputs.rows; i++)
	{
		for (int j = 0; j < Train_inputs.cols; j++)
		{
			Train_inputs.at<float>(i, j) = train_inputs[i][j];
		}
	}
	//------------------------set label------------------------//
	for (int i = 0; i < kinds; i++)
	{
		for (int j = 0; j < train_num_every_kind; j++)
		{	
			Train_labels.at<float>(train_num_every_kind * i + j, i) = 1;
		}
	}

#ifdef  __DEBUG__
	ofstream fout;
	fout.open("Trainning_feature_vector_and_label.txt");
	fout << Train_inputs << endl;
	fout <<"\n\n" << endl;
	fout << Train_labels << endl;
	fout.close();
#endif
	//------------------------set trainning params------------------------//
	CvANN_MLP_TrainParams params;
	params.train_method = CvANN_MLP_TrainParams::BACKPROP;
	params.bp_dw_scale = 0.1;
	params.bp_moment_scale = 0.1;
	params.term_crit = cvTermCriteria(CV_TERMCRIT_ITER+CV_TERMCRIT_EPS, 5000, 0.01);
	//------------------------set trainning network------------------------//
	CvANN_MLP ANN;
	//set hidden layer to 58,we can get classification rate to 91%
	int ar[] = { 48, 58, 34 };
	Mat LayerSize(1, 3, CV_32S, ar);
	ANN.create(LayerSize, CvANN_MLP::SIGMOID_SYM);
	//----------------------------start trainning ----------------------------//
#ifdef  __DEBUG__
	cout << "Start ANN trainning" << endl;
	double t = (double)cvGetTickCount();
#endif

	ANN.train(Train_inputs, Train_labels,Mat(), Mat(),params);
	
#ifdef  __DEBUG__
	t = ((double)cvGetTickCount() - t) / (1000 * cvGetTickFrequency());
	cout << "ANN trainning Finished! Relative data is stored in ANN.xml" << endl;
	cout << "Trainning Time is : " << t << " ms\n\n" << endl;
#endif
	ANN.save("ANN.xml");

	//----------------------------prepare to test ----------------------------//
	//----------------------------get test inputs----------------------------//
	int test_num_every_kind = 10;
	vector<vector<float>> test_inputs;
	Mat Test_inputs(test_num_every_kind*kinds, feat_num, CV_32FC1);
	Mat Test_labels(test_num_every_kind*kinds, kinds, CV_32FC1, Scalar::all(0));
	

	GetInput(test_file, test_inputs);
	for (int i = 0; i < Test_inputs.rows; i++)
	{
		for (int j = 0; j < Test_inputs.cols; j++)
		{
			Test_inputs.at<float>(i, j) = test_inputs[i][j];
		}
	}
	//----------------------------start to test----------------------------//
	Mat Test_outputs(1, kinds, CV_32FC1);
	int error_num = 0;
	float error_rate = 0;
	float correct_rate = 0;
#ifdef  __DEBUG__
	t = (double)cvGetTickCount();
	cout << "Start ANN testing!" << endl;
#endif
	for (int i = 0; i < Test_inputs.rows; i++)
	{
		Point maxidx;

		//get a picture feature vector then predict output
		Mat tmp = Test_inputs.rowRange(i, i+1);
		ANN.predict(tmp, Test_outputs);

		//get maximun number index(maxidx.x), it represent
		//kind of picture
		minMaxLoc(Test_outputs, NULL, NULL, NULL, &maxidx);
		
		//compare predict result with standard out, if fault,
		//print out message
		if ((maxidx.x) != (i / test_num_every_kind))
		{
			error_num++;
		#ifdef  __DEBUG__
			cout << "Filed to predict at line : " << i << endl;
			cout << "predicted = " << maxidx.x << ", but should be\t: " << (i / test_num_every_kind) << endl;
		#endif
		}
	}
	error_rate = float(error_num) / (test_num_every_kind*kinds);
	correct_rate = 100 * (1 - error_rate);
#ifdef  __DEBUG__
	cout << "Finish ANN testing!\n\n" << endl;
	cout << "There are " << (test_num_every_kind*kinds) << " test pictures, " << error_num << " can't be classified correctly!" << endl;
	cout << "Ratio of crrect is : "<< correct_rate << endl;
	t = ((double)cvGetTickCount() - t) / (1000 * cvGetTickFrequency());
	cout << "Testing Time is : " << t << " ms\n\n" << endl;
#endif
	return;
}
