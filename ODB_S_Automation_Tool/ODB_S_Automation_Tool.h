
// ODB_S_Automation_Tool.h : PROJECT_NAME ���� ���α׷��� ���� �� ��� �����Դϴ�.
//

#pragma once

#ifndef __AFXWIN_H__
	#error "PCH�� ���� �� ������ �����ϱ� ���� 'stdafx.h'�� �����մϴ�."
#endif

#include "resource.h"		// �� ��ȣ�Դϴ�.


// CODB_S_Automation_ToolApp:
// �� Ŭ������ ������ ���ؼ��� ODB_S_Automation_Tool.cpp�� �����Ͻʽÿ�.
//

class CODB_S_Automation_ToolApp : public CWinApp
{
public:
	CODB_S_Automation_ToolApp();

// �������Դϴ�.
public:
	virtual BOOL InitInstance();

// �����Դϴ�.

	DECLARE_MESSAGE_MAP()
};

extern CODB_S_Automation_ToolApp theApp;