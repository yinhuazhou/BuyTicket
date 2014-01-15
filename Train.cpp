#include "stdafx.h"
#include "echttp/http.hpp"
#include "Train.h"
#include <iostream>
#include <fstream>
#include "BuyTicketDlg.h"
#include "VerifyDlg.h"
#include <WinInet.h>
#include "xxtea.h"
#include <boost/random/mersenne_twister.hpp>
#include <boost/random/uniform_int_distribution.hpp>
#include "Ticket.hpp"

#pragma comment(lib,"Wininet.lib")

boost::random::mt19937 rand_gen;

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
    buy_list=new queue<Ticket>();
    LoadStation();

    m_http->Request.set_defalut_userAgent("Mozilla/5.0 (compatible; MSIE 9.0; qdesk 2.5.1177.202; Windows NT 6.1; WOW64; Trident/6.0)");
    LoadDomain();

    m_http->Get(m_sPurl);
    m_http->Request.m_header.insert("Referer",m_sPurl);

    string initUrl=m_http->Get(m_sPurl+"login/init")->as_string();

    if(initUrl.find("/otn/dynamicJs/loginJs")!=string::npos) {
        string loginjs=echttp::substr(initUrl,"/otn/dynamicJs/loginJs","\"");
        string loginjs2=echttp::substr(initUrl,"/otn/resources/merged/login_js.js?scriptVersion=","\"");

        m_http->Request.set_defalut_referer(m_sPurl+"login/init");
        m_http->Get(m_sPurl+"resources/merged/login_js.js?scriptVersion="+loginjs2);
        string ret= m_http->Get(m_sPurl+"dynamicJs/loginJs"+loginjs)->as_string();
        encrypt_code(ret);

        //判断是否有隐藏随机监测url
        string ready_str=echttp::substr(ret,"$(document).ready(function(){","success");
        if(ready_str.find("jq({url :'")!=string::npos) {
            string url=echttp::substr(ready_str,"jq({url :'","'");
            m_http->Post("https://"+m_sdomain+""+url,"");
        }
    } else {
        m_pdlg->m_listbox.AddString("获取登录信息异常！");
    }
}


CTrain::~CTrain(void)
{
    delete m_http;
}

//从加密js生成验证信息
void CTrain::encrypt_code(string src)
{
    if(src.find("gc(){var key='")==string::npos) {
        encrypt_str="";
    } else {
        string key=echttp::substr(src,"gc(){var key='","';");
        string code=xxtea_encode("1111",key);
        encrypt_str="&"+echttp::UrlEncode(key)+"="+echttp::UrlEncode(code);
    }
}

