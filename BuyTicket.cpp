
// BuyTicket.cpp : 定义应用程序的类行为。
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


// CBuyTicketApp 构造

CBuyTicketApp::CBuyTicketApp()
{
	// 支持重新启动管理器
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_RESTART;

	// TODO: 在此处添加构造代码，
	// 将所有重要的初始化放置在 InitInstance 中
}


// 唯一的一个 CBuyTicketApp 对象

CBuyTicketApp theApp;


// CBuyTicketApp 初始化

BOOL CBuyTicketApp::InitInstance()
{
	// 如果一个运行在 Windows XP 上的应用程序清单指定要
	// 使用 ComCtl32.dll 版本 6 或更高版本来启用可视化方式，
	//则需要 InitCommonControlsEx()。否则，将无法创建窗口。
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// 将它设置为包括所有要在应用程序中使用的
	// 公共控件类。
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();


	AfxEnableControlContainer();

	// 创建 shell 管理器，以防对话框包含
	// 任何 shell 树视图控件或 shell 列表视图控件。
	CShellManager *pShellManager = new CShellManager;

	// 激活“Windows Native”视觉管理器，以便在 MFC 控件中启用主题
	CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows));

	// 标准初始化
	// 如果未使用这些功能并希望减小
	// 最终可执行文件的大小，则应移除下列
	// 不需要的特定初始化例程
	// 更改用于存储设置的注册表项
	// TODO: 应适当修改该字符串，
	// 例如修改为公司或组织名
	SetRegistryKey(_T("应用程序向导生成的本地应用程序"));

	CBuyTicketDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: 在此放置处理何时用
		//  “确定”来关闭对话框的代码
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: 在此放置处理何时用
		//  “取消”来关闭对话框的代码
	}
	else if (nResponse == -1)
	{
		TRACE(traceAppMsg, 0, "警告: 对话框创建失败，应用程序将意外终止。\n");
		TRACE(traceAppMsg, 0, "警告: 如果您在对话框上使用 MFC 控件，则无法 #define _AFX_NO_MFC_CONTROLS_IN_DIALOGS。\n");
	}

	// 删除上面创建的 shell 管理器。
	if (pShellManager != NULL)
	{
		delete pShellManager;
	}

	// 由于对话框已关闭，所以将返回 FALSE 以便退出应用程序，
	//  而不是启动应用程序的消息泵。
	return FALSE;
}

int GetEncoderClsid(const WCHAR* format, CLSID* pClsid)
{
	UINT  num = 0;          // number of image encoders
	UINT  size = 0;         // size of the image encoder array in bytes
	ImageCodecInfo* pImageCodecInfo = NULL;
	//2.获取GDI+支持的图像格式编码器种类数以及ImageCodecInfo数组的存放大小
	GetImageEncodersSize(&num, &size);
	if(size == 0)
		return -1;  // Failure
	//3.为ImageCodecInfo数组分配足额空间
	pImageCodecInfo = (ImageCodecInfo*)(malloc(size));
	if(pImageCodecInfo == NULL)
		return -1;  // Failure
	//4.获取所有的图像编码器信息
	GetImageEncoders(num, size, pImageCodecInfo);
	//5.查找符合的图像编码器的Clsid
	for(UINT j = 0; j < num; ++j)
	{
		if( wcscmp(pImageCodecInfo[j].MimeType, format) == 0 )
		{
			*pClsid = pImageCodecInfo[j].Clsid;
			free(pImageCodecInfo);
			return j;  // Success
		}    
	}
	//6.释放步骤3分配的内存
	free(pImageCodecInfo);
	return -1;  // Failure
}

int ImgConvert(LPCSTR iFName,LPCSTR oFName,LPCSTR sType)
{
	GdiplusStartupInput gdiplusStartupInput;
	ULONG_PTR gdiplusToken;
	//1.初始化GDI+，以便后续的GDI+函数可以成功调用
	GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
	CLSID   encoderClsid;
	Status  stat;
	USES_CONVERSION; 
	//7.创建Image对象并加载图片
	Image* image = Image::FromFile(A2W(iFName));//new Image(A2W(iFName)); 
	// Get the CLSID of the PNG encoder.
	GetEncoderClsid(A2W(sType)/*_T("image/png")*/, &encoderClsid);
	//8.调用Image.Save方法进行图片格式转换，并把步骤3)得到的图像编码器Clsid传递给它
	stat = image->Save(A2W(oFName), &encoderClsid, NULL);
	if(stat == Ok)
		printf("Bird.png was saved successfully/n");
	else
		printf("Failure: stat = %d/n", stat); 
	//9.释放Image对象
	delete image;
	//10.清理所有GDI+资源
	GdiplusShutdown(gdiplusToken);
	return 0;
}
