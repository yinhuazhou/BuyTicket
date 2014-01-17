#pragma once
#include <map>
#include <boost/smart_ptr.hpp>
#include <boost/function.hpp>
#include <boost/bind.hpp>
#include <queue>

using namespace std;
class CBuyTicketDlg;
class CTicket;
#define MAX_PASSANGER 3

namespace echttp{
	class http;
	class respone;
}

class CPassenger
{
public:
	CPassenger()
	{
		m_sname.clear();
		m_sidcard.clear();
		m_sphone.clear();
		m_check = true;
	}
	//乘客信息 passenger infomation
	string m_sname;
	string m_sidcard;
	string m_sphone;
	bool m_check;
};

class CTrain
{
private:
	echttp::http *m_http;
	string m_suname;
	string m_supass;
	string m_syzcode;
	CBuyTicketDlg* m_pdlg;
	string getSuggest(void);
	map<string,string> m_station;

    string m_sdomain;
	string m_sPurl;

    //车站信息
	string m_sfromCode;
	string m_toCode;
    string m_sencrypt;

    //乘客信息 passanger infomation
    string m_sname[MAX_PASSANGER];
    string m_sidcard[MAX_PASSANGER];
    string m_sphone[MAX_PASSANGER];

	bool isTicketEnough(string tickstr);
    string xxtea_encode(string data,string key);
    void encrypt_code(string src);
	bool findandencrypt(string sAll,string s2Find,string s_ref);
public:
	CTrain(CBuyTicketDlg *dlg);
	~CTrain(void);

	void SetCookie(string cookies);
    //购票队列
    queue<CTicket> *m_pvBlist;
	
	bool Login(string username, string password, string code);
	bool GetCode(void);

	void SerachTicketPage();
    void RecvSearchTicketPage(boost::shared_ptr<echttp::respone> respone);

	void SearchTicket(string fromStation,string toStation,string date);
	void RecvTicket(boost::shared_ptr<echttp::respone> respone);
	
	void showMsg(string msg);
	bool submitOrder(CTicket ticket);
	void RecvSubmitOrder(boost::shared_ptr<echttp::respone> respone,CTicket ticket);
	string loadCode2(void);
	bool m_isInBuy;
	string m_strain;
	vector<CPassenger> m_vPassanger;
	vector<CTicket> m_vTicket;
	void confrimOrder(boost::shared_ptr<echttp::respone> respone, string pstr);
	bool m_Success;
    void LoadStation(void);
    void LoadPassanger(void);
	string  LoadWebPassanger(void);
    void LoadDomain(void);
    void CheckUserOnline(void);
    void RecvCheckUserOnline(boost::shared_ptr<echttp::respone> respone);
    void RecvNothing(boost::shared_ptr<echttp::respone> respone);
    bool CheckQueueCount(CTicket ticket, string token);
};

