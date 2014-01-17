
// BuyTicketDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "BuyTicket.h"
#include "Train.h"
#include "BuyTicketDlg.h"
#include "afxdialogex.h"
#include "VerifyDlg.h"
#include <iostream>
#include <fstream>
#include "Ticket.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CBuyTicketDlg 对话框

CTrain *pTrain;

void TreeVisit(CTreeCtrl& tree, HTREEITEM hItem)
{  
	//DoWithItem(hItem); //访问当前节点  
	if (tree.ItemHasChildren(hItem))  
	{
		HTREEITEM hChildItem = tree.GetChildItem(hItem);  
		while(hChildItem!=NULL)  
		{
			TreeVisit(tree, hChildItem);     //递归遍历孩子节点  
			hChildItem = tree.GetNextItem(hChildItem, TVGN_NEXT);  
		}  
	}  
}

CBuyTicketDlg::CBuyTicketDlg(CWnd* pParent /*=NULL*/)
    : CDialogEx(CBuyTicketDlg::IDD, pParent)
{
    m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CBuyTicketDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_MSG, m_listbox);
	DDX_Control(pDX, IDC_LIST_PASS, m_Passlist);
	DDX_Control(pDX, IDC_LIST_TICKET, m_ListTik);
}

BEGIN_MESSAGE_MAP(CBuyTicketDlg, CDialogEx)
    ON_WM_PAINT()
    ON_WM_QUERYDRAGICON()
    ON_BN_CLICKED(IDC_BTN_LOGIN, &CBuyTicketDlg::OnBnClickedLogin)
    ON_BN_CLICKED(IDC_BTN_QUERY, &CBuyTicketDlg::OnBnClickedQuery)
    ON_WM_TIMER()
    ON_BN_CLICKED(IDC_OPEN_URL, &CBuyTicketDlg::OnBnClickedOpenUrl)
    ON_STN_CLICKED(IDC_PIC, &CBuyTicketDlg::OnStnClickedPic)
    ON_BN_CLICKED(IDC_BLOG, &CBuyTicketDlg::OnBnClickedBlog)
	ON_BN_CLICKED(IDC_BTN_STOP, &CBuyTicketDlg::OnBnClickedBtnStop)
	ON_BN_CLICKED(IDC_CHK_VPASS, &CBuyTicketDlg::OnBnClickedChkVpass)
	ON_NOTIFY(LVN_BEGINLABELEDIT, IDC_LIST_PASS, &CBuyTicketDlg::OnLvnBeginlabeleditListPass)
	ON_NOTIFY(LVN_ENDLABELEDIT, IDC_LIST_PASS, &CBuyTicketDlg::OnLvnEndlabeleditListPass)

	ON_MESSAGE(WM_QUICKLIST_GETLISTITEMDATA, OnGetListItem)
	ON_MESSAGE(WM_QUICKLIST_CLICK, OnListClick)

	ON_NOTIFY(NM_CLICK, IDC_LIST_PASS, &CBuyTicketDlg::OnNMClickListPass)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_PASS, &CBuyTicketDlg::OnNMDblclkListPass)
	ON_BN_CLICKED(IDC_BTN_CMSG, &CBuyTicketDlg::OnBnClickedBtnCmsg)
END_MESSAGE_MAP()

