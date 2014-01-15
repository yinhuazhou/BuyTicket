#pragma once

#include "common.hpp"
#include <map>
#include "detail/header.hpp"
#include <boost/regex.hpp>
#include "file.hpp"

namespace  echttp{

class respone
{
public:
    typedef	boost::function<void(int type,size_t total,size_t now)> StatusCallBack;

    respone();
    ~respone();

    int         error_code;
	string error_msg;//错误信息

    int status_code;//http状态码
    header_option header;//返回头部

    size_t length;//返回内容大小
	vector<char> body;//返回内容

    string save_path;//保存文件path，若下载文件，则此值为文件路径

	bool parse_header(string);

    bool save_body(vector<char> buffer);

    void notify_status(int type,size_t total,size_t now);

    void register_notify_callback(StatusCallBack cb);

	string as_string();

private:
    StatusCallBack m_status_cb;

};

respone::respone()
    :m_status_cb(0)
	,length(0)
{
}

respone::~respone()
{
}

bool respone::parse_header(string header_str)
{
	if(header_str.find("HTTP")!=string::npos)
	{
		string h=header_str.substr(header_str.find(" ")+1);
		h=h.substr(0,h.find(" "));
		status_code=convert<int,string>(h);

		boost::smatch result;
		string regtxt("\\b(.+?): (.*?)\r\n");
		boost::regex rx(regtxt);

		string::const_iterator it=header_str.begin();
		string::const_iterator end=header_str.end();

		while (regex_search(it,end,result,rx))
		{
			string key=result[1];
			string value=result[2];
			header.insert(key,value);
			it=result[0].second;
		}
        return true;

	}else
	{
		status_code=-1;
        error_code=10;
        error_msg="inviald header string.";
        return false;
	}
}

bool respone::save_body(vector<char> buffer)
{
    if (save_path=="")
    {
        body.insert(body.end(),buffer.begin(),buffer.end());
        length+=buffer.size();
    }else
    {
        file myfile;
        boost::system::error_code ec;
        myfile.open(save_path,ec);

        if(myfile.is_open())
        {
            myfile.write(&buffer.front(),length,buffer.size());
            length+=buffer.size();
        }
        myfile.close();
    }
	return true;
}

void respone::register_notify_callback(StatusCallBack cb)
{
    m_status_cb=cb;
}

void respone::notify_status(int type,size_t total,size_t now)
{
    if(m_status_cb)
    {
        m_status_cb(type,total,now);
    }
}

string respone::as_string()
{
	return string(body.begin(),body.end());
}

	
}