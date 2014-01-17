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
    //��λ��Ϣ
	string m_seat[13];
	bool m_bCheck[13];
    //��վ��Ϣ
    string train_no;
    string station_train_code;//���α�ţ�����K540
    string from_station_telecode;
    string from_station_name;
    string to_station_telecode;
    string to_station_name;
	string start_time;
	string arrive_time;
	string lishi;
    string yp_info;//δ֪��Ϣ
    string location_code;
    string secret_str;
    string start_train_date;//�˳����ڣ�����20140127

    //�˳���Ϣ
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
