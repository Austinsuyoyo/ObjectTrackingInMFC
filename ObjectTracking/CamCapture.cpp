#include "stdafx.h"
#include "CamCapture.h"


//test
extern CVisualTracker		ObjectTracker;
cv::Rect CCamCapture::m_Recttest ;

BOOL CCamCapture::m_bTargetObj	= false;
IplImage* CCamCapture::m_pImage = NULL;
IplImage* CCamCapture::m_pROI	= NULL;
CvRect	CCamCapture::m_TargetRect = NULL;
CvPoint CCamCapture::m_OriginPoint = NULL;
CvPoint CCamCapture::m_EndPoint = NULL;
//**************************************
//        CCamCapture Class
//			OPNECV 1.0
//**************************************

CCamCapture::CCamCapture()
{
	m_pCapture = NULL;
	m_bCamInited = false;
	m_heventThreadDone = NULL;
}

CCamCapture::~CCamCapture()
{
	m_bCamInited= false;
	m_bTargetObj= false;
	onGrabLoop_cvClose();
}

BOOL CCamCapture::CreateCapture()
{
	if (m_bCamInited) return true;
	else {
		if (!GetProcessIdOfThread(m_heventThreadDone)) {
			// 創建並開啟線程
			DWORD dwThreadID;
			m_heventThreadDone = CreateThread(NULL, 0, threadGrabImage, (LPVOID*) this, 0, &dwThreadID);
			return (GetProcessIdOfThread(m_heventThreadDone)) ? true : false;
		}
		else{
			// 關閉線程
			CloseHandle(m_heventThreadDone);
			return false;
		}
	}

}

void CCamCapture::DestroyCapture()
{
	if(m_bCamInited)
	{
		m_State = STOP;
		WaitForSingleObject(m_heventThreadDone, Default_WaitSecond);
	}
	else return;
}

void CCamCapture::SetCaptureCallback( pfnCaptureCallback pfnCaptureCB )
{
	cvSetMouseCallback("MAIN FRAME", onMouseCB, pfnCaptureCB);
}

//Austin add it 
void CCamCapture::SetCaptureSize(double width, double height) 
{
	cvSetCaptureProperty(m_pCapture,CV_CAP_PROP_FRAME_WIDTH, width);
	cvSetCaptureProperty(m_pCapture,CV_CAP_PROP_FRAME_HEIGHT, height);
}

