#pragma once
#include <map>
#include <boost/smart_ptr.hpp>
#include <boost/function.hpp>
#include <boost/bind.hpp>
#include <queue>

using namespace std;
class CBuyTicketDlg;
class Ticket;
#define MAX_PASSANGER 3

namespace echttp{
	class http;
	class respone;
}
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
    string encrypt_str;

    //乘客信息 passanger infomation
    string m_sname[MAX_PASSANGER];
    string m_sidcard[MAX_PASSANGER];
    string m_sphone[MAX_PASSANGER];

    //购票队列
    queue<Ticket> *buy_list;

	bool isTicketEnough(string tickstr);
    string xxtea_encode(string data,string key);
    void encrypt_code(string src);

public:
	CTrain(CBuyTicketDlg *dlg);
	~CTrain(void);

	void SetCookie(string cookies);
	
	bool Login(string username, string password, string code);
	bool GetCode(void);

	void SerachTicketPage();
    void RecvSearchTicketPage(boost::shared_ptr<echttp::respone> respone);

	void SearchTicket(string fromStation,string toStation,string date);
	void RecvTicket(boost::shared_ptr<echttp::respone> respone);
	
	void showMsg(string msg);
	bool submitOrder(Ticket ticket);
	void RecvSubmitOrder(boost::shared_ptr<echttp::respone> respone,Ticket ticket);
	string loadCode2(void);
	bool m_isInBuy;
	string m_strain;
	void confrimOrder(boost::shared_ptr<echttp::respone> respone, string pstr);
	bool m_Success;
    void LoadStation(void);
    void LoadPassanger(void);
    void LoadDomain(void);
    void CheckUserOnline(void);
    void RecvCheckUserOnline(boost::shared_ptr<echttp::respone> respone);
    void RecvNothing(boost::shared_ptr<echttp::respone> respone);
    bool CheckQueueCount(Ticket ticket, string token);
};

