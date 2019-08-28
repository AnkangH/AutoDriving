#include<iostream>
#include<fstream>  
#include <GL/glut.h>
#include<vector>
#include<string>
#include<windows.h>
using namespace std;
void display(void);
void init(void);
void reshape(int w, int h);
vector<pair<double, double>> getPointWithTime(string);
vector<pair<double, double>> getPoint(string);
vector<pair<double, double>> point;
void idle(void);
int cnt = 0;
int main(int argc, char** argv) {
	
	point=getPoint("1_decode_trans.txt");
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowSize(800, 800);
	glutCreateWindow(argv[0]);
	init();
	glutDisplayFunc(display);
	//glutReshapeFunc(reshape);
	glutIdleFunc(idle);
	glutMainLoop();
	return 0;
}
void idle(void)
{
	int size = point.size();
	if (cnt < size)
	{
		cout << "point[" << cnt << "]=" << point[cnt].first << "," << point[cnt].second << endl;
		cnt++;
		display();
		Sleep(2);
	}
}
void display(void)
{ 
	glClear(GL_COLOR_BUFFER_BIT);
	glColor3f(0.0f, 0.0f, 0.0f);
	glPointSize(2);
	int size = point.size();
	glBegin(GL_POINTS);
	for (int i = 0; i < cnt; i++)
	{
		double y = (point[i].first - point[0].first) * pow(10, 5);
		double x = (point[i].second - point[0].second) * pow(10, 5);
		x /= 200.0;
		y /= 200.0;
		//y += 0.8;
		glVertex2d(x, y);
	}
	glEnd();
	glutSwapBuffers();
	//glFlush();
}
void init(void) {
	glClearColor(1.0f, 1.0f, 1.0f,0.0f);
	glClear(GL_COLOR_BUFFER_BIT);
}
void reshape(int w, int h) {
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0.0, (GLdouble)w, 0.0, (GLdouble)h);
}
vector<pair<double, double>> getPointWithTime(string file)
{
	vector<pair<double, double>> res;
	ifstream inFile(file);
	if (!inFile.is_open())
	{
		cout << "Can't open gps1_decode.txt." << endl;
		return res;
	}
	string str;
	while (getline(inFile,str))
	{
		int size = str.size();
		if (size == 0)
			break;
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
						cout << lat << endl;
					}
					if (str[j] == 'l' && str[j + 1] == 'o' && str[j + 2] == 'n' && str[j + 3] == '=')
					{
						for (int k = j + 4; k < size; k++)
						{
							lon += str[k];
							if (str[k] == ',')
								break;
						}
						break;
						cout << lon;
					}
					if (!lon.empty())
						break;
				}
			}
			if (!lon.empty())
				break;
		}
		res.push_back({stod(lat), stod(lon)});
	}
	return res;
}
vector<pair<double, double>> getPoint(string file)
{
	ifstream inFile(file);
	vector<pair<double, double>> res;
	if (!inFile.is_open())
	{
		cout << "Can't open gps1_decode.txt." << endl;
		return res;
	}
	string s;
	while (getline(inFile, s))
	{
		string temp;
		double lat;
		for (auto p : s)
		{
			if (p != ',')
				temp += p;
			else
			{
				lat = stod(temp);
				temp.clear();
			}
		}
		res.push_back({ stod(temp),lat });
	}
	return res;
}
