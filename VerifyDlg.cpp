// YzDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "BuyTicket.h"
#include "VerifyDlg.h"
#include "afxdialogex.h"
#include "Train.h"

// CVerifyDlg �Ի���
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
	ON_BN_CLICKED(IDC_BUTTON1, &CVerifyDlg::OnBnClickedButton1)
	ON_WM_PAINT()
	ON_STN_CLICKED(IDC_PIC2, &CVerifyDlg::OnStnClickedPic2)
END_MESSAGE_MAP()

// CVerifyDlg ��Ϣ�������

void CVerifyDlg::OnBnClickedButton1()
{
	CString code;
	GetDlgItem(IDC_EDIT_IMG2)->GetWindowText(code);
	yzcode=code.GetBuffer();
	if(yzcode=="")	{
		MessageBox("���벻��Ϊ��!");
	}	else	{
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
    file_path=m_pTrain->loadCode2();
	loadImg();
}

void CVerifyDlg::loadImg(void)
{
	CRect rect;
	GetDlgItem(IDC_PIC2)->GetClientRect(&rect);     //m_pictureΪPicture Control�ؼ���������ÿؼ����������
	CImage image;       //ʹ��ͼƬ��
	image.Load(file_path.c_str());   //װ��·����ͼƬ��Ϣ��ͼƬ��
    if(!image.IsNull()){
        CDC* pDC = GetDlgItem(IDC_PIC2)->GetWindowDC();    //�����ʾ�ؼ���DC
	    image.Draw( pDC -> m_hDC,rect);      //ͼƬ���ͼƬ����Draw����	
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
            OnBnClickedButton1();
            return TRUE;
        }
	}	
	return CDialogEx::PreTranslateMessage(pMsg);
}
