// ODB_S_Automation_Console.cpp : 콘솔 응용 프로그램에 대한 진입점을 정의합니다.
//

#include "stdafx.h"
#include <exception>
#include "ODB_S_Automation_Console.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#include "my_zip.h"

CWinApp theApp;

using namespace std;

typedef struct _stMyFileInfo
{
	CString str_file_full_path;
	CString str_file_name;
	CString str_file_title;
}myFileInfo;


typedef struct _stFindInfo
{
	int nCnt;
	CString str_file_full_path;
	CString str_file_name;
}FindInfo;

BOOL FindFile(IN CString str_find_path, IN CString str_filter, OUT CList<myFileInfo, myFileInfo&> &list_file)
{
	try
	{
		CFileFind finder;
		BOOL working = finder.FindFile(str_find_path + str_filter);
		while (working)
		{
			working = finder.FindNextFile();

			if (finder.IsDots())
				continue;

//			wprintf(L"%s\n", finder.GetFilePath());

			if (finder.IsDirectory())
				FindFile(finder.GetFilePath(), str_filter, list_file);


			myFileInfo myFile;
			myFile.str_file_full_path = finder.GetFilePath();
			myFile.str_file_name = finder.GetFileName();
			myFile.str_file_title = finder.GetFileTitle();

			list_file.AddTail(myFile);
		}
	}
	catch (exception& e)
	{
		CString str_err;
		str_err.Format(L"exception, str_find_path(%s), e.what(%s)\n", str_find_path, e.what());
		MessageBox(NULL, str_err, NULL, MB_OK | MB_ICONERROR);
		return FALSE;
	}

	return TRUE;
}

BOOL RenameFile(IN CString str_src_file, IN CString str_dest_file)
{
	try
	{
		CFile::Rename(str_src_file, str_dest_file);
	}
	catch (exception& e)
	{
		CString str_err;
		str_err.Format(L"exception, str_source_file(%s), e.what(%s)\n", str_src_file, e.what());
		MessageBox(NULL, str_err, NULL, MB_OK | MB_ICONERROR);
		return FALSE;
	}

	return TRUE;
}

CString GetDateFolderName()
{
	CString str_folder_name("");
	try
	{
		CTime cur_time = CTime::GetCurrentTime();

		str_folder_name.Format(L"%04d_%02d_%02d_%02d%02d%02d", cur_time.GetYear(), cur_time.GetMonth(), cur_time.GetDay(), cur_time.GetHour(), cur_time.GetMinute(), cur_time.GetSecond());
	}
	catch (exception& e)
	{
		CString str_err;
		str_err.Format(L"exception, e.what(%s)\n", e.what());
		MessageBox(NULL, str_err, NULL, MB_OK | MB_ICONERROR);
	}

	return str_folder_name;
}

// FIXME : 폴더에 공백이 존재할 경우에 대한 처리
void ExtractSmali(IN CString str_target_unzip_path, IN CString str_src_path, IN CString str_jar_name)
{
	try
	{
		CString str_jar_path(str_src_path), str_out_path(str_target_unzip_path), str_dex_path(str_target_unzip_path);
		str_jar_path += L"\\";
		str_jar_path += str_jar_name;
		str_dex_path += L"\\classes.dex";
		str_out_path += L"\\out";

		CString str_command;
		str_command = "-jar ";
		str_command += str_jar_path;

		str_command += " -x ";
		str_command += str_dex_path;

		str_command += " -o ";
		str_command += str_out_path;

#ifdef _DEBUG
		wprintf(L"str_command : %s\n\n", str_command.GetBuffer(0));
#endif

		SHELLEXECUTEINFO ShExecInfo = {0, };
		ShExecInfo.cbSize = sizeof(SHELLEXECUTEINFO);
		ShExecInfo.fMask = SEE_MASK_NOCLOSEPROCESS;
		ShExecInfo.lpVerb = L"open";
		ShExecInfo.lpFile = L"java";
		ShExecInfo.lpParameters = str_command;
		ShExecInfo.nShow = SW_SHOW;
		ShellExecuteEx(&ShExecInfo);
		WaitForSingleObject(ShExecInfo.hProcess, INFINITE);
	}
	catch (exception& e)
	{
		CString str_err;
		str_err.Format(L"exception, str_target_unzip_path(%s), str_src_path(%s), e.what(%s)\n", str_target_unzip_path, str_src_path, e.what());
		MessageBox(NULL, str_err, NULL, MB_OK | MB_ICONERROR);
	}
}

