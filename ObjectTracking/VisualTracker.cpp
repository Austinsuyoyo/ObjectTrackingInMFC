/**
*  @file		VisualTracker.cpp
*  @brief		Implement Object Tracking
*  @author		AustinSu
*  @date		20/7/1017
*  @version		1.0
*/
#include "stdafx.h"
#include "VisualTracker.h"

CVisualTracker::CVisualTracker()
{
}


CVisualTracker::~CVisualTracker()
{
}

void CVisualTracker::SetMethodType(VT_MethodType Type)
{
	if(m_CurrType!= Type)m_bROIchanged = 0;
	m_CurrType = Type;
}

void CVisualTracker::SetVT_Params(VT_MethodType Type, VT_Params Param)
{
	if(Type == TempMatch)m_CurrParams.TM_Param = Param.TM_Param;
	if(Type == MeanShift)m_CurrParams.MS_Param = Param.MS_Param;
	if(Type == CAMShift)m_CurrParams.CS_Param = Param.CS_Param;
}
void CVisualTracker::SetROI(cv::Mat* pROI)
{
	m_bROIchanged = 0;
	m_pROI = pROI;
}
CVisualTracker::VT_MethodType CVisualTracker::GetMethodType() const
{
	return m_CurrType;
}

CVisualTracker::VT_Params CVisualTracker::GetVT_Params() const
{
	return m_CurrParams;
}

cv::Mat * CVisualTracker::GetROI() const
{
	return m_pROI;
}

BOOL CVisualTracker::Tracking(cv::Mat & Frame, cv::Rect & TrackRect)
{
	if (m_pROI != NULL) {
		if (m_CurrType == TempMatch) {
			if (TrackingByTempMatching(Frame, TrackRect))return 1;
			else return 0;
		}
		else if (m_CurrType == MeanShift) {
			if (TrackingByMeanShift(Frame, TrackRect))return 1;
			else return 0;
		}
		else if (m_CurrType == CAMShift) {
			if (TrackingByCamShift(Frame, TrackRect))return 1;
			else return 0;
		}
		return 0;
	}
	else
		return 0;
}


void CVisualTracker::ShowResult(cv::Mat & Frame, cv::Rect & TrackRect)
{
	imshow("MAIN FRAME", Frame);
}

void CVisualTracker::ShowHist()
{
	cv::Mat histimg = cv::Mat::zeros(200, 320, CV_8UC3);
	histimg = cv::Scalar::all(0);
	int binW = histimg.cols / m_hsize;
	cv::Mat buf(1, m_hsize, CV_8UC3);

	for (int i = 0; i < m_hsize; i++)
		buf.at<cv::Vec3b>(i) = cv::Vec3b(cv::saturate_cast<uchar>(i*180. / m_hsize), 255, 255);
	cvtColor(buf, buf, cv::COLOR_HSV2BGR);

	for (int i = 0; i < m_hsize; i++)
	{
		int val = cv::saturate_cast<int>(m_hist.at<float>(i)*histimg.rows / 255);
		cv::rectangle(histimg, cv::Point(i*binW, histimg.rows),
			cv::Point((i + 1)*binW, histimg.rows - val),
			cv::Scalar(buf.at<cv::Vec3b>(i)), -1, 8);
	}
	imshow("histimg", histimg);
}

BOOL CVisualTracker::TrackingByTempMatching(cv::Mat & Frame, cv::Rect & TrackRect)
{
	/** @see http://docs.opencv.org/3.2.0/de/da9/tutorial_template_matching.html
	*/
	cv::Mat result;

	/// Create the result matrix
	int result_cols = Frame.cols - m_pROI->cols + 1;
	int result_rows = Frame.rows - m_pROI->rows + 1;
	result.create(result_rows, result_cols, CV_32FC1);
	
	/// Do the Matching and Normalize
	cv::matchTemplate(Frame, *m_pROI, result, m_CurrParams.TM_Param);
	cv::normalize(result, result, 0, 1, cv::NORM_MINMAX, -1, cv::Mat());

	/// Localizing the best match with minMaxLoc
	double matchVal; double minVal; double maxVal; cv::Point minLoc; cv::Point maxLoc;
	minMaxLoc(result, &minVal, &maxVal, &minLoc, &maxLoc, cv::Mat());

	/// For SQDIFF and SQDIFF_NORMED, the best matches are lower values. For all the other methods, the higher the better
	if (m_CurrParams.TM_Param == TM_SQDIFF || m_CurrParams.TM_Param == TM_SQDIFF_NORMED) {
		matchLoc = minLoc; matchVal = minVal;
	}
	else {
		matchLoc = maxLoc; matchVal = maxVal;
	}

	/// Threshold
	if (matchVal > 0.5)return 1;

	return 0;
}

BOOL CVisualTracker::TrackingByMeanShift(cv::Mat & Frame, cv::Rect & TrackRect)
{
	/** @see https://github.com/opencv/opencv/blob/master/samples/cpp/camshiftdemo.cpp
	*/

	ImgHueExtraction(Frame);
	trackWindow = TrackRect;
	if (!m_bROIchanged)
	{
		// Object has been selected by user, set up CAMShift search properties once
		PrepareForBackProject(TrackRect);

		m_bROIchanged = 1; // Don't set up again, unless user selects new ROI
		
		ShowHist();
	}

	const float* phranges = m_hranges;
	cv::calcBackProject(&m_hue, 1, 0, m_hist, m_backproj, &phranges);
	m_backproj &= m_mask;
	if (cv::meanShift(m_backproj, TrackRect,
		cv::TermCriteria(cv::TermCriteria::EPS | cv::TermCriteria::COUNT, 10, 1)) == 0)
		return 0;


	if (TrackRect.area() <= 1)
	{
		int cols = m_backproj.cols, rows = m_backproj.rows, r = (MIN(cols, rows) + 5) / 6;
		TrackRect = cv::Rect(TrackRect.x - r, TrackRect.y - r,
			TrackRect.x + r, TrackRect.y + r) &
			cv::Rect(0, 0, cols, rows);
		return 0;
	}

	if (0)//backprojMode
		cv::cvtColor(m_backproj, Frame, cv::COLOR_GRAY2BGR);
	return 1;
}

BOOL CVisualTracker::TrackingByCamShift(cv::Mat & Frame, cv::Rect & TrackRect)
{
	/** @see https://github.com/opencv/opencv/blob/master/samples/cpp/camshiftdemo.cpp
	*/
	ImgHueExtraction(Frame);

	if (!m_bROIchanged)
	{
		// Object has been selected by user, set up CAMShift search properties once
		PrepareForBackProject(TrackRect);
		//trackWindow = TrackRect;
		m_bROIchanged = 1; // Don't set up again, unless user selects new ROI
		
		ShowHist();
	}

	// Perform CAMShift
	const float* phranges = m_hranges;
	cv::calcBackProject(&m_hue, 1, 0, m_hist, m_backproj, &phranges);
	m_backproj &= m_mask;

	trackBox = cv::CamShift(m_backproj, TrackRect,
		cv::TermCriteria(cv::TermCriteria::EPS | cv::TermCriteria::COUNT, 10, 1));
	//  area too small   ||     avoid NAN (Not a Number)   ||     avoid NAN (Not a Number)
	if (TrackRect.area() <= 1 || (trackBox.size.width != trackBox.size.width) || (trackBox.size.height != trackBox.size.height))
	{
		int cols = m_backproj.cols, rows = m_backproj.rows, r = (MIN(cols, rows) + 5) / 6;
		TrackRect = cv::Rect(TrackRect.x - r, TrackRect.y - r,
			TrackRect.x + r, TrackRect.y + r) &
			cv::Rect(0, 0, cols, rows);
		return 0;
	}

	if (0)//backprojMode
		cv::cvtColor(m_backproj, Frame, cv::COLOR_GRAY2BGR);

	return 1;

}

void CVisualTracker::ImgHueExtraction(cv::Mat & Frame)
{
	/** @see https://github.com/opencv/opencv/blob/master/samples/cpp/camshiftdemo.cpp 
	*/

	if (m_CurrType == CAMShift) {
		m_vmin = m_CurrParams.CS_Param.vmin;
		m_vmax = m_CurrParams.CS_Param.vmax;
		m_smin = m_CurrParams.CS_Param.smin;
	}
	else if (m_CurrType == MeanShift)
	{
		m_vmin = m_CurrParams.MS_Param.vmin;
		m_vmax = m_CurrParams.MS_Param.vmax;
		m_smin = 30;
	}
	cv::cvtColor(Frame, m_hsv, cv::COLOR_BGR2HSV);
	cv::inRange(m_hsv, cv::Scalar(0, m_smin, MIN(m_vmin,m_vmax)),
				cv::Scalar(180, 256, MAX(m_vmin, m_vmax)),m_mask);
	m_hue.create(m_hsv.size(), m_hsv.depth());
	int ch[] = { 0,0 };
	cv::mixChannels(&m_hsv, 1, &m_hue, 1, ch, 1);
}

void CVisualTracker::PrepareForBackProject(cv::Rect & selection)
{	
	/** @see https://github.com/opencv/opencv/blob/master/samples/cpp/camshiftdemo.cpp
	*/
	m_hsize = 16;
	const float* phranges = m_hranges;
	cv::Mat roi(m_hue, selection), maskroi(m_mask, selection);
	cv::calcHist(&roi, 1, 0, maskroi, m_hist, 1, &m_hsize, &phranges);
	cv::normalize(m_hist, m_hist, 0, 255, cv::NORM_MINMAX);

}

void CVisualTracker::TrackerInit(std::string mode, cv::Mat & Frame, cv::Rect2d & roiRect2d)
{
	Tracker = cv::Tracker::create(mode);
	Tracker->init(Frame, roiRect2d);
}

void CVisualTracker::TrackerUpdate(cv::Mat & Frame, cv::Rect2d & roiRect2d)
{
	Tracker->update(Frame, roiRect2d);
}

