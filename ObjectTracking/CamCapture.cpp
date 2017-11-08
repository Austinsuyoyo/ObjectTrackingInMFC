#include "stdafx.h"
#include "CamCapture.h"

BOOL CCamCapture::m_bTargetObj	= false;
IplImage* CCamCapture::m_pImage = NULL;
IplImage* CCamCapture::m_pROI	= NULL;
CvRect	CCamCapture::m_TargetRect;
CvPoint CCamCapture::m_OriginPoint;
CvPoint CCamCapture::m_EndPoint;
static double t;
CvFont Font;
char buf[32];
CCamCapture::CCamCapture()
{
	m_pCapture = NULL;
	m_bCamInited = false;
}

CCamCapture::~CCamCapture()
{
	m_bCamInited= false;
	m_bTargetObj= false;
	onGrabLoop_cvClose();
}

BOOL CCamCapture::CreateCapture()
{
	DWORD dwThreadID;
	m_heventThreadDone = CreateThread(NULL, 0, threadGrabImage, (LPVOID*) this, 0, &dwThreadID);   
	return (m_heventThreadDone != NULL) ?  true : false;
}

void CCamCapture::DestroyCapture()
{
	if(m_bCamInited)
	{
		m_State = STOP;
		WaitForSingleObject(m_heventThreadDone, Default_WaitSecond);
	}
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
	onGrabLoop_cvInit();
	while(m_bCamInited)
	{
		switch(m_State)
		{
		case START:
			// refresh and get camera frame.
			m_pImage = cvQueryFrame(m_pCapture); 
			// set screen color : white.
			if(m_pImage == NULL) cvSet(m_pImage, cvScalarAll(255));	
			// 滑鼠響應函式
			cvSetMouseCallback("MAIN FRAME", onMouseCB, NULL);
			// clone the original image.
			IplImage* m_pImageClone; 
			m_pImageClone = cvCreateImage(cvGetSize(m_pImage), m_pImage->depth, m_pImage->nChannels);
			cvCopy(m_pImage, m_pImageClone, NULL);
			// Draw Mouse select ROI
			if (m_bTargetObj) {
				cvRectangle(m_pImageClone, m_OriginPoint, m_EndPoint, cvScalar(0xff, 0, 0), 3, 8, 0);
				cvCircle(m_pImageClone, m_OriginPoint, 10, cvScalar(0, 0xff, 0, 0), CV_FILLED, CV_AA, 0);
				cvCircle(m_pImageClone, m_EndPoint, 10, cvScalar(0, 0xff, 0, 0), CV_FILLED, CV_AA, 0);
				if (m_EndPoint.x < m_OriginPoint.x)
					cvCircle(m_pImageClone, cvPoint(m_EndPoint.x, m_OriginPoint.y), 10, cvScalar(0, 0xff, 0, 0), CV_FILLED, CV_AA, 0);
				else
					cvCircle(m_pImageClone, cvPoint(m_OriginPoint.x + m_TargetRect.width, m_OriginPoint.y), 10, cvScalar(0, 0xff, 0, 0), CV_FILLED, CV_AA, 0);
				if (m_EndPoint.y < m_OriginPoint.y)
					cvCircle(m_pImageClone, cvPoint(m_OriginPoint.x, m_EndPoint.y), 10, cvScalar(0, 0xff, 0, 0), CV_FILLED, CV_AA, 0);
				else
					cvCircle(m_pImageClone, cvPoint(m_OriginPoint.x, m_TargetRect.height + m_OriginPoint.y), 10, cvScalar(0, 0xff, 0, 0), CV_FILLED, CV_AA, 0);
					
			}
			//FPS START--------------------
			t = (double)cvGetTickCount();


			//FPS END----------------------
			t = ((double)cvGetTickCount() - t) / (double)cvGetTickFrequency();
			
			sprintf(buf, "%5.3f", 1/t);	// 1 / t = FPS
			cvInitFont(&Font, CV_FONT_HERSHEY_SIMPLEX, 0.5f, 0.5f,1, 2);
			cvPutText(m_pImageClone, buf, cvPoint(5, 20), &Font, cvScalar(0, 0, 255));
			//------------------------------
			//if(m_bTargetObj)
			cvShowImage("MAIN FRAME", m_pImageClone);
			cvReleaseImage(&m_pImageClone);

			onGrabLoop_DrawROI(m_pImage);
			cvWaitKey(Default_WaitSecond);
			break;

		case PAUSE:
			cvSetMouseCallback("MAIN FRAME", onMouseCB, NULL); // mouse event (opneCV)
			if(m_bTargetObj)
			{
				IplImage* m_pImageClone; // clone the original image.
				m_pImageClone = cvCreateImage(cvGetSize(m_pImage), m_pImage->depth, m_pImage->nChannels);
				cvCopy(m_pImage, m_pImageClone, NULL);

				if (m_bTargetObj) {
					cvRectangle(m_pImageClone, m_OriginPoint, m_EndPoint, cvScalar(0xff, 0, 0), 3, 8, 0);
					cvCircle(m_pImageClone, m_OriginPoint, 10, cvScalar(0, 0xff, 0, 0), CV_FILLED, CV_AA, 0);
					cvCircle(m_pImageClone, m_EndPoint, 10, cvScalar(0, 0xff, 0, 0), CV_FILLED, CV_AA, 0);
					if (m_EndPoint.x < m_OriginPoint.x)
						cvCircle(m_pImageClone, cvPoint(m_EndPoint.x, m_OriginPoint.y), 10, cvScalar(0, 0xff, 0, 0), CV_FILLED, CV_AA, 0);
					else
						cvCircle(m_pImageClone, cvPoint(m_OriginPoint.x + m_TargetRect.width, m_OriginPoint.y), 10, cvScalar(0, 0xff, 0, 0), CV_FILLED, CV_AA, 0);
					if (m_EndPoint.y < m_OriginPoint.y)
						cvCircle(m_pImageClone, cvPoint(m_OriginPoint.x, m_EndPoint.y), 10, cvScalar(0, 0xff, 0, 0), CV_FILLED, CV_AA, 0);
					else
						cvCircle(m_pImageClone, cvPoint(m_OriginPoint.x, m_TargetRect.height + m_OriginPoint.y), 10, cvScalar(0, 0xff, 0, 0), CV_FILLED, CV_AA, 0);
				}

				cvShowImage("MAIN FRAME", m_pImageClone);
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
			//判斷攝影機影相與ROI影像中，是否有影像，有的話進行Release
			break;

		default:
			break;
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

void CCamCapture::onGrabLoop_cvClose()
{
	if(m_pCapture!= NULL) cvReleaseCapture(&m_pCapture); // cvReleaseCapture would release capture and image.
	//if(m_pImage->imageData != NULL) cvReleaseImage(&m_pImage);
	if(m_pROI	 != NULL) cvReleaseImage(&m_pROI);
	//if(cvGetWindowHandle("MAIN FRAME") != NULL)	cvDestroyWindow("MAIN FRAME");
	if(cvGetWindowHandle("ROI") != NULL)	cvDestroyWindow("ROI");
}

void CCamCapture::onMouseCB( int event, int x, int y, int flags, void* param )
{
	if(event == CV_EVENT_LBUTTONDOWN && !m_bTargetObj)
	{
		m_bTargetObj = true;
		m_OriginPoint = cvPoint(x, y);
		m_EndPoint = m_OriginPoint;
		m_TargetRect = cvRect(x, y, 0, 0);
		//destroy target frame
	}

	if((event == CV_EVENT_MOUSEMOVE) && (m_bTargetObj))
	{


		//170206 添加避免游標超出邊界
		int border_x, border_y;
		border_x = (x > m_pImage->width) ? m_pImage->width : (x < 0) ? 0 : x;
		border_y = (y > m_pImage->height) ? m_pImage->height : (y < 0) ? 0 : y;

		// EndPoint
		m_EndPoint = cvPoint(border_x, border_y);

		m_TargetRect.width  = abs(m_EndPoint.x - m_OriginPoint.x);
		m_TargetRect.height = abs(m_EndPoint.y - m_OriginPoint.y);
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
			//roiRect2d.x = roiRect.x;
			//roiRect2d.y = roiRect.y;
			//roiRect2d.height = roiRect.height;
			//roiRect2d.width = roiRect.width;
			// For Tracker
			//onetime = 1;
		}
	}
}

void CCamCapture::onGrabLoop_DrawROI( IplImage* frame )
{

	// 擷取圖像 170719避免點到一下
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
		if (m_TargetRect.height > 0 && m_TargetRect.width > 0)
		{
			if (m_pROI != NULL)cvReleaseImage(&m_pROI);
			cvSetImageROI(frame, m_TargetRect);
			m_pROI = cvCreateImage(cvGetSize(frame), frame->depth, frame->nChannels);
			cvCopy(frame, m_pROI, NULL);
			cvShowImage("roiTarget", m_pROI);
			cvResetImageROI(frame);
		}
		if(m_pROI != NULL)cvShowImage("roiTarget", m_pROI);
	}

}

void CCamCapture::onGrabLoop_TextFPS()
{

}