bool CTrain::Login(string username, string password, string code)
{
    m_suname=username;
    m_supass=password;
    m_syzcode=code;
    ofstream file(CONFIG_PATH"\\登录错误.txt",ios::app);
    m_http->Request.m_header.insert("x-requested-with","XMLHttpRequest");
    string pstr="loginUserDTO.user_name="+m_suname+"&userDTO.password="+m_supass+"&randCode="+m_syzcode;
    string res=m_http->Post(m_sPurl+"login/loginAysnSuggest",pstr)->as_string();
    res=echttp::Utf8Decode(res);

    if(res.find("{\"loginCheck\":\"Y\"}")!=string::npos) {
        m_pdlg->m_listbox.AddString("登录成功");
        SetCookie(m_http->Request.m_cookies.cookie_string());
        m_http->Post(m_sPurl+"login/init","_json_att=");
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
    m_http->Request.set_defalut_referer(m_sPurl+"login/init");
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
    m_http->Request.set_defalut_referer(m_sPurl+"leftTicket/init");
    m_http->Get(m_sPurl+"leftTicket/init",boost::bind(&CTrain::RecvSearchTicketPage,this,_1));
    CheckUserOnline();
}

void CTrain::RecvSearchTicketPage(boost::shared_ptr<echttp::respone> respone)
{
    string sources;
    if (respone->as_string()!="") {
        sources=respone->as_string();
        if(sources.find("/otn/dynamicJs/queryJs")!=string::npos) {
            string authJs=echttp::substr(sources,"/otn/dynamicJs/queryJs","\"");
            m_http->Request.set_defalut_referer(m_sPurl+"leftTicket/init");
            string ret= m_http->Get(m_sPurl+"dynamicJs/queryJs"+authJs)->as_string();
            encrypt_code(ret);
            //判断是否有隐藏随机监测url
            string ready_str=echttp::substr(ret,"$(document).ready(function(){","success");
            if(ready_str.find("jq({url :'")!=string::npos) {
                string url=echttp::substr(ready_str,"jq({url :'","'");
                m_http->Post("https://"+m_sdomain+""+url,"");
            }
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
    m_http->Request.set_defalut_referer(m_sPurl+"leftTicket/init");

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
    string restr=echttp::Utf8Decode(respone->as_string());
    if(restr!=""&&restr!="-10" &&restr.find("queryLeftNewDTO")!=string::npos) {
        ofstream webfile(CONFIG_PATH"\\web.txt",ios::app);
        webfile<<restr<<"\r\n";
        webfile.close();
        while(restr.find("queryLeftNewDTO")!=string::npos) {
            string ticketInfo=echttp::substr(restr,"queryLeftNewDTO","{\"queryLeftNewDTO");
            Ticket ticket(ticketInfo);
            restr=restr.substr(restr.find("queryLeftNewDTO")+5);
            string trainstr=echttp::substr(restr,"_train_code\":\"","\"");
            if(m_strain!=""&&m_strain.find(trainstr)==string::npos) {
                continue;
            }
            if(isTicketEnough(ticket.first_seat)&&BST_CHECKED==m_pdlg->IsDlgButtonChecked(IDC_CHECK_YDZ)) {
                if(restr.find("secretStr")!=string::npos) {
                    showMsg(trainstr+"有一等座"+"---车票数目:"+ticket.first_seat);
                    ticket.SetBuySeat("M");
                    buy_list->push(ticket);
                } else {
                    showMsg("未知一等座余票信息");
                }
            }
            if(isTicketEnough(ticket.second_seat)&&BST_CHECKED==m_pdlg->IsDlgButtonChecked(IDC_CHECK_EDZ)) {
                if(restr.find("secretStr")!=string::npos) {
                    ticket.SetBuySeat("O");
                    buy_list->push(ticket);
                    showMsg(trainstr+"有二等座"+"---车票数目:"+ticket.second_seat);
                } else {
                    showMsg("未知二等座余票信息");
                }
            }
            if(isTicketEnough(ticket.soft_bed)&&BST_CHECKED==m_pdlg->IsDlgButtonChecked(IDC_CHECK_RW)) {
                if(restr.find("secretStr")!=string::npos) {
                    ticket.SetBuySeat("4");
                    buy_list->push(ticket);
                    showMsg(trainstr+"有软卧"+"---车票数目:"+ticket.soft_bed);
                } else {
                    showMsg("未知软卧余票信息");
                }
            }

            if(isTicketEnough(ticket.hard_bed)&&BST_CHECKED==m_pdlg->IsDlgButtonChecked(IDC_CHECK_YW)) {
                if(restr.find("secretStr")!=string::npos) {
                    ticket.SetBuySeat("3");
                    buy_list->push(ticket);
                    showMsg(trainstr+"有卧铺"+"---车票数目:"+ticket.hard_bed);
                } else {
                    showMsg("未知卧铺余票信息");
                }
            }

            if(isTicketEnough(ticket.hard_seat) &&BST_CHECKED==m_pdlg->IsDlgButtonChecked(IDC_CHECK_YZ)) {
                showMsg(trainstr+"有硬座"+"---车票数目:"+ticket.hard_seat);
                if(restr.find("secretStr")!=string::npos) {
                    ticket.SetBuySeat("1");
                    buy_list->push(ticket);
                } else {
                    showMsg("未知硬座信息");
                }
            }
            if(isTicketEnough(ticket.no_seat) &&BST_CHECKED==m_pdlg->IsDlgButtonChecked(IDC_CHK_WZ)) {
                showMsg(trainstr+"无座"+"---车票数目:"+ticket.no_seat);
                if(restr.find("secretStr")!=string::npos) {
                    ticket.SetBuySeat("empty");
                    buy_list->push(ticket);
                } else {
                    showMsg("未知无座信息");
                }
            }
        }

        //如果检测到相应的票，就下单
        int queue_size=buy_list->size();
        if(queue_size>0 && !buy_list->empty()) {
            Ticket task_ticket=buy_list->front();
            submitOrder(task_ticket);
            showMsg("开始购买:"+task_ticket.station_train_code);

            while (buy_list->size()>0) {
                if(queue_size==buy_list->size()) { //如果当前队列数，和之前保留的队列数一致，则有任务在执行
                    continue;
                    Sleep(50);
                }

                queue_size=buy_list->size();
                if(!buy_list->empty()) {
                    Ticket task_ticket=buy_list->front();
                    submitOrder(task_ticket);
                    showMsg("开始购买:"+task_ticket.station_train_code);
                }
            }
        } else {
            showMsg("没有卧铺或者硬座");
        }
    } else {
        showMsg("发生错误，请检查参数:"+echttp::Utf8Decode(echttp::substr(restr,"\"messages\":\[\"","\"")));
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

bool CTrain::submitOrder(Ticket ticket)
{
    m_isInBuy=true;
    string pstr="secretStr="+ticket.secret_str+"&train_date="+ticket.train_date+"&back_train_date=2014-01-01&tour_flag=dc&purpose_codes=ADULT"+
                     "&query_from_station_name="+ticket.from_station_name+"&query_to_station_name="+ticket.to_station_name+"&undefined";
    showMsg("尝试买:"+ticket.station_train_code);
    boost::shared_ptr<echttp::respone> ret=m_http->Post(m_sPurl+"leftTicket/submitOrderRequest",pstr);
    string recvStr=ret->as_string();
    if(recvStr.find("status\":true")!=string::npos) {
        m_http->Post(m_sPurl+"confirmPassenger/initDc","_json_att=",boost::bind(&CTrain::RecvSubmitOrder,this,_1,ticket));
    } else {
        showMsg("预定错误!"+echttp::Utf8Decode(echttp::substr(recvStr,"\"messages\":\[\"","\"")));
        buy_list->pop();
    }
    return false;
}

void CTrain::RecvSubmitOrder(boost::shared_ptr<echttp::respone> respone,Ticket ticket)
{
    string restr=echttp::Utf8Decode(respone->as_string());
    LoadPassanger();
    m_http->Request.set_defalut_referer(m_sPurl+"confirmPassenger/initDc");
#ifdef _VERSION_1_
    if(restr.find("/otsweb/dynamicJsAction.do")!=string::npos)   {
       string authJs=echttp::substr(restr,"/otsweb/dynamicJsAction.do","\"");
        string ret= m_http->Get("https://dynamic.12306.cn/otsweb/dynamicJsAction.do"+authJs)->as_string();
    	//判断是否有隐藏随机监测url
    	string ready_str=echttp::substr(ret,"$(document).ready(function(){","success");
    	if(ready_str.find("jq({url :'")!=string::npos) 	{
    		string url=echttp::substr(ready_str,"jq({url :'","'");
    		m_http->Post("https://dynamic.12306.cn"+url,"");
    	}
   }
#endif // _VERSION_1_
    string TOKEN=echttp::substr(restr,"globalRepeatSubmitToken = '","'");
    string keyCheck=echttp::substr(restr,"'key_check_isChange':'","'");
    string leftTicketStr=echttp::substr(restr,"leftTicketStr':'","'");
    string trainLocation=echttp::substr(restr,"train_location':'","'");
    string checkbox2="";
    string seattype=ticket.seat_type;//座位类型 3为卧铺 1为硬座

    string code_path=loadCode2();
    CVerifyDlg VerifyDlg;
    VerifyDlg.m_pTrain=this;//传递本类指针到对话框
    VerifyDlg.file_path=code_path;
    if(VerifyDlg.DoModal()) {
        showMsg("延时一下，过快会被封！");
        //Sleep(1000);
checkcode:
        string randcode=VerifyDlg.yzcode;
#ifdef _VERSION_1_
        string user2info="oldPassengers=&checkbox9=Y";
        m_http->Get("https://dynamic.12306.cn/otsweb/order/traceAction.do?method=logClickPassenger&passenger_name="+myName+"&passenger_id_no="+IdCard+"&action=checked");
        if(myName2!="")  {
        	user2info="passengerTickets="+seattype+"%2C0%2C1%2C"+myName2+"%2C1%2C"+IdCard2+"%2C"+Phone2
        		+"%2CY&oldPassengers="+myName2+"%2C1%2C"+IdCard2+"&passenger_2_seat="
        		+seattype+"&passenger_2_ticket=1&passenger_2_name="
        		+myName2+"&passenger_2_cardtype=1&passenger_2_cardno="
        		+IdCard2+"&passenger_2_mobileno="+Phone2
        		+"&checkbox9=Y";
        	checkbox2="&checkbox1=1";
        	m_http->Get("https://dynamic.12306.cn/otsweb/order/traceAction.do?method=logClickPassenger&passenger_name="+myName2+"&passenger_id_no="+IdCard2+"&action=checked");
        }
#endif // _VERSION_1_
        string passanger_info;

		if(m_sname[2]!="")	{
			passanger_info=
				seattype+"%2C0%2C1%2C"+m_sname[0]+"%2C1%2C"+m_sidcard[0]+"%2C"+m_sphone[0]+"%2CN_"+
				seattype+"%2C0%2C1%2C"+m_sname[1]+"%2C1%2C"+m_sidcard[1]+"%2C"+m_sphone[1]+"%2CN_"+
				seattype+"%2C0%2C1%2C"+m_sname[2]+"%2C1%2C"+m_sidcard[2]+"%2C"+m_sphone[2]
				+"%2CN&oldPassengerStr="+m_sname[0]+"%2C1%2C"+m_sidcard[0]+"%2C1_"+
					m_sname[1]+"%2C1%2C"+m_sidcard[1]+"%2C1_"+
					m_sname[2]+"%2C1%2C"+m_sidcard[2]+"%2C1_";
		} else if(m_sname[1]!="") {
			passanger_info=
				seattype+"%2C0%2C1%2C"+m_sname[0]+"%2C1%2C"+m_sidcard[0]+"%2C"+m_sphone[0]+"%2CN_"+
				seattype+"%2C0%2C1%2C"+m_sname[1]+"%2C1%2C"+m_sidcard[1]+"%2C"+m_sphone[1]
				+"%2CN&oldPassengerStr="+m_sname[0]+"%2C1%2C"+m_sidcard[0]+"%2C1_"+
					m_sname[1]+"%2C1%2C"+m_sidcard[1]+"%2C1_";
		}else{
			passanger_info=
				seattype+"%2C0%2C1%2C"+m_sname[0]+"%2C1%2C"+m_sidcard[0]+"%2C"+m_sphone[0]
				+"%2CN&oldPassengerStr="+m_sname[0]+"%2C1%2C"+m_sidcard[0]+"%2C1_";
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
                buy_list->pop();
                return;
            }
            m_http->Request.m_header.insert("Content-Type","application/x-www-form-urlencoded; charset=UTF-8");
            m_http->Request.m_header.insert("X-Requested-With","XMLHttpRequest");
            pstr="passengerTicketStr="+passanger_info+"&randCode="+randcode+"&purpose_codes=00"+
                 +"&key_check_isChange="+keyCheck+"&leftTicketStr="+leftTicketStr+"&train_location="+trainLocation
                 +"&_json_att=&REPEAT_SUBMIT_TOKEN="+TOKEN;
            //提交订单
            m_http->Post(m_sPurl+"confirmPassenger/confirmSingleForQueue",pstr,boost::bind(&CTrain::confrimOrder,this,_1,pstr));
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
                buy_list->pop();
            }
        } else {
            showMsg("检查订单操作错误:"+echttp::substr(checkstr,"errMsg","}"));
            buy_list->pop();
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
    m_http->Request.m_header.insert("Referer",m_sPurl+"confirmPassenger/initDc");
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
            buy_list->pop();
        } else if(result.find("请重试")!=string::npos) {
            if(!m_Success) {
                Sleep(1000);
                m_http->Post(m_sPurl+"confirmPassenger/confirmSingleForQueue",pstr,boost::bind(&CTrain::confrimOrder,this,_1,pstr));
                showMsg(result+"继续抢!");
            }
        } else {
            showMsg(result);
            buy_list->pop();
        }
    } else {
        if(!m_Success) {
            m_http->Post(m_sPurl+"confirmPassenger/confirmSingleForQueue",pstr,boost::bind(&CTrain::confrimOrder,this,_1,pstr));
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
// 			TRACE( " %s\n", sname );
			// Get next token: 
			token = strtok( NULL, seps ); // C4996
			scode = token;
// 			TRACE( " %s\n", scode );
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

bool CTrain::CheckQueueCount(Ticket ticket, string token)
{
    string pstr="train_date="+echttp::UrlEncode(echttp::Date2UTC(ticket.start_train_date))+"&train_no="+ticket.train_no
                     +"&stationTrainCode="+ticket.station_train_code+"&seatType="+ticket.seat_type+"&fromStationTelecode="
                     +ticket.from_station_telecode+"&toStationTelecode="+ticket.to_station_telecode+"&leftTicket="+ticket.yp_info
                     +"&purpose_codes=00&_json_att=&REPEAT_SUBMIT_TOKEN=5fed7925e6b4cce795f2091eaa041a90"+token;

    string url=m_sPurl+"confirmPassenger/getQueueCount";
    string queue_result=m_http->Post(url,pstr)->as_string();

    if(queue_result.find("op_2\":\"false")!=string::npos) {
        return true;
    } else {
        return false;
    }
}
