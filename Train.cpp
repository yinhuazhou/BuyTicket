#include "stdafx.h"
#include "echttp/http.hpp"
#include "Train.h"
#include <iostream>
#include <fstream>
#include "Ticket.h"
#include "BuyTicketDlg.h"
#include "VerifyDlg.h"
#include <WinInet.h>
#include "xxtea.h"
#include <boost/random/mersenne_twister.hpp>
#include <boost/random/uniform_int_distribution.hpp>
#include "cJSON.h"

#pragma comment(lib,"Wininet.lib")

boost::random::mt19937 rand_gen;

// 	"gg_num":"--","gr_num":"--","qt_num":"--",
// 	"rw_num":"1","rz_num":"--","tz_num":"--",
// 	"wz_num":"无","yb_num":"--","yw_num":"无",
// 	"yz_num":"9","ze_num":"--","zy_num":"--",
// 	"swz_num":"--"
SEATMAP g_seatlist[] = {
	{"9", "商务座","swz_num"},		{"P", "特等座","tz_num"},	{"M", "一等座","zy_num"},
	{"O", "二等座","ze_num"},	{"6", "高级软卧","gr_num"},
	{"4", "软卧","rw_num"},			{"3", "硬卧","yw_num"},			{"2", "软座","rz_num"},
	{"1", "硬座","yz_num"},			{"empty", "无座","wz_num"},		{"empty", "其它","yb_num"}
};
const int g_sSize =ARRAY_SIZE(g_seatlist);

CTicket::CTicket(string s_ticket)
{
	train_no=echttp::substr(s_ticket,"train_no\":\"","\"");
	from_station_telecode=echttp::substr(s_ticket,"from_station_telecode\":\"","\"");
	from_station_name=echttp::substr(s_ticket,"from_station_name\":\"","\"");
	to_station_telecode=echttp::substr(s_ticket,"to_station_telecode\":\"","\"");
	to_station_name=echttp::substr(s_ticket,"to_station_name\":\"","\"");
	start_time = echttp::substr(s_ticket,"start_time\":\"","\"");
	arrive_time= echttp::substr(s_ticket,"arrive_time\":\"","\"");
	lishi= echttp::substr(s_ticket,"lishi\":\"","\"");
	canWebBuy= echttp::substr(s_ticket,"canWebBuy\":\"","\"");
	yp_info=echttp::substr(s_ticket,"yp_info\":\"","\"");
	start_train_date=echttp::substr(s_ticket,"start_train_date\":\"","\"");
	train_date=echttp::DateFormat(start_train_date,"%Y-%m-%d");
	location_code=echttp::substr(s_ticket,"location_code\":\"","\"");
	secret_str=echttp::substr(s_ticket,"secretStr\":\"","\"");
	station_train_code=echttp::substr(s_ticket,"station_train_code\":\"","\"");
	for(int i=0;i<g_sSize;i++) {
		m_bCheck[i] = false;
		string squery=g_seatlist[i].sQuery+"\":\"";
		m_seat[i] = echttp::substr(s_ticket,squery,"\"");
	}
}

bool CTicket::IscanWebBuy()
{
	return((canWebBuy=="Y")&&(!secret_str.empty()));
}

string CTrain::xxtea_encode(string data_,string key_)
{
    xxtea_long ret_len=0;
    boost::shared_array<char> v(new char[data_.size()*2]);//*2 because need enough to storage buf in encode inside.
    memset(v.get(),0,data_.size()*2);
    memcpy(v.get(),data_.c_str(),data_.size());

    boost::shared_array<char> k(new char[key_.size()*2]);
    memset(k.get(),0,key_.size()*2);
    memcpy(k.get(),key_.c_str(),key_.size());

    unsigned char * ret=xxtea_encrypt((unsigned char *)v.get(),data_.size(),(unsigned char *)k.get(),&ret_len);
    char* buf=new char[ret_len*2+1];
    memset(buf,0,ret_len+1);
    for( xxtea_long i = 0; i < ret_len; ++i) {
        unsigned char c=*(ret+i);
        sprintf(buf+2*i, "%02x", c);
    }
    string result=echttp::base64_encode((unsigned char*)buf,ret_len*2);
    delete[] buf;
    free(ret);
    return result;
}

