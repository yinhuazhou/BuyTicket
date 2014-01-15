#include "common.hpp"
#include "respone.hpp"
#include "request.hpp"
#include <boost/regex.hpp>
#include "client.hpp"
#include "iopool.hpp"
#include "detail/up_task.hpp"

namespace echttp
{
	class http
	{
	public:
		request Request;
		typedef	boost::function<void(boost::shared_ptr<respone>)> HttpCallBack;
        typedef	boost::function<void(int type,size_t total,size_t now)> StatusCallBack;

		http(void)
		{
			m_ioServ=&iopool::Instance(2)->io;
		}

		~http(void){}


        boost::shared_ptr<respone> Get(string url)
		{
			return _get("GET",url);
		}

        boost::shared_ptr<respone> Get(string url,string save_path)
		{
			return _get("GET",url,save_path);
		}


        void Get(string url,HttpCallBack cb)
		{
			_get("GET",url,cb);
			return ;
		}

        void Get(string url,string save_path,HttpCallBack cb)
		{
			_get("GET",url,save_path,cb);
			return ;
		}


        boost::shared_ptr<respone> Delete(string url)
		{
			return _get("DELETE",url);
		}


        void Delete(string url,HttpCallBack cb)
		{
            _get("DELETE",url,cb);
			return ;
		}


		boost::shared_ptr<respone> Post(string url,string data)
		{

			return _post("POST",url,data);
		}

		void Post(string url,string data,HttpCallBack cb)
		{
			_post("POST",url,data,cb);
			return ;
		}

        void Post(string url,vector<char> data,HttpCallBack cb)
		{
			_post("POST",url,data,cb);
			return ;
		}


		boost::shared_ptr<respone> Put(string url,string data)
		{
			return _post("PUT",url,data);
		}


		void Put(string url,string data,HttpCallBack cb)
		{
			_post("PUT",url,data,cb);
			return ;
		}


        void Put(string url,vector<char> data,HttpCallBack cb)
		{
			_post("PUT",url,data,cb);
			return ;
		}

        ///异步PUT请求，put的数据从文件读取
        // @url 请求url
        // @file_path 发送的文件路径
        // @cb 异步回调函数
        // @status_cb 写入，读取的状态回调。
        void PutFromFile(string url,string file_path,HttpCallBack cb,size_t pos=0,size_t size=0)
		{
            _post_file("PUT",url,file_path,cb,pos,size);
			return ;
		}

        ///同步PUT请求，put的数据从文件读取
        // @url 请求url
        // @file_path 发送的文件路径
        // @ status_cb 写入，读取的状态回调。
        boost::shared_ptr<respone> PutFromFile(string url,string file_path,size_t pos=0,size_t size=0)
		{
			return _post_file("PUT",url,file_path,pos,size);
		}

        boost::shared_ptr<respone> PutToFile(string url,string data,string save_path)
		{
			return _post("PUT",url,data,save_path);
		}

        void PutToFile(string url,string data,string save_path,HttpCallBack cb)
		{
            _post("PUT",url,data,save_path,cb);
			return ;
		}

        ///异步PUT请求，并将结果写入文件
        // @url 请求url
        // @data 发送的vector<char>数据
        // @save_path 返回数据保存的文件路径
        // @cb 异步回调函数
        // @ status_cb 写入，读取的状态回调。
        void PutToFile(string url,vector<char> data,string save_path,HttpCallBack cb)
		{
			_post("PUT",url,data,save_path,cb);
			return ;
		}

		//http client的回调，删除httpclient
		void MessageBack(boost::shared_ptr<respone> result,HttpCallBack cb,client *httpclient)
		{

			if(cb!=NULL)
			{
				Request.m_cookies.parse_header(result->header.header_string());
				cb(result);
			}

			if(httpclient)
			{
				delete httpclient;
			}
			
		}

        void RegisterStatusCallBack(StatusCallBack cb)
        {
            m_status_callback=cb;
        }

		

