
// ObjectTracking.h : PROJECT_NAME ���ε{�����D�n���Y��
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�� PCH �]�t���ɮ׫e���]�t 'stdafx.h'"
#endif

#include "resource.h"		// �D�n�Ÿ�


// CObjectTrackingApp: 
// �аѾ\��@�����O�� ObjectTracking.cpp
//

class CObjectTrackingApp : public CWinApp
{
public:
	CObjectTrackingApp();

// �мg
public:
	virtual BOOL InitInstance();

// �{���X��@

	DECLARE_MESSAGE_MAP()
};

extern CObjectTrackingApp theApp;