CTrain::CTrain(CBuyTicketDlg *pdlg)
    : m_isInBuy(false)
    , m_Success(false)
{
    m_pdlg=pdlg;
    m_http=new echttp::http();
    m_pvBlist=new queue<CTicket>();
    LoadStation();
    m_http->Request.set_defalut_userAgent("Mozilla/5.0 (compatible; MSIE 9.0; qdesk 2.5.1177.202; Windows NT 6.1; WOW64; Trident/6.0)");
    LoadDomain();
    m_http->Get(m_sPurl);
    m_http->Request.m_header.insert("Referer",m_sPurl);

    string initUrl=m_http->Get(m_sPurl+LOGIN_INIT)->as_string();

    if(findandencrypt(initUrl,DYNAMIC_LOGINJS,LOGIN_INIT)/*initUrl.find(DYNAMIC_LOGINJS)!=string::npos*/) {
//         string loginjs=echttp::substr(initUrl,DYNAMIC_LOGINJS,"\"");
// //         string loginjs2=echttp::substr(initUrl,"resources/merged/login_js.js?scriptVersion=","\"");
// 
//         m_http->Request.set_defalut_referer(m_sPurl+LOGIN_INIT);
// //         m_http->Get(m_sPurl+"resources/merged/login_js.js?scriptVersion="+loginjs2);
//         string ret= m_http->Get(m_sPurl+DYNAMIC_LOGINJS+loginjs)->as_string();
//         encrypt_code(ret);
//         //判断是否有隐藏随机监测url
//         string ready_str=echttp::substr(ret,"$(document).ready(function(){","success");
//         if(ready_str.find("jq({url :'")!=string::npos) {
//             string url=echttp::substr(ready_str,"jq({url :'","'");
//             m_http->Post(m_sPurl+url,"");
//         }
    } else {
        m_pdlg->m_listbox.AddString("获取登录信息异常！");
    }
}

CTrain::~CTrain(void)
{
	m_station.clear();
    delete m_http;
}

//从加密js生成验证信息
void CTrain::encrypt_code(string src)
{
    if(src.find("gc(){var key='")==string::npos) {
        m_sencrypt="";
    } else {
        string key=echttp::substr(src,"gc(){var key='","';");
        string code=xxtea_encode("1111",key);
        m_sencrypt="&"+echttp::UrlEncode(key)+"="+echttp::UrlEncode(code);
    }
}
bool CTrain::findandencrypt(string sAll,string s2Find,string s_ref) {
	if(sAll.find(s2Find)!=string::npos) {
		string authJs=echttp::substr(sAll,s2Find,"\"");
		m_http->Request.set_defalut_referer(m_sPurl+s_ref);
		string ret= m_http->Get(m_sPurl+s2Find+authJs)->as_string();
		encrypt_code(ret);
		//判断是否有隐藏随机监测url
		string ready_str=echttp::substr(ret,"$(document).ready(function(){","success");
		if(ready_str.find("jq({url :'")!=string::npos) {
			string url=echttp::substr(ready_str,"jq({url :'","'");
			m_http->Post(m_sPurl+url,"");
		}
		return true;
	}
	return false;
}
bool CTrain::Login(string username, string password, string code)
{
    m_suname=username;
    m_supass=password;
    m_syzcode=code;
    ofstream file(CONFIG_PATH"\\LoginError.txt",ios::app);
    m_http->Request.m_header.insert("x-requested-with","XMLHttpRequest");
    string pstr="loginUserDTO.user_name="+m_suname+"&userDTO.password="+m_supass+"&randCode="+m_syzcode;
    string res=m_http->Post(m_sPurl+"login/loginAysnSuggest",pstr)->as_string();
    res=echttp::Utf8Decode(res);

    if(res.find("{\"loginCheck\":\"Y\"}")!=string::npos) {
        m_pdlg->m_listbox.AddString("登录成功");
        SetCookie(m_http->Request.m_cookies.cookie_string());
        m_http->Post(m_sPurl+LOGIN_INIT,"_json_att=");
        return true;
    } else if(res.find("验证码不正确")!=string::npos) {
        m_pdlg->m_listbox.AddString("验证码不正确");
        return false;
    } else {
        file<<res;
        file.close();
        showMsg("登录失败："+echttp::substr(res,"messages\":[\"","]"));
        return false;
    }
}