//User clicked on list. Toggle checkbox/image if hit
LRESULT CBuyTicketDlg::OnListClick(WPARAM wParam, LPARAM lParam)
{
	//Make sure message comes from list box
// 	ASSERT( (HWND)wParam == m_Passlist.GetSafeHwnd() );
	if ((HWND)wParam == m_Passlist.GetSafeHwnd())	{
		CQuickList::CListHitInfo *hit= (CQuickList::CListHitInfo*) lParam;
		TRACE(_T("User hit item %d, subitem %d "), hit->m_item, hit->m_subitem);
		if(hit->m_onButton) {
			TRACE(_T("On check box. "));
			if (hit->m_subitem==0)	{
				pTrain->m_vPassanger[hit->m_item].m_check = !pTrain->m_vPassanger[hit->m_item].m_check;
				m_Passlist.RedrawCheckBoxs(hit->m_item, hit->m_item, hit->m_subitem);
			}
		} 
// 	else if(hit->m_onImage) {
// 		TRACE(_T("On image. "));
// 		pTrain->m_vPassanger[hit->m_item].m_image[hit->m_subitem] = (pTrain->m_vPassanger[hit->m_item].m_image[hit->m_subitem]+1)%IMAGECOUNT;
// 		m_Passlist.RedrawImages(hit->m_item, hit->m_item, hit->m_subitem);
// 	}
	}
	if ((HWND)wParam == m_ListTik.GetSafeHwnd())	{
		CQuickList::CListHitInfo *hit= (CQuickList::CListHitInfo*) lParam;
		TRACE(_T("User hit item %d, subitem %d "), hit->m_item, hit->m_subitem);
		if(hit->m_onButton) {
			TRACE(_T("On check box. "));
			if (hit->m_subitem>3)	{
				pTrain->m_vTicket[hit->m_item].m_bCheck[hit->m_subitem-4] = 
					!pTrain->m_vTicket[hit->m_item].m_bCheck[hit->m_subitem-4];
				m_ListTik.RedrawCheckBoxs(hit->m_item, hit->m_item, hit->m_subitem);
			}
		} 

	}
	TRACE(_T("\n"));
	return 0;
}

