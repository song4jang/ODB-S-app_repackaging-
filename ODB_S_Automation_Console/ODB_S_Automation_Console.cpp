// ODB_S_Automation_Console.cpp : �ܼ� ���� ���α׷��� ���� �������� �����մϴ�.
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

BOOL FindFile(IN CString str_find_path, OUT CList<myFileInfo, myFileInfo&> &list_file)
{
	try
	{
		CFileFind finder;
		BOOL working = finder.FindFile(str_find_path);
		while (working)
		{
			working = finder.FindNextFile();

			if (finder.IsDots())
				continue;

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


int main()
{
    int nRetCode = 0;

    HMODULE hModule = ::GetModuleHandle(nullptr);

    if (hModule != nullptr)
    {
        if (!AfxWinInit(hModule, nullptr, ::GetCommandLine(), 0))
        {
            wprintf(L"�ɰ��� ����: MFC�� �ʱ�ȭ���� ���߽��ϴ�.\n");
            nRetCode = 1;
        }
        else
        {
			// 0. �Է� ���ڸ� �ʱ�ȭ �Ѵ�.
			// argv[1] : �۾���� ���
			// argv[2] : java path
			// argv[3] : find command
			int nCnt = 0;
			
			LPWSTR *pStr = CommandLineToArgvW(GetCommandLine(), &nCnt);

			CString str_src_path;
			if (nCnt >= 1)
			{
				str_src_path.Format(L"%s", pStr[1]);  //�迭 ó�� �����ִ�. // pStr[0]�� ��������. 1�����Ͱ� ����
			}
			else
			{
				AfxMessageBox(L"not enough argument");
				return 0;
			}

			// 1. �۾� ��� ���丮 ���� - �۾���� ���丮�� ����(��_��_��_�ú���)
			CString str_target_dir(str_src_path);
			str_target_dir += "\\";
			str_target_dir += GetDateFolderName();

			if (!CreateDirectory(str_target_dir, NULL))
			{
				printf("CreateDirectory failed(%d)\n", GetLastError());
				return 1;
			}
			
			// 2. �۾� ��� ������ �����Ͽ�, �۾� ��� ���丮�� ������ ���� ������ �Ѵ�.
			// 2.1 �̸�����(apk->zip) ����
			// 2.2 ��������
			CList <myFileInfo, myFileInfo&> list_found_file;
			FindFile(str_src_path + L"\\*.apk", list_found_file);

			CString str_target_unzip_path;
			CString str_target_full_path;

			POSITION pos = list_found_file.GetHeadPosition();
			for (int i = 0; i < list_found_file.GetCount(); i++)
			{
				myFileInfo st_file_info = list_found_file.GetNext(pos);

				str_target_full_path = str_target_dir;
				//str_target_unzip_path = str_target_full_path;
				str_target_full_path += L"\\";
				str_target_full_path += st_file_info.str_file_title;
				str_target_unzip_path = str_target_full_path;
				str_target_full_path += L".zip";
#ifdef _DEBUG
				printf("\nfile : %ws\n", str_target_full_path.GetBuffer(0));
#endif

				CopyFileEx(st_file_info.str_file_full_path, str_target_full_path, NULL, NULL, NULL, COPY_FILE_FAIL_IF_EXISTS);

				UnZipFile(str_target_full_path, str_target_unzip_path);
			}
			
			// 3. �� ���� �� ������ ���� �Ѵ�.

			//CString str_src_zip_file("C:\\Users\\song4jang\\Documents\\Visual Studio 2015\\Projects\\apk\\org.prowl.torquefree.zip");
			//CString str_dest_file("C:\\Users\\song4jang\\Documents\\Visual Studio 2015\\Projects\\apk\\org.prowl.torquefree");
			//UnZipFile(str_src_zip_file, str_dest_file);
        }
    }
    else
    {
        wprintf(L"�ɰ��� ����: GetModuleHandle ����\n");
        nRetCode = 1;
    }

    return nRetCode;
}