bool CTrain::GetCode(void)
{
    m_http->Request.set_defalut_referer(m_sPurl+LOGIN_INIT);
    int status_code=m_http->Get(string(m_sPurl+"passcodeNew/getPassCodeNew?module=login&rand=sjrand"),
		string(CONFIG_PATH"\\buyticket.png"))->status_code;
    return status_code==200;
}

string CTrain::getSuggest(void)
{
    string res=m_http->Post("https://dynamic.12306.cn/otsweb/loginAction.do?method=loginAysnSuggest","")->as_string();
    if(res=="") return "";
    return echttp::substr(res,"{\"loginRand\":\"","\"");
}

//访问查询余票信息页面（定时访问，以保持在线）
void CTrain::SerachTicketPage()
{
    m_http->Request.set_defalut_referer(m_sPurl+LEFT_TICKET_INIT);
    m_http->Get(m_sPurl+LEFT_TICKET_INIT,boost::bind(&CTrain::RecvSearchTicketPage,this,_1));
    CheckUserOnline();
}

void CTrain::RecvSearchTicketPage(boost::shared_ptr<echttp::respone> respone)
{
    string sources;
    if (respone->as_string()!="") {
        sources=respone->as_string();
        if(findandencrypt(sources,DYNAMIC_QUERYJS,LEFT_TICKET_INIT)/*sources.find(DYNAMIC_QUERYJS)!=string::npos*/) {
//             string authJs=echttp::substr(sources,DYNAMIC_QUERYJS,"\"");
//             m_http->Request.set_defalut_referer(m_sPurl+LEFT_TICKET_INIT);
//             string ret= m_http->Get(m_sPurl+DYNAMIC_QUERYJS+authJs)->as_string();
//             encrypt_code(ret);
//             //判断是否有隐藏随机监测url
//             string ready_str=echttp::substr(ret,"$(document).ready(function(){","success");
//             if(ready_str.find("jq({url :'")!=string::npos) {
//                 string url=echttp::substr(ready_str,"jq({url :'","'");
//                 m_http->Post(m_sPurl+url,"");
//             }
        } else {
            showMsg("获取查询车票页面异常！");
        }
    } else {
        showMsg("查票页面已空白!!!");
    }
}

void CTrain::SearchTicket(string fromStation,string toStation,string date)
{
    LoadDomain();
    m_sfromCode=m_station[fromStation];
    m_toCode=m_station[toStation];
    m_http->Request.set_defalut_referer(m_sPurl+LEFT_TICKET_INIT);

    m_http->Request.m_header.insert("x-requested-with","XMLHttpRequest");
    m_http->Request.m_header.insert("Content-Type","application/x-www-form-urlencoded");
    string url=m_sPurl+"leftTicket/query?leftTicketDTO.train_date="+date
                    +"&leftTicketDTO.from_station="+m_sfromCode+"&leftTicketDTO.to_station="+m_toCode
                    +"&purpose_codes=ADULT";
    m_http->Get(url,boost::bind(&CTrain::RecvTicket,this,_1));
    //5 percent to flush search page;
    boost::random::uniform_int_distribution<> dist(1, 60);
    if(dist(rand_gen)==20) {
        SerachTicketPage();
    }
}

bool CTrain::isTicketEnough(string tickstr)
{
    if(tickstr=="--" || tickstr=="*" || tickstr.find("无")!=string::npos)
        return false;
    if(tickstr.find("有")!=string::npos)
        return true;
    CString fullname2;
    m_pdlg->GetDlgItem(IDC_FULLNAME2)->GetWindowText(fullname2);
    fullname2.Trim();
	CString fullname3;
	m_pdlg->GetDlgItem(IDC_FULLNAME3)->GetWindowText(fullname3);
	fullname3.Trim();
    int ticket_num=atoi(tickstr.c_str());
	if(!fullname3.IsEmpty()){
		return ticket_num>=3;
	} else if(!fullname2.IsEmpty())
    if(!fullname2.IsEmpty()) {
        return ticket_num>=2;
    } else {
        return ticket_num>=1;
    }
	return true;
}