//Get item data
LRESULT CBuyTicketDlg::OnGetListItem(WPARAM wParam, LPARAM lParam)
{
	//wParam is a handler to the list
	//Make sure message comes from list box
// 	ASSERT( (HWND)wParam == m_Passlist.GetSafeHwnd() );
	if ((HWND)wParam == m_Passlist.GetSafeHwnd())	{
		//lParam is a pointer to the data that is needed for the element
		CQuickList::CListItemData* data = (CQuickList::CListItemData*) lParam;
		//Get which item and subitem that is asked for.
		int item = data->GetItem();
		int subItem = data->GetSubItem();
		CPassenger& sampledata = pTrain->m_vPassanger[item];
		UpdateData();
		// 	BOOL noSel[3] = { m_noSel1, m_noSel2, m_noSel3 };

		data->m_noSelection = (/*noSel[*/subItem/*]*/ == 0);

#ifndef QUICKLIST_NOEDIT
		BOOL m_fullrow = TRUE;
		if(subItem < 2) {
			//Only column one is editable if not full row select
			if(m_fullrow || subItem == 0)
				data->m_allowEdit = true;
		}
#endif

#ifndef QUICKLIST_NOCOLORS
		if(m_winColors==0) {
			if(data->GetItem()%2 == 0) {
				data->m_colors.m_textColor =			RGB( 10, 5, 20);
				data->m_colors.m_backColor =			RGB( 180, 180, 255);
#ifndef QUICKLIST_NONAVIGATION
				data->m_colors.m_navigatedTextColor =	RGB( 200, 220, 230);
				data->m_colors.m_navigatedBackColor =	RGB( 140, 60, 70);
#endif

				data->m_colors.m_hotTextColor =			RGB( 170, 10, 150);

				data->m_colors.m_selectedBackColorNoFocus = RGB( 20, 20, 120);
				data->m_colors.m_selectedBackColor =	RGB( 30, 0, 96);
				data->m_colors.m_selectedTextColor =	RGB( 240, 200, 128);
			} else {
				data->m_colors.m_textColor =			RGB( 40, 10, 40);
				data->m_colors.m_backColor =			RGB( 220, 220, 255);
#ifndef QUICKLIST_NONAVIGATION
				data->m_colors.m_navigatedTextColor =	RGB( 200, 200, 230);
				data->m_colors.m_navigatedBackColor =	RGB( 160, 70, 70);
#endif

				data->m_colors.m_hotTextColor =			RGB( 170, 10, 150);
				data->m_colors.m_selectedBackColorNoFocus = RGB( 20, 60, 120);
				data->m_colors.m_selectedBackColor =	RGB( 30, 50, 96);
				data->m_colors.m_selectedTextColor =	RGB( 240, 250, 128);
			}
		}
#endif

#ifndef QUICKLIST_NOTEXTSTYLE
		if(m_normalStyle==0) {
			data->m_textStyle.m_bold = sampledata.m_bold[subItem];
			data->m_textStyle.m_italic = sampledata.m_italic[subItem];
		}
#endif

#ifndef QUICKLIST_NOIMAGE
		BOOL drawImage[3] = { m_image1, m_image2, m_image3};

		if(drawImage[subItem] ) {
			//data->m_image.m_imageList = &m_imgList;
			data->m_image.m_imageID	  = sampledata.m_image[subItem];
			if(subItem==1)
				data->m_image.m_imageList = &m_imgList;
			if(subItem==2)
				data->m_image.m_imageList = &m_imgLargeList;

			data->m_image.m_noSelection = (m_noImgSel!=0);
		}
#endif

#ifndef QUICKLIST_NOBUTTON
		// 	BOOL drawCheck[3] = { m_check1, m_check2, m_check3 };
		if(/*drawCheck[subItem]*/subItem==0) {
			int style[] = {/*DFCS_BUTTONRADIO,*/ DFCS_BUTTONCHECK, DFCS_BUTTONPUSH};
			data->m_button.m_draw = true;
			data->m_button.m_noSelection = false/*(m_noCheckSel != 0)*/;
			data->m_button.m_style = style[subItem];
			if(	sampledata.m_check )
				data->m_button.m_style |= DFCS_CHECKED;
		}
#endif
		if(data->GetSubItem() == 0) {
			data->m_text.Format(_T("%d %s"), item+1, sampledata.m_sname.c_str());
		} else if(data->GetSubItem() == 1) {
			data->m_text.Format(_T("%s"), sampledata.m_sidcard.c_str());
		} else {
			data->m_text.Format(_T("%s"), sampledata.m_sphone.c_str());
		}

#ifndef QUICKLIST_NOTOOLTIP
		//Tool tip text
		data->m_tooltip = CString(_T("Tooltip: ")) + data->m_text;
#endif

#ifndef QUICKLIST_NOPROGRESSBAR
		if(subItem == 2 && m_progress) {
			UINT edgestyle[] = {EDGE_BUMP, EDGE_ETCHED, EDGE_RAISED, EDGE_SUNKEN};
			data->m_progressBar.m_maxvalue = 100;;
			data->m_progressBar.m_value    = sampledata.m_result;
			if(m_progBorder != 0)
				data->m_progressBar.m_edge	   = edgestyle[m_progBorder-1];
			else
				data->m_progressBar.m_edge	   = 0;
			if(m_winColors==0)
				data->m_progressBar.m_fillColor = RGB(   40,
				60-30*(data->GetItem()%3),
				30 );
		}
#endif
	}

	if ((HWND)wParam == m_ListTik.GetSafeHwnd())	{
		//lParam is a pointer to the data that is needed for the element
		CQuickList::CListItemData* data = (CQuickList::CListItemData*) lParam;
		//Get which item and subitem that is asked for.
		int item = data->GetItem();
		int subItem = data->GetSubItem();
		UpdateData();
		if (pTrain->m_vTicket.size()>item)	{
			CTicket& pTdata = /*pTrain->m_pvBlist[item]*/pTrain->m_vTicket[item];
#ifndef QUICKLIST_NOBUTTON
			if(subItem>3) {
				int style= DFCS_BUTTONCHECK;
				data->m_button.m_draw = true;
				data->m_button.m_noSelection = false/*(m_noCheckSel != 0)*/;
				data->m_button.m_style = style;
				if(	pTdata.m_bCheck[subItem-4] )
					data->m_button.m_style |= DFCS_CHECKED;
			}
#endif
			switch (subItem) {
			case 0 :
				data->m_text.Format(_T("%d %s"), item+1, pTdata.station_train_code.c_str());
				break;
			case 1 :
				data->m_text.Format(_T("%s"), pTdata.start_time.c_str());
				break;
			case 2 :
				data->m_text.Format(_T("%s"), pTdata.arrive_time.c_str());
				break;
			case 3 :
				data->m_text.Format(_T("%s"), pTdata.lishi.c_str());
				break;
			default:
				data->m_text.Format(_T("%s"), pTdata.m_seat[subItem-4].c_str());
				break;
			}
		}
	}
	return 0;
}

