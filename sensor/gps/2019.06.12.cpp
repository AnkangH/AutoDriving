#include<iostream>
#include<fstream>
#include<string>        //for string
#include<time.h>        //for time
#include<sys/time.h>    //for time
#include<vector>        //for vector
#include<algorithm>     //for pow
//for serialPort
#include<unistd.h>  /* UNIX standard function definitions */
#include<fcntl.h>   /* File control definitions */
#include<termios.h> /* POSIX terminal control definitions */
struct gps_info
{
    bool valid=false;
    double lat=0.0;//维度
    double lon=0.0;//精度
    double speed=0.0;
    double dir=0.0;
};
using namespace std;
int openSerial(char* port);//open and initialize serial port
string getTime(void);//get system clock time
bool gpsDecode(char* input,gps_info& decode);
//true for GNRMC,otherwise not GNRMC;input for gps message;decode for decode message of it.
double str2double(string);
//2019/06/12 已验证string->double的准确性
int buffSize=512;
int main(int argc,char** argv)
{ 
    /* 
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
    cout<<"Checking antenna...maybe cost few seconds.If program stop here for a long time, means";
    cout<<" that there is no serial message"<<endl;
    bool antenna=false;//天线标志位
    vector<string> temp;//保存10个gps信息用于检查
    int antenna_error_cnt=0;//记录天线错误数目 用于提高检查的鲁棒性
    for(int i=0;i<50000;i++) //0.1ms检查一次串口消息 记录10个message
    {
        auto buff=new char[buffSize];
        int size=read(com,buff,buffSize);
        if(size>10)
        {
            string s;
            s+=buff;
            temp.push_back(s);
        }
        if(int(temp.size())>=30)
        {
           break;
        }
        if(i>30000&&temp.size()==0)
        {
            break;
        }
        usleep(1000);
        delete[] buff;
    }
    if(temp.size()==0)
    {
        cout<<" No Serial Message.Please check ttyUSB* port."<<endl;
        return 0;
    }
    //检查这50个消息中标志天线的消息
    else for(auto p:temp)
    {
        int size=p.size(); 
        if(p[0]!='$')
        {
            antenna_error_cnt++;
        }
        if(antenna_error_cnt>=10)
        {
            cout<<"Serial port message is not belong to GPS."<<endl;
            return 0;
        }
        if(p[0]=='$'&&p[1]=='G'&&p[2]=='P'&&p[3]=='T'&&p[4]=='X'&&p[5]=='T')
        {
           for(int i=6;i<size;i++)
           {
               if(p[i]=='O')
               {
                    if(p[i+1]=='K')
                    {
                        antenna=true;
                        cout<<"Antenna OK, data record begining."<<endl;
                        break;
                    }
                    else
                    {
                        cout<<"Please check antenna physical connection."<<endl;
                        return 0;
                    }
                    
               }
           }
        }
        if(antenna)
        {
            break;
        }
    }
    //主程序 记录gps信息
    while(1)
    { 
        auto buff=new char[buffSize]{};
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
        delete[] buff;
    }*/
    return 0;
}
int openSerial(char* port)
{
    int com;
    com=open(port,O_RDWR);
    string time=getTime();
    if(com!=-1)
    {
        termios options;//参数结构体
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
bool gpsDecode(char* str,gps_info& info)
{
    if(str[1]=='G'&&str[2]=='N'&&str[3]=='R'&&str[4]=='M'&&str[5]=='C')
    {
        //根据NMEA分割消息
        vector<string> s;
        string temp;
        int cnt;
        while(*str!='\0')
        {
            if(s.size()==14)
            {
                break;
            }
            if(*str==',')
            {
                s.push_back(temp);
                temp.clear();
            }
            else
            {
                temp+=*str;
            }
            str++; 
        }
        if(s[2]=="A")//定位有效
        {
            info.valid=true;
            double lat=str2double(s[3]);
            if(s[4]=="N")
            {
                info.lat=lat;
            }
            else
            {
                info.lat=-lat;
            }
            double lon=str2double(s[4]);
            if(s[4]=="E")
            {
                info.lon=lon;
            }
            else
            {
                info.lon=-lon;
            }
        }
        else
        {
            info.valid=false;
        }
        
        return true;  
        }
    else 
    {
        return false;
    }
}
double str2double(string str)
{
    int size=str.size();
    if(size==0)
    {
        return 0;
    }
    int pos=0;
    double res=0.0;
    for(int i=0;i<size;i++)
    {
        if(str[i]=='.')
        {
            pos=i;
            break;
        }
    }
    for(int i=0;i<size;i++)
    {
        if(i<pos)
        {
            res+=(str[i]-'0')*pow(10,pos-i-1);
        }
        if(i>pos)
        {
            res+=(str[i]-'0')*pow(10,pos-i);
        }
    }
    return res;
}
