// ODB_S_Automation_Console.cpp : �ܼ� ���� ���α׷��� ���� �������� �����մϴ�.
//

#include "stdafx.h"
#include <exception>
#include "ODB_S_Automation_Console.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// ���ɻ��� ������ COM�� �̿��� windows default unzip�� ������� �ʴ´�.
#include "my_zip.h"

// 7zip�� �̿��Ѵ�.
#include "my_7zip.h"

CWinApp theApp;

using namespace std;

typedef struct _stMyFileInfo
{
	CString str_file_full_path;		// c:\\aaa.txt
	CString str_file_name;			// aaa.txt
	CString str_file_title;			// aaa
}myFileInfo;

typedef struct _stFindFileInfo
{
	CString str_file_full_path;
	CString str_file_name;
	CString str_line_content;
	int num_line;
}FindFileInfo;

typedef struct _stFindInfo
{
	CString str_apk_file_name;
	FindFileInfo st_find_file_info;
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

// FIXME : ������ ������ ������ ��쿡 ���� ó��
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

void FindKeyword(IN CString str_find_path, IN CString str_filter, IN CString str_find_keyword, OUT CList<FindInfo, FindInfo&> &list_find)
{
	try
	{
		// 1. ��� ������ �����ϰ�, �˻��Ѵ�.
		// ��� : str_find_path ����
		// ���� : str_find_keyword
		CList <myFileInfo, myFileInfo&> list_found_file;
		FindFile(str_find_path + CString(L"\\out"), str_filter, list_found_file);

		POSITION pos = list_found_file.GetHeadPosition();
		CStdioFile stdio_src_file;
		CString str_read_line;

		for (int i = 0; i < list_found_file.GetCount(); i++)
		{
			myFileInfo st_file_info = list_found_file.GetNext(pos);

			if (stdio_src_file.Open(st_file_info.str_file_full_path, CFile::modeRead))
			{
				int nLineCnt = 0;
				while (stdio_src_file.ReadString(str_read_line))
				{
					nLineCnt++;
					
					//str_read_line.MakeUpper();
					//str_find_keyword.MakeUpper();

					if (str_read_line.Find(str_find_keyword) != -1)
					{
						FindInfo info;
						info.st_find_file_info.num_line = nLineCnt;
						info.st_find_file_info.str_file_full_path = st_file_info.str_file_full_path;
						info.st_find_file_info.str_file_name = st_file_info.str_file_name;
						info.str_apk_file_name = str_find_path + CString(L".apk");
						info.st_find_file_info.str_line_content = str_read_line.GetString();

						list_find.AddTail(info);
#ifdef _DEBUG
						wprintf(L"nLineCnt %d, st_file_info.str_file_full_path : %s\n\n", nLineCnt, st_file_info.str_file_full_path.GetBuffer(0));
#endif
					}
				}
				stdio_src_file.Close();
			}
		}
	}
	catch (exception& e)
	{
		CString str_err;
		str_err.Format(L"exception, e.what(%s)\n", e.what());
		MessageBox(NULL, str_err, NULL, MB_OK | MB_ICONERROR);
	}
}

void WriteResult(IN CString str_find_command, IN CString str_src_path, IN CList <FindInfo, FindInfo&> &list_found_info)
{
	
}

//void PrintResult(IN CString str_find_command, IN CString str_src_path, IN CList <FindInfo, FindInfo&> &list_found_info)
void PrintResult(IN CString str_find_command, IN CString str_src_path, IN CList <FindInfo, FindInfo&> &list_found_info)
{
	wprintf(L"\n\n\n\n###################################### Result of Analyzing #####################################\n");
	wprintf(L"1. Find Command : %s\n", str_find_command.GetBuffer(0));
	wprintf(L"2. Target Path : %s\n", str_src_path.GetBuffer(0));

	CString str_apk_name;
	POSITION pos = list_found_info.GetHeadPosition();
	for (int i = 0; i < list_found_info.GetCount(); i++)
	{
		FindInfo st_find_info = list_found_info.GetNext(pos);

		//apk file name�� ���ο� apk �� ���ö��� ����ϵ��� �Ѵ�.
		if (str_apk_name.Compare(st_find_info.str_apk_file_name))
		{
			wprintf(L"\nAPK File Name : %s\n", st_find_info.str_apk_file_name.GetBuffer(0));
			st_find_info.str_apk_file_name.ReleaseBuffer();
			str_apk_name = st_find_info.str_apk_file_name;
		}

		wprintf(L"\t\t\tLine(%d), file path(%s)\n\t\t\t\tline(%s)\n", st_find_info.st_find_file_info.num_line, st_find_info.st_find_file_info.str_file_full_path.GetBuffer(0), st_find_info.st_find_file_info.str_line_content.GetBuffer(0));
		st_find_info.st_find_file_info.str_file_full_path.ReleaseBuffer();
		st_find_info.st_find_file_info.str_line_content.ReleaseBuffer();
	}
}

// �ϳ��� �ۿ� ���� �м� �����̶�, AnalysisList���� ������ AppName�� ������ ���� �� �� �ִ�. ��? AnalysisList�� ��Ī�Ǵ� AT Command �߽����� �׸��� �����ϹǷ�.
int GetAppCountinAnalysisList(IN CList <FindInfo, FindInfo&> &list_found_info)
{
	int vulerable_app_cnt = 0;
	POSITION pos = list_found_info.GetHeadPosition();
	CString str_before_apk_name;

	for (int i = 0; i < list_found_info.GetCount(); i++)
	{
		FindInfo st_find_info = list_found_info.GetNext(pos);

		if (!str_before_apk_name.CompareNoCase(st_find_info.str_apk_file_name))
			continue;
		 
		str_before_apk_name = st_find_info.str_apk_file_name;
		vulerable_app_cnt++;
	}

	return vulerable_app_cnt;
}
void PrintResultSummary(IN CList <CString, CString&> &list_target_unzip_path, IN CList <FindInfo, FindInfo&> &list_found_info)
{
	wprintf(L"\n\n\n\n###################################### App Risk Analysis #####################################\n");
	wprintf(L"1. Total App Count: %lld\n", list_target_unzip_path.GetCount());
	int vulerable_app_cnt = GetAppCountinAnalysisList(list_found_info);
	wprintf(L"2. Vulnerable App Count: %d(%.1lf %%)\n", vulerable_app_cnt, (double)vulerable_app_cnt / list_target_unzip_path.GetCount() * 100.0);
	wprintf(L"3. Vulnerable App Name :\n");
	 
	// Vulnerable App �Ǻ�
	CString str_before_apk_name;
	POSITION pos = list_found_info.GetHeadPosition();
	int app_cnt = 1;
	for (int i = 0; i < list_found_info.GetCount(); i++)
	{
		FindInfo st_find_info = list_found_info.GetNext(pos);
		
		if (!str_before_apk_name.CompareNoCase(st_find_info.str_apk_file_name))
			continue;
		
		str_before_apk_name = st_find_info.str_apk_file_name;

		wprintf(L"\t %d) %ws\n", app_cnt++, st_find_info.str_apk_file_name.GetBuffer(0));
		st_find_info.str_apk_file_name.ReleaseBuffer();
	}
}

BOOL GetFindCommand(IN CString str_command_file, IN CList<CString, CString&> &list_find_command)
{
	CStdioFile stdio_src_file;
	CString str_read_line;

	if (stdio_src_file.Open(str_command_file, CFile::modeRead))
	{
		while (stdio_src_file.ReadString(str_read_line))
		{
			list_find_command.AddTail(str_read_line);
		}
		
		stdio_src_file.Close();
	}
	else
	{
		return FALSE;
	}
	return TRUE;
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
			// 0. �Է� ����
			// argv[1] : �۾���� ���(apk, baksmali ��� �����ؾ� �Ѵ�)
			// argv[2] : find command
			int nCnt = 0;
			
			LPWSTR *pStr = CommandLineToArgvW(GetCommandLine(), &nCnt);

			CString str_src_path;
			CList<CString, CString&> list_find_command;
			
			// AT Command�� ������ �������鼭, ����(AT_Command.txt)���� �Է� �޴� �������� �����Ѵ�.
			/*
			if (nCnt >= 2)
			{
				str_src_path.Format(L"%s", pStr[1]);  //�迭 ó�� �����ִ�. // pStr[0]�� ��������. 1�����Ͱ� ����
				for (int i = 2; i < nCnt; i++) // command�� 2���� ���� ����
					list_find_command.AddTail((CString)pStr[i]);
			}
			else
			{
				AfxMessageBox(L"not enough argument");
				return 0;
			}
			*/

			str_src_path.Format(L"%s", pStr[1]);

			CString str_command_file("AT_Command.txt");
			if (FALSE == GetFindCommand(str_command_file, list_find_command))
			{
				AfxMessageBox(L"GetFindCommand failed");
				return 0;
			}
			
			// 1. �۾� ��� ���丮 ���� - �۾���� ���丮�� ����(��_��_��_�ú���)
			CString str_target_dir(str_src_path);
			str_target_dir += L"\\";
			str_target_dir += GetDateFolderName();

			if (!CreateDirectory(str_target_dir, NULL))
			{
				wprintf(L"CreateDirectory failed(%d)\n", GetLastError());
				return 1;
			}
			
			// 2. �۾� ��� ������ �����Ͽ�, �۾� ��� ���丮�� ������ ���� ������ �Ѵ�.
			// �̸�����(apk->zip) ����, ��������, dex -> smali, find command
			CList <myFileInfo, myFileInfo&> list_found_file;
			FindFile(str_src_path, CString(L"\\*.apk"), list_found_file);

			CList <CString, CString&> list_target_unzip_path;

			CString str_target_unzip_path;
			CString str_target_full_path;

			wprintf(L"\n\n####################################### Analyzing App(APK File) #######################################\n");

			POSITION pos = list_found_file.GetHeadPosition();
			for (int i = 0; i < list_found_file.GetCount(); i++)
			{
				myFileInfo st_file_info = list_found_file.GetNext(pos);

				str_target_full_path = str_target_dir;
				str_target_full_path += L"\\";
				str_target_full_path += st_file_info.str_file_title;
				str_target_unzip_path = str_target_full_path;
				str_target_full_path += L".zip";

				CString str_apk_file_name = str_target_unzip_path + CString(L".apk");
				wprintf(L"\n%d - App Name : %s\n", i+1, str_apk_file_name.GetBuffer(0));
				str_apk_file_name.ReleaseBuffer();

				// 2.1 �̸�����(apk->zip) ����
				CopyFileEx(st_file_info.str_file_full_path, str_target_full_path, NULL, NULL, NULL, COPY_FILE_FAIL_IF_EXISTS);
				
				// 2.2 ��������
				UnZipFile(str_target_full_path, str_target_unzip_path);

				// 2.3 dex -> smali
				ExtractSmali(str_target_unzip_path, str_src_path, L"baksmali-2.0.5.jar");

				list_target_unzip_path.AddTail(str_target_unzip_path);
			}

			// 2.5 find command
			CList <FindInfo, FindInfo&> list_found_info;
			pos = list_target_unzip_path.GetHeadPosition();

			for (int i = 0; i < list_target_unzip_path.GetCount(); i++)
			{
				str_target_unzip_path = list_target_unzip_path.GetNext(pos);

				POSITION pos_cmd = list_find_command.GetHeadPosition();
				for (int j = 0; j < list_find_command.GetCount(); j++)
				{
					CString str_find_command = list_find_command.GetNext(pos_cmd);
					FindKeyword(str_target_unzip_path, CString(L"\\*.*"), str_find_command, list_found_info);
					
					// ���� ������ ���� �ڼ��� �������
					PrintResult(str_find_command, str_target_unzip_path + CString(L".apk"), list_found_info);
				}
			}
			
			// �� �� ��� ���
			PrintResultSummary(list_target_unzip_path, list_found_info);
		
		}
    }
    else
    {
        wprintf(L"�ɰ��� ����: GetModuleHandle ����\n");
        nRetCode = 1;
    }

    return nRetCode;
}