DWORD WINAPI CCamCapture::threadGrabImage( LPVOID pparam )
{
	CCamCapture* pCapture = ((CCamCapture*)pparam);
	pCapture->doGrabLoop();
	return 0;
}
void CCamCapture::doGrabLoop()
{
	// 計算 FPS 開始--------------------
	double time_start = (double)cvGetTickCount();


	onGrabLoop_cvInit();
	while(m_bCamInited)
	{

		switch(m_State)
		{
		case START:
			// 抓取圖像
			m_pImage = cvQueryFrame(m_pCapture); 
			// 刷白圖像
			if(m_pImage == NULL ) cvSet(m_pImage, cvScalarAll(255));
			// 滑鼠響應函式
			cvSetMouseCallback("MAIN FRAME", onMouseCB, NULL);
			// 複製圖像
			IplImage* m_pImageClone; 
			m_pImageClone = cvCreateImage(cvGetSize(m_pImage), m_pImage->depth, m_pImage->nChannels);
			cvCopy(m_pImage, m_pImageClone, NULL);
			// 劃出選取範圍
			onGrabLoop_DrawMouseSelect(m_pImageClone);

			m_ImageMat = cv::cvarrToMat(m_pImage);
			if (m_pROI != NULL) {
				m_ROImat = cv::cvarrToMat(m_pROI);
				ObjectTracker.SetROI(&m_ROImat);
				//cv::imshow("test ROI", m_ROImat);
			}
			if (ObjectTracker.GetROI() != NULL) {
				ObjectTracker.SetMethodType(ObjectTracker.TempMatch);
				if (ObjectTracker.Tracking(m_ImageMat, m_Recttest)) {
					cvRectangle(m_pImageClone, ObjectTracker.matchLoc,
						cv::Point(ObjectTracker.matchLoc.x + m_TargetRect.width, ObjectTracker.matchLoc.y + m_TargetRect.height), cv::Scalar(0, 0, 255), 2, 8, 0);
				}
			}
			


			// 計算 FPS 區塊--------------------
			frameCounter++;
			timeNow = std::time(0) - timeBegin;
			if (timeNow - tick >= 1)
			{
				tick++;
				fps = frameCounter;
				frameCounter = 0;
			}
			// 計算 FPS 結束--------------------

			// 顯示FPS
			onGrabLoop_ShowFPS(m_pImageClone, fps);
			// 顯示圖像 171108避免已經進入STOP狀態卻顯示圖像
			if(m_State==START)cvShowImage("MAIN FRAME", m_pImageClone);
			// 釋放圖像
			cvReleaseImage(&m_pImageClone);
			// 顯示ROI區域
			onGrabLoop_DrawROI(m_pImage);
			cvWaitKey(Default_WaitSecond);
			break;

		case PAUSE:
			cvSetMouseCallback("MAIN FRAME", onMouseCB, NULL); // mouse event (opneCV)
			if(m_bTargetObj)
			{
				// 複製圖像
				IplImage* m_pImageClone; // clone the original image.
				m_pImageClone = cvCreateImage(cvGetSize(m_pImage), m_pImage->depth, m_pImage->nChannels);
				cvCopy(m_pImage, m_pImageClone, NULL);
				// 劃出選取範圍
				onGrabLoop_DrawMouseSelect(m_pImageClone);
				// 計算 FPS 結束--------------------
				//t = ((double)cvGetTickCount() - t) / (double)cvGetTickFrequency();
				// 顯示FPS
				//onGrabLoop_ShowFPS(m_pImageClone,t);
				// 顯示圖像
				cvShowImage("MAIN FRAME", m_pImageClone);
				// 釋放圖像
				cvReleaseImage(&m_pImageClone);
			}
			else
			{
				if((m_pCapture != NULL) && (m_pImage != NULL)) cvShowImage("MAIN FRAME", m_pImage); // show original image to cover rectangle.
			}
			cvWaitKey(Default_WaitSecond);
			break;

		case STOP:
			m_bCamInited = false;
			onGrabLoop_cvClose();
			break;

		default:
			break;
		}
		while (time_to_wait > ((double)cvGetTickCount() - time_start) / cvGetTickFrequency() * 1000)
		{
			Sleep(1);
		}
	}
	onGrabLoop_cvClose(); 
	CloseHandle(m_heventThreadDone); // close thread.
}
void CCamCapture::onGrabLoop_cvInit()
{
	m_pCapture	= cvCreateCameraCapture(0);
	
	if(m_pCapture != NULL)
	{
		m_bCamInited = true;
		//m_State = PAUSE;
	}
	else
	{
		m_bCamInited = false;
	}
}



