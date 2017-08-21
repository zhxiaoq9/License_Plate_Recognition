#include "PlateLocate.hpp"


namespace cpr{

	//const float PlateLocate::DEFAULT_MIN_RATIO = 2.5f;
	//const float PlateLocate::DEFAULT_MAX_RATIO = 5.0f;

	PlateLocate::PlateLocate(){ 
		init(); 		
	}
	PlateLocate::PlateLocate(const Mat &param):m_mat(param){ 
		init();		
	}


	vector<Mat> PlateLocate::startPlateLocate(PreProcess PP){
		vector<vector<Point> >contours;
		vector<Mat> plates;
		Mat element;
		if (m_first_processe_detected){
			//threshold:
			//0: binary
			//1: binary inverted
			//2: threshold truncated
			//3: threshold to zero
			//4: threshold to zero inverted
			threshold(m_mat, m_mat, 0, 255, THRESH_BINARY | CV_THRESH_OTSU);
			//Sobel(m_mat, m_mat, m_mat.depth(), 1, 0, m_sobel_size_x);
			//morphology type
			//0: MORPH_RECT;
			//1: MORPH_CROSS;
			//2: MORPH_ELLIPSE;				
			element = getStructuringElement(m_morpy_type, Size(m_morpy_x, m_morpy_y), Point(-1, -1));
			morphologyEx(m_mat, m_mat, MORPH_CLOSE, element);
			//medianBlur(m_mat, m_mat, 3);
			findContours(m_mat.clone(), contours, RETR_EXTERNAL, CHAIN_APPROX_NONE);
			plates = verifyPlate(contours, PP.m_mat_backup, true);
		}
		
		//if can not find plate in first process then begin the second process
		if (plates.empty()){
			m_first_processe_detected = false;
			//if this method can not detect the plate, then begin the second method of Preprocess
			PP.setType(false);
			PP.startPreProcess();
			m_mat = PP.getMat();
			threshold(m_mat, m_mat, 0, 255, THRESH_BINARY | CV_THRESH_OTSU);
			Sobel(m_mat, m_mat, m_mat.depth(), 1, 0, m_sobel_size_x);
			element = getStructuringElement(m_morpy_type, Size(m_morpy_x, m_morpy_y), Point(-1, -1));
			morphologyEx(m_mat, m_mat, MORPH_CLOSE, element);
			medianBlur(m_mat, m_mat, 3);
			findContours(m_mat.clone(), contours, RETR_EXTERNAL, CHAIN_APPROX_NONE);
			plates = verifyPlate(contours, PP.m_mat_backup, true);
		}
		
		if (!plates.empty()){
			drawContours(PP.m_mat_backup, contours, -1, Scalar(0, 0, 255), 1, 8);
		}
		
#ifdef __DEBUG__
		cout << contours.size() << " plates found.\n";
		for (int i = 0; i < contours.size(); i++){
			RotatedRect rr = minAreaRect(contours[i]);
			Size rect_size = rr.size;
			if (rect_size.width < rect_size.height){
				rr.angle = rr.angle + 90;
				int tmp = rect_size.width;
				rect_size.width = rect_size.height;
				rect_size.height = tmp;
			}
			cout << "\tplate " << i << ":\n";
			cout << "\t\twidth = " << rect_size.width << "\n";
			cout << "\t\theight = " << rect_size.height << "\n";
			cout << "\t\tangle = " << rr.angle << "\n";
		}
		namedWindow("imageAfetrPlateLocate", CV_WINDOW_AUTOSIZE);
		imshow("imageAfetrPlateLocate", m_mat);
#endif
		m_mat = PP.m_mat_backup.clone();
		return plates;
	}

	vector<Mat> PlateLocate::verifyPlate(vector<vector<Point> > &param, const Mat &img, bool delate){
		vector<Mat> valid_rect;
		vector<vector<Point> >::iterator it = param.begin();
		RotatedRect rr;
		Mat roi;
		float ratio;
		float angle;
		while (it != param.end()){
			rr = minAreaRect(*it);
			Size rect_size = rr.size;
			Point2f center = rr.center;
			angle = rr.angle;
			//在OpenCV中,minAreaRect返回的角度为水平轴与矩形宽度width
			//的夹角.这个值在(-90,0]之间.而且矩形width可能比height小,
			//也可能比height大.在进行车牌检测时需要返回的是车牌长边与
			//水平轴的夹角.所以当width>height时,返回的angle正是需要的
			//angle,但是当width<height时此时返回的是短边与横轴的夹角.
			//此时真正的长边与横轴间夹角为-(90+angle).可参考下文中链接.
			//http://blog.csdn.net/a553654745/article/details/45743063
			if (rect_size.width < rect_size.height){
				angle = angle + 90;
				int tmp = rect_size.width;
				rect_size.width = rect_size.height;
				rect_size.height = tmp;
			}
			//plate center location verify
			int ignore_width = img.cols*m_ingore_location;
			if (center.x < ignore_width || center.x > img.cols - ignore_width ||
				center.y < ignore_width || center.y > img.rows - ignore_width){
				if (delate)
					it = param.erase(it);
				else
					++it;
				continue;
			}

			//absolute width and length verify
			if (rect_size.width < m_min_width || rect_size.width > m_max_width ||
				rect_size.height < m_min_height && rect_size.height > m_max_height){
				if (delate)
					it = param.erase(it);
				else
					++it;
				continue;
			}
			
			//ratio verify	
			if (rect_size.height == 0){
				if (delate)
					it = param.erase(it);
				else
					++it;
				continue;
			}
			else{
				ratio = rect_size.width / rect_size.height;
			}				
			if (ratio < m_min_ratio || ratio > m_max_ratio){
				if (delate)
					it = param.erase(it);
				else
					++it;
				continue;
			}
			//angle verify
			if (abs(angle) > m_max_angle){
				if (delate)
					it = param.erase(it);
				else
					++it;
				continue;
			}

			//如果倾斜角度超过了设定的值那么就进行倾斜校正，否则直接从
			//原图像返回图片		
			if (abs(angle) > m_adjust_degree){	
				//得到最小外接矩形
				Rect src_img = rr.boundingRect();
				//得到倾斜校正变换矩阵
				Mat rotate = getRotationMatrix2D(rr.center, angle, 1);				
				//将原图片进行旋转得到旋转后的图片
				Mat img_rotated;
				warpAffine(img, img_rotated, rotate, img.size(), INTER_CUBIC);
				//从旋转后的图片中获取感兴趣区域
				getRectSubPix(img_rotated, rect_size, rr.center, roi);						
			}
			else{
				getRectSubPix(img, rect_size, rr.center, roi);
			}
			//统一图片大小为136*36
			resize(roi, roi, Size(136, 36), 0.0, 0.0, INTER_CUBIC);
			valid_rect.push_back(roi);
			++it;
		}
		return valid_rect;
	}


