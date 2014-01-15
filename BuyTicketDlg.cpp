
// BuyTicketDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "BuyTicket.h"
#include "BuyTicketDlg.h"
#include "afxdialogex.h"
#include "Train.h"
#include "VerifyDlg.h"
#include <iostream>
#include <fstream>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CBuyTicketDlg �Ի���

CTrain *pTrain;

CBuyTicketDlg::CBuyTicketDlg(CWnd* pParent /*=NULL*/)
    : CDialogEx(CBuyTicketDlg::IDD, pParent)
{
    m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CBuyTicketDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_LIST1, m_listbox);
}

BEGIN_MESSAGE_MAP(CBuyTicketDlg, CDialogEx)
    ON_WM_PAINT()
    ON_WM_QUERYDRAGICON()
    ON_BN_CLICKED(IDC_BUTTON1, &CBuyTicketDlg::OnBnClickedButton1)
    ON_BN_CLICKED(IDC_BUTTON2, &CBuyTicketDlg::OnBnClickedButton2)
    ON_WM_TIMER()
    ON_BN_CLICKED(IDC_OPEN_URL, &CBuyTicketDlg::OnBnClickedOpenUrl)
    ON_STN_CLICKED(IDC_PIC, &CBuyTicketDlg::OnStnClickedPic)
    ON_BN_CLICKED(IDC_BLOG, &CBuyTicketDlg::OnBnClickedBlog)
	ON_BN_CLICKED(IDC_BTN_STOP, &CBuyTicketDlg::OnBnClickedBtnStop)
END_MESSAGE_MAP()


// CBuyTicketDlg ��Ϣ�������

BOOL CBuyTicketDlg::OnInitDialog()
{
    CDialogEx::OnInitDialog();

    // ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
    //  ִ�д˲���
    SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
    SetIcon(m_hIcon, FALSE);		// ����Сͼ��

    pTrain=new CTrain(this);
    // TODO: �ڴ���Ӷ���ĳ�ʼ������
    if (pTrain->GetCode())
        LoadYzCode();

	if(access(CONFIG_PATH , 0) != 0) {
		CreateDirectory(CONFIG_PATH,NULL);
	}

    ifstream userfile(CONFIG_PATH"\\buyticket.dat");
    string  uname,upass,time,fromStation,toStation,date,specialTrain;
	string name[MAX_PASSANGER],id[MAX_PASSANGER],phone[MAX_PASSANGER];

    if(userfile.is_open())  {
        getline(userfile,uname);
        getline(userfile,upass);
        getline(userfile,time);
        getline(userfile,fromStation);
        getline(userfile,toStation);
        getline(userfile,date);
        getline(userfile,specialTrain);

        GetDlgItem(IDC_EDIT_DOMAIN)->SetWindowText("kyfw.12306.cn");
        GetDlgItem(IDC_UNAME)->SetWindowText(uname.c_str());
        GetDlgItem(IDC_UPASS)->SetWindowText(upass.c_str());

        GetDlgItem(IDC_FROMCITY)->SetWindowText(fromStation.c_str());
        GetDlgItem(IDC_TOCITY)->SetWindowText(toStation.c_str());
        GetDlgItem(IDC_DATE)->SetWindowText(date.c_str());

        GetDlgItem(IDC_TRAIN)->SetWindowText(specialTrain.c_str());

		for (int i=0;i<MAX_PASSANGER;i++)	{
			getline(userfile,name[i]);
			getline(userfile,id[i]);
			getline(userfile,phone[i]);
			GetDlgItem(IDC_FULLNAME+i)->SetWindowText(name[i].c_str());
			GetDlgItem(IDC_IDCARD+i)->SetWindowText(id[i].c_str());
			GetDlgItem(IDC_PHONE+i)->SetWindowText(phone[i].c_str());
		}
	}

	userfile.close();

    if(time=="") time="5";
    GetDlgItem(IDC_EDIT_TIME)->SetWindowText(time.c_str());

    ((CButton*)GetDlgItem(IDC_CHECK_YW))->SetCheck(TRUE);
    ((CButton*)GetDlgItem(IDC_CHECK_EDZ))->SetCheck(TRUE);

    return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CBuyTicketDlg::OnPaint()
{
    LoadYzCode();
    if (IsIconic()) {
        CPaintDC dc(this); // ���ڻ��Ƶ��豸������
        SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);
        // ʹͼ���ڹ����������о���
        int cxIcon = GetSystemMetrics(SM_CXICON);
        int cyIcon = GetSystemMetrics(SM_CYICON);
        CRect rect;
        GetClientRect(&rect);
        int x = (rect.Width() - cxIcon + 1) / 2;
        int y = (rect.Height() - cyIcon + 1) / 2;

        // ����ͼ��
        dc.DrawIcon(x, y, m_hIcon);
    } else {
        CDialogEx::OnPaint();
    }
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CBuyTicketDlg::OnQueryDragIcon()
{
    return static_cast<HCURSOR>(m_hIcon);
}

void CBuyTicketDlg::OnBnClickedButton1()
{
    CString code,name,pass;
    GetDlgItem(IDC_CODE)->GetWindowText(code);
    GetDlgItem(IDC_UNAME)->GetWindowText(name);
    GetDlgItem(IDC_UPASS)->GetWindowText(pass);
    string yzcode=code.GetBuffer();
    pTrain->Login(name.GetBuffer(),pass.GetBuffer(),yzcode);
}

bool CBuyTicketDlg::LoadYzCode(void)
{
    CRect rect;
    GetDlgItem(IDC_PIC)->GetClientRect(&rect);     //m_pictureΪPicture Control�ؼ���������ÿؼ����������
    CImage image;       //ʹ��ͼƬ��
    image.Load(CONFIG_PATH"\\buyticket.png");   //װ��·����ͼƬ��Ϣ��ͼƬ��
    if(!image.IsNull()) {
        CDC* pDC = GetDlgItem(IDC_PIC)->GetWindowDC();    //�����ʾ�ؼ���DC
        image.Draw( pDC -> m_hDC,rect);      //ͼƬ���ͼƬ����Draw����
        ReleaseDC(pDC);
    }
    return true;
}

void CBuyTicketDlg::OnBnClickedButton2()
{
    CString time;
    GetDlgItem(IDC_EDIT_TIME)->GetWindowText(time);
    int ntime=atof(time)*100;
    pTrain->m_isInBuy=false;
    pTrain->SerachTicketPage();
    KillTimer(1);
    SetTimer(1,ntime,NULL);
    ofstream userfile(CONFIG_PATH"\\buyticket.dat");
    CString uname,upass,fromStation,toStation,date,specialTrain;
    CString name[MAX_PASSANGER],id[MAX_PASSANGER],phone[MAX_PASSANGER];
    GetDlgItem(IDC_UNAME)->GetWindowText(uname);
    GetDlgItem(IDC_UPASS)->GetWindowText(upass);
    GetDlgItem(IDC_FROMCITY)->GetWindowText(fromStation);
    GetDlgItem(IDC_TOCITY)->GetWindowText(toStation);
    GetDlgItem(IDC_DATE)->GetWindowText(date);
    GetDlgItem(IDC_TRAIN)->GetWindowText(specialTrain);

	userfile<<uname.GetBuffer()<<"\n";
    userfile<<upass.GetBuffer()<<"\n";
    userfile<<time.GetBuffer()<<"\n";
    userfile<<fromStation.GetBuffer()<<"\n";
    userfile<<toStation.GetBuffer()<<"\n";
    userfile<<date.GetBuffer()<<"\n";
    userfile<<specialTrain.GetBuffer()<<"\n";

	for (int i=0;i<MAX_PASSANGER;i++)	{
		GetDlgItem(IDC_FULLNAME+i)->GetWindowText(name[i]);
		GetDlgItem(IDC_IDCARD+i)->GetWindowText(id[i]);
		GetDlgItem(IDC_PHONE+i)->GetWindowText(phone[i]);
		userfile<<name[i].GetBuffer()<<"\n";
		userfile<<id[i].GetBuffer()<<"\n";
		userfile<<phone[i].GetBuffer()<<"\n";
	}
    userfile.close();
}


void CBuyTicketDlg::OnTimer(UINT_PTR nIDEvent)
{
    // TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
    CString fromcity,tocity,date,train;
    GetDlgItem(IDC_FROMCITY)->GetWindowText(fromcity);
    GetDlgItem(IDC_TOCITY)->GetWindowText(tocity);
    GetDlgItem(IDC_DATE)->GetWindowText(date);
    GetDlgItem(IDC_TRAIN)->GetWindowText(train);
    if(!pTrain->m_isInBuy) {
        pTrain->m_strain=train.GetBuffer();
        pTrain->SearchTicket(fromcity.GetBuffer(),tocity.GetBuffer(),date.GetBuffer());
    }

    CDialogEx::OnTimer(nIDEvent);
}


void CBuyTicketDlg::OnBnClickedOpenUrl()
{
    ShellExecute(NULL, _T("open"), _T("iexplore"), 
		_T("https://kyfw.12306.cn/otn/index/initMy12306"), 
		NULL, SW_SHOWNORMAL);
}

void CBuyTicketDlg::OnStnClickedPic()
{
    pTrain->GetCode();
    LoadYzCode();
}

void CBuyTicketDlg::OnBnClickedBlog()
{
    ShellExecute(NULL, _T("open"), 
		_T("http://www.xiaoqin.in/index.php?a=details&aid=110"),
		NULL, NULL, SW_SHOWNORMAL);
}

void CBuyTicketDlg::OnOK()
{
    // ���λس��ر�
    //CDialogEx::OnOK();
}

void CBuyTicketDlg::OnCancel()
{
    CDialogEx::OnCancel();
}

BOOL CBuyTicketDlg::PreTranslateMessage(MSG* pMsg)
{
    if(pMsg->message == WM_KEYDOWN) {
        switch(pMsg->wParam) {
        case VK_ESCAPE: //ESC
            return TRUE;
        }
    }
    return CDialogEx::PreTranslateMessage(pMsg);
}


void CBuyTicketDlg::OnBnClickedBtnStop()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	KillTimer(1);
}