void CTrain::RecvTicket(boost::shared_ptr<echttp::respone> respone)
{
	m_vTicket.clear();
    string restr=echttp::Utf8Decode(respone->as_string());
    if(restr!=""&&restr!="-10" &&restr.find("queryLeftNewDTO")!=string::npos) {
        ofstream webfile(CONFIG_PATH"\\web.txt",ios::app);
        webfile<<restr<<"\r\n";
        webfile.close();
        while(restr.find("queryLeftNewDTO")!=string::npos) {
            string ticketInfo=echttp::substr(restr,"queryLeftNewDTO","{\"queryLeftNewDTO");
            CTicket ticket(ticketInfo);
            restr=restr.substr(restr.find("queryLeftNewDTO")+5);
            string trainstr=echttp::substr(restr,"_train_code\":\"","\"");
            if(m_strain!=""&&m_strain.find(trainstr)==string::npos) {
                continue;
            }
			if (ticket.IscanWebBuy())	{
				m_vTicket.push_back(ticket);
			}
			for (int i=2;i<9;i++) {
				if(isTicketEnough(ticket.m_seat[i])&&BST_CHECKED==m_pdlg->IsDlgButtonChecked(IDC_CHK_YDZ+i-2)) {
					if(restr.find("secretStr")!=string::npos) {
						showMsg(trainstr+"有"+g_seatlist[i].sReadable+"---车票数目:"+ticket.m_seat[i]);
						ticket.SetBuySeat(g_seatlist[i].sCode/*"M"*/);
						m_pvBlist->push(ticket);
					} else {
						showMsg("未知"+g_seatlist[i].sReadable+"余票信息");
					}
				}
			}
        }
		m_pdlg->UpdateTicketList();
        //如果检测到相应的票，就下单
        int queue_size=m_pvBlist->size();
        if(queue_size>0 && !m_pvBlist->empty()) {
            CTicket task_ticket=m_pvBlist->front();
            submitOrder(task_ticket);
            showMsg("开始购买:"+task_ticket.station_train_code);
            while (m_pvBlist->size()>0) {
                if(queue_size==m_pvBlist->size()) { //如果当前队列数，和之前保留的队列数一致，则有任务在执行
                    Sleep(50);
					continue;
                }
                queue_size=m_pvBlist->size();
                if(!m_pvBlist->empty()) {
                    CTicket task_ticket=m_pvBlist->front();
                    submitOrder(task_ticket);
                    showMsg("开始购买:"+task_ticket.station_train_code);
                }
				if (m_Success)
					return ;
            }
        } else {
            showMsg("没有卧铺或者硬座");
        }
    } else {
        showMsg("发生错误，请检查参数:"+echttp::Utf8Decode(echttp::substr(restr,"\"messages\":[\"","\"]")));
    }
}

void CTrain::showMsg(string msg)
{
    CTime tm;
    tm=CTime::GetCurrentTime();
    string str=tm.Format("%m月%d日 %X：");
    str+=msg;
    m_pdlg->m_listbox.InsertString(0,str.c_str());
    ofstream file(CONFIG_PATH"\\ticketlog.txt",ios::app);
    file<<str<<"\r\n";
    file.close();
}

bool CTrain::submitOrder(CTicket ticket)
{
    m_isInBuy=true;
	time_t ltime;
	time(&ltime);
	ltime+=24*3600*30;
	struct tm *pTm =localtime(&ltime);
	CString tData;
	tData.Format("%04d-%02d-%02d",pTm->tm_year+1900,pTm->tm_mon+1,pTm->tm_mday);
//	2014-01-01
    string pstr="secretStr="+ticket.secret_str+
		"&train_date="+ticket.train_date+
		"&back_train_date="+tData.GetBuffer()+
		"&tour_flag=dc&purpose_codes=ADULT"+
		"&query_from_station_name="+ticket.from_station_name+
		"&query_to_station_name="+ticket.to_station_name+"&undefined";
    showMsg("尝试买:"+ticket.station_train_code);
    boost::shared_ptr<echttp::respone> ret=m_http->Post(m_sPurl+"leftTicket/submitOrderRequest",pstr);
    string recvStr=ret->as_string();
    if(recvStr.find("status\":true")!=string::npos) {
        m_http->Post(m_sPurl+CONFIRM_PASS_INIT,"_json_att=",boost::bind(&CTrain::RecvSubmitOrder,this,_1,ticket));
    } else {
        showMsg("预定错误!"+echttp::Utf8Decode(echttp::substr(recvStr,"\"messages\":[\"","\"]")));
        m_pvBlist->pop();
    }
    return false;
}

