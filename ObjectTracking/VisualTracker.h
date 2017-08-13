/**
*  @file		VisualTracker.cpp
*  @brief		Implement Object Tracking
*  @author		AustinSu
*  @date		20/7/1017
*  @version		1.0
*/
#pragma once
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/tracking.hpp>
class CVisualTracker
{
public:
	enum VT_MethodType { TempMatch = 1, MeanShift, CAMShift };
	enum TempMatchParam {
		TM_SQDIFF = CV_TM_SQDIFF,
		TM_SQDIFF_NORMED, TM_CCORR, TM_CCORR_NORMED, TM_CCOEFF,
		TM_CCOEFF_NORMED
	};
	typedef struct
	{
		int vmin;
		int vmax;
	} MeanShiftParam;
	typedef struct
	{
		int vmin;
		int vmax;
		int smin;
	} CamShiftParam;
	typedef struct
	{
		TempMatchParam TM_Param;
		MeanShiftParam MS_Param;
		CamShiftParam CS_Param;

	} VT_Params;
	CVisualTracker();
	~CVisualTracker();

	VT_MethodType				GetMethodType() const;
	VT_Params					GetVT_Params() const;
	cv::Mat*					GetROI() const;
	void						SetMethodType(VT_MethodType Type);
	void						SetVT_Params(VT_MethodType Type, VT_Params Param);
	void						SetROI(cv::Mat* pROI);
	BOOL						Tracking(cv::Mat &Frame, cv::Rect &TrackRect);
	void						ShowResult(cv::Mat &Frame, cv::Rect &TrackRect);
	void						ShowHist();// Austin add it
	cv::Point					matchLoc;// Austin add it
	cv::RotatedRect				trackBox;// Austin add it
	cv::Rect					trackWindow;// Austin add it
	void						TrackerInit(std::string mode, cv::Mat &Frame, cv::Rect2d &roiRect2d);// Austin add it
	void						TrackerUpdate(cv::Mat & Frame, cv::Rect2d & roiRect2d);
private:
	VT_MethodType				m_CurrType;
	VT_Params					m_CurrParams;
	cv::Mat*					m_pROI;

	/**
	 *   @brief  Tracking Object By Template Matching
	 *   @param  Frame is source image
	 *   @param  TrackRect is ROI rectangle
	 *   @return true on success, false otherwise
	 */
	BOOL						TrackingByTempMatching(cv::Mat &Frame, cv::Rect &TrackRect);
	/** 
	 *   @brief  Tracking Object By Mean Shift Method
	 *   @param  Frame is source image
	 *   @param  TrackRect is ROI rectangle
	 *   @return true on success, false otherwise
	 */
	BOOL						TrackingByMeanShift(cv::Mat &Frame, cv::Rect &TrackRect);
	/**
	*   @brief  Tracking Object By Continuously Adaptive Mean(CAM) Shift Method
	*   @param  Frame is source image
	*   @param  TrackRect is ROI rectangle
	*   @return true on success, false otherwise
	*/
	BOOL						TrackingByCamShift(cv::Mat &Frame, cv::Rect &TrackRect);
	void						ImgHueExtraction(cv::Mat &Frame);
	void						PrepareForBackProject(cv::Rect &selection);


	BOOL						m_bROIchanged;
	cv::Mat 					m_hsv;
	cv::Mat						m_hue;
	cv::Mat						m_mask;
	cv::MatND					m_hist;
	cv::Mat						m_backproj;
	int							m_vmin;
	int							m_vmax;
	int							m_smin;
	int							m_hsize;
	float						m_hranges[2] = { 0,180 };
	cv::Ptr<cv::Tracker>		Tracker;
};

