
// ObjectTrackingDlg.cpp : 實作檔
//

#include "stdafx.h"
#include "ObjectTracking.h"
#include "ObjectTrackingDlg.h"
#include "afxdialogex.h"

// 辛辛苦苦創的類別==========================================
#include "VisualTracker.h"
#include "CamCapture.h"
CVisualTracker		ObjectTracker;
CCamCapture2		CamCapture;
// Global Variables==========================================
CString				Tracking_status;


//#ifdef _DEBUG
//#define new DEBUG_NEW
//#endif


// 對 App About 使用 CAboutDlg 對話方塊

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

	// 對話方塊資料
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支援

														// 程式碼實作
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CObjectTrackingDlg 對話方塊



CObjectTrackingDlg::CObjectTrackingDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_OBJECTTRACKING_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CObjectTrackingDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CObjectTrackingDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_START, &CObjectTrackingDlg::OnBnClickedStart)
	ON_BN_CLICKED(IDC_PAUSE, &CObjectTrackingDlg::OnBnClickedPause)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER1, &CObjectTrackingDlg::OnCustomdrawSlider1)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER2, &CObjectTrackingDlg::OnCustomdrawSlider2)
	ON_NOTIFY(NM_CUSTOMDRAW, IDC_SLIDER3, &CObjectTrackingDlg::OnCustomdrawSlider3)
	ON_CBN_SELENDOK(IDC_COMBO_VTC, &CObjectTrackingDlg::OnCbnSelendokComboVtc)
	ON_BN_CLICKED(IDC_CHECK, &CObjectTrackingDlg::OnBnClickedCheck)
END_MESSAGE_MAP()


// CObjectTrackingDlg 訊息處理常式

BOOL CObjectTrackingDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 將 [關於...] 功能表加入系統功能表。

	// IDM_ABOUTBOX 必須在系統命令範圍之中。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 設定此對話方塊的圖示。當應用程式的主視窗不是對話方塊時，
	// 框架會自動從事此作業
	SetIcon(m_hIcon, TRUE);			// 設定大圖示
	SetIcon(m_hIcon, FALSE);		// 設定小圖示

									// 新添加初始化Picture Contorl  170206ref: https://www.zhihu.com/question/29611790/answer/45068617
									//cv::namedWindow("MAIN FRAME", cv::WINDOW_AUTOSIZE);
	cvNamedWindow("MAIN FRAME", CV_WINDOW_AUTOSIZE);
	HWND hWnd = (HWND)cvGetWindowHandle("MAIN FRAME");
	HWND hParent = ::GetParent(hWnd);
	::SetParent(hWnd, GetDlgItem(IDC_STATIC_CV)->m_hWnd);
	::ShowWindow(hParent, SW_HIDE);

	// Disable Slider Control
	GetDlgItem(IDC_SLIDER1)->EnableWindow(FALSE);
	GetDlgItem(IDC_SLIDER2)->EnableWindow(FALSE);
	GetDlgItem(IDC_SLIDER3)->EnableWindow(FALSE);

	return TRUE;  // 傳回 TRUE，除非您對控制項設定焦點
}

void CObjectTrackingDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果將最小化按鈕加入您的對話方塊，您需要下列的程式碼，
// 以便繪製圖示。對於使用文件/檢視模式的 MFC 應用程式，
// 框架會自動完成此作業。

void CObjectTrackingDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 繪製的裝置內容

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 將圖示置中於用戶端矩形
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 描繪圖示
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// 當使用者拖曳最小化視窗時，
// 系統呼叫這個功能取得游標顯示。
HCURSOR CObjectTrackingDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CObjectTrackingDlg::OnBnClickedStart()
{

	// 取得按鈕上的Handler
	CButton* StartBtn = (CButton*)GetDlgItem(IDC_START);
	CButton* PauseBtn = (CButton*)GetDlgItem(IDC_PAUSE);
	// 取得按鈕上的文字
	CString StartStr;
	CString PauseStr;
	StartBtn->GetWindowTextW(StartStr);
	PauseBtn->GetWindowTextW(PauseStr);

	if (CamCapture.GetCaptureState() == CaptureState::STOP) {
		if (!CamCapture.CreateCapture())return;
		else {
			CamCapture.StartCapture();

			// 設定影像擷取大小 170206ref:http://answers.opencv.org/question/34461/how-to-set-camera-resolution-webcam-with-opencv/ / http://stackoverflow.com/questions/28126352/how-to-get-id-static-picture-control-rect-size-in-mfc
			CRect CapRect;
			GetDlgItem(IDC_STATIC_CV)->GetWindowRect(&CapRect);
			ScreenToClient(&CapRect);
			CamCapture.SetCaptureSize(CapRect.Width(), CapRect.Height());

			//變更START上的字串
			StartBtn->SetWindowTextW(_T("Stop Capture"));
		}
	}
	else {
		//變更PAUSE上的字串 
		PauseBtn->SetWindowTextW(_T("Pause"));
		//更改START 字串
		StartBtn->SetWindowTextW(_T("Start Capture"));

		//make state=stop
		CamCapture.StopCapture();

		//清除 Picture Control
		CWnd *pWndPic = GetDlgItem(IDC_STATIC_CV);
		CClientDC DC(pWndPic);
		CRect rectPic;
		CBrush brushWhite(RGB(255, 255, 255));
		pWndPic->GetClientRect(&rectPic);
		DC.FillRect(rectPic, &brushWhite);
	}
}

void CObjectTrackingDlg::OnBnClickedPause()
{
	CButton* PauseBtn = (CButton*)GetDlgItem(IDC_PAUSE);

	switch (CamCapture.GetCaptureState()) {
	case CaptureState::START:
		PauseBtn->SetWindowTextW(_T("Continue"));
		CamCapture.PauseCapture();
		break;
	case CaptureState::PAUSE:
		PauseBtn->SetWindowTextW(_T("Pause"));
		CamCapture.StartCapture();
		break;
	case CaptureState::STOP:
		PauseBtn->SetWindowTextW(_T("Pause"));
		break;
	}
}