// CBuyTicketDlg 消息处理程序
void CBuyTicketDlg::UpdatePassList()
{
	m_Passlist.LockWindowUpdate();
	m_Passlist.SetItemCount( (int) pTrain->m_vPassanger.size() );
	m_Passlist.UnlockWindowUpdate();
	m_Passlist.RedrawItems(m_Passlist.GetTopIndex(),
		m_Passlist.GetTopIndex()+m_Passlist.GetCountPerPage());
}
void CBuyTicketDlg::UpdateTicketList()
{
	m_ListTik.LockWindowUpdate();
	m_ListTik.SetItemCount( (int) pTrain->m_pvBlist->size() );
	m_ListTik.UnlockWindowUpdate();
	m_ListTik.RedrawItems(m_ListTik.GetTopIndex(),
		m_ListTik.GetTopIndex()+m_ListTik.GetCountPerPage());
}

BOOL CBuyTicketDlg::OnInitDialog()
{
    CDialogEx::OnInitDialog();

    // 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
    //  执行此操作
    SetIcon(m_hIcon, TRUE);			// 设置大图标
    SetIcon(m_hIcon, FALSE);		// 设置小图标

    pTrain=new CTrain(this);
    // TODO: 在此添加额外的初始化代码
    if (pTrain->GetCode())
        LoadYzCode();

	if(access(CONFIG_PATH , 0) != 0) {
		CreateDirectory(CONFIG_PATH,NULL);
	}

// 	CTreeCtrl tree;
// 	HTREEITEM hRoot = tree.GetRootItem();
// 	TreeVisit(tree, hRoot);

	ListView_SetExtendedListViewStyleEx(m_Passlist.m_hWnd, LVS_EX_FULLROWSELECT, LVS_EX_FULLROWSELECT);
	ListView_SetExtendedListViewStyleEx(m_Passlist.m_hWnd, LVS_EX_SUBITEMIMAGES , LVS_EX_SUBITEMIMAGES );
	//ListView_SetExtendedListViewStyleEx(m_list.m_hWnd, LVS_EX_TRACKSELECT, LVS_EX_TRACKSELECT );
	ListView_SetExtendedListViewStyleEx(m_Passlist.m_hWnd, LVS_EX_CHECKBOXES, LVS_EX_CHECKBOXES );
	ListView_SetExtendedListViewStyleEx(m_Passlist.m_hWnd, LVS_EX_HEADERDRAGDROP, LVS_EX_HEADERDRAGDROP);
	ListView_SetExtendedListViewStyleEx(m_Passlist.m_hWnd, LVS_EX_GRIDLINES, LVS_EX_GRIDLINES);

	m_Passlist.InsertColumn(0, _T("姓名"), LVCFMT_LEFT, 100);
	m_Passlist.InsertColumn(1, _T("身份证号"), LVCFMT_LEFT, 180);
	m_Passlist.InsertColumn(2, _T("电话号码"), LVCFMT_CENTER, 150);

	ListView_SetExtendedListViewStyleEx(m_ListTik.m_hWnd, LVS_EX_GRIDLINES, LVS_EX_GRIDLINES);
	m_ListTik.InsertColumn(0, _T("车次"), LVCFMT_LEFT, 80);
	m_ListTik.InsertColumn(1, _T("发站"), LVCFMT_LEFT, 50);
	m_ListTik.InsertColumn(2, _T("到站"), LVCFMT_LEFT, 50);
	m_ListTik.InsertColumn(3, _T("历时"), LVCFMT_LEFT, 50);
	int len =0;
	for (int i=0;i<g_sSize;i++){
		len = g_seatlist[i].sReadable.size()*6+20;
		m_ListTik.InsertColumn(4+i, _T(g_seatlist[i].sReadable.c_str()), LVCFMT_LEFT, len);
	}
	OnBnClickedChkVpass();

    ifstream userfile(CONFIG_PATH"\\buyticket.dat");
    string uname,upass,time,fromStation,toStation,date,specialTrain;
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

    ((CButton*)GetDlgItem(IDC_CHK_YW))->SetCheck(TRUE);
    ((CButton*)GetDlgItem(IDC_CHK_EDZ))->SetCheck(TRUE);

    return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CBuyTicketDlg::OnPaint()
{
    LoadYzCode();
    if (IsIconic()) {
        CPaintDC dc(this); // 用于绘制的设备上下文
        SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);
        // 使图标在工作区矩形中居中
        int cxIcon = GetSystemMetrics(SM_CXICON);
        int cyIcon = GetSystemMetrics(SM_CYICON);
        CRect rect;
        GetClientRect(&rect);
        int x = (rect.Width() - cxIcon + 1) / 2;
        int y = (rect.Height() - cyIcon + 1) / 2;

        // 绘制图标
        dc.DrawIcon(x, y, m_hIcon);
    } else {
        CDialogEx::OnPaint();
    }
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CBuyTicketDlg::OnQueryDragIcon()
{
    return static_cast<HCURSOR>(m_hIcon);
}

void CBuyTicketDlg::OnBnClickedLogin()
{
    CString code,name,pass;
    GetDlgItem(IDC_CODE)->GetWindowText(code);
    GetDlgItem(IDC_UNAME)->GetWindowText(name);
    GetDlgItem(IDC_UPASS)->GetWindowText(pass);
    string yzcode=code.GetBuffer();
    pTrain->Login(name.GetBuffer(),pass.GetBuffer(),yzcode);
	string oPass =pTrain->LoadWebPassanger();
	UpdatePassList();
}

bool CBuyTicketDlg::ClearBack(void)
{
	char szIFile[50]={0},szOFile[50]={0},szType[20]={0};
	sprintf(szIFile,CONFIG_PATH"\\buyticket.png");
	sprintf(szOFile,CONFIG_PATH"\\buyticket.bmp");
	sprintf(szType,"image/bmp");
	ImgConvert(szIFile,szOFile,szType);
	BYTE tBuf[8096];
	FILE* pFile = fopen(szOFile,"rb");
	fseek(pFile,0,SEEK_END);
	int len=ftell(pFile);
	fseek(pFile,0,SEEK_SET);
	int ret = fread(tBuf,len,1,pFile);
	fclose(pFile);
	for (int i=0;i<len;i++)	{
		if (tBuf[i]==0xCA)	
			tBuf[i]=0xFF;
	}
	pFile = fopen(szOFile,"wb+");
	fwrite(tBuf,len,1,pFile);
	fclose(pFile);
	return true;
}

bool CBuyTicketDlg::LoadYzCode(void)
{
    CRect rect;
    GetDlgItem(IDC_PIC)->GetClientRect(&rect);     
	//m_picture为Picture Control控件变量，获得控件的区域对象
	ClearBack();
    CImage image;       //使用图片类
    image.Load(CONFIG_PATH"\\buyticket.bmp");   //装载路径下图片信息到图片类
    if(!image.IsNull()) {
        CDC* pDC = GetDlgItem(IDC_PIC)->GetWindowDC();    //获得显示控件的DC
        image.Draw( pDC -> m_hDC,rect);      //图片类的图片绘制Draw函数
        ReleaseDC(pDC);
    }
    return true;
}

void CBuyTicketDlg::OnBnClickedQuery()
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
    // TODO: 在此添加消息处理程序代码和/或调用默认值
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
    // 屏蔽回车关闭
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
	// TODO: 在此添加控件通知处理程序代码
	KillTimer(1);
	pTrain->m_Success = false;
}

