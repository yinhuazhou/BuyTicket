#pragma once
#include "../common.hpp"
#include "../file.hpp"

#include <boost/filesystem.hpp>

namespace echttp
{

class up_task
{
public:
    string ip;
    string port;

    size_t total_size;

    bool is_end;// 需要上传的数据是否已经全部读出
    bool header_end;
    bool is_ssl;
    up_task(string header,vector<char> data,bool isfile,size_t pos=0,size_t up_size=0)
        :is_end(false)
        ,header_end(false)
        ,is_file(isfile)
        ,pos(pos)
        ,start_pos(pos)
        ,is_ssl(false)
        ,data(data) {
        this->header=header;

        if(isfile) {
            total_size=up_size>0?up_size:fs::file_size(data);

        } else {
            total_size=data.size();
        }
    }

    size_t get_pos() {
        return pos;
    }

    vector<char> get_write_data(size_t length) {
        if(!header_end) {
            header_end=true;
            if(data.empty())
                this->is_end=true;

            return vector<char>(header.begin(),header.end());
        } else {
            if(is_file) {
                return get_file_data(length);
            } else {
                return get_char_data(length);
            }
        }
    }



private:

    bool is_file;
    vector<char> data;
    size_t start_pos;
    size_t pos;

    string header;

    vector<char> get_char_data(size_t length) {
        size_t rest_size=data.size()-pos;//剩余字节

        if(rest_size<=length) {
            this->is_end=true;
            pos+=rest_size;
            return data;
        } else {
            vector<char>buf(data.begin(),data.begin()+length);
            data.erase(data.begin(),data.begin()+length);

            pos+=length;
            return buf;
        }
    }

    vector<char> get_file_data(size_t length) {
        size_t rest_size=total_size-(pos-start_pos);

        if(rest_size<=length) {
            char *buf=new char[rest_size];
            file myfile;
            boost::system::error_code ec;
            myfile.open(data,ec);

            if(myfile.is_open()) {
                myfile.read(buf,pos,rest_size);
                pos+=rest_size;
            }

            vector<char> vectorBuf(buf,buf+rest_size);
            delete[] buf;

            this->is_end=true;
            return vectorBuf;

        } else {
            char *buf=new char[length];
            file myfile;
            boost::system::error_code ec;
            myfile.open(data,ec);

            if(myfile.is_open()) {
                myfile.read(buf,pos,length);
                pos+=length;
                vector<char> vectorBuf(buf,buf+length);
                delete[] buf;
                return vectorBuf;
            } else {
                this->is_end=true;
                return vector<char>();
            }

        }

    }

};

}