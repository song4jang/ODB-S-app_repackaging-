

#include "stdafx.h"
#include "my_zip.h"

void CreateEmptyZipFile(CString strPath)
{
	BYTE startBuffer[] = { 80, 75, 5, 6, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
	FILE *f = NULL;
	_wfopen_s(&f, strPath.GetBuffer(strPath.GetLength()), _T("wb"));
	if (f)
	{
		strPath.ReleaseBuffer();
		fwrite(startBuffer, sizeof(startBuffer), 1, f);
		fclose(f);
	}
}

void ZipFile(CString strSrc, CString strDest)
{
	//Create an empty zip file
	CreateEmptyZipFile(strDest);

	BSTR bstrSource = strSrc.AllocSysString();
	BSTR bstrDest = strDest.AllocSysString();

	HRESULT hResult = S_FALSE;
	IShellDispatch *pIShellDispatch = NULL;
	Folder *pToFolder = NULL;
	VARIANT variantDir, variantFile, variantOpt;

	CoInitialize(NULL);

	hResult = CoCreateInstance(CLSID_Shell, NULL, CLSCTX_INPROC_SERVER,
		IID_IShellDispatch, (void **)&pIShellDispatch);

	if (SUCCEEDED(hResult))
	{
		VariantInit(&variantDir);
		variantDir.vt = VT_BSTR;
		variantDir.bstrVal = bstrDest;
		hResult = pIShellDispatch->NameSpace(variantDir, &pToFolder);

		if (SUCCEEDED(hResult))
		{
			VariantInit(&variantFile);
			variantFile.vt = VT_BSTR;
			variantFile.bstrVal = bstrSource;

			VariantInit(&variantOpt);
			variantOpt.vt = VT_I4;
			variantOpt.lVal = FOF_NO_UI;

			hResult = pToFolder->CopyHere(variantFile, variantOpt);
			Sleep(1000);
			pToFolder->Release();
		}

		pIShellDispatch->Release();
	}

	CoUninitialize();
}

BOOL UnZipFile(CString strSrc, CString strDest)
{
	if (0 == CreateDirectory(strDest, NULL))
		return FALSE;

	BSTR source = strSrc.AllocSysString();
	BSTR dest = strDest.AllocSysString();

	HRESULT hResult = S_FALSE;
	IShellDispatch *pIShellDispatch = NULL;
	Folder *pToFolder = NULL;
	VARIANT variantDir, variantFile, variantOpt;

	CoInitialize(NULL);

	hResult = CoCreateInstance(CLSID_Shell, NULL, CLSCTX_INPROC_SERVER, IID_IShellDispatch, (void **)&pIShellDispatch);
	if (SUCCEEDED(hResult))
	{
		VariantInit(&variantDir);
		variantDir.vt = VT_BSTR;
		variantDir.bstrVal = dest;
		hResult = pIShellDispatch->NameSpace(variantDir, &pToFolder);

		if (SUCCEEDED(hResult))
		{
			Folder *pFromFolder = NULL;
			VariantInit(&variantFile);
			variantFile.vt = VT_BSTR;
			variantFile.bstrVal = source;
			pIShellDispatch->NameSpace(variantFile, &pFromFolder);

			FolderItems *fi = NULL;
			pFromFolder->Items(&fi);

			VariantInit(&variantOpt);
			variantOpt.vt = VT_I4;
			variantOpt.lVal = FOF_NO_UI;

			VARIANT newV;
			VariantInit(&newV);
			newV.vt = VT_DISPATCH;
			newV.pdispVal = fi;
			hResult = pToFolder->CopyHere(newV, variantOpt);
			//	Sleep(1000);
			pFromFolder->Release();
			pToFolder->Release();
		}
		pIShellDispatch->Release();
	}
	else
	{
		return FALSE;
	}

	CoUninitialize();

	return TRUE;
}