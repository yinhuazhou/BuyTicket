#pragma once
#ifndef _TICKET_HEADER_
#define _TICKET_HEADER_
#include <string>

typedef struct tagSEATMAP{
	string sCode;			
	string sReadable;			
	string sQuery;
} SEATMAP;

extern SEATMAP g_seatlist[];
extern const int g_sSize;
class CTicket
{
public:
    //座位信息
	string m_seat[13];
	bool m_bCheck[13];
    //车站信息
    string train_no;
    string station_train_code;//车次编号，例如K540
    string from_station_telecode;
    string from_station_name;
    string to_station_telecode;
    string to_station_name;
	string start_time;
	string arrive_time;
	string lishi;
    string yp_info;//未知信息
    string location_code;
    string secret_str;
    string start_train_date;//乘车日期，例如20140127

    //乘车信息
    string train_date;
    string seat_type;
	string canWebBuy;
	CTicket(string ticket_str);
	bool IscanWebBuy();
    void SetBuySeat(string seat)
    {
        seat_type=seat;
    }
};

#endif // _TICKET_HEADER_
