#pragma once
#ifndef _ECHTTP_COMMON_
#define _ECHTTP_COMMON_
#include <string>
#include "function.hpp"
#include <boost/smart_ptr.hpp>
#include <boost/function.hpp>
#include <boost/bind.hpp>
using namespace std;

namespace  echttp
{
struct ClientResult {
    int errorCode;
    boost::shared_array<char> msg;
    size_t len;
    string header;
    string errMsg;
};
}
#endif // _ECHTTP_COMMON_

