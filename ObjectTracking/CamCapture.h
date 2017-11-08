#include <opencv/cv.h>
#include <opencv/highgui.h>

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
	//Austin add it 
	void SetCaptureSize(double width, double height);

protected:
	static DWORD WINAPI threadGrabImage( LPVOID pparam );
	virtual void doGrabLoop();
	virtual void onGrabLoop_cvInit();
	virtual void onGrabLoop_cvClose();
	// camera inited flag
	BOOL m_bCamInited;
	// Event indicating that the thread is about to exit.
	HANDLE m_heventThreadDone;
	// camera capture state
	CaptureState m_State;
	// target selection
	static BOOL m_bTargetObj;

private:
	static void onMouseCB( int event, int x, int y, int flags, void* param );
	void onGrabLoop_DrawROI(IplImage* frame);
	void onGrabLoop_TextFPS();//Austin add it 
	static IplImage* m_pImage;
	static IplImage* m_pROI;
	static CvRect	m_TargetRect;
	static CvPoint	m_OriginPoint;
	static CvPoint	m_EndPoint;
	CvCapture*	m_pCapture;
	pfnCaptureCallback m_pfnCustomCB;
};
/*
class CCamCapture2 : CCamCapture
{
	CCamCapture2();
	~CCamCapture2();
	cv::Mat* GetSelectedROI() const { return m_pROI; }
	cv::Rect GetTargetRect() const { return m_TargetRect; }
	void SetTargetRect(cv::Rect Rect) { m_TargetRect = Rect; }
	void SetCaptureCallback(pfnCaptureCallback2 pfnCaptureCB);

	
	//Austin add it 
	void SetCaptureSize(double width, double height);
};*/