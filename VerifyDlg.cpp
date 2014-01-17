// YzDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "BuyTicket.h"
#include "VerifyDlg.h"
#include "afxdialogex.h"
#include "Train.h"

// CVerifyDlg 对话框
IMPLEMENT_DYNAMIC(CVerifyDlg, CDialogEx)

CVerifyDlg::CVerifyDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CVerifyDlg::IDD, pParent)
{

}

CVerifyDlg::~CVerifyDlg()
{
}

void CVerifyDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CVerifyDlg, CDialogEx)
	ON_BN_CLICKED(IDC_BTN_OK, &CVerifyDlg::OnBnClickedOK)
	ON_WM_PAINT()
	ON_STN_CLICKED(IDC_PIC2, &CVerifyDlg::OnStnClickedPic2)
END_MESSAGE_MAP()

// CVerifyDlg 消息处理程序

void CVerifyDlg::OnBnClickedOK()
{
	CString code;
	GetDlgItem(IDC_EDIT_IMG2)->GetWindowText(code);
	m_sVercode=code.GetBuffer();
	if(m_sVercode=="")	{
		MessageBox("输入不能为空!");
	} else {
		OnOK();
	}
}

BOOL CVerifyDlg::OnInitDialog()
{
	return CDialogEx::OnInitDialog();
}

void CVerifyDlg::OnPaint()
{
	loadImg();
	CDialogEx::OnPaint();
}

void CVerifyDlg::OnStnClickedPic2()
{
    m_spath=m_pTrain->loadCode2();
	loadImg();
}

void CVerifyDlg::loadImg(void)
{
	CRect rect;
	GetDlgItem(IDC_PIC2)->GetClientRect(&rect);     //m_picture为Picture Control控件变量，获得控件的区域对象
	CImage image;       //使用图片类
	image.Load(m_spath.c_str());   //装载路径下图片信息到图片类
    if(!image.IsNull()){
        CDC* pDC = GetDlgItem(IDC_PIC2)->GetWindowDC();    //获得显示控件的DC
	    image.Draw( pDC -> m_hDC,rect);      //图片类的图片绘制Draw函数	
	    ReleaseDC(pDC);
    }	
}


BOOL CVerifyDlg::PreTranslateMessage(MSG* pMsg)
{
	if(pMsg -> message == WM_KEYDOWN)
	{
		CEdit* pEdit = (CEdit*)GetDlgItem(IDC_EDIT_IMG2);
        ASSERT(pEdit);
        if(pMsg->hwnd == pEdit->GetSafeHwnd() && VK_RETURN == pMsg->wParam)
        {
            OnBnClickedOK();
            return TRUE;
        }
	}	
	return CDialogEx::PreTranslateMessage(pMsg);
}
