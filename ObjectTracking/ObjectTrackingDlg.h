
// ObjectTrackingDlg.h : ���Y��
//

#pragma once


// CObjectTrackingDlg ��ܤ��
class CObjectTrackingDlg : public CDialogEx
{
// �غc
public:
	CObjectTrackingDlg(CWnd* pParent = NULL);	// �зǫغc�禡

// ��ܤ�����
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_OBJECTTRACKING_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV �䴩


// �{���X��@
protected:
	HICON m_hIcon;

	// ���ͪ��T�������禡
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