	private:
		boost::asio::io_service *m_ioServ;
        StatusCallBack m_status_callback;


        //类似get方法,delete之类的
        boost::shared_ptr<respone> _get(string method,string url)
		{

			up_task task=Request.make_task(method,url);

            boost::shared_ptr<respone> respone_(new respone());

            if(m_status_callback)
            {
                respone_->register_notify_callback(m_status_callback);
                m_status_callback=0;
            }

            client client(*m_ioServ,task,respone_);

			boost::shared_ptr<respone> result=client.send();

			Request.m_cookies.parse_header(result->header.header_string());

			return result;

		}

        //get to filepath
        boost::shared_ptr<respone> _get(string method,string url,string save_path)
		{

			up_task task=Request.make_task(method,url);

            boost::shared_ptr<respone> respone_(new respone());
            respone_->save_path=save_path;

            if(m_status_callback)
            {
                respone_->register_notify_callback(m_status_callback);
                m_status_callback=0;
            } //注册读写状态回调

			client client(*m_ioServ,task,respone_);

			boost::shared_ptr<respone> result=client.send();

			Request.m_cookies.parse_header(result->header.header_string());

			return result;

		}


        void _get(string method,string url,HttpCallBack cb)
		{
			up_task task=Request.make_task(method,url);

            boost::shared_ptr<respone> respone_(new respone());
            if(m_status_callback)
            {
                respone_->register_notify_callback(m_status_callback);
                m_status_callback=0;
            }

            client* httpClient=new client(*m_ioServ,task,respone_);

			httpClient->send(boost::bind(&http::MessageBack,this,_1,cb,httpClient));
			return ;
		}

        void _get(string method,string url,string save_path,HttpCallBack cb)
		{
			up_task task=Request.make_task(method,url);

            boost::shared_ptr<respone> respone_(new respone());
            respone_->save_path=save_path;
            if(m_status_callback)
            {
                respone_->register_notify_callback(m_status_callback);
                m_status_callback=0;
            }

			client* httpClient=new client(*m_ioServ,task,respone_);

			httpClient->send(boost::bind(&http::MessageBack,this,_1,cb,httpClient));
			return ;
		}



        //类似post方法
        boost::shared_ptr<respone> _post(string method,string url,string data)
		{
			up_task  task=Request.make_task(method,url,vector<char>(data.begin(),data.end()));

            boost::shared_ptr<respone> respone_(new respone());
            if(m_status_callback)
            {
                respone_->register_notify_callback(m_status_callback);
                m_status_callback=0;
            }

            client client(*m_ioServ,task,respone_);

			boost::shared_ptr<respone> respone=client.send();

			Request.m_cookies.parse_header(respone->header.header_string());

			return respone;
		}

        boost::shared_ptr<respone> _post(string method,string url,string data,string save_path)
		{
			up_task  task=Request.make_task(method,url,vector<char>(data.begin(),data.end()));

            boost::shared_ptr<respone> respone_(new respone());
            respone_->save_path=save_path;
            if(m_status_callback)
            {
                respone_->register_notify_callback(m_status_callback);
                m_status_callback=0;
            }

            client client(*m_ioServ,task,respone_);

			boost::shared_ptr<respone> respone=client.send();

			Request.m_cookies.parse_header(respone->header.header_string());

			return respone;
		}

        boost::shared_ptr<respone> _post_file(string method,string url,string file_path,size_t pos=0,size_t size=0)
		{
			up_task  task=Request.make_file_task(method,url,vector<char>(file_path.begin(),file_path.end()),pos,size);

            boost::shared_ptr<respone> respone_(new respone());
            if(m_status_callback)
            {
                respone_->register_notify_callback(m_status_callback);
                m_status_callback=0;
            }

            client client(*m_ioServ,task,respone_);

			boost::shared_ptr<respone> respone=client.send();

			Request.m_cookies.parse_header(respone->header.header_string());

			return respone;
		}

