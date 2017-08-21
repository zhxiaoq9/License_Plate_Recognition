////////////////////////////////////////////////////////////
//Name:			PlateLocate header
//Date:			2017-05-15
//Author:		ZhangXiaoqiang
//Version:		1.0
//Description:	
////////////////////////////////////////////////////////////

#ifndef __PLATE_LOCATE__
#define __PLATE_LOCATE__

#include "CommonHead.hpp"
#include "HelpFun.hpp"
#include "Plate.hpp"
#include "PreProcess.hpp"

namespace cpr{
	
class PlateLocate{
public:
	PlateLocate();
	explicit PlateLocate(const Mat &param);

	vector<Mat> startPlateLocate(PreProcess PP);
	void showImg() const;
	Mat  getMat() const;
	

	int  getSobelSizeX() const;
	bool getDetectedProcess() const;
	int  getMorpyType() const;
	int  getMorpyX() const;
	int  getMorpyY() const;
	float getPlateMinRatio()const;
	float getPlateMaxRation() const;
	int  getPlateMaxAngle() const;

	void setSobelSizeX(int param);
	void setIsDetect(bool param);
	void setMorpyType(int param);
	void setMorpySize(int param0, int param1);
	void setPlateRatio(float param0, float param1);
	void setPlateWidth(int param0, int param1);
	void setPlateHeight(int param0, int param1);
	void setPlateMaxAngle(int param);
	void setPlateQuality(PlateQuality param);
	void setDefaultPlateParam(PlateQuality param);
	//static const int  DEFAULT_SOBEL_SIZE_X =3 ;
	//static const bool DEFAULT_FIRST_PROCESS_DETECTED = false;
	//static const int  DEFAULT_MORPY_TYPE = 0;
	//static const int  DEFAULT_MORPY_X = 17;
	//static const int  DEFAULT_MORPY_Y = 3;
	//static const float DEFAULT_MIN_RATIO;
	//static const float DEFAULT_MAX_RATIO;
	//static const int  DEFAULT_MIN_WIDTH = 20;
	//static const int  DEFAULT_MAX_WIDTH = 200;
	//static const int  DEFAULT_MIN_HEIGHT = 10;
	//static const int  DEFAULT_MAX_HEIGHT = 90;
	//static const int  DEFAULT_MAX_ANGLE = 30;
	//static const int  DEFAULT_PLATE_NUMBER = 0;
private:
	Mat  m_mat;
	
	PlateQuality m_plate_quality;
	float m_ingore_location;
	//if the plate tilt degree is larger than this
	//value adjust it
	int  m_adjust_degree;
	bool m_first_processe_detected;
	int  m_sobel_size_x;
	//morphology operation
	int  m_morpy_type;
	int  m_morpy_x;
	int  m_morpy_y;
	//plate ratio for width/height
	float m_min_ratio;
	float m_max_ratio;
	int   m_min_width;
	int   m_max_width;
	int   m_min_height;
	int   m_max_height;
	//plate maximum angle in x direction
	int  m_max_angle;

	void init();
	
	
	vector<Mat> verifyPlate(vector<vector<Point> > &param, const Mat &img, bool delate);

};
	
};


#endif