void FindKeyword(IN CString str_find_path, IN CString str_find_keyword, OUT CList<FindInfo, FindInfo&> &list_find)
{
	try
	{
		CList <myFileInfo, myFileInfo&> list_found_file;
		FindFile(str_find_path, str_find_keyword, list_found_file);

		for (int i = 0; i < list_found_file.GetCount(); i++)
		{
			
		}
		
		//list_found_file
		//FindInfo find_info;
		//list_find.AddTail(find_info);
	}
	catch (exception& e)
	{
		CString str_err;
		str_err.Format(L"exception, e.what(%s)\n", e.what());
		MessageBox(NULL, str_err, NULL, MB_OK | MB_ICONERROR);
	}
}

int main()
{
    int nRetCode = 0;

    HMODULE hModule = ::GetModuleHandle(nullptr);

    if (hModule != nullptr)
    {
        if (!AfxWinInit(hModule, nullptr, ::GetCommandLine(), 0))
        {
            wprintf(L"심각한 오류: MFC를 초기화하지 못했습니다.\n");
            nRetCode = 1;
        }
        else
        {
			// 0. 입력 인자
			// argv[1] : 작업대상 경로(apk, baksmali 모두 존재해야 한다)
			// argv[2] : find command
			int nCnt = 0;
			
			LPWSTR *pStr = CommandLineToArgvW(GetCommandLine(), &nCnt);

			CString str_src_path;
			if (nCnt >= 2)
			{
				str_src_path.Format(L"%s", pStr[1]);  //배열 처럼 쓸수있다. // pStr[0]은 실행파일. 1번부터가 인자
			}
			else
			{
				AfxMessageBox(L"not enough argument");
				return 0;
			}

			// 1. 작업 대상 디렉토리 생성 - 작업대상 디렉토리명 구성(년_월_일_시분초)
			CString str_target_dir(str_src_path);
			str_target_dir += L"\\";
			str_target_dir += GetDateFolderName();

			if (!CreateDirectory(str_target_dir, NULL))
			{
				wprintf(L"CreateDirectory failed(%d)\n", GetLastError());
				return 1;
			}
			
			// 2. 작업 대상 파일을 색출하여, 작업 대상 디렉토리로 다음과 같은 행위를 한다.
			// 이름변경(apk->zip) 복사, 압축해제, dex -> smali
			CList <myFileInfo, myFileInfo&> list_found_file;
			FindFile(str_src_path, CString(L"\\*.apk"), list_found_file);

			CList <FindInfo, FindInfo&> list_found_info;

			CString str_target_unzip_path;
			CString str_target_full_path;

			POSITION pos = list_found_file.GetHeadPosition();
			for (int i = 0; i < list_found_file.GetCount(); i++)
			{
				myFileInfo st_file_info = list_found_file.GetNext(pos);

				str_target_full_path = str_target_dir;
				str_target_full_path += L"\\";
				str_target_full_path += st_file_info.str_file_title;
				str_target_unzip_path = str_target_full_path;
				str_target_full_path += L".zip";
#ifdef _DEBUG
				wprintf(L"\n%d - file : %s\n", i, str_target_full_path.GetBuffer(0));
#endif
				// 2.1 이름변경(apk->zip) 복사
				CopyFileEx(st_file_info.str_file_full_path, str_target_full_path, NULL, NULL, NULL, COPY_FILE_FAIL_IF_EXISTS);
				
				// 2.2 압축해제
				UnZipFile(str_target_full_path, str_target_unzip_path);

				// 2.3 dex -> smali
				ExtractSmali(str_target_unzip_path, str_src_path, L"baksmali-2.0.5.jar");

				// 2.4 find command
				FindKeyword(str_target_unzip_path + CString(L"\\out"), CString(L"\\*.*"), list_found_info);
			}
        }
    }
    else
    {
        wprintf(L"심각한 오류: GetModuleHandle 실패\n");
        nRetCode = 1;
    }

    return nRetCode;
}