void CTrain::RecvSubmitOrder(boost::shared_ptr<echttp::respone> respone,CTicket ticket)
{
    string restr=echttp::Utf8Decode(respone->as_string());
    LoadPassanger();
    m_http->Request.set_defalut_referer(m_sPurl+CONFIRM_PASS_INIT);
    string TOKEN=echttp::substr(restr,"globalRepeatSubmitToken = '","'");
    string keyCheck=echttp::substr(restr,"'key_check_isChange':'","'");
    string leftTicketStr=echttp::substr(restr,"leftTicketStr':'","'");
    string trainLocation=echttp::substr(restr,"train_location':'","'");
    string checkbox2="";
    string seattype=ticket.seat_type;//座位类型 3为卧铺 1为硬座

    string code_path=loadCode2();
    CVerifyDlg VerifyDlg;
    VerifyDlg.m_pTrain=this;//传递本类指针到对话框
    VerifyDlg.m_spath=code_path;
    if(VerifyDlg.DoModal()) {
        showMsg("延时一下，过快会被封！");
checkcode:
        string randcode=VerifyDlg.m_sVercode;
        string passanger_info,nPass,oPass;
		nPass.clear();
		oPass.clear();
		for (int i=0;i<MAX_PASSANGER;i++)	{
			if (m_sname[i]!="")	{
				nPass +=seattype+"%2C0%2C1%2C"+m_sname[i]+"%2C1%2C"+m_sidcard[i]+"%2C"+m_sphone[i]+"%2CN_";
				oPass +=m_sname[i]+"%2C1%2C"+m_sidcard[i]+"%2C1_";
			}
		}
		if (!nPass.empty())	{
			passanger_info = nPass+"&oldPassengerStr="+oPass;
			string pinfo = echttp::Utf8Decode(echttp::UrlDecode(passanger_info));
		}
        string pstr="cancel_flag=2&bed_level_order_num=000000000000000000000000000000&passengerTicketStr="
			+passanger_info+"&tour_flag=dc&randCode="+randcode+
			"&_json_att=&REPEAT_SUBMIT_TOKEN="+TOKEN;
        string url=m_sPurl+"confirmPassenger/checkOrderInfo";
        string checkstr=m_http->Post(url,pstr)->as_string();
        checkstr=echttp::Utf8Decode(checkstr);

        if(checkstr.find("submitStatus\":true")!=string::npos
                && checkstr.find("status\":true")!=string::npos) {
            //检查排队队列，防止白提交
            bool ticket_enough=CheckQueueCount(ticket,TOKEN);
            if(!ticket_enough) {
                showMsg("很遗憾,"+ticket.station_train_code+"排队人数已满");
                m_pvBlist->pop();
                return;
            }
            m_http->Request.m_header.insert("Content-Type","application/x-www-form-urlencoded; charset=UTF-8");
            m_http->Request.m_header.insert("X-Requested-With","XMLHttpRequest");
            pstr="passengerTicketStr="+passanger_info+"&randCode="+randcode+"&purpose_codes=00"+
                 +"&key_check_isChange="+keyCheck+"&leftTicketStr="+leftTicketStr+"&train_location="+trainLocation
                 +"&_json_att=&REPEAT_SUBMIT_TOKEN="+TOKEN;
            //提交订单
            m_http->Post(m_sPurl+CONFIRM_PASS_SQUEUE,pstr,boost::bind(&CTrain::confrimOrder,this,_1,pstr));
        } else if(checkstr.find("请重试")!=string::npos) {
            showMsg(checkstr);
            Sleep(1000);
            goto checkcode;
        } else if(checkstr.find("验证码")!=string::npos) {
            showMsg("验证码错误，请重新输入！");
            int ret=VerifyDlg.DoModal();
            if(ret==IDOK) {
                goto checkcode;
            } else if(ret==IDCANCEL) {
                showMsg("您选择了取消购票！");
                m_pvBlist->pop();
            }
        } else {
            showMsg("检查订单操作错误:"+echttp::substr(checkstr,"errMsg","}"));
            m_pvBlist->pop();
        }
    }
}

