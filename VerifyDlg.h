#pragma once
#include <string>

using namespace std;

// CVerifyDlg �Ի���
class CTrain;

class CVerifyDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CVerifyDlg)

public:
	CVerifyDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CVerifyDlg();

// �Ի�������
	enum { IDD = IDD_VERIFY_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	CTrain* m_pTrain;
	string m_sVercode;
    string m_spath;
	afx_msg void OnBnClickedOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg void OnStnClickedPic2();
	void loadImg(void);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
};
