#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <ctime>
#include <windows.h>
#include "VisualTracker.h"
const int Default_WaitSecond = 5; 
typedef enum
{
	STOP,
	PAUSE,
	START,
}CaptureState;

typedef void (__cdecl *pfnCaptureCallback)(IplImage* pFrame);	// OpenCV 1.0
typedef void (__cdecl *pfnCaptureCallback2)(cv::Mat& Frame);	// OpenCV 2.0

class CCamCapture
{
public:
	CCamCapture();
	~CCamCapture();
	BOOL CreateCapture();
	void DestroyCapture();
	void StartCapture() { m_State = START; }
	void PauseCapture() { m_State = PAUSE; }
	void StopCapture()	{ m_State =  STOP; }
	CaptureState GetCaptureState() const { return m_State; }
	IplImage* GetSelectedROI() const { return m_pROI; }
	CvRect GetTargetRect() const { return m_TargetRect; }
	void SetTargetRect(CvRect Rect) { m_TargetRect = Rect; }
	void SetCaptureCallback(pfnCaptureCallback pfnCaptureCB);

	void SetCaptureSize(double width, double height);	//Austin add it 
	void SetShowFPS(BOOL val) { m_bFPS_State = val; }//Austin add it
protected:
	static DWORD WINAPI threadGrabImage( LPVOID pparam );
	virtual void doGrabLoop();
	virtual void onGrabLoop_cvInit();
	virtual void onGrabLoop_cvClose();
	BOOL m_bCamInited;// camera inited flag
	HANDLE m_heventThreadDone;// Event indicating that the thread is about to exit.
	CaptureState m_State;// camera capture state
	static BOOL m_bTargetObj;// target selection

	//FPS section
	BOOL m_bFPS_State;	// camera FPS state
	CvFont Font;	
	char buf[32];
    int time_to_wait = 50;
    int frameCounter = 0;
    int tick = 0;
    int fps ;
    std::time_t timeBegin = std::time(0);
	std::time_t timeNow;
	//FPS section end

private:
	static void onMouseCB( int event, int x, int y, int flags, void* param );
	void onGrabLoop_DrawROI(IplImage* frame);
	void onGrabLoop_ShowFPS(IplImage * frame, double t);//Austin add it
	void onGrabLoop_DrawMouseSelect(IplImage* frame);//Austin add it
	static IplImage* m_pImage;
	static IplImage* m_pROI;
	static CvRect	m_TargetRect;
	static CvPoint	m_OriginPoint;
	static CvPoint	m_EndPoint;
	CvCapture*	m_pCapture;
	pfnCaptureCallback m_pfnCustomCB;
	//test
	cv::Mat m_ImageMat;
	cv::Mat m_ROImat;
	static cv::Rect		m_Recttest;
};

class CCamCapture2 :public CCamCapture
{
public:
	CCamCapture2();
	~CCamCapture2();
	cv::Mat*			GetSelectedROI() const { return &m_ROI; }
	cv::Rect			GetTargetRect() const { return m_TargetRect; }
	void				SetTargetRect(cv::Rect Rect) { m_TargetRect = Rect; }
	void				SetCaptureCallback(pfnCaptureCallback2 pfnCaptureCB);
	void				SetCaptureSize(double width, double height);//Austin
protected:
	virtual void		doGrabLoop();
	virtual void		onGrabLoop_cvInit();
	virtual void		onGrabLoop_cvClose();
	static BOOL				m_bTrackerInit;
private:
	static void			onMouseCB2(int event, int x, int y, int flags, void* param);
	void				onGrabLoop_DrawROI(cv::Mat& frame);
	void				onGrabLoop_ShowFPS(cv::Mat& frame, double t);//Austin
	void				onGrabLoop_DrawMouseSelect(cv::Mat& frame);//Austin
	static cv::Mat		m_Image;
	static cv::Mat		m_ROI;
	static cv::Rect		m_TargetRect;
	static cv::Rect2d	m_TargetRect2d;
	static cv::Point	m_OriginPoint;
	static cv::Point	m_EndPoint;
	cv::VideoCapture    m_cap;
	pfnCaptureCallback2 m_pfnCustomCB;
   
};