BuyTicket
=========

for 12306 Buy Ticket write in C++ 

��https://github.com/qhgongzi/xilinTicket 2014��1��14��

�Ļ������������¸��ģ�

1����������־��������Ϣ���ڵ�ǰ����Ŀ¼ �µ�12306Ŀ¼�У�

2����station  map���12306Ŀ¼�µ�City_code.txt�ļ��м��أ������б�����

�����±��������������Ժ���ܳ��ֵ�վ�������Ӧ��code��

3��ȥ��������this-> ��std:: ��

4����//�˿���Ϣ passanger infomation�������ʾ��
    string m_sname[MAX_PASSANGER];
    string m_sidcard[MAX_PASSANGER];
    string m_sphone[MAX_PASSANGER];

����resource.h �Ķ��� ��

#define IDC_FULLNAME                    1110
#define IDC_FULLNAME2                   1111
#define IDC_FULLNAME3                   1112
#define IDC_IDCARD                      1120
#define IDC_IDCARD2                     1121
#define IDC_IDCARD3                     1122
#define IDC_PHONE                       1130
#define IDC_PHONE2                      1131
#define IDC_PHONE3                      1132

Ҳ���������ģ�Ϊ�˸��ÿ�ʹ��������䣺

for (int i=0;i<MAX_PASSANGER;i++) {
   getline(userfile,name[i]);
   getline(userfile,id[i]);
   getline(userfile,phone[i]);
   GetDlgItem(IDC_FULLNAME+i)->SetWindowText(name[i].c_str());
   GetDlgItem(IDC_IDCARD+i)->SetWindowText(id[i].c_str());
   GetDlgItem(IDC_PHONE+i)->SetWindowText(phone[i].c_str());
  }

 

������׼�������ǽ�12306�е��ÿ���Ϣ��ȡ���������ڶԻ����ϡ�
