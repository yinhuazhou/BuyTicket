BuyTicket
=========

for 12306 Buy Ticket write in C++ 

在https://github.com/qhgongzi/xilinTicket 2014年1月14日

的基础上作了以下更改：

1、将所有日志和配置信息放在当前程序目录 下的12306目录中；

2、将station  map表从12306目录下的City_code.txt文件中加载，这样有便于在

不得新编译的情况下增加以后可能出现的站名和其对应的code。

3、去掉了所有this-> 和std:: 。

4、将//乘客信息 passanger infomation用数组表示。
    string m_sname[MAX_PASSANGER];
    string m_sidcard[MAX_PASSANGER];
    string m_sphone[MAX_PASSANGER];

另外resource.h 的定义 ：

#define IDC_FULLNAME                    1110
#define IDC_FULLNAME2                   1111
#define IDC_FULLNAME3                   1112
#define IDC_IDCARD                      1120
#define IDC_IDCARD2                     1121
#define IDC_IDCARD3                     1122
#define IDC_PHONE                       1130
#define IDC_PHONE2                      1131
#define IDC_PHONE3                      1132

也采用连续的，为了更好看使用以下语句：

for (int i=0;i<MAX_PASSANGER;i++) {
   getline(userfile,name[i]);
   getline(userfile,id[i]);
   getline(userfile,phone[i]);
   GetDlgItem(IDC_FULLNAME+i)->SetWindowText(name[i].c_str());
   GetDlgItem(IDC_IDCARD+i)->SetWindowText(id[i].c_str());
   GetDlgItem(IDC_PHONE+i)->SetWindowText(phone[i].c_str());
  }

 

接下来准备做的是将12306中的旅客信息获取出来回显在对话框上。
