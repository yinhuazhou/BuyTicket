
// BuyTicketDlg.h : 头文件
//

#pragma once
#include "afxwin.h"
#include "resource.h"


// CBuyTicketDlg 对话框
class CBuyTicketDlg : public CDialogEx
{
// 构造
public:
	CBuyTicketDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_BUYTICKET_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButton1();
	bool LoadYzCode(void);
	CListBox m_listbox;
	afx_msg void OnBnClickedButton2();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnBnClickedOpenUrl();
	afx_msg void OnStnClickedPic();
	afx_msg void OnBnClickedBlog();
	virtual void OnOK();
	virtual void OnCancel();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnBnClickedBtnStop();
};
