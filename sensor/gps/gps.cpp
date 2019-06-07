#include<iostream>
#include<fstream>
//#include<stdio.h>   /* Standard input/output definitions */
//#include<stdlib.h> //for atoi
#include<string>  /* String function definitions */
#include<unistd.h>  /* UNIX standard function definitions */
#include<fcntl.h>   /* File control definitions */
#include<errno.h>   /* Error number definitions */
#include<termios.h> /* POSIX terminal control definitions */
#include<time.h>
#include<sys/time.h>
#include<vector>
using namespace std;
int openSerial(char* port);//open and initialize serial port
string getTime(void);//get system clock time
int buffSize=512;
int main(int argc,char** argv)
{ 
    //判断调用参数
    if(argc!=3)
    {
        cout<<"Usage: $sudo ./main port filename(include /path and .txt)"<<endl;
        return 0;
    }
    int com=openSerial(argv[1]);//打开串口
    int cnt=0;
    //打开串口失败 退出
    if(com==-1)
    {
        return 0;
    }
    //判断天线是否正常
    cout<<"Checking antenna..."<<endl;
    bool antenna=false;//天线标志位
    vector<string> temp;//保存10个gps信息用于检查
    for(int i=0;i<1000;i++) //0.1ms检查一次串口消息 记录10个message
    {
        auto buff=new char[buffSize];
        int size=read(com,buff,buffSize);
        if(size>10)
        {
            string s;
            s+=buff;
            temp.push_back(s);
        }
        if(temp.size()==10)
        {
            break;
        }
        usleep(100);
    }
    //检查这10个消息中标志天线的消息
    for(auto p:temp)
    {
        int size=p.size(); 
        if(p[0]=='$'&&p[1]=='G'&&p[2]=='P'&&p[3]=='T'&&p[4]=='X'&&p[5]=='T')
        {
           for(int i=6;i<size;i++)
           {
               if(p[i]=='O')
               {
                    if(p[i+1]=='K')
                    {
                        antenna=true;
                    }
                    else
                    {
                        antenna=false;
                    }
                    
               }
           }
        }
    }
    //打印天线情况
    if(!antenna)
    {
        cout<<"Please check antenna physical connection."<<endl;
        return 0;
    }
    else
    {
        cout<<"Antenna OK, data record begining."<<endl;
    }
    //主程序 记录gps信息
    while(1)
    { 
        auto buff=new char[buffSize]{};
        //char buff[512]={};
        string time=getTime();
        int flag=read(com,buff,buffSize);
        if(flag>6)//防止空消息
        {
            cout<<time+buff;
            ofstream file(argv[2],ios::app);//打开文件
            if(!file.is_open())
            {
                cout<<"File "<<argv[2]<<" is not exist."<<endl;
                return 0;
            }
            file<<time+buff;
            file.close();
        }
        usleep(100);
    }
    return 0;
}
int openSerial(char* port)
{
    int com;
    com=open(port,O_RDWR);
    string time=getTime();
    if(com!=-1)
    {
        struct termios options;//参数结构体
        tcgetattr(com, &options);//获取当前参数
        //波特率9600
        options.c_ispeed=B9600;
        options.c_ospeed=B9600;
        //8数据位 无校验 一个停止位
        options.c_cflag &= ~PARENB;
        options.c_cflag &= ~CSTOPB;
        options.c_cflag &= ~CSIZE;
        options.c_cflag |= CS8;
        tcsetattr(com, TCSANOW, &options);
        cout<<"SerialPort open success."<<endl;
    }
    else 
    {
        cout<<"SerialPort open failure."<<endl;
    }
    return com;
}
string getTime(void)
{
    timeval tv; 
	time_t timep;
    tm* timeNow;
	gettimeofday(&tv, NULL);//获取当下精确的s和us的时间
	time(&timep);//获取从1900年至今的秒数
	timeNow = gmtime(&timep); //注意tm_year和tm_mon的转换后才是实际的时间
	timeNow->tm_year+=1900;//实际年
	timeNow->tm_mon+=1;//实际月
    timeNow->tm_hour+=8;//实际小时
	long int ms = (tv.tv_sec*1000.0 + tv.tv_usec / 1000.0) - timep * 1000.0; //实际ms
    string res="";
    res+='[';
    res+=to_string(timeNow->tm_year);
    res+=' ';
    res+=to_string(timeNow->tm_mon);
    res+=' ';
    res+=to_string(timeNow->tm_mday);
    res+=' ';
    res+=to_string(timeNow->tm_hour);
    res+=':';
    res+=to_string(timeNow->tm_min);
    res+=':';
    res+=to_string(timeNow->tm_sec);
    res+=':';
    res+=to_string(int(ms));
    res+=']';
    return res;
}
