////////////////////////////////////////////////////////////
//Name:			PreProcess header
//Date:			2017-05-12
//Author:		ZhangXiaoqiang
//Version:		1.0
//Description:	
////////////////////////////////////////////////////////////

#ifndef __PRE_PROCESS__
#define __PRE_PROCESS__

#include "CommonHead.hpp"

namespace cpr{

struct hsv{
	//0-180
	uchar   m_min_h;
	uchar   m_max_h;
	//0-255
	uchar   m_min_s;
	uchar   m_max_s;
	uchar   m_min_v;
	uchar   m_max_v;
	hsv():m_min_h(115), m_max_h(125), m_min_s(100), m_max_s(255), m_min_v(100), m_max_v(150){};
	hsv(const hsv &param){
		m_min_h = param.m_min_h;
		m_max_h = param.m_max_h;
		m_min_s = param.m_min_s;
		m_max_s = param.m_max_s;
		m_min_v = param.m_min_v;
		m_max_v = param.m_max_v;

	}
	hsv& operator=(const hsv &param){
		m_min_h = param.m_min_h;
		m_max_h = param.m_max_h;
		m_min_s = param.m_min_s;
		m_max_s = param.m_max_s;
		m_min_v = param.m_min_v;
		m_max_v = param.m_max_v;
		return *this;
	}
};


class PreProcess{
public:
	PreProcess();
	explicit PreProcess(const Mat &param);
		
	friend class PlateLocate;
	void startPreProcess();
	void showImg() const;

	bool  getType() const;
	float getFactor() const;
	bool  getIsResize() const;
	int   getResizeType() const;
	int   getGaussBlurSizeX() const;
	int   getGaussBlurSizeY() const;
	hsv   getHSV() const;
	Mat   getMat() const;
	Mat   getMatBackup() const;

	void setType(bool param);
	void setResizeFactor(float param);
	void setIsResize(bool param);
	void setResizeType(int param);
	void setGaussBlurSize(int param0, int param1);
	void setHSV(const hsv param);
	void setMat(Mat &param);

	//static const bool  DEFAULT_TYPE = true;
	//static const float DEFAULT_FACOTR;
	//static const bool  DEFAULT_IS_RESIZE = false;
	//static const int   DEFAULT_RESIZE_TYPE = 2;
	//static const int   DEFAULT_GAUSS_BLUR_X = 5;
	//static const int   DEFAULT_GAUSS_BLUR_Y = 5;

private:
	Mat   m_mat;
	Mat   m_mat_backup;
	//true:  the plate is found in first process
	//false: the plate is not found in first process
	bool  m_type;
	//image resize factor
	//m_resize_type:
	//0:INTER_NEAREST
	//1:INTER_LINEAR
	//2:INTER_CUBIC
	//3:INTER_AREA
	//4:INTER_LANCZOS4
	float m_factor;
	bool  m_is_resize;
	int   m_resize_type;
	//should be odd number
	int   m_gauss_blur_x;
	int   m_gauss_blur_y;
	//parameter for color floodFill process
	hsv   m_blue_hsv;
	hsv   m_yellow_hsv;

	void  init();
	void  startFloodFill();
	void startColorMatch(Mat &param);

};

}


#endif
