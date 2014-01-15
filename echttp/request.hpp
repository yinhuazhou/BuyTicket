#pragma once

#include <map>
#include "common.hpp"
#include "function.hpp"
#include "cookie.hpp"
#include "url.hpp"
#include "file.hpp"
#include "detail/escape_string.hpp"
#include "detail/header.hpp"
#include "detail/up_task.hpp"

namespace echttp
{

class request
{
public:

	cookie_option m_cookies;
	header_option m_header;

    string proxy_ip;
    string proxy_port;

	request(void)
	{
		m_defalut_user_agent="Echoes Http Client";
        m_defalut_connection="Keep-Alive";
        m_defalut_accept="*/*";
	}

	~request(void)
	{
	}

	void set_defalut_userAgent(string user_agent)
	{
		m_defalut_user_agent=user_agent;
	}

	void set_defalut_accept(string accept)
	{
		m_defalut_accept=accept;
	}

	void set_defalut_connection(string connection)
	{
		m_defalut_connection=connection;
	}

	void set_defalut_referer(string referer)
	{
		m_defalut_referer=referer;
	}
    

    up_task make_task(string method,const url &u)
    {
        up_task task(get_header(method,u),vector<char>(),false);
        set_task_connection(task,u);
        return task;
    }

    up_task make_task(string method, const url &u,vector<char> data)
    {
		if (m_header.find("Content-Length")=="")
        {
			m_header.insert("Content-Length",echttp::convert<string>(data.size()));
		}
        if(method=="POST" && m_header.find("Content-Type")=="")
        {
            m_header.insert("Content-Type","application/x-www-form-urlencoded");
        }

        up_task task(get_header(method,u),data,false);
        set_task_connection(task,u);
        return task;
        
    }

    up_task make_file_task(string method,const url &u,vector<char> path,size_t pos=0,size_t size=0)
    {
        if (m_header.find("Content-Length")=="")
        {
            size_t file_size=fs::file_size(path);
            m_header.insert("Content-Length",echttp::convert<string>(file_size));
        }
       
        if(method=="POST" && m_header.find("Content-Type")=="")
        {
            m_header.insert("Content-Type","application/x-www-form-urlencoded");
        }

        up_task task(get_header(method,u),path,true,pos,size);
        set_task_connection(task,u);
        return task;
    }

private:
    string m_defalut_user_agent;
    string m_defalut_connection;
    string m_defalut_accept;
	string m_defalut_referer;

    string get_header(string method,const url &u)
    {
        set_common_header();
        m_header.insert("Host",u.host());

        string cookie_string=m_cookies.cookie_string();
        if(cookie_string!="")
        {
            m_header.insert("Cookie",cookie_string);
        }

        string uri=u.request_uri();
        if(proxy_ip!="" && proxy_port!="")
        {
            uri=u.protocol()+"://"+u.host()+uri;
        }

        string header=method+" "+uri+" HTTP/1.1\r\n";
        header+=m_header.header_string();
        header+="\r\n";

        m_header.clear();
        return header;
    }

    void set_common_header()
    {
        if(m_header.find("User-Agent")=="")
        {
            m_header.insert("User-Agent",m_defalut_user_agent);
        }

        if(m_header.find("Connection")=="")
        {
            m_header.insert("Connection",m_defalut_connection);
        }

        if(m_header.find("Accept")=="")
        {
            m_header.insert("Accept",m_defalut_accept);
        }

		if(m_header.find("Referer")=="")
        {
            m_header.insert("Referer",m_defalut_referer);
        }
    }

	void set_task_connection(up_task &task,const url &u)
    {
        if(proxy_ip!="" && proxy_port!="")
        {
            task.ip=proxy_ip;
            task.port=proxy_port;
        }else
		{
			task.ip=u.ip();
			task.port=echttp::convert<string>(u.port());
		}
        if(u.protocol()=="https")
        {
            task.is_ssl=true;
        }

    }



};
}