void CCamCapture::onMouseCB( int event, int x, int y, int flags, void* param )
{
	if(event == CV_EVENT_LBUTTONDOWN && !m_bTargetObj)
	{
		m_bTargetObj = true;
		m_OriginPoint = cvPoint(x, y);
		m_EndPoint = m_OriginPoint;
		m_TargetRect = cvRect(x, y, 0, 0);
		if (cvGetWindowHandle("roiTarget") != NULL)	cvDestroyWindow("roiTarget");
	}

	if((event == CV_EVENT_MOUSEMOVE) && (m_bTargetObj))
	{


		//170206 添加避免游標超出邊界
		int border_x, border_y;
		border_x = (x > m_pImage->width) ? m_pImage->width : (x < 0) ? 0 : x;
		border_y = (y > m_pImage->height) ? m_pImage->height : (y < 0) ? 0 : y;

		// EndPoint
		m_EndPoint = cvPoint(border_x, border_y);

		// 171108 width取正
		m_TargetRect.width  = abs(m_EndPoint.x - m_OriginPoint.x);
		m_TargetRect.height = abs(m_EndPoint.y - m_OriginPoint.y);

		// 171108 解決RECT x ,y, width, height取得正確值
		// ↖
		if ( (m_EndPoint.x < m_OriginPoint.x) && (m_EndPoint.y < m_OriginPoint.y))
		{
			m_TargetRect.x = m_EndPoint.x;
			m_TargetRect.y = m_EndPoint.y;
		}
		// ↗
		if ((m_EndPoint.x > m_OriginPoint.x) && (m_EndPoint.y < m_OriginPoint.y))
		{
			m_TargetRect.x = m_OriginPoint.x;
			m_TargetRect.y = m_OriginPoint.y -m_TargetRect.height;
		}
		// ↙
		if ((m_EndPoint.x < m_OriginPoint.x) && (m_EndPoint.y > m_OriginPoint.y))
		{
			m_TargetRect.x = m_EndPoint.x;
			m_TargetRect.y = m_EndPoint.y-m_TargetRect.height;
		}

	}

	if(event == CV_EVENT_LBUTTONUP && (m_bTargetObj))
	{
		m_bTargetObj = false;

		// 擷取圖像 170719避免點到一下
		if (m_TargetRect.width != 0 || m_TargetRect.width != 0) {
			m_Recttest.x = m_TargetRect.x;
			m_Recttest.y = m_TargetRect.y;
			m_Recttest.width = m_TargetRect.width;
			m_Recttest.height = m_TargetRect.height;
		}
	}
}

void CCamCapture::onGrabLoop_DrawROI( IplImage* frame )
{

	// 圈選狀態
	if (m_bTargetObj) {
		if (m_TargetRect.height > 0 && m_TargetRect.width > 0)
		{
			if (m_pROI != NULL)cvReleaseImage(&m_pROI);
			cvSetImageROI(frame, m_TargetRect);
			m_pROI = cvCreateImage(cvGetSize(frame), frame->depth, frame->nChannels);
			cvCopy(frame, m_pROI, NULL);
			cvShowImage("roiTarget", m_pROI);
			cvResetImageROI(frame);
		}
	}
	else {
		if(m_pROI != NULL)cvShowImage("roiTarget", m_pROI);
	}

}
void CCamCapture::onGrabLoop_cvClose()
{
	if (m_pCapture != NULL) cvReleaseCapture(&m_pCapture); // cvReleaseCapture would release capture and image.
														   //if(m_pImage->imageData != NULL) cvReleaseImage(&m_pImage);
	if (m_pROI != NULL) cvReleaseImage(&m_pROI);
	//if(cvGetWindowHandle("MAIN FRAME") != NULL)	cvDestroyWindow("MAIN FRAME");
	if (cvGetWindowHandle("roiTarget") != NULL)	cvDestroyWindow("roiTarget");
}
void CCamCapture::onGrabLoop_ShowFPS(IplImage * frame,double fps)
{
	if (m_bFPS_State) {
		sprintf(buf, "%5.3f", fps);
		cvInitFont(&Font, CV_FONT_HERSHEY_SIMPLEX, 0.5f, 0.5f, 1, 2);
		cvPutText(frame, buf, cvPoint(5, 20), &Font, cvScalar(0, 0, 255));
	}
}
void CCamCapture::onGrabLoop_DrawMouseSelect(IplImage * frame)
{
	if (m_bTargetObj) {
		cvRectangle(frame, m_OriginPoint, m_EndPoint, cvScalar(0xff, 0, 0), 3, 8, 0);
		cvCircle(frame, m_OriginPoint, 10, cvScalar(0, 0xff, 0, 0), CV_FILLED, CV_AA, 0);
		cvCircle(frame, m_EndPoint, 10, cvScalar(0, 0xff, 0, 0), CV_FILLED, CV_AA, 0);
		if (m_EndPoint.x < m_OriginPoint.x)
			cvCircle(frame, cvPoint(m_EndPoint.x, m_OriginPoint.y), 10, cvScalar(0, 0xff, 0, 0), CV_FILLED, CV_AA, 0);
		else
			cvCircle(frame, cvPoint(m_OriginPoint.x + m_TargetRect.width, m_OriginPoint.y), 10, cvScalar(0, 0xff, 0, 0), CV_FILLED, CV_AA, 0);
		if (m_EndPoint.y < m_OriginPoint.y)
			cvCircle(frame, cvPoint(m_OriginPoint.x, m_EndPoint.y), 10, cvScalar(0, 0xff, 0, 0), CV_FILLED, CV_AA, 0);
		else
			cvCircle(frame, cvPoint(m_OriginPoint.x, m_TargetRect.height + m_OriginPoint.y), 10, cvScalar(0, 0xff, 0, 0), CV_FILLED, CV_AA, 0);
	}
}
//**************************************
//        CCamCapture2 Class
//			OPNECV 2.0
//**************************************
cv::Mat		CCamCapture2::m_Image;
cv::Mat		CCamCapture2::m_ROI;
cv::Rect	CCamCapture2::m_TargetRect;
cv::Rect2d	CCamCapture2::m_TargetRect2d;
cv::Point	CCamCapture2::m_OriginPoint;
cv::Point	CCamCapture2::m_EndPoint;
BOOL		CCamCapture2::m_bTrackerInit;
CCamCapture2::CCamCapture2()
{
	m_bCamInited = false;
	m_bTrackerInit = false;
	m_heventThreadDone = NULL;
}

CCamCapture2::~CCamCapture2()
{
	m_bCamInited = false;
	m_bTargetObj = false;
	onGrabLoop_cvClose();
}

void CCamCapture2::SetCaptureCallback(pfnCaptureCallback2 pfnCaptureCB)
{
	cv::setMouseCallback("MAIN FRAME", onMouseCB2, NULL);
}

void CCamCapture2::SetCaptureSize(double width, double height)
{
	m_cap.set(CV_CAP_PROP_FRAME_WIDTH, width);
	m_cap.set(CV_CAP_PROP_FRAME_HEIGHT, height);
}

void CCamCapture2::doGrabLoop()
{	
	onGrabLoop_cvInit();
	cv::Mat m_ImageClone;
	while (m_bCamInited)
	{
		// 計算 FPS 開始--------------------
		double t = (double)cv::getTickCount();

		switch (m_State)
		{
		case START:
			// 抓取圖像
			m_cap >> m_Image;
			// 刷白圖像
			if (m_Image.empty()) {
				cv::Mat m_Image(m_Image.size(), CV_8UC3, cv::Scalar(255, 255, 255));
				break;
			}
			// 滑鼠響應函式
			cv::setMouseCallback("MAIN FRAME", onMouseCB2, NULL);
			// 複製圖像
			m_Image.copyTo(m_ImageClone);
			// 劃出選取範圍
			onGrabLoop_DrawMouseSelect(m_ImageClone);
			m_bTargetObj = m_bTargetObj;
			if (!m_ROI.empty() && !m_bTargetObj && !m_TargetRect.empty() && (ObjectTracker.GetMethodType() > 3)) //for MIL,BOOSTING,MEDIANFLOW,TLD,KCF
			{
				ObjectTracker.TrackerByOpencv(m_Image, m_TargetRect2d);
				rectangle(m_ImageClone, m_TargetRect2d, cv::Scalar(0, 0, 255), 2, 1);;
			}

			if (!m_Image.empty() && !m_ROI.empty() && !m_TargetRect.empty() && !m_bTargetObj && (ObjectTracker.GetMethodType() <= 3)) //for TemplateMatch,CAMShift,MeanShift
			{
				if(!m_TargetRect.empty())
				if (ObjectTracker.Tracking(m_Image, m_TargetRect)) {
					if(ObjectTracker.GetMethodType()==1)
						cv::rectangle(m_ImageClone,
						ObjectTracker.matchLoc,
						cv::Point(ObjectTracker.matchLoc.x + m_ROI.cols, ObjectTracker.matchLoc.y + m_ROI.rows),
						cv::Scalar(0, 0, 255), 2, 8, 0
						);
					if (ObjectTracker.GetMethodType() == 2)
						cv::rectangle(m_ImageClone, 
							cv::Point(ObjectTracker.trackWindow.x, ObjectTracker.trackWindow.y),
							cv::Point(ObjectTracker.trackWindow.x + ObjectTracker.trackWindow.width, ObjectTracker.trackWindow.y + ObjectTracker.trackWindow.height),
							cv::Scalar(0, 0, 255), 3, CV_AA
						);
					if (ObjectTracker.GetMethodType() == 3)
						cv::ellipse(m_ImageClone, ObjectTracker.trackBox, cv::Scalar(0, 0, 255), 3, cv::LINE_AA);

				}

			}

			// 計算 FPS 結束--------------------
			t = ((double)cv::getTickCount() - t) / cv::getTickFrequency();
			// 顯示FPS
			onGrabLoop_ShowFPS(m_ImageClone, 1/t);
			// 顯示圖像 171108避免已經進入STOP狀態卻顯示圖像
			if (m_State == START)cv::imshow("MAIN FRAME", m_ImageClone);
			// 顯示ROI區域
			onGrabLoop_DrawROI(m_Image);

			cvWaitKey(Default_WaitSecond);
			break;

		case PAUSE:
			cv::setMouseCallback("MAIN FRAME", onMouseCB2, NULL);
			if (m_bTargetObj)
			{
				// 複製圖像
				m_Image.copyTo(m_ImageClone);
				// 劃出選取範圍
				onGrabLoop_DrawMouseSelect(m_ImageClone);
				// 計算 FPS 結束--------------------
				t = ((double)cvGetTickCount() - t) / (double)cvGetTickFrequency();
				// 顯示FPS
				onGrabLoop_ShowFPS(m_ImageClone, 1/t);
				// 顯示圖像
				cv::imshow("MAIN FRAME", m_ImageClone);
			}
			else
				if ((m_cap.isOpened()) && (!m_Image.empty()))cv::imshow("MAIN FRAME", m_Image); 
			
			cvWaitKey(Default_WaitSecond);
			break;

		case STOP:
			m_bCamInited = false;
			onGrabLoop_cvClose();
			break;

		default:
			break;
		}
	}
	onGrabLoop_cvClose();
	CloseHandle(m_heventThreadDone); // close thread.
}