void CTrain::SetCookie(string cookies)
{
    while(cookies.find(";")!=string::npos) {
        string cookie=cookies.substr(0,cookies.find_first_of(" "));
        cookie=cookie+"expires=Sun,22-Feb-2099 00:00:00 GMT";
        ::InternetSetCookieA("https://kyfw.12306.cn",NULL,cookie.c_str());
        cookies=cookies.substr(cookies.find_first_of(" ")+1);
    }
}

string CTrain::loadCode2(void)
{
    boost::random::uniform_int_distribution<> dist1(1, 50000);
    rand_gen.seed(time(0));
    int randcode=dist1(rand_gen);
    string randstr=echttp::convert<string>(randcode);
    string yz_code=CONFIG_PATH"\\buyticket"+randstr+".png";
    m_http->Request.m_header.insert("Referer",m_sPurl+CONFIRM_PASS_INIT);
    m_http->Get(string(m_sPurl+"passcodeNew/getPassCodeNew?module=passenger&rand=randp"),yz_code);
    return yz_code;
}

void CTrain::confrimOrder(boost::shared_ptr<echttp::respone> respone,string pstr)
{
    string result;
    if (respone->as_string()!="") {
        result=echttp::Utf8Decode(respone->as_string());
        if(result.find("status\":true")!=string::npos) {
            m_Success=true;
            showMsg("!!!!!预定成功，速速到12306付款");
            m_pvBlist->pop();
        } else if(result.find("请重试")!=string::npos) {
            if(!m_Success) {
                Sleep(1000);
                m_http->Post(m_sPurl+CONFIRM_PASS_SQUEUE,pstr,boost::bind(&CTrain::confrimOrder,this,_1,pstr));
                showMsg(result+"继续抢!");
            }
        } else {
            showMsg(result);
            m_pvBlist->pop();
        }
    } else {
        if(!m_Success) {
            m_http->Post(m_sPurl+CONFIRM_PASS_SQUEUE,pstr,boost::bind(&CTrain::confrimOrder,this,_1,pstr));
            showMsg("返回空，继续抢!");
        }
    }
    return ;
}

void CTrain::LoadStation(void)
{
	ifstream cityfile(CONFIG_PATH"\\City_code.txt");
	string sline,sname,scode;
	char seps[] = ":\"\"\n";
	char *token;
	int i = 0;
	if(cityfile.is_open())  {
		do 	{
			getline(cityfile,sline,cityfile.widen( ',' ));
			if(sline.size()<3)
				break ;
			token = strtok( (char*)sline.c_str(), seps ); // C4996
			sname = token;
// 			sname = echttp::Utf8Decode(sname);
			// Get next token: 
			token = strtok( NULL, seps ); // C4996
			scode = token;
 			m_station[sname] = scode;
			i++;
		} while (!sline.empty());
	}
	TRACE( "i= %d\n", i );

// 	i = 0;
// 	ofstream ofile("OCity_code1.txt");
// 	map <string,string> :: iterator m1_Iter;
// 	for (m1_Iter = m_station.begin( );m1_Iter!=m_station.end();m1_Iter++) {
// 		sname = m1_Iter->first;
// 		scode = m1_Iter->second;
// 		ofile<<sname<<":"<<scode<<",\n";
// 		i++;
// 	}
// 	ofile.close();
// 	TRACE( "i= %d\n", i );
}

