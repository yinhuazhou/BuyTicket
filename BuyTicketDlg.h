
// BuyTicketDlg.h : ͷ�ļ�
//

#pragma once
#include "afxwin.h"
#include "resource.h"


// CBuyTicketDlg �Ի���
class CBuyTicketDlg : public CDialogEx
{
// ����
public:
	CBuyTicketDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_BUYTICKET_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
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