void CObjectTrackingDlg::OnCbnSelendokComboVtc()
{
	// TODO: 在此加入控制項告知處理常式程式碼

	// Get ComboBox Index
	CComboBox* m_CB_VTC = (CComboBox*)GetDlgItem(IDC_COMBO_VTC);
	CString strCBText;
	int nIndex = m_CB_VTC->GetCurSel();
	m_CB_VTC->GetLBText(nIndex, strCBText);
	Tracking_status = strCBText;

	CSliderCtrl   *pSlidCtrl1 = (CSliderCtrl*)GetDlgItem(IDC_SLIDER1);
	CSliderCtrl   *pSlidCtrl2 = (CSliderCtrl*)GetDlgItem(IDC_SLIDER2);
	CSliderCtrl   *pSlidCtrl3 = (CSliderCtrl*)GetDlgItem(IDC_SLIDER3);

	CVisualTracker::VT_Params Para;

	if (Tracking_status == "TemplateMatch") {
		GetDlgItem(IDC_SLIDER1)->EnableWindow(TRUE);
		GetDlgItem(IDC_SLIDER2)->EnableWindow(FALSE);
		GetDlgItem(IDC_SLIDER3)->EnableWindow(FALSE);
		GetDlgItem(IDC_STATIC_Slider1_Text)->SetWindowTextW(_T("Method"));
		GetDlgItem(IDC_STATIC_Slider2_Text)->SetWindowTextW(_T(""));
		GetDlgItem(IDC_STATIC_Slider3_Text)->SetWindowTextW(_T(""));

		pSlidCtrl1->SetPos(5);
		pSlidCtrl1->SetRange(0, 5);					//設定參數TM的範圍
		pSlidCtrl2->SetPos(0);
		pSlidCtrl3->SetPos(0);

		//Set Parameter
		Para = ObjectTracker.GetVT_Params();
		Para.TM_Param = ObjectTracker.TM_CCOEFF_NORMED;
		ObjectTracker.SetVT_Params(ObjectTracker.TempMatch, Para);

		//Set Method
		ObjectTracker.SetMethodType(ObjectTracker.TempMatch);
	}
	else if (Tracking_status == "CAMShift") {

		GetDlgItem(IDC_SLIDER1)->EnableWindow(TRUE);
		GetDlgItem(IDC_SLIDER2)->EnableWindow(TRUE);
		GetDlgItem(IDC_SLIDER3)->EnableWindow(TRUE);
		GetDlgItem(IDC_STATIC_Slider1_Text)->SetWindowTextW(_T("Vmin"));
		GetDlgItem(IDC_STATIC_Slider2_Text)->SetWindowTextW(_T("Vmax"));
		GetDlgItem(IDC_STATIC_Slider3_Text)->SetWindowTextW(_T("Smin"));
		pSlidCtrl1->SetRange(0, 50);
		pSlidCtrl2->SetRange(0, 300);
		pSlidCtrl3->SetRange(0, 100);
		pSlidCtrl1->SetPos(10);
		pSlidCtrl2->SetPos(256);
		pSlidCtrl3->SetPos(30);

		//Set Parameter
		Para = ObjectTracker.GetVT_Params();
		Para.CS_Param.vmin = 10;
		Para.CS_Param.vmax = 256;
		Para.CS_Param.smin = 30;
		ObjectTracker.SetVT_Params(ObjectTracker.CAMShift, Para);
		//Set Method
		ObjectTracker.SetMethodType(ObjectTracker.CAMShift);
	}
	else if (Tracking_status == "MeanShift") {

		GetDlgItem(IDC_SLIDER1)->EnableWindow(TRUE);
		GetDlgItem(IDC_SLIDER2)->EnableWindow(TRUE);
		GetDlgItem(IDC_SLIDER3)->EnableWindow(FALSE);
		GetDlgItem(IDC_STATIC_Slider1_Text)->SetWindowTextW(_T("Vmin"));
		GetDlgItem(IDC_STATIC_Slider2_Text)->SetWindowTextW(_T("Vmax"));
		pSlidCtrl1->SetRange(0, 50);
		pSlidCtrl2->SetRange(0, 300);
		pSlidCtrl1->SetPos(10);
		pSlidCtrl2->SetPos(256);
		pSlidCtrl3->SetPos(30);

		//Set Parameter
		Para = ObjectTracker.GetVT_Params();
		Para.MS_Param.vmin = 10;
		Para.MS_Param.vmax = 256;
		ObjectTracker.SetVT_Params(ObjectTracker.MeanShift, Para);

		//Set Method
		ObjectTracker.SetMethodType(ObjectTracker.MeanShift);
	}
	else if (Tracking_status == "MIL") {
		GetDlgItem(IDC_SLIDER1)->EnableWindow(FALSE);
		GetDlgItem(IDC_SLIDER2)->EnableWindow(FALSE);
		GetDlgItem(IDC_SLIDER3)->EnableWindow(FALSE);

		//Set Method
		ObjectTracker.SetMethodType(ObjectTracker.MIL);
	}
	else if (Tracking_status == "BOOSTING") {
		GetDlgItem(IDC_SLIDER1)->EnableWindow(FALSE);
		GetDlgItem(IDC_SLIDER2)->EnableWindow(FALSE);
		GetDlgItem(IDC_SLIDER3)->EnableWindow(FALSE);

		//Set Method
		ObjectTracker.SetMethodType(ObjectTracker.BOOSTING);
	}
	else if (Tracking_status == "MEDIANFLOW") {
		GetDlgItem(IDC_SLIDER1)->EnableWindow(FALSE);
		GetDlgItem(IDC_SLIDER2)->EnableWindow(FALSE);
		GetDlgItem(IDC_SLIDER3)->EnableWindow(FALSE);

		//Set Method
		ObjectTracker.SetMethodType(ObjectTracker.MEDIANFLOW);
	}
	else if (Tracking_status == "TLD") {
		GetDlgItem(IDC_SLIDER1)->EnableWindow(FALSE);
		GetDlgItem(IDC_SLIDER2)->EnableWindow(FALSE);
		GetDlgItem(IDC_SLIDER3)->EnableWindow(FALSE);

		//Set Method
		ObjectTracker.SetMethodType(ObjectTracker.TLD);
	}
	else if (Tracking_status == "KCF") {
		GetDlgItem(IDC_SLIDER1)->EnableWindow(FALSE);
		GetDlgItem(IDC_SLIDER2)->EnableWindow(FALSE);
		GetDlgItem(IDC_SLIDER3)->EnableWindow(FALSE);

		//Set Method
		ObjectTracker.SetMethodType(ObjectTracker.KCF);
	}
	else {
		ObjectTracker.SetMethodType(ObjectTracker.None);
		GetDlgItem(IDC_SLIDER1)->EnableWindow(FALSE);
		GetDlgItem(IDC_SLIDER2)->EnableWindow(FALSE);
		GetDlgItem(IDC_SLIDER3)->EnableWindow(FALSE);
	}

}