void CCamCapture2::onGrabLoop_cvInit()
{
	m_cap.open(0);

	if (m_cap.isOpened())
		m_bCamInited = true;
	else
		m_bCamInited = false;
}

void CCamCapture2::onGrabLoop_cvClose()
{
	if (m_cap.isOpened()) m_cap.release();													   
}

void CCamCapture2::onMouseCB2(int event, int x, int y, int flags, void* param)
{
	if(event == CV_EVENT_LBUTTONDOWN && !m_bTargetObj)
	{
		m_bTargetObj = true;
		m_OriginPoint = cv::Point(x, y);
		m_EndPoint = m_OriginPoint;
		m_TargetRect = cv::Rect(x, y, 0, 0);
		cv::destroyWindow("roiTarget");
	}

	if((event == CV_EVENT_MOUSEMOVE) && (m_bTargetObj))
	{
		m_bTrackerInit = false;
		//170206 添加避免游標超出邊界
		int border_x, border_y;
		border_x = (x > m_Image.size().width) ? m_Image.size().width : (x < 0) ? 0 : x;
		border_y = (y > m_Image.size().height) ? m_Image.size().height : (y < 0) ? 0 : y;

		// EndPoint
		m_EndPoint = cvPoint(border_x, border_y);

		// 171108 width取正
		m_TargetRect.width  = abs(m_EndPoint.x - m_OriginPoint.x);
		m_TargetRect.height = abs(m_EndPoint.y - m_OriginPoint.y);

		// 171108 解決RECT x ,y, width, height取得正確值
		// ↖
		if ( (m_EndPoint.x < m_OriginPoint.x) && (m_EndPoint.y < m_OriginPoint.y))
		{
			m_TargetRect.x = m_EndPoint.x;
			m_TargetRect.y = m_EndPoint.y;
		}
		// ↗
		if ((m_EndPoint.x > m_OriginPoint.x) && (m_EndPoint.y < m_OriginPoint.y))
		{
			m_TargetRect.x = m_OriginPoint.x;
			m_TargetRect.y = m_OriginPoint.y -m_TargetRect.height;
		}
		// ↙
		if ((m_EndPoint.x < m_OriginPoint.x) && (m_EndPoint.y > m_OriginPoint.y))
		{
			m_TargetRect.x = m_EndPoint.x;
			m_TargetRect.y = m_EndPoint.y-m_TargetRect.height;
		}

	}

	if(event == CV_EVENT_LBUTTONUP && (m_bTargetObj))
	{
		m_bTargetObj = false;
		m_bTrackerInit = false;
	}

}

