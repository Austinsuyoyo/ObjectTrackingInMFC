
// ObjectTrackingDlg.h : 標頭檔
//

#pragma once


// CObjectTrackingDlg 對話方塊
class CObjectTrackingDlg : public CDialogEx
{
// 建構
public:
	CObjectTrackingDlg(CWnd* pParent = NULL);	// 標準建構函式

// 對話方塊資料
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_OBJECTTRACKING_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支援


// 程式碼實作
protected:
	HICON m_hIcon;

	// 產生的訊息對應函式
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	void onMouse(int event, int x, int y);
	static void onMouse(int event, int x, int y, int flags, void* param);
	afx_msg void OnBnClickedStart();
	afx_msg void OnBnClickedPause();
	afx_msg void OnCustomdrawSlider1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnCustomdrawSlider2(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnCustomdrawSlider3(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnCbnSelendokComboVtc();

};