void CObjectTrackingDlg::OnCustomdrawSlider1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	// TODO: 在此加入控制項告知處理常式程式碼

	// Show slider position
	CSliderCtrl   *pSlidCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER1);
	CString str;
	str.Format(_T("%d"), pSlidCtrl->GetPos());
	GetDlgItem(IDC_STATIC_Slider1)->SetWindowText(str);

	// Get Parameter Value
	CVisualTracker::VT_Params Para;

	if (Tracking_status == "TemplateMatch") {
		switch (pSlidCtrl->GetPos()) {
		case ObjectTracker.TM_SQDIFF:			Para.TM_Param = ObjectTracker.TM_SQDIFF;		break;
		case ObjectTracker.TM_SQDIFF_NORMED:	Para.TM_Param = ObjectTracker.TM_SQDIFF_NORMED; break;
		case ObjectTracker.TM_CCORR:			Para.TM_Param = ObjectTracker.TM_CCORR;			break;
		case ObjectTracker.TM_CCORR_NORMED:		Para.TM_Param = ObjectTracker.TM_CCORR_NORMED;	break;
		case ObjectTracker.TM_CCOEFF:			Para.TM_Param = ObjectTracker.TM_CCOEFF;		break;
		case ObjectTracker.TM_CCOEFF_NORMED:	Para.TM_Param = ObjectTracker.TM_CCOEFF_NORMED; break;
		default: return;
		}
		ObjectTracker.SetVT_Params(ObjectTracker.TempMatch, Para);
	}
	else if (Tracking_status == "MeanShift") {
		Para.MS_Param = ObjectTracker.GetVT_Params().MS_Param;
		Para.MS_Param.vmin = pSlidCtrl->GetPos();
		ObjectTracker.SetVT_Params(ObjectTracker.MeanShift, Para);
	}
	else if (Tracking_status == "CAMShift") {
		Para.CS_Param = ObjectTracker.GetVT_Params().CS_Param;
		Para.CS_Param.vmin = pSlidCtrl->GetPos();
		ObjectTracker.SetVT_Params(ObjectTracker.CAMShift, Para);
	}

	*pResult = 0;
}


void CObjectTrackingDlg::OnCustomdrawSlider2(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	// TODO: 在此加入控制項告知處理常式程式碼

	// Show slider position
	CSliderCtrl   *pSlidCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER2);
	CString str;
	str.Format(_T("%d"), pSlidCtrl->GetPos());
	GetDlgItem(IDC_STATIC_Slider2)->SetWindowText(str);

	// Get Parameter Value
	CVisualTracker::VT_Params Para;

	if (Tracking_status == "CAMShift") {
		Para.CS_Param = ObjectTracker.GetVT_Params().CS_Param;
		Para.CS_Param.vmax = pSlidCtrl->GetPos();
		ObjectTracker.SetVT_Params(ObjectTracker.CAMShift, Para);
	}
	else if (Tracking_status == "MeanShift") {
		Para.MS_Param = ObjectTracker.GetVT_Params().MS_Param;
		Para.MS_Param.vmax = pSlidCtrl->GetPos();
		ObjectTracker.SetVT_Params(ObjectTracker.MeanShift, Para);
	}

	*pResult = 0;
}


void CObjectTrackingDlg::OnCustomdrawSlider3(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	// TODO: 在此加入控制項告知處理常式程式碼
	// Show slider position
	CSliderCtrl   *pSlidCtrl = (CSliderCtrl*)GetDlgItem(IDC_SLIDER3);
	CString str;
	str.Format(_T("%d"), pSlidCtrl->GetPos());
	GetDlgItem(IDC_STATIC_Slider3)->SetWindowText(str);

	// Get Parameter Value
	CVisualTracker::VT_Params Para;

	if (Tracking_status == "CAMShift") {
		Para.CS_Param = ObjectTracker.GetVT_Params().CS_Param;
		Para.CS_Param.smin = pSlidCtrl->GetPos();
		ObjectTracker.SetVT_Params(ObjectTracker.CAMShift, Para);
	}

	*pResult = 0;
}

void CObjectTrackingDlg::OnBnClickedCheck()
{
	// Get Checkbox status & send to CamCapture Class  171108
	CamCapture.SetShowFPS(((CButton*)GetDlgItem(IDC_CHECK))->GetCheck());
}
