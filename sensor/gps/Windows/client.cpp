#include<iostream>
#include<fstream>			//for ifstream
#include<WinSock2.h>		//for socket
#include<string>
#include<vector>
#pragma comment(lib, "ws2_32.lib")
using namespace std;
vector<pair<double, double>> getPointsFromFile(string filename);//从gps记录获取原始经纬度
vector<pair<double, double>> transPoint(string filename);//利用web service将原始经纬度转换为高度地图经纬度
pair<double, double> getGpsFromBuffer(string s);
int main()
{
	string file = "1_decode.txt";
	auto point = transPoint(file);
	ofstream out("1_decode_trans.txt");
	out.setf(ios_base::fixed);
	out.precision(6);
	for (auto p : point)
		out << p.first << "," << p.second << endl;
	out.close();
	return 0;
}
vector<pair<double, double>> getPointsFromFile(string filename)
{
	ifstream inFile(filename);
	if (!inFile.is_open())
		cout << "Can't open gps1_decode.txt." << endl;
	vector<pair<double, double>> point;
	string str;
	while (getline(inFile, str))
	{
		//string str;
		//getline(inFile, str);
		int size = str.size();
		if (size == 0)
		{
			cout << "Empty file,abort." << endl;
			return point;
		}
		string lat;
		string lon;
		for (int i = 0; i < size; i++)
		{
			if (str[i] == ']')
			{
				for (int j = i + 1; j < size; j++)
				{
					if (str[j] == 'l' && str[j + 1] == 'a' && str[j + 2] == 't' && str[j + 3] == '=')
					{
						for (int k = j + 4; k < size; k++)
						{
							lat += str[k];
							if (str[k] == ',')
								break;
						}
						//cout << lat << endl;
					}
					if (str[j] == 'l' && str[j + 1] == 'o' && str[j + 2] == 'n' && str[j + 3] == '=')
					{
						for (int k = j + 4; k < size; k++)
						{
							lon += str[k];
							if (str[k] == ',')
								break;
						}
						//cout << lon;
					}
				}
				if (!lon.empty())
					break;//跳出本行
			}
		}
		point.push_back(pair<double, double>{stod(lat), stod(lon)});
		lat.clear();
		lon.clear();
	}
	return point;
}
pair<double, double> getGpsFromBuffer(string s)
{
	int n = s.size();
	pair<double, double> res;
	for (int i = 0; i < n; i++)
	{
		if (s[i] == '<' && s[i + 1] == 'l' && s[i + 2] == 'o' && s[i + 3] == 'c' && s[i + 4] == 'a' && s[i + 5] == 't')
		{
			int k = i + 11;
			string temp;
			while (s[k] != '<')
			{
				if(s[k]!=',')
					temp += s[k];
				else
				{
					res.first = stod(temp);
					temp.clear();
				}
				k++;
			}
			res.second = stod(temp);
			return res;
		}
	}
	return res;
}
vector<pair<double, double>> transPoint(string filename)
{
	auto pointsOriginal = getPointsFromFile(filename);
	int n = pointsOriginal.size();
	auto res = pointsOriginal;
	cout.setf(ios_base::fixed);
	cout.precision(6);
	for (int i = 0; i < n; i++)
	{
		WSADATA wsaData;
		WSAStartup(MAKEWORD(2, 2), &wsaData);//初始化
		string hostName = "restapi.amap.com";//高德地图api域名
		struct hostent* p_host = gethostbyname(hostName.c_str());//域名->ip地址
		if (p_host == nullptr)
		{
			cout << "DNS failur,abort." << endl;
			exit(EXIT_FAILURE);
		}
		sockaddr_in addr_server;
		addr_server.sin_family = AF_INET;
		addr_server.sin_port = htons(80);//默认端口80
		memcpy(&(addr_server.sin_addr), p_host->h_addr_list[0], sizeof(addr_server.sin_addr));
		int sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);//tcp通讯
		int error = connect(sock, (sockaddr*)& addr_server, sizeof(addr_server));//三次握手建立通讯
		if (error == -1)
		{
			cout << "Connect failed " << endl;
			closesocket(sock);
			exit(EXIT_FAILURE);
		}
		string m_data = "/v3/assistant/coordinate/convert?locations=";//服务类型对应url
		string back = "&coordsys=gps&output=xml";//控制参数对应url
		string position = to_string(pointsOriginal[i].second) + "," + to_string(pointsOriginal[i].first);//获取经纬度
		string key = "&key=61921621324d0c4c99eaf19c3f605baa";//用户密匙
		m_data += position;
		m_data += back;
		m_data += key;
		string sendData = "GET " + m_data + " HTTP/1.1\r\n"; //GET后要有空格 HTTP前要有空格
		sendData += "Host:" + hostName + "\r\n";
		sendData += "Connection:close\r\n";
		sendData += "\r\n";
		send(sock, sendData.c_str(), sendData.size(), 0);//发送请求
		string  m_readBuffer;
		if (m_readBuffer.empty())
			m_readBuffer.resize(1024);//buffer大小设为1024 实际上坐标转换使用buffer小于800
		int readCount = recv(sock, &m_readBuffer[0], m_readBuffer.size(), 0);//接收服务器响应
		//cout << "Response:" << m_readBuffer << endl;
		res[i] = getGpsFromBuffer(m_readBuffer);//记录转换后的经纬度
		cout << "" << pointsOriginal[i].second << "," << pointsOriginal[i].first << endl;//控制栏显示
		closesocket(sock);//关闭连接
	}
	return res;
}