void CBuyTicketDlg::OnBnClickedChkVpass()
{
	// TODO: 在此添加控件通知处理程序代码
	CEdit *ed = (CEdit *)GetDlgItem(IDC_UPASS);
	if (BST_CHECKED==IsDlgButtonChecked(IDC_CHK_VPASS))	{
		ed->SetPasswordChar(0);
// 		ed->ModifyStyle(NULL,ES_PASSWORD); 
	} else {
		ed->SetPasswordChar(TEXT('*'));
// 		ed->ModifyStyle(NULL,ed->GetStyle() & ES_PASSWORD); 
	}
	ed->SetFocus();
// 	UpdateData(FALSE);
}


void CBuyTicketDlg::OnLvnBeginlabeleditListPass(NMHDR *pNMHDR, LRESULT *pResult)
{
	NMLVDISPINFO *pDispInfo = reinterpret_cast<NMLVDISPINFO*>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
// 	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;
// 	int iItem = pDispInfo->item.iItem;
// 	// Here can you change the text that should be edited.
// 	// In this case we don't want to edit the number in the first column
// 	if(pDispInfo->item.iSubItem == 0) {
// 		lstrcpyn(pDispInfo->item.pszText,
// 			pTrain->m_vPassanger[iItem].m_sname,
// 			pDispInfo->item.cchTextMax);
// 	}
	*pResult = 0;
}


