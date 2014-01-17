#pragma once
#include "../common.hpp"

namespace echttp
{
class header_option
{
public:
    // 定义option_item类型.
    typedef pair<string, string> option_item;
    // 定义option_item_list类型.
    typedef vector<option_item> option_item_list;
    // for boost::assign::insert
    typedef option_item value_type;
public:
    header_option() {}
    ~header_option() {}

public:

    // 这样就允许这样的应用:
    // http_stream s;
    // s.request_options(request_opts()("cookie","XXXXXX"));
    header_option& operator()(const string& key, const string& val) {
        insert(key, val);
        return *this;
    }

    // 添加选项, 由key/value形式添加.
    void insert(const string& key, const string& val) {
        m_opts.push_back(option_item(key, val));
    }

    // 添加选项，由 part 形式.
    void insert(value_type& item) {
        m_opts.push_back(item);
    }

    // 删除选项.
    header_option& remove(const string& key) {
        for (option_item_list::iterator i = m_opts.begin(); i != m_opts.end(); i++) {
            if (i->first == key) {
                m_opts.erase(i);
                return *this;
            }
        }
        return *this;
    }

    // 查找指定key的value.
    bool find(const string& key, string& val) const {
        string s = key;
        boost::to_lower(s);
        for (option_item_list::const_iterator f = m_opts.begin(); f != m_opts.end(); f++) {
            string temp = f->first;
            boost::to_lower(temp);
            if (temp == s) {
                val = f->second;
                return true;
            }
        }
        return false;
    }

    // 查找指定的 key 的 value. 没找到返回 ""，　这是个偷懒的帮助.
    string find(const string& key) const {
        string v;
        find(key,v);
        return v;
    }

    // 得到Header字符串.
    string header_string() const {
        string str;
        for (option_item_list::const_iterator f = m_opts.begin(); f != m_opts.end(); f++) {
            str += (f->first + ": " + f->second + "\r\n");
        }
        return str;
    }

    // 清空.
    void clear() {
        m_opts.clear();
    }

    // 返回所有option.
    option_item_list& option_all() {
        return m_opts;
    }

    // 返回当前option个数.
    int size() const {
        return m_opts.size();
    }

protected:
    option_item_list m_opts;
};

}