void CCamCapture2::onGrabLoop_DrawROI(cv::Mat & frame)
{
	// 圈選狀態
	if (m_bTargetObj) {
		if (m_TargetRect.height > 0 && m_TargetRect.width > 0)
		{
			m_TargetRect2d = m_TargetRect;
			cv::Mat temp(m_Image, m_TargetRect);
			m_ROI = temp.clone();
			cv::imshow("roiTarget", m_ROI);
			ObjectTracker.SetROI(&m_ROI);
		}
	}
	else {
		if (m_ROI.data != NULL) cv::imshow("roiTarget", m_ROI);
	}
}

void CCamCapture2::onGrabLoop_ShowFPS(cv::Mat & frame, double t)
{
	if (m_bFPS_State) {
		cv::putText(frame, cv::format("FPS = %3.3f", t), cvPoint(5, 20), cv::FONT_HERSHEY_SIMPLEX, 0.8, cv::Scalar(0, 0, 255), 2);
	}
}

void CCamCapture2::onGrabLoop_DrawMouseSelect(cv::Mat & frame)
{
	if (m_bTargetObj) {
		cv::rectangle(frame, m_OriginPoint, m_EndPoint, cv::Scalar(0xff, 0, 0), 3, 8, 0);
		cv::circle(frame, m_OriginPoint, 10, cv::Scalar(0, 0xff, 0, 0), CV_FILLED, CV_AA, 0);
		cv::circle(frame, m_EndPoint, 10, cv::Scalar(0, 0xff, 0, 0), CV_FILLED, CV_AA, 0);
		if (m_EndPoint.x < m_OriginPoint.x)
			cv::circle(frame, cv::Point(m_EndPoint.x, m_OriginPoint.y), 10, cv::Scalar(0, 0xff, 0, 0), CV_FILLED, CV_AA, 0);
		else
			cv::circle(frame, cv::Point(m_OriginPoint.x + m_TargetRect.width, m_OriginPoint.y), 10, cv::Scalar(0, 0xff, 0, 0), CV_FILLED, CV_AA, 0);
		if (m_EndPoint.y < m_OriginPoint.y)
			cv::circle(frame, cv::Point(m_OriginPoint.x, m_EndPoint.y), 10, cv::Scalar(0, 0xff, 0, 0), CV_FILLED, CV_AA, 0);
		else
			cv::circle(frame, cv::Point(m_OriginPoint.x, m_TargetRect.height + m_OriginPoint.y), 10, cv::Scalar(0, 0xff, 0, 0), CV_FILLED, CV_AA, 0);
	}
}