string CTrain::LoadWebPassanger(void)
{
	string sPass=m_http->Get(m_sPurl+"passengers/init")->as_string();
	string oPass = echttp::Utf8Decode(sPass);
	string iPass=echttp::substr(oPass,"var passengers=",";");	
	for (int i=0;i<iPass.size();i++) {
		if (iPass[i]=='\'')	{
			iPass[i] = '\"';
		}
	}
	int len = iPass.size();
	cJSON *root = cJSON_Parse(iPass.c_str());
	if (root==NULL)
		return iPass;
	root=root->child;
// {"passenger_type_name":"\u6210\u4EBA",
//	"isUserSelf":"Y",
//	"passenger_id_type_code":"1",
//	"passenger_name":"\u5468\u5F15\u534E",
//	"passenger_id_type_name":"\u4E8C\u4EE3\u8EAB\u4EFD\u8BC1",
//	"passenger_type":"1",
//	"passenger_id_no":"432423197704174851",
//	"mobile_no":"13694228708"}
	cJSON *child =root;
	while(root!=NULL) {
		CPassenger tPass;
		child =root->child;
		while(child!=NULL) {
			if (strcmp(child->string,"passenger_name")==0)	{
				tPass.m_sname = child->valuestring;
				tPass.m_sname = echttp::Utf8Decode(tPass.m_sname);
				tPass.m_check = true;
			}
			if (strcmp(child->string,"passenger_id_no")==0){
				tPass.m_sidcard = child->valuestring;
			}
			if (strcmp(child->string,"mobile_no")==0){
				tPass.m_sphone = child->valuestring;
			}
			child =child->next;
		}
		m_vPassanger.push_back(tPass);
		root = root->next;
	}
	cJSON_Print(root);
	return iPass;
}
void CTrain::LoadPassanger(void)
{
	CString fullname[MAX_PASSANGER],idcard[MAX_PASSANGER],phone[MAX_PASSANGER];
	for (int i=0;i<MAX_PASSANGER;i++)	{
		m_pdlg->GetDlgItem(IDC_FULLNAME+i)->GetWindowText(fullname[i]);
		m_pdlg->GetDlgItem(IDC_IDCARD+i)->GetWindowText(idcard[i]);
		m_pdlg->GetDlgItem(IDC_PHONE+i)->GetWindowText(phone[i]);
		m_sname[i]=fullname[i].GetBuffer();
		m_sidcard[i]=idcard[i].GetBuffer();
		m_sphone[i]=phone[i].GetBuffer();
		m_sname[i]=echttp::UrlEncode(echttp::Utf8Encode(m_sname[i]));
	}
}

void CTrain::LoadDomain(void)
{
    CString domain;
    m_pdlg->GetDlgItem(IDC_EDIT_DOMAIN)->GetWindowText(domain);
    m_sdomain=domain.GetBuffer();
    if(m_sdomain=="") 
		m_sdomain="kyfw.12306.cn";
	m_sPurl="https://"+m_sdomain+"/otn/"; //
}

void CTrain::CheckUserOnline(void)
{
    m_http->Post(m_sPurl+"login/checkUser","_json_att=",boost::bind(&CTrain::RecvCheckUserOnline,this,_1));
}

void CTrain::RecvCheckUserOnline(boost::shared_ptr<echttp::respone> respone)
{
    string result;
    if (respone->as_string()!="") {
        result=respone->as_string();
        if(result.find("data\":{\"flag\":false")!=string::npos) {
            showMsg("已掉线，请重新登录！！！");
        } else {
            showMsg("用户在线状态正常");
        }
    } else {
        showMsg("检查用户在线状态返回空白!!!");
    }
}

void CTrain::RecvNothing(boost::shared_ptr<echttp::respone> respone)
{
    return;
}

bool CTrain::CheckQueueCount(CTicket ticket, string token)
{
    string pstr="train_date="+echttp::UrlEncode(echttp::Date2UTC(ticket.start_train_date))+
		"&train_no="+ticket.train_no+
		"&stationTrainCode="+ticket.station_train_code+
		"&seatType="+ticket.seat_type+
		"&fromStationTelecode="+ticket.from_station_telecode+
		"&toStationTelecode="+ticket.to_station_telecode+
		"&leftTicket="+ticket.yp_info+
		"&purpose_codes=00&_json_att=&REPEAT_SUBMIT_TOKEN=5fed7925e6b4cce795f2091eaa041a90"+token;
    string url=m_sPurl+"confirmPassenger/getQueueCount";
    string queue_result=m_http->Post(url,pstr)->as_string();

    if(queue_result.find("op_2\":\"false")!=string::npos) {
        return true;
    } else {
        return false;
    }
}
