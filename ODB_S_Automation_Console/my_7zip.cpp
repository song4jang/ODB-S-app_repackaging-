#include "stdafx.h"
#include "my_zip.h"

BOOL UnZipFile(CString strSrc, CString strDest)
{
	SHELLEXECUTEINFO execinfo;
	CString strParam("-y x ");
	strParam += strSrc;
	strParam += " -o";
	strParam += strDest;

	// 실행을 위해 구조체 세트
	ZeroMemory(&execinfo, sizeof(execinfo));
	execinfo.cbSize = sizeof(execinfo);
	execinfo.lpVerb = L"open";
	execinfo.lpFile = L"..\\3rdParty\\7zip\\bin\\7za.exe";
	execinfo.lpParameters = strParam;
	execinfo.fMask = SEE_MASK_FLAG_NO_UI | SEE_MASK_NOCLOSEPROCESS;
	execinfo.nShow = SW_SHOWDEFAULT;

	::ShellExecuteEx(&execinfo);

	WaitForSingleObject(execinfo.hProcess, INFINITE);
	
	return TRUE;
}