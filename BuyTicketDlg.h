
// BuyTicketDlg.h : ͷ�ļ�
//

#pragma once
#include "afxwin.h"
#include "resource.h"
#include "afxcmn.h"
#include <vector>

#include "Quick/quicklist.h"
using namespace std;

#ifdef USEXPTHEMES
#include "theme.h"
#endif

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
	afx_msg void OnBnClickedLogin();
	bool LoadYzCode(void);
	bool ClearBack(void);
	afx_msg void OnBnClickedQuery();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnBnClickedOpenUrl();
	afx_msg void OnStnClickedPic();
	afx_msg void OnBnClickedBlog();
	afx_msg LRESULT OnGetListItem(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnListClick(WPARAM wParam, LPARAM lParam);

	void UpdatePassList();
public:
	CListBox m_listbox;
	virtual void OnOK();
	virtual void OnCancel();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnBnClickedBtnStop();
	afx_msg void OnBnClickedChkVpass();
	CQuickList m_Passlist;
	afx_msg void OnLvnBeginlabeleditListPass(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnEndlabeleditListPass(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMClickListPass(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMDblclkListPass(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedBtnCmsg();
	void UpdateTicketList();
	CQuickList m_ListTik;
};
