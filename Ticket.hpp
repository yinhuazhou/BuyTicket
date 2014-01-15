#pragma once
#include <string>
#include "echttp/function.hpp"

class Ticket
{
public:
    //座位信息
    string first_seat;
    string second_seat;
    string soft_bed;
    string hard_bed;
    string hard_seat;

    //车站信息
    string train_no;
    string station_train_code;//车次编号，例如K540
    string from_station_telecode;
    string from_station_name;
    string to_station_telecode;
    string to_station_name;
    string yp_info;//未知信息
    string location_code;
    string secret_str;
    string start_train_date;//乘车日期，例如20140127

    //乘车信息
    string train_date;
    string seat_type;
	string no_seat;

    Ticket(string ticket_str)
    {
        train_no=echttp::substr(ticket_str,"train_no\":\"","\"");
        from_station_telecode=echttp::substr(ticket_str,"from_station_telecode\":\"","\"");
        from_station_name=echttp::substr(ticket_str,"from_station_name\":\"","\"");
        to_station_telecode=echttp::substr(ticket_str,"to_station_telecode\":\"","\"");
        to_station_name=echttp::substr(ticket_str,"to_station_name\":\"","\"");
        yp_info=echttp::substr(ticket_str,"yp_info\":\"","\"");
        start_train_date=echttp::substr(ticket_str,"start_train_date\":\"","\"");
        train_date=echttp::DateFormat(start_train_date,"%Y-%m-%d");
        location_code=echttp::substr(ticket_str,"location_code\":\"","\"");
        secret_str=echttp::substr(ticket_str,"secretStr\":\"","\"");
        station_train_code=echttp::substr(ticket_str,"station_train_code\":\"","\"");

        first_seat=echttp::substr(ticket_str,"\"zy_num\":\"","\"");
        second_seat=echttp::substr(ticket_str,"\"ze_num\":\"","\"");
        soft_bed=echttp::substr(ticket_str,"\"rw_num\":\"","\"");
        hard_bed=echttp::substr(ticket_str,"\"yw_num\":\"","\"");
        hard_seat=echttp::substr(ticket_str,"\"yz_num\":\"","\"");
		no_seat=echttp::substr(ticket_str,"\"wz_num\":\"","\"");
    }

    void SetBuySeat(string seat)
    {
        seat_type=seat;
    }
};