        boost::shared_ptr<respone> _post_file(string method,string url,string file_path,string save_path,size_t pos=0,size_t size=0)
		{
			up_task  task=Request.make_file_task(method,url,vector<char>(file_path.begin(),file_path.end()),pos,size);

            boost::shared_ptr<respone> respone_(new respone());
            respone_->save_path=save_path;
            if(m_status_callback)
            {
                respone_->register_notify_callback(m_status_callback);
                m_status_callback=0;
            }

            client client(*m_ioServ,task,respone_);

			boost::shared_ptr<respone> respone=client.send();

			Request.m_cookies.parse_header(respone->header.header_string());

			return respone;
		}


        void _post(string method,string url,string data,HttpCallBack cb)
		{
			up_task  task=Request.make_task(method,url,vector<char>(data.begin(),data.end()));

            boost::shared_ptr<respone> respone_(new respone());
            if(m_status_callback)
            {
                respone_->register_notify_callback(m_status_callback);
                m_status_callback=0;
            }

            client* httpClient=new client(*m_ioServ,task,respone_);

			httpClient->send(boost::bind(&http::MessageBack,this,_1,cb,httpClient));

			return ;
		}

        void _post(string method,string url,string data,string save_path,HttpCallBack cb)
		{
			up_task  task=Request.make_task(method,url,vector<char>(data.begin(),data.end()));

            boost::shared_ptr<respone> respone_(new respone());
            respone_->save_path=save_path;
            if(m_status_callback)
            {
                respone_->register_notify_callback(m_status_callback);
                m_status_callback=0;
            }

            client* httpClient=new client(*m_ioServ,task,respone_);

			httpClient->send(boost::bind(&http::MessageBack,this,_1,cb,httpClient));

			return ;
		}

        void _post_file(string method,string url,string file_path,HttpCallBack cb,size_t pos=0,size_t size=0)
		{
			up_task  task=Request.make_file_task(method,url,vector<char>(file_path.begin(),file_path.end()),pos,size);

            boost::shared_ptr<respone> respone_(new respone());
            if(m_status_callback)
            {
                respone_->register_notify_callback(m_status_callback);
                m_status_callback=0;
            }

            client* httpClient=new client(*m_ioServ,task,respone_);

			httpClient->send(boost::bind(&http::MessageBack,this,_1,cb,httpClient));

			return ;
		}

        void _post_file(string method,string url,string file_path,string save_path,HttpCallBack cb,size_t pos=0,size_t size=0)
		{
            up_task  task=Request.make_file_task(method,url,vector<char>(file_path.begin(),file_path.end()),pos,size);

            boost::shared_ptr<respone> respone_(new respone());
            respone_->save_path=save_path;
            if(m_status_callback)
            {
                respone_->register_notify_callback(m_status_callback);
                m_status_callback=0;
            }

            client* httpClient=new client(*m_ioServ,task,respone_);

			httpClient->send(boost::bind(&http::MessageBack,this,_1,cb,httpClient));

			return ;
		}

        void _post(string method,string url,vector<char> data,HttpCallBack cb)
		{
			up_task  task=Request.make_task(method,url,data);

            boost::shared_ptr<respone> respone_(new respone());
            if(m_status_callback)
            {
                respone_->register_notify_callback(m_status_callback);
                m_status_callback=0;
            }
            client* httpClient=new client(*m_ioServ,task,respone_);

			httpClient->send(boost::bind(&http::MessageBack,this,_1,cb,httpClient));

			return ;
		}

        void _post(string method,string url,vector<char> data,string save_path,HttpCallBack cb)
		{
			up_task  task=Request.make_task(method,url,data);

            boost::shared_ptr<respone> respone_(new respone());
            respone_->save_path=save_path;
            if(m_status_callback)
            {
                respone_->register_notify_callback(m_status_callback);
                m_status_callback=0;
            }
            client* httpClient=new client(*m_ioServ,task,respone_);

			httpClient->send(boost::bind(&http::MessageBack,this,_1,cb,httpClient));

			return ;
		}


	};


}

