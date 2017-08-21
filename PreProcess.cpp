#include "PreProcess.hpp"

namespace cpr{
	//const float PreProcess::DEFAULT_FACOTR = 1.0f;

	PreProcess::PreProcess(){ init(); }
	PreProcess::PreProcess(const Mat &param) {
		init();		
		m_mat = param.clone();
		m_mat_backup = param.clone();		
	}

	void PreProcess::startPreProcess(){		
		if (m_type){
			if (m_is_resize){
				resize(m_mat, m_mat, Size(0, 0), m_factor, m_factor, m_resize_type);
				resize(m_mat_backup, m_mat_backup, Size(0, 0), m_factor, m_factor, m_resize_type);
			}
		}
		else{
			if (m_is_resize){
				resize(m_mat_backup, m_mat_backup, Size(0, 0), m_factor, m_factor, m_resize_type);
			}
			m_mat = m_mat_backup.clone();
		}
		
		// preprocess in color floorfill		
		if (m_type){									
			startFloodFill();
			cvtColor(m_mat, m_mat, CV_RGB2GRAY);
		}
		else{
			startColorMatch(m_mat);
			//GaussianBlur(m_mat, m_mat, Size(m_gauss_blur_x, m_gauss_blur_y), 0.0, 0.0);	
			
		}
		
	}


	void  PreProcess::showImg() const {
		namedWindow("PreProcessing", CV_WINDOW_AUTOSIZE);
		imshow("PreProcessing", m_mat);
	}
	bool  PreProcess::getType() const{ return m_type; }
	float PreProcess::getFactor() const{ return m_factor; }
	bool  PreProcess::getIsResize() const{ return m_is_resize; }
	int   PreProcess::getResizeType() const{ return m_resize_type; }
	int   PreProcess::getGaussBlurSizeX() const{ return m_gauss_blur_x; }
	int   PreProcess::getGaussBlurSizeY() const{ return m_gauss_blur_y; }
	hsv   PreProcess::getHSV() const{ return m_blue_hsv; }
	Mat   PreProcess::getMat() const{ return m_mat; }
	Mat   PreProcess::getMatBackup() const{ return m_mat_backup; }

	void  PreProcess::setType(bool param) { m_type = param; }
	void  PreProcess::setResizeFactor(float param){ m_factor = param; }
	void  PreProcess::setIsResize(bool param){ m_is_resize = param; }
	void  PreProcess::setResizeType(int param){
		assert(param >= 0 && param <= 4);
		m_resize_type = param;
	}
	void  PreProcess::setGaussBlurSize(int param0, int param1){
		m_gauss_blur_x = param0;
		m_gauss_blur_y = param1;
	}
	void  PreProcess::setHSV(const hsv param){ m_blue_hsv = param; }
	void  PreProcess::setMat(Mat &param){ m_mat = param; m_mat_backup = param; }

	void PreProcess::init(){
		m_type = true;
		m_factor = 1.0f;
		m_gauss_blur_x = 5;
		m_gauss_blur_y = 5;
		m_is_resize = false;
		m_resize_type = 2;
		/*
		m_blue_hsv.m_min_h = 115;
		m_blue_hsv.m_max_h = 125;
		m_blue_hsv.m_min_s = 43;
		m_blue_hsv.m_max_s = 255;
		m_blue_hsv.m_min_v = 46;
		m_blue_hsv.m_max_v = 255;
		*/
		m_blue_hsv.m_min_h = 100;
		m_blue_hsv.m_max_h = 140;
		m_blue_hsv.m_min_s = 89;
		m_blue_hsv.m_max_s = 255;
		m_blue_hsv.m_min_v = 89;
		m_blue_hsv.m_max_v = 255;

		m_yellow_hsv.m_min_h = 115;
		m_yellow_hsv.m_max_h = 125;
		m_yellow_hsv.m_min_s = 43;
		m_yellow_hsv.m_max_s = 255;
		m_yellow_hsv.m_min_v = 100;
		m_yellow_hsv.m_max_v = 250;
	}

	void PreProcess::startColorMatch(Mat &param){
		Mat hsv_img;
		vector<Mat> split_hsv;
		cvtColor(param, hsv_img, COLOR_BGR2HSV);
		//h:split_hsv[0]
		//s:split_hsv[1]
		//v:split_hsv[2]		
		split(hsv_img, split_hsv);
		equalizeHist(split_hsv[2], split_hsv[2]);
		merge(split_hsv, hsv_img);
		uchar* h;
		uchar* s;
		uchar* v;
		int rows = param.rows;
		int cols = param.cols;
		for (int i = 0; i < rows; i++){
			for (int j = 0; j < cols; j++){
				h = (split_hsv[0].data + split_hsv[0].step[0] * i + split_hsv[0].step[1] * j);
				s = (split_hsv[1].data + split_hsv[1].step[0] * i + split_hsv[1].step[1] * j);
				v = (split_hsv[2].data + split_hsv[2].step[0] * i + split_hsv[2].step[1] * j);
				if (((*h) < m_blue_hsv.m_min_h || (*h) > m_blue_hsv.m_max_h) ||
					((*s) < m_blue_hsv.m_min_s || (*s) > m_blue_hsv.m_max_s) ||
					((*v) < m_blue_hsv.m_min_v || (*v) < m_blue_hsv.m_max_v)) {
					*h = 0;
					*s = 0;
					*v = 0;
				}
				else{
					*h = 0;
					*s = 0;
					*v = 255;
				}
			}
		}
		//merge(split_hsv, hsv_img);
		vector<Mat> split_done;
		split(hsv_img, split_done);
		param = split_done[2];
	}

	void PreProcess::startFloodFill(){
		Point seed(m_mat.rows / 2, m_mat.cols / 2);
		Rect ccomp;
		int diff = 20;
		floodFill(m_mat, seed, Scalar(0, 0, 0), &ccomp, Scalar(diff, diff, diff), Scalar(diff, diff, diff), 4);
		//floodFill(m_mat, seed, Scalar(255, 255, 255), &ccomp, Scalar(diff, diff, diff), Scalar(diff, diff, diff), FLOODFILL_FIXED_RANGE | 4);		
	}
}