void CBuyTicketDlg::OnLvnEndlabeleditListPass(NMHDR *pNMHDR, LRESULT *pResult)
{
// 	NMLVDISPINFO *pDispInfo = reinterpret_cast<NMLVDISPINFO*>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;
	int iItem = pDispInfo->item.iItem;
	// TODO: Add your control notification handler code here
	if(pDispInfo->item.pszText != NULL) {
		if(pDispInfo->item.iSubItem == 1)
			pTrain->m_vPassanger[iItem].m_sidcard = pDispInfo->item.pszText;
		else if(pDispInfo->item.iSubItem == 0)
			pTrain->m_vPassanger[iItem].m_sname = pDispInfo->item.pszText;

		//Select next item?
		if(m_Passlist.GetLastEndEditKey() == VK_RETURN) {
			//If column 0, select in next column
			if(pDispInfo->item.iSubItem == 0) {
				m_Passlist.EditSubItem(iItem, 1);
			} else {
				//Select next item if possible
				if(iItem+1 < m_Passlist.GetItemCount())
					m_Passlist.EditSubItem(iItem+1, 0);
			}
		}
	}
	*pResult = 0;
}

void CBuyTicketDlg::OnNMClickListPass(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	NMLISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	int item=-1, subitem=-1;
	bool oncheck=false;
	bool onimage=false;
	if(m_Passlist.HitTest(pNMListView->ptAction, item, subitem, &oncheck, &onimage)) {
		TRACE(_T("User right hit item %d, subitem %d \n"), item, subitem);
		if(!oncheck && !onimage) {
			m_Passlist.RedrawSubitems(item, item, subitem);
		}
	}
	*pResult = 0;
}

void CBuyTicketDlg::OnNMDblclkListPass(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: 在此添加控件通知处理程序代码
	NMLISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	int item=-1, subitem=-1;
	if(m_Passlist.HitTest(pNMListView->ptAction, item, subitem)) {
		m_Passlist.SetHotItem(item);
	}
	*pResult = 0;
}


void CBuyTicketDlg::OnBnClickedBtnCmsg()
{
	// TODO: 在此添加控件通知处理程序代码
// 	m_listbox.SetWindowText("");
// 	while(m_listbox.GetCount())
// 		m_listbox.DeleteString( 0 ); 
	m_listbox.ResetContent() ;
	UpdateData(FALSE);
// 	m_listbox.InsertString()
}
