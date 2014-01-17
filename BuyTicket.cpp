
// BuyTicket.cpp : ����Ӧ�ó��������Ϊ��
//

#include "stdafx.h"
#include "BuyTicket.h"
#include <string>
#include "BuyTicketDlg.h"
#include <windows.h> 
#include <gdiplus.h>
#include <stdio.h>
using namespace Gdiplus;
using namespace std;

#pragma comment(lib,"gdiplus")

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CBuyTicketApp

BEGIN_MESSAGE_MAP(CBuyTicketApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CBuyTicketApp ����

CBuyTicketApp::CBuyTicketApp()
{
	// ֧����������������
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_RESTART;

	// TODO: �ڴ˴���ӹ�����룬
	// ��������Ҫ�ĳ�ʼ�������� InitInstance ��
}


// Ψһ��һ�� CBuyTicketApp ����

CBuyTicketApp theApp;


// CBuyTicketApp ��ʼ��

BOOL CBuyTicketApp::InitInstance()
{
	// ���һ�������� Windows XP �ϵ�Ӧ�ó����嵥ָ��Ҫ
	// ʹ�� ComCtl32.dll �汾 6 ����߰汾�����ÿ��ӻ���ʽ��
	//����Ҫ InitCommonControlsEx()�����򣬽��޷��������ڡ�
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// ��������Ϊ��������Ҫ��Ӧ�ó�����ʹ�õ�
	// �����ؼ��ࡣ
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();


	AfxEnableControlContainer();

	// ���� shell ���������Է��Ի������
	// �κ� shell ����ͼ�ؼ��� shell �б���ͼ�ؼ���
	CShellManager *pShellManager = new CShellManager;

	// ���Windows Native���Ӿ����������Ա��� MFC �ؼ�����������
	CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows));

	// ��׼��ʼ��
	// ���δʹ����Щ���ܲ�ϣ����С
	// ���տ�ִ���ļ��Ĵ�С����Ӧ�Ƴ�����
	// ����Ҫ���ض���ʼ������
	// �������ڴ洢���õ�ע�����
	// TODO: Ӧ�ʵ��޸ĸ��ַ�����
	// �����޸�Ϊ��˾����֯��
	SetRegistryKey(_T("Ӧ�ó��������ɵı���Ӧ�ó���"));

	CBuyTicketDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: �ڴ˷��ô����ʱ��
		//  ��ȷ�������رնԻ���Ĵ���
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: �ڴ˷��ô����ʱ��
		//  ��ȡ�������رնԻ���Ĵ���
	}
	else if (nResponse == -1)
	{
		TRACE(traceAppMsg, 0, "����: �Ի��򴴽�ʧ�ܣ�Ӧ�ó���������ֹ��\n");
		TRACE(traceAppMsg, 0, "����: ������ڶԻ�����ʹ�� MFC �ؼ������޷� #define _AFX_NO_MFC_CONTROLS_IN_DIALOGS��\n");
	}

	// ɾ�����洴���� shell ��������
	if (pShellManager != NULL)
	{
		delete pShellManager;
	}

	// ���ڶԻ����ѹرգ����Խ����� FALSE �Ա��˳�Ӧ�ó���
	//  ����������Ӧ�ó������Ϣ�á�
	return FALSE;
}

int GetEncoderClsid(const WCHAR* format, CLSID* pClsid)
{
	UINT  num = 0;          // number of image encoders
	UINT  size = 0;         // size of the image encoder array in bytes
	ImageCodecInfo* pImageCodecInfo = NULL;
	//2.��ȡGDI+֧�ֵ�ͼ���ʽ�������������Լ�ImageCodecInfo����Ĵ�Ŵ�С
	GetImageEncodersSize(&num, &size);
	if(size == 0)
		return -1;  // Failure
	//3.ΪImageCodecInfo����������ռ�
	pImageCodecInfo = (ImageCodecInfo*)(malloc(size));
	if(pImageCodecInfo == NULL)
		return -1;  // Failure
	//4.��ȡ���е�ͼ���������Ϣ
	GetImageEncoders(num, size, pImageCodecInfo);
	//5.���ҷ��ϵ�ͼ���������Clsid
	for(UINT j = 0; j < num; ++j)
	{
		if( wcscmp(pImageCodecInfo[j].MimeType, format) == 0 )
		{
			*pClsid = pImageCodecInfo[j].Clsid;
			free(pImageCodecInfo);
			return j;  // Success
		}    
	}
	//6.�ͷŲ���3������ڴ�
	free(pImageCodecInfo);
	return -1;  // Failure
}

int ImgConvert(LPCSTR iFName,LPCSTR oFName,LPCSTR sType)
{
	GdiplusStartupInput gdiplusStartupInput;
	ULONG_PTR gdiplusToken;
	//1.��ʼ��GDI+���Ա������GDI+�������Գɹ�����
	GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
	CLSID   encoderClsid;
	Status  stat;
	USES_CONVERSION; 
	//7.����Image���󲢼���ͼƬ
	Image* image = Image::FromFile(A2W(iFName));//new Image(A2W(iFName)); 
	// Get the CLSID of the PNG encoder.
	GetEncoderClsid(A2W(sType)/*_T("image/png")*/, &encoderClsid);
	//8.����Image.Save��������ͼƬ��ʽת�������Ѳ���3)�õ���ͼ�������Clsid���ݸ���
	stat = image->Save(A2W(oFName), &encoderClsid, NULL);
	if(stat == Ok)
		printf("Bird.png was saved successfully/n");
	else
		printf("Failure: stat = %d/n", stat); 
	//9.�ͷ�Image����
	delete image;
	//10.��������GDI+��Դ
	GdiplusShutdown(gdiplusToken);
	return 0;
}