	void PlateLocate::showImg() const {
		namedWindow("PlateLocate", CV_WINDOW_AUTOSIZE);
		imshow("PlateLocate", m_mat);
	}

	float PlateLocate::getPlateMinRatio()const{ return m_min_ratio; }
	float PlateLocate::getPlateMaxRation() const{ return m_max_ratio; }
	Mat  PlateLocate::getMat() const { return m_mat; }
	int  PlateLocate::getSobelSizeX() const{ return m_sobel_size_x; }
	int  PlateLocate::getMorpyType() const{ return m_morpy_type; }
	int  PlateLocate::getMorpyX() const{ return m_morpy_x; }
	int  PlateLocate::getMorpyY() const{ return m_morpy_y; }
	int  PlateLocate::getPlateMaxAngle() const{ return m_max_angle; }
	bool PlateLocate::getDetectedProcess() const{ return m_first_processe_detected; }
	void PlateLocate::setSobelSizeX(int param){ m_sobel_size_x = param; }
	void PlateLocate::setIsDetect(bool param){ m_first_processe_detected = param; }
	void PlateLocate::setPlateRatio(float param0, float param1){ 
		m_min_ratio = param0;
		m_max_ratio = param1;
	}
	void PlateLocate::setPlateWidth(int param0, int param1){
		m_min_width = param0;
		m_max_width = param1;
	}
	void PlateLocate::setPlateHeight(int param0, int param1){
		m_min_height = param0;
		m_max_height = param1;
	}
	void PlateLocate::setPlateMaxAngle(int param){ m_max_angle = param; }
	//param: morphology type
	//0: MORPH_RECT;
	//1: MORPH_CROSS;
	//2: MORPH_ELLIPSE;
	void PlateLocate::setMorpyType(int param){
		assert(param >= 0 && param <= 2);
		m_morpy_type = param;
	}
	void PlateLocate::setMorpySize(int param0, int param1){
		m_morpy_x = param0;
		m_morpy_y = param1;
	}
	void PlateLocate::setPlateQuality(PlateQuality param){ m_plate_quality = param; }
	
	void PlateLocate::setDefaultPlateParam(PlateQuality param){
		if (param == PlateQuality::EASY){
			m_morpy_type = 0;
			m_morpy_x = 17;
			m_morpy_y = 5;
			m_min_ratio = 2.5f;
			m_max_ratio = 4.0f;
			m_min_width = 40;
			m_max_width = 200;
			m_min_height = 20;
			m_max_height = 70;
			m_max_angle = 30;
			m_ingore_location = 1.0 / 8.0;
		}
		else if (PlateQuality::MEDIUM){
			m_morpy_type = 0;
			m_morpy_x = 13;
			m_morpy_y = 5;
			m_min_ratio = 2.5f;
			m_max_ratio = 5.0f;
			m_min_width = 30;
			m_max_width = 200;
			m_min_height = 10;
			m_max_height = 70;
			m_max_angle = 30;
			m_ingore_location = 1.0 / 10.0;
		}
		else if (PlateQuality::HARD){
			m_morpy_type = 0;
			m_morpy_x = 9;
			m_morpy_y = 5;
			m_min_ratio = 1.0f;
			m_max_ratio = 4.0f;
			m_min_width = 30;
			m_max_width = 100;
			m_min_height = 15;
			m_max_height = 50;
			m_max_angle = 30;
			m_ingore_location = 1.0 / 16.0;
		}

	}
	void PlateLocate::init(){
		m_first_processe_detected = false;
		m_sobel_size_x = 3;	
		m_adjust_degree = 60;
		m_plate_quality = PlateQuality::EASY;
		setDefaultPlateParam(m_plate_quality);
	}
}

