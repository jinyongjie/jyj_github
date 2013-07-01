// test_opengl.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "test_opengl.h"



#ifndef BUILDINGS_H
#define BUILDINGS_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <malloc.h>
#include <time.h>
#include <math.h>
#include <wtypes.h>



//包含GLWindow.h头文件
// 定义键盘类
class Keys              
{
public:
	// 构造函数
	Keys() { Clear(); }
	// 清空所有的按键信息
	void Clear() { ZeroMemory(&m_KeyDown, sizeof(m_KeyDown)); }
	// 判断某个键是否按下
	bool IsPressed(unsigned int key) { return (key < MAX_KEYS) ? (m_KeyDown[key] == true) : false; }
	// 设置某个键被按下
	void SetPressed(unsigned int key) { if (key < MAX_KEYS) m_KeyDown[key] = true; }
	// 设置某个键被释放
	void SetReleased(unsigned int key) { if (key < MAX_KEYS) m_KeyDown[key] = false; }
private:
	static const unsigned int MAX_KEYS = 1024;
	bool m_KeyDown[MAX_KEYS];         //保存1024个按键的状态
};

// windows窗口类
class GLWindow               
{
public:
	// 重要的窗口函数 
	// 构造函数
	GLWindow();
	// 创建windows窗口
	bool Create(const wchar_t * window_title, const wchar_t * class_name, bool fullscreen,HINSTANCE h_instance, LPVOID lpParam);
	// 删除OpenGL窗口
	void Destroy();
	// 改变窗口的显示设置
	bool ChangeScreenSetting();
	// 当窗口大小改变时，通知OpenGL调整大小
	void ReshapeGL();
	// Swap Buffers (Double Buffering)
	void SwapBuffers() { ::SwapBuffers(m_hDC); }
	// 下面函数为设置/返回窗口类的属性
	// 设置窗口左上角的位置
	void SetPosX(int x);
	void SetPosX(unsigned short x) { SetPosX((signed short)x); }  
	void SetPosY(int y);
	void SetPosY(unsigned short y) { SetPosY((signed short)y); }  
	// 返回窗口的大小
	int  GetWidth();
	int  GetHeight();
	// 设置窗口的大小
	void SetWidth(int width);
	void SetHeight(int height);
	// 返回窗口左上角的位置
	int  GetPosX();
	int  GetPosY();
	// 设置窗口的颜色位深
	void SetHiColor() { m_BitsPerPixel = 16; }
	void SetTrueColor() { m_BitsPerPixel = 32; }
	//void SetFullScreen(bool mode) { m_IsFullScreen = mode; }
	//bool GetFullScreen() { return m_IsFullScreen; }
	// 重载运算符，可以让GL_Window m_Window声明后的m_Window作为窗口句柄使用
	operator HWND() { return m_hWnd; }
private:
	HWND m_hWnd;              // 窗口句柄
	HDC  m_hDC;              // 设备描述表
	HGLRC m_hRC;              // OpenGL渲染描述表
	int  m_WindowPosX;            // 窗口的左上角的X位置
	int  m_WindowPosY;            // 窗口的左上角的Y位置
	int  m_WindowWidth;            // 窗口的宽度
	int  m_WindowHeight;            // 窗口的高度
	int  m_ScreenWidth;            // 全屏的宽度
	int  m_ScreenHeight;            // 全屏的高度
	int  m_BitsPerPixel;            // 颜色位深
	bool m_IsFullScreen;            // 是否全屏
};

// 基本的程序类，继承它用来创建OpenGL程序
class GLApplication           
{
public:
	// 创建一个全局的Create函数，这个函数必须被继承类实现
	static GLApplication * Create(const wchar_t * class_name); //创建你自己的子类
	// 虚析构函数
	virtual ~GLApplication() {};
protected:
	// 下面的函数必须被继承类实现，完成基本的OpenGL渲染过程
	virtual bool Init() = 0;       //OpenGL的初始化 
	virtual void Uninit() = 0;       //OpenGL的卸载
	virtual void Update(DWORD milliseconds) = 0;    //执行OpenGL程序的更新
	virtual void Draw() = 0;         //绘制OpenGL场景
	// 通用的函数
	void ToggleFullscreen();         //切换全屏/窗口模式
	void TerminateApplication();        //结束程序
	void ResizeDraw(bool enable) { m_ResizeDraw = enable; } //设置在窗口改变大小的时候，可以绘制
	Keys m_Keys;            //按键类
	// 构造函数
	GLApplication(const wchar_t * class_name); 
	void ReDraw();
private:
	// 程序的主循环
	friend int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow);
	int  Main(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow);
	// 消息处理回调函数
	friend LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT Message(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	friend INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);

	static const UINT WM_TOGGLEFULLSCREEN = (WM_USER + 1);  //自定义消息，在切换窗口模式的时候发送
	GLWindow m_Window;          //Window类
	const wchar_t* m_ClassName;         //程序名
	bool  m_IsProgramLooping;        //程序循环标记，如果为false，则退出程序
	bool        m_CreateFullScreen;                             //若为true，则创建全屏模式
	bool  m_IsVisible;         //窗口是否可见
	bool  m_ResizeDraw;         //是否在改变大小时，调用了绘制函数
	DWORD  m_LastTickCount;        //上一次计时器的值
};

          //包含基本的框架类
#define srandom srand
// 从GL_Application派生出一个子类
class Buildings : GLApplication
{
public:
	~Buildings(void);
public:
	bool Init();          //< 执行所有的初始化工作，如果成功函数返回true
	void Uninit();          //< 执行所有的卸载工作
	void Update(DWORD milliseconds);       //< 执行所有的更新操作，传入的参数为两次操作经过的时间，以毫秒为单位
	void Draw();            //< 执行所有的绘制操作
	void    SetupMaterials(void);
	void    initTable();
	//Create display lists for a leaf, a set of leaves, and a stem
	void    CreateTreeLists(void);
	void    MakeTable(void);
	void    DrawTable(void);
	void    DrawScene(void);
	void    CreateTree(void);
	//recursive tree drawing thing, fleshed out from class notes pseudocode
	void FractalTree(int level);
	void  MakeAxesList(int displaylistnum);
	void  Output(float x, float y,float z, char *string);
	void   DrawAll(void);
	void LightFunction();
	void static Ontime(int time);
	void static Menu(int choice);
	void static Pan(int x, int y);
	void static Rotate(int x, int y);
	void static Motion(int x, int y);
	void static Mouse(int button, int state, int x, int y);
	void static Up(void);
	void static Down(void);
	void static Left(void);
	void static Right(void);
	void ScaleUp();
	void ScaleDown();
	void UpdateByMouse();

private:
	friend class GLApplication;        //< 父类为它的一个友元类，可以用来创建程序的实例，见函数GL_Application * GL_Application::Create(const char * class_name)
	Buildings(const wchar_t * class_name);      //< 构造函数
	POINT _mousePos; 
};
#endif // BUILDINGS_H

//Constructors.h
#include <vector>


#include <vector>
#include <D:\code\jiege_browser\third_src\glutdlls37beta\include/glut.h>
static int currentAnimateNum =0;
static int startAnimateNum =1000;
static int maxAnimateNum =1;
static float scale = 1;
static float transx=-11;
static float transy=7;
static float rotx , roty;

struct Point
{
	float x;
	float y;
	float z;
public:
	Point() {
	}
	Point(float x,float y,float z)
	{
		this->x = x;
		this->y = y;
		this->z = z;
	}
};
class CubeShape
{
private:
	Point p;
	float length;
	float width;
	float height;
	float coords[24][3];
	float animateNum;
	void initCoords();
public:
	CubeShape(Point p,float length,float width,float height);
	virtual void DrawShape();
	static float scale;
	static float transx;
	static float transy;
	static float transz;

};

class Constructors
{
private:
	std::vector<CubeShape*>  elements;
public:
	Constructors(void);
	~Constructors(void);
	void buildPillar(float x,float y,float z);
	void buildPillars(int floor);

	void buildBeam(float x,float y,float z);

	void buildBeams(int floor);

	void buildCeiling(float x,float y,float z);

	void buildCeilings(int floor);

	void buildGlass(float x,float y,float z);

	void buildGlasses(int floor);

	void buildWall(float x,float y,float z);

	void buildWalls(int floor);

	void buildExotericWall();

	//Construct the building
	void Construct(void);
};

//Elements.h

class PillarShape:public CubeShape
{
public :
	PillarShape(std::vector<CubeShape*> &elements,Point p,float length,float width,float height);

	virtual void DrawShape();
};
class WallShape:public CubeShape
{
public :
	WallShape(std::vector<CubeShape*> &elements,Point p,float length,float width,float height);
	virtual void DrawShape();

};
class ExotericWallShape:public CubeShape
{
public :
	ExotericWallShape(std::vector<CubeShape*> &elements,Point p,float length,float width,float height);
	virtual void DrawShape();
};

class BeamShape:public CubeShape
{
public :
	BeamShape(std::vector<CubeShape*> &elements,Point p,float length,float width,float height);
	virtual void DrawShape();
};
class CeilingShape:public CubeShape
{
public :
	CeilingShape(std::vector<CubeShape*> &elements,Point p,float length,float width,float height);
	virtual void DrawShape();
};

class GlassShape:public CubeShape
{
public :
	GlassShape(std::vector<CubeShape*> &elements,Point p,float length,float width,float height);
	virtual void DrawShape();
};

//GLFrame.h

//======================================================================
//
//  @file  Framework.h
//
//  项目描述： OpenGL程序框架演示
//  文件描述:  键盘类和程序框架类
//  适用平台： Windows98/2000/NT/XP/WIN7
// 
//  你必须在你的继承类中完成以下函数的实现
//                   
//  GLApplication * GLApplication::Create(const char * class_name)  
//  创建你的子类的一个实例  
//                   
//  bool Init();              
//  执行所有的初始化工作，如果成功函数返回true       
//                   
//  void Uninit();             
//  执行所有的卸载工作          
//                   
//  void Update(DWORD milliseconds);          
//  执行所有的更新操作，传入的参数为两次操作经过的时间，以毫秒为单位
//                   
//  void Draw();               
//  执行所有的绘制操作
//                  
//======================================================================
#ifndef __GLFRAME_H__
#define __GLFRAME_H__

#endif // __GLFRAMEWORK_H__

//GLWindow.h

//========================================================
//
//  @file  GLWindows.h
//
//  项目描述： OpenGL程序框架演示
//  文件描述:  程序窗口类 
//  适用平台： Windows98/2000/NT/XP/WIN7
// 
//   
//========================================================
#ifndef __GLWINDOW_H__
#define __GLWINDOW_H__
#include <windows.h>                                             // 包含windows.h头文件
#pragma warning(disable: 4311)                                  // 禁止编译器出现类型转换的警告
#pragma warning(disable: 4312)
#pragma warning(disable: 4244)

#endif // __GLWINDOW_H__

//image.h

#ifdef __cplusplus
extern "C" {
#endif
	//RGB Image Structure
	typedef struct _RGBImageRec {
		int sizeX, sizeY;
		unsigned char *data;
	} RGBImageRec;
	extern RGBImageRec *RGBImageLoad(char *);
#ifdef __cplusplus
}
#endif
//Resource.h

//{{NO_DEPENDENCIES}}
// Microsoft Visual C++ generated include file.
// Used by Building.rc
//
#define IDC_MYICON                      2
#define IDD_BUILDING_DIALOG             102
#define IDS_APP_TITLE                   103
#define IDD_ABOUTBOX                    103
#define IDM_ABOUT                       104
#define IDM_EXIT                        105
#define IDI_BUILDING                    107
#define IDI_SMALL                       108
#define IDC_BUILDING                    109
#define IDR_MAINFRAME                   128
#define ID_TRANSFORMATION_TRANSLATION   32771
#define ID_TRANSFORMATION_ROTATION      32772
#define ID_Menu                         32773
#define ID_VIEW_WIREFRAME               32774
#define ID_VIEW_LIGHT                   32775
#define ID_VIEW_CLIP                    32776
#define ID_ANIMATION_START              32777
#define IDC_STATIC                      -1
// Next default values for new objects
//
#ifdef APSTUDIO_INVOKED
#ifndef APSTUDIO_READONLY_SYMBOLS
#define _APS_NO_MFC                     1
#define _APS_NEXT_RESOURCE_VALUE        129
#define _APS_NEXT_COMMAND_VALUE         32778
#define _APS_NEXT_CONTROL_VALUE         1000
#define _APS_NEXT_SYMED_VALUE           110
#endif
#endif
// stdafx.h

// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件
//
#pragma once
#include "targetver.h"
#define WIN32_LEAN_AND_MEAN             // 从 Windows 头中排除极少使用的资料
// Windows 头文件:
#include <windows.h>
// C 运行时头文件
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>

// TODO: 在此处引用程序需要的其他头文件
//targetver.h

#pragma once
// 以下宏定义要求的最低平台。要求的最低平台
// 是具有运行应用程序所需功能的 Windows、Internet Explorer 等产品的
// 最早版本。通过在指定版本及更低版本的平台上启用所有可用的功能，宏可以
// 正常工作。
// 如果必须要针对低于以下指定版本的平台，请修改下列定义。
// 有关不同平台对应值的最新信息，请参考 MSDN。
#ifndef WINVER                          // 指定要求的最低平台是 Windows Vista。
#define WINVER 0x0600           // 将此值更改为相应的值，以适用于 Windows 的其他版本。
#endif
#ifndef _WIN32_WINNT            // 指定要求的最低平台是 Windows Vista。
#define _WIN32_WINNT 0x0600     // 将此值更改为相应的值，以适用于 Windows 的其他版本。
#endif
#ifndef _WIN32_WINDOWS          // 指定要求的最低平台是 Windows 98。
#define _WIN32_WINDOWS 0x0410 // 将此值更改为适当的值，以适用于 Windows Me 或更高版本。
#endif
#ifndef _WIN32_IE                       // 指定要求的最低平台是 Internet Explorer 7.0。
#define _WIN32_IE 0x0700        // 将此值更改为相应的值，以适用于 IE 的其他版本。
#endif
//Buildings.cpp

#include "StdAfx.h"


#include <gl\gl.h>            // 包含OpenGL头文件
#include <gl\glu.h>            
#pragma comment(lib, "opengl32.lib")       //包含OpenGL链接库文件
#pragma comment(lib, "glu32.lib")       
typedef enum { NOTALLOWED, MOUNTAIN, TREE, ISLAND, BIGMTN, STEM, LEAF,
MOUNTAIN_MAT, WATER_MAT, LEAF_MAT, TREE_MAT, STEMANDLEAVES,
AXES } DisplayLists;
#define USE_ZBUFFER
#define USE_TEXTURE
#define DEG2RAD (3.14159/180.0)
#define TABLE_TEXTURE "D:\\code\\Intra3D\\Data\\Image\\BRICK.RGB"
#define MAX_OBJECTS 2
static GLint table_list;

static int doubleBuffer = 1;
#ifdef WIN32
#define drand48() (((float) rand())/((float) RAND_MAX))
#define srand48(x) (srand((x)))
#endif
int Level   = 4;        //levels of recursion for fractals   
int Fract   = TREE;    // What fractal are we building
int lightMode =0;
int lineMode =0;
int clipMode = 0;
static int ox = -1, oy = -1;
static int mot = 0;
static bool running =false;
#define PAN 1
#define ROT 2
void *font = GLUT_BITMAP_TIMES_ROMAN_24;
void *fonts[] =
{
	GLUT_BITMAP_9_BY_15,
	GLUT_BITMAP_TIMES_ROMAN_10,
	GLUT_BITMAP_TIMES_ROMAN_24
};

// 创建一个程序的实例
GLApplication * GLApplication::Create(const wchar_t * class_name)
{
	Buildings * buildings = new Buildings(class_name);
	return reinterpret_cast<GLApplication *>(buildings);
}
Buildings::~Buildings(void)
{
}
// 构造函数
Buildings::Buildings(const wchar_t * class_name) : GLApplication(class_name)
{
	/// 初始化用户自定义的程序变量
}
// 初始化OpenGL
bool Buildings::Init()         
{
	// 用户自定义的初始化过程
	static float lmodel_ambient[] =
	{0.0, 0.0, 0.0, 0.0};
	static float lmodel_twoside[] =
	{GL_FALSE};
	static float lmodel_local[] =
	{GL_FALSE};
	static float light0_ambient[] =
	{0.1, 0.1, 0.1, 1.0};
	//{0, 0, 0, 1.0};//The dedault color is black
	static float light0_diffuse[] =
	{1.0, 1.0, 1.0,1.0};//The default color is white
	//{0.0, 0.0, 0.0,1.0}; Set the color to be black
	static float light0_position[] =
		//{1, 0.5, 1, 1};
		//If set the fourth value 0,the light will be positioned
		// at infinity
	{1, 0.5, 1, 0};
	static float light0_specular[] =
	{0.0, 0.0, 0.0, 1.0};
	static float bevel_mat_ambient[] =
	{0.0, 0.0, 0.0, 1.0};
	static float bevel_mat_shininess[] =
	{40.0};
	static float bevel_mat_specular[] =
	{0.0, 0.0, 0.0, 0.0};
	static float bevel_mat_diffuse[] =
	{1.0, 0.0, 0.0, 0.0};
	SetupMaterials();
	CreateTreeLists();
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glEnable(GL_DEPTH_TEST);
	glClearDepth(1.0);
	glClearColor(0.5, 0.5, 0.5, 1.0);
	//Set the background color to be black
	//glClearColor(0.0, 0.0, 0.0, 1.0);
	glLightfv(GL_LIGHT0, GL_AMBIENT, light0_ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light0_diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light0_specular);
	glLightfv(GL_LIGHT0, GL_POSITION, light0_position);
	glEnable(GL_LIGHT0);
	glLightModelfv(GL_LIGHT_MODEL_LOCAL_VIEWER, lmodel_local);
	glLightModelfv(GL_LIGHT_MODEL_TWO_SIDE, lmodel_twoside);
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lmodel_ambient);
	glEnable(GL_LIGHTING);
	glMaterialfv(GL_FRONT, GL_AMBIENT, bevel_mat_ambient);
	glMaterialfv(GL_FRONT, GL_SHININESS, bevel_mat_shininess);
	glMaterialfv(GL_FRONT, GL_SPECULAR, bevel_mat_specular);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, bevel_mat_diffuse);
	glColorMaterial(GL_FRONT_AND_BACK, GL_DIFFUSE);
	glEnable(GL_COLOR_MATERIAL);
	glShadeModel(GL_FLAT);
	srandom(time(NULL));
	initTable();
	CubeShape::transx=-11;
	CubeShape::transy=7;

	//glutSpecialFunc(Special);
	//glutCreateMenu(Menu);
	//glutAddMenuEntry("Animation", 1);
	//glutAddMenuEntry("Rotate", 2);
	//glutAddMenuEntry("Translate", 3);
	//glutAddMenuEntry("Light",4);
	//glutAddMenuEntry("Wireframe",5);
	//glutAddMenuEntry("Clip",6);
	//glutAddMenuEntry("Quit",7);
	//glutMouseFunc(Mouse);
	//glutMotionFunc(Motion);
	//glutAttachMenu(GLUT_RIGHT_BUTTON);
	glFlush();
	ResizeDraw(true);           // 改变OpenGL窗口大小，直接调用子类的函数
	return true;            //成功返回
}
void Buildings::Ontime(int time)
{
	if (!running)
	{
		return;
	}
	currentAnimateNum =1;
	startAnimateNum++;
	glutPostRedisplay();
	if (startAnimateNum<=maxAnimateNum)
	{
		glutTimerFunc(10,Ontime,0.1);
	}
}
void Buildings::Menu(int choice)
{
	switch(choice) {
case 1:
	startAnimateNum =0;
	running =true;
	glutTimerFunc(50,Ontime,0.1);
	break;
case 2:
	running =false;
	mot = ROT;
	break;
case 3:
	running =false;
	mot = PAN;
	break;
case 4:
	if (lightMode==0)
	{
		//Set the background color to be black
		glClearColor(0.0, 0.0, 0.0, 1.0);
		glDisable(GL_LIGHT0);
		glEnable(GL_LIGHT1);
		glEnable(GL_LIGHT2);
		lightMode=1;
	}
	else
	{
		glClearColor(0.5, 0.5, 0.5, 1.0);
		glDisable(GL_LIGHT1);
		glDisable(GL_LIGHT2);
		glEnable(GL_LIGHT0);
		lightMode=0;
	}
	glutPostRedisplay();
	break;
case 5:
	if (lineMode==0)
	{
		glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
		lineMode=1;
	}
	else
	{
		glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
		lineMode=0;
	}
	glutPostRedisplay();
	break;
case 6:
	if (clipMode==0)
	{
		//The equation is x/0.1+y/-0.1+z/0.02-1=0
		double eqn[4]={0.1,-0.1,0.02,-1};
		glClipPlane(GL_CLIP_PLANE0,eqn);
		glEnable(GL_CLIP_PLANE0);
		clipMode=1;
	}
	else
	{
		glDisable(GL_CLIP_PLANE0);
		clipMode=0;
	}
	glutPostRedisplay();
	break;
case 7:
	exit(0);
	break;
	}
}
void Buildings::Pan(int x, int y)
{
	transx += (x - ox) / 500.;
	transy -= (y - oy) / 500.;
	ox = x;
	oy = y;
	glutPostRedisplay();
}
void Buildings::Rotate(int x, int y)
{
	rotx += x - ox;
	if (rotx > 360.)
		rotx -= 360.;
	else if (rotx < -360.)
		rotx += 360.;
	roty += y - oy;
	if (roty > 360.)
		roty -= 360.;
	else if (roty < -360.)
		roty += 360.;
	ox = x;
	oy = y;
	glutPostRedisplay();
}
void Buildings::Motion(int x, int y)
{
	if (mot == PAN)
		Pan(x, y);
	else if (mot == ROT)
		Rotate(x, y);
}
void Buildings::Mouse(int button, int state, int x, int y)
{
	if (state == GLUT_DOWN) {
		switch (mot) {
case PAN:
	Motion(ox = x, oy = y);
	break;
case ROT:
	Motion(ox = x, oy = y);
	break;
		}
	}
}
void Buildings::ScaleUp(void)
{
	CubeShape::scale+=0.1;
}
void Buildings::ScaleDown(void)
{
	CubeShape::scale-=0.1;
}
void Buildings::Up(void)
{
	CubeShape::transz-=0.1;
}
void Buildings::Down(void)
{
	CubeShape::transz+=0.1;
}
void Buildings::Left(void)
{
	CubeShape::transx-=0.1;
}
void Buildings::Right(void)
{
	CubeShape::transx+=0.1;
}
// 用户自定义的卸载函数
void Buildings::Uninit()         
{
	// 用户自定义的卸载过程
	///......
	///......
}

// 程序更新函数
void Buildings::Update(DWORD milliseconds)      
{
	if (m_Keys.IsPressed(VK_ESCAPE) == true)     // 按ESC退出
	{
		TerminateApplication();         
	}
	if (m_Keys.IsPressed(VK_F1) == true)      // 按F1切换窗口/全屏模式
	{
		ToggleFullscreen();          
	}

	if (m_Keys.IsPressed(187))
	{
		ScaleUp();
		m_Keys.SetReleased(187);  
		ReDraw();
	}
	if (m_Keys.IsPressed(189))
	{
		ScaleDown();
		m_Keys.SetReleased(189); 
		ReDraw();
	}
	if (m_Keys.IsPressed(VK_UP))
	{
		Up();
		ReDraw();
	}
	if (m_Keys.IsPressed(VK_DOWN))
	{
		Down();
		ReDraw();   ;
	}
	if (m_Keys.IsPressed(VK_LEFT))
	{
		Left();
		ReDraw();   ;
	}
	if (m_Keys.IsPressed(VK_RIGHT))
	{
		Right();
		ReDraw();
	}
	if (m_Keys.IsPressed(WM_LBUTTONDOWN))
	{
		UpdateByMouse();
		ReDraw();
	}

}
void Buildings::UpdateByMouse()
{
	POINT mousePos; 
	// 得到当前鼠标位置
	GetCursorPos(&mousePos);      
	ShowCursor(TRUE);
	// 如果鼠标没有移动,则不用更新
	if( (mousePos.x == _mousePos.x) && (mousePos.y == _mousePos.y) )
		return;
	CubeShape::transx+=(mousePos.x-_mousePos.x)*0.1;
	CubeShape::transy-=(mousePos.y-_mousePos.y)*0.1;
}
//Create display lists for a leaf, a set of leaves, and a stem
void Buildings::CreateTreeLists(void)
{
	GLUquadricObj *cylquad = gluNewQuadric();
	int i;
	glNewList(STEM, GL_COMPILE);
	glPushMatrix();
	gluCylinder(cylquad, 0.1, 0.08, 1, 10, 2 );
	glPopMatrix();
	glEndList();
	glNewList(LEAF, GL_COMPILE); 
	glBegin(GL_TRIANGLES);
	glNormal3f(-0.1, 0, 0.25);  // not normalized
	glVertex3f(0, 0, 0);
	glVertex3f(0.25, 0.25, 0.1);
	glVertex3f(0, 0.5, 0);
	glNormal3f(0.1, 0, 0.25);
	glVertex3f(0, 0, 0);
	glVertex3f(0, 0.5, 0);
	glVertex3f(-0.25, 0.25, 0.1);
	glEnd();
	glEndList();
	glNewList(STEMANDLEAVES, GL_COMPILE);
	glPushMatrix();
	glPushAttrib(GL_LIGHTING_BIT);
	glCallList(STEM);
	glCallList(LEAF_MAT);
	for(i = 0; i < 3; i++) {
		glTranslatef(0, 0.333, 0);
		glRotatef(90, 0, 1, 0);
		glPushMatrix();
		glRotatef(0, 0, 1, 0);
		glRotatef(50, 1, 0, 0);
		glCallList(LEAF);
		glPopMatrix();
		glPushMatrix();
		glRotatef(180, 0, 1, 0);
		glRotatef(60, 1, 0, 0);
		glCallList(LEAF);
		glPopMatrix();
	}
	glPopAttrib();
	glPopMatrix();
	glEndList();
}

void  Buildings::SetupMaterials(void)
{
	GLfloat tree_ambuse[] =   { 0.4, 0.25, 0.1, 1.0 };
	GLfloat tree_specular[] = { 0.0, 0.0, 0.0, 1.0 };
	GLfloat tree_shininess[] = { 0 };
	GLfloat leaf_ambuse[] =   { 0.0, 0.8, 0.0, 1.0 };
	GLfloat leaf_specular[] = { 0.0, 0.8, 0.0, 1.0 };
	GLfloat leaf_shininess[] = { 10 };
	glNewList(TREE_MAT, GL_COMPILE);
	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, tree_ambuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, tree_specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, tree_shininess);
	glEndList();
	glNewList(LEAF_MAT, GL_COMPILE);
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, leaf_ambuse);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, leaf_specular);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, leaf_shininess);
	glEndList();
}

void  Buildings::initTable()
{
	RGBImageRec *image;
	MakeTable();
	//Setup texture
#ifdef USE_TEXTURE
	image = RGBImageLoad(TABLE_TEXTURE);
	gluBuild2DMipmaps(GL_TEXTURE_2D, 3, image->sizeX, image->sizeY,
		GL_RGB, GL_UNSIGNED_BYTE, image->data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
#endif
#ifndef USE_ZBUFFER
	glEnable(GL_CULL_FACE);
#endif
	glShadeModel(GL_FLAT);
	glEnable(GL_NORMALIZE);
}
void Buildings::MakeTable(void)
{
	static GLfloat table_mat[] =
	{1.0, 1.0, 1.0, 0.6};
	static GLfloat gray[] =
	{0.4, 0.4, 0.4, 1.0};
	table_list = glGenLists(1);
	glNewList(table_list, GL_COMPILE);
	//load table's texture
	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, table_mat);
	// glMaterialfv( GL_FRONT, GL_EMISSION, gray );
	glMaterialfv(GL_FRONT, GL_DIFFUSE, table_mat);
	glMaterialfv(GL_FRONT, GL_AMBIENT, gray);
	//draw textured square for the table
	//glTexCoord2f(GLfloat s, GLfloat t)函数原型
	//有两个参数：GLfloat s, GLfloat t
	//第一个参数代表X坐标。 0.0f 是纹理的左侧。 0.5f 是纹理的中点， 1.0f 是纹理的右侧。
	//第二个参数代表Y坐标。 0.0f 是纹理的底部。 0.5f 是纹理的中点， 1.0f 是纹理的顶部。
	//一个完全纹理的四个顶点的坐标分别是(0.0f, 0.0f)、(0.0f, 1.0f)、(1.0f, 1.0f)、(1.0f, 0.0f)，分别对应左下、左上、右上、右下角。
	//可以这么认为，纹理一个挨着一个粘在坐标轴上，显示多个、显示那部分完全由四个顶点坐标决定。
	//比如显示3行2列个位图，坐标可以这样：(0.0f, 0.0f)、(0.0f, 3.0f)、(2.0f, 3.0f)、(2.0f, 0.0f)
	glPushMatrix();
	glScalef(1.0, 1.0, 1.0);

	glBegin(GL_POLYGON);
	glNormal3f(0.0, 0.0,1.0);
	glTexCoord2f(0.0, 0.0);
	glVertex3f(-7.5, 0.0, 0.0);
	glTexCoord2f(2.0, 0.0);
	glVertex3f(-7.5, -5.0, 0.0);
	glTexCoord2f(2.0, 3.0);
	glVertex3f(-3.5, -5.0, 0.0);
	glTexCoord2f(0.0, 3.0);
	glVertex3f(-3.5, 0, 0.0);
	glEnd();
	glBegin(GL_POLYGON);
	glNormal3f(0.0, 0.0,1.0);
	glTexCoord2f(0.0, 0.0);
	glVertex3f(-7.5, -5.0, 0.0);
	glTexCoord2f(2.0, 0.0);
	glVertex3f(-7.5, -10.0, 0.0);
	glTexCoord2f(2.0, 3.0);
	glVertex3f(-3.5, -10.0, 0.0);
	glTexCoord2f(0.0, 3.0);
	glVertex3f(-3.5, -5, 0.0);
	glEnd();
	glBegin(GL_POLYGON);
	glNormal3f(0.0, 0.0,1.0);
	glTexCoord2f(0.0, 0.0);
	glVertex3f(-7.5, -10.0, 0.0);
	glTexCoord2f(3.0, 0.0);
	glVertex3f(-7.5, -15.0, 0.0);
	glTexCoord2f(2.0,3.0);
	glVertex3f(-3.5, -15.0, 0.0);
	glTexCoord2f(0.0, 3.0);
	glVertex3f(-3.5, -10, 0.0);
	glEnd();
	glPopMatrix();
	glDisable(GL_TEXTURE_2D);
	glEndList();
}
void Buildings::DrawTable(void)
{
	glCallList(table_list);
}

void Buildings::DrawScene(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	glPushMatrix();
	//draw table into stencil planes
	glEnable(GL_STENCIL_TEST);
#ifdef USE_ZBUFFER
	glDisable(GL_DEPTH_TEST);
#endif
	glStencilFunc(GL_ALWAYS, 1, 0xffffffff);
	glStencilOp(GL_REPLACE, GL_REPLACE, GL_REPLACE);
	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
	DrawTable();
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
#ifdef USE_ZBUFFER
	glEnable(GL_DEPTH_TEST);
#endif
	glDisable(GL_STENCIL_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
#ifdef USE_TEXTURE
	glEnable(GL_TEXTURE_2D);
#endif
	DrawTable();
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_BLEND);
	glPushMatrix();
	glPopMatrix();
	glPopMatrix();
}
long TreeSeed;   // for srand48 - remember so we can build "same tree"
// at a different level
//recursive tree drawing thing, fleshed out from class notes pseudocode
void Buildings::FractalTree(int level)
{
	long savedseed;  // need to save seeds while building tree too
	if (level == Level)
	{ 
		glColor3ub(0, 255, 0);
		glPushMatrix();
		glRotatef(drand48()*180, 0, 0, 1);
		glCallList(STEMANDLEAVES);
		glPopMatrix();
	}
	else
	{
		glColor3ub(222, 184, 135 );
		glCallList(STEM);
		glPushMatrix();
		glRotatef(drand48()*180, 0, 0, 1);
		glTranslatef(0, 0, 1);
		glScalef(0.7, 0.7, 0.7);
		savedseed = (long) drand48()*ULONG_MAX;    //recurse on a 3-way branching
		glPushMatrix();   
		glRotatef(110 + drand48()*40, 0, 0, 1);
		glRotatef(30 + drand48()*20, 0, 1, 0);
		FractalTree(level + 1);
		glPopMatrix();
		srand48(savedseed);
		savedseed = (long) drand48()*ULONG_MAX;
		glPushMatrix();
		glRotatef(-130 + drand48()*40, 0, 0,1);
		glRotatef(30 + drand48()*20, 0, 1, 0);
		FractalTree(level + 1);
		glPopMatrix();
		srand48(savedseed);
		glPushMatrix();
		glRotatef(-20 + drand48()*40, 0, 0, 1);
		glRotatef(30 + drand48()*20, 0, 1, 0);
		FractalTree(level + 1);
		glPopMatrix();
		glPopMatrix();
	}
}
//draw and build display list for tree
void Buildings::CreateTree(void)
{
	srand48(TreeSeed);
	glNewList(TREE, GL_COMPILE);
	glPushMatrix();
	glPushAttrib(GL_LIGHTING_BIT);
	glCallList(TREE_MAT);
	glTranslatef(-3, 0, 0);
	FractalTree(0);
	glTranslatef(0, -3, 0);
	FractalTree(0);
	glTranslatef(0, -3, 0);
	FractalTree(0);
	glTranslatef(0, -3, 0);
	FractalTree(0);
	glTranslatef(0, -3, 0);
	FractalTree(0);
	glTranslatef(0, -3, 0);
	FractalTree(0);
	glTranslatef(0,15, 0);
	glTranslatef(-5, 0, 0);
	FractalTree(0);
	glTranslatef(0, -3, 0);
	FractalTree(0);
	glTranslatef(0, -3, 0);
	FractalTree(0);
	glTranslatef(0, -3, 0);
	FractalTree(0);
	glTranslatef(0, -3, 0);
	FractalTree(0);
	glTranslatef(0, -3, 0);
	FractalTree(0);
	glTranslatef(0,17, 0);
	glTranslatef(10, 0, 0);
	FractalTree(0);
	glTranslatef(3, 0, 0);
	FractalTree(0);
	glTranslatef(3, 0, 0);
	FractalTree(0);
	glTranslatef(6, 0, 0);
	FractalTree(0);
	glTranslatef(3, 0, 0);
	FractalTree(0);
	glTranslatef(3, 0, 0);
	FractalTree(0);
	glTranslatef(3, 0, 0);
	FractalTree(0);
	glTranslatef(3, 0, 0);
	FractalTree(0);
	glTranslatef(3, 0, 0);
	FractalTree(0);
	glTranslatef(-27, 0, 0);
	glTranslatef(0, 8, 0);
	FractalTree(0);
	glTranslatef(3, 0, 0);
	FractalTree(0);
	glTranslatef(3, 0, 0);
	FractalTree(0);
	glTranslatef(6, 0, 0);
	FractalTree(0);
	glTranslatef(3, 0, 0);
	FractalTree(0);
	glTranslatef(3, 0, 0);
	FractalTree(0);
	glTranslatef(3, 0, 0);
	FractalTree(0);
	glTranslatef(3, 0, 0);
	FractalTree(0);
	glTranslatef(3, 0, 0);
	FractalTree(0);
	glPopAttrib();
	glPopMatrix();
	glEndList(); 
}
//draw axes -- was helpful to debug/design things
void  Buildings::MakeAxesList(int displaylistnum)
{
	glLineWidth(2);
	glNewList(displaylistnum, GL_COMPILE);
	glMatrixMode(GL_MODELVIEW);
	Output(28, 0, 0, "X");
	Output(0, 5, 0, "Y");
	Output(0, 0, 27, "Z");
	glBegin(GL_LINES);
	glVertex3f(30, 0, 0); glVertex3f(-30, 0, 0);
	glVertex3f(0, 30, 0); glVertex3f(0, -30, 0);
	glVertex3f(0, 0, 30); glVertex3f(0, 0, -30);
	glEnd();
	glEndList(); 
}
void Buildings::Output(float x, float y,float z, char *string)
{
	int len, i;
	glRasterPos3f(x, y,z);
	len = (int) strlen(string);
	for (i = 0; i < len; i++)
	{
		glutBitmapCharacter(font, string[i]);
	}
}
void Buildings::LightFunction()
{
	if (lightMode==1)
	{
		glColor3ub(255, 255,255);
		glPushMatrix();
		glTranslatef(0.0f,-5.0f,9.0f);
		glutSolidSphere (0.5, 16, 16);
		glPopMatrix();
	}
	GLfloat light1_ambient[]={0,0,0,1.0};
	GLfloat light1_diffuse[]={1.0,1.0,1.0,1.0};
	GLfloat light1_specular[]={1.0,0.9,0.9,1.0};
	GLfloat light1_position[]={0.0,-5.0,11,1.0};
	GLfloat spot_direction1[]={.0,0.0,-1.0};
	glLightfv(GL_LIGHT1,GL_AMBIENT,light1_ambient);
	glLightfv(GL_LIGHT1,GL_DIFFUSE,light1_diffuse);
	glLightfv(GL_LIGHT1,GL_SPECULAR,light1_specular);
	glLightfv(GL_LIGHT1,GL_POSITION,light1_position);
	glLightf(GL_LIGHT1,GL_SPOT_CUTOFF,40.0);
	glLightfv(GL_LIGHT1,GL_SPOT_DIRECTION,spot_direction1);
	if (lightMode==1)
	{
		glColor3ub(255, 255,0);
		glPushMatrix();
		glTranslatef(20.0f,0.0f,9.0f);
		glutSolidSphere (0.5, 16, 16);
		glPopMatrix();
	}
	GLfloat light2_ambient[]={0.,0.,0.,1.0};
	GLfloat light2_diffuse[]={1.0,1.0,0.0,1.0};
	GLfloat light2_specular[]={1.0,1,0,1.0};
	GLfloat light2_position[]={20.0,0.0,11,1.0};
	GLfloat spot_direction2[]={.0,0.0,-1.0};
	glLightfv(GL_LIGHT2,GL_AMBIENT,light2_ambient);
	glLightfv(GL_LIGHT2,GL_DIFFUSE,light2_diffuse);
	glLightfv(GL_LIGHT2,GL_SPECULAR,light2_specular);
	glLightfv(GL_LIGHT2,GL_POSITION,light2_position);
	glLightf(GL_LIGHT2,GL_SPOT_CUTOFF,45.0);
	glLightfv(GL_LIGHT2,GL_SPOT_DIRECTION,spot_direction2);
}
void Buildings::DrawAll(void)
{
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glRotatef(rotx, 0.,0,  1.);
	glRotatef(roty, 0, 0., 1.);
	// 得到当前鼠标位置
	GetCursorPos(&_mousePos); 
	Constructors *constructors = new Constructors();
	constructors->Construct();
	LightFunction();
}
// 绘制函数
void Buildings::Draw()           
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(50, 1.0, 0.1, 100.0);
	gluLookAt(0.0,28,31,  // eye is at (0.0,8.5,7.0)
		0.0, -2.0, 15.0,      // center is at (0,0,1.0)
		0.0, 1.0, 1.);      // up is in positive Y direction
	glColor3ub(125, 125,125);
	DrawScene();
	CreateTree();
	DrawAll();
	glCallList(Fract);
	Output(25, 0,25.0, "Construct a building");
	glColor3ub(255, 0, 0);
	MakeAxesList(AXES);
	glColor3ub(255, 0, 0);
	glCallList(AXES);
	glFlush();             //< 强制执行所有的OpenGL命令
}
//Constructors.cpp

Constructors::Constructors(void)
{
}
Constructors::~Constructors(void)
{
}
void Constructors::buildPillar(float x,float y,float z)
{
	PillarShape *pillar= new PillarShape(elements, Point(9+x,3.0+y,0+z),0.3,0.3,3);
	//first line pillars
	pillar= new PillarShape(elements, Point(6+x,3.0+y,0+z),0.3,0.3,3);
	pillar= new PillarShape( elements,Point(3+x,3.0+y,0+z),0.3,0.3,3);
	pillar= new PillarShape( elements,Point(-3+x,3.0+y,0+z),0.3,0.3,3);
	pillar= new PillarShape(elements, Point(-6+x,3.0+y,0+z),0.3,0.3,3);
	pillar= new PillarShape( elements,Point(-9+x,3.0+y,0+z),0.3,0.3,3);
}
void Constructors::buildPillars(int floor)
{
	buildPillar(0,0,(floor-1)*3);
	buildPillar(0,-3,(floor-1)*3);
	buildPillar(0,-6,(floor-1)*3);
	buildPillar(0,-9,(floor-1)*3);
	buildPillar(0,-12,(floor-1)*3);
	buildPillar(0,-15,(floor-1)*3);
	buildPillar(0,-18,(floor-1)*3);
}
void Constructors::buildBeam(float x,float y,float z)
{
	BeamShape *beam= new BeamShape( elements,Point(9+x,3+y,0+z),0.3,6.2,0.3);
	beam= new BeamShape(elements, Point(-3+x,3+y,0+z),0.3,6.2,0.3);
	beam= new BeamShape( elements,Point(9+x,0+y,0+z),0.3,18,0.3);
	beam= new BeamShape( elements,Point(9+x,-3+y,0+z),0.3,18,0.3);
	beam= new BeamShape(elements, Point(9+x,-6+y,0+z),0.3,6.2,0.3);
	beam= new BeamShape( elements,Point(-3+x,-6+y,0+z),0.3,6.2,0.3);
	beam= new BeamShape( elements,Point(9+x,-9+y,0+z),0.3,6.2,0.3);
	beam= new BeamShape(elements, Point(-3+x,-9+y,0+z),0.3,6.2,0.3);
	beam= new BeamShape(elements, Point(9+x,-12+y,0+z),0.3,6.2,0.3);
	beam= new BeamShape( elements,Point(-3+x,-12+y,0+z),0.3,6.2,0.3);
	beam= new BeamShape( elements,Point(9+x,-15+y,0+z),0.3,6.2,0.3);
	beam= new BeamShape(elements, Point(-3+x,-15+y,0+z),0.3,6.2,0.3);
	beam= new BeamShape(elements, Point(9+x,3+y,0+z),18,0.3,0.3);
	beam= new BeamShape(elements, Point(6+x,3+y,0+z),18,0.3,0.3);
	beam= new BeamShape(elements, Point(3+x,3+y,0+z),18,0.3,0.3);
	beam= new BeamShape(elements, Point(-9+x,3+y,0+z),18,0.3,0.3);
	beam= new BeamShape(elements, Point(-6+x,3+y,0+z),18,0.3,0.3);
	beam= new BeamShape(elements, Point(-3+x,3+y,0+z),18,0.3,0.3);
}
void Constructors::buildBeams(int floor)
{
	buildBeam(0,0,(floor-1)*3);
}
void Constructors::buildCeiling(float x,float y,float z)
{
	BeamShape *ceiling= new BeamShape(elements, Point(9+x,3+y,3+z),18,3,0.3);
	ceiling= new BeamShape(elements, Point(6+x,-6+y,3+z),6,3,0.3);
	ceiling= new BeamShape(elements, Point(-3+x,-6+y,3+z),6,3,0.3);
	ceiling= new BeamShape(elements, Point(-6+x,3+y,3+z),18,3,0.3);
	ceiling= new BeamShape(elements, Point(6+x,3+y,3+z),6,3,0.3);
	ceiling= new BeamShape(elements, Point(-3+x,3+y,3+z),6,3,0.3);
	ceiling= new BeamShape(elements, Point(3+x,0+y,3+z),3,6,0.3);
	ceiling= new BeamShape(elements, Point(3+x,-3+y,3+z),3,6,0.3);
	//road
	ceiling= new BeamShape(elements, Point(27,16.5,-0.4),7,50,0.3);
}

void Constructors::buildCeilings(int floor)
{
	buildCeiling(0,0,(floor-1)*3);
}
void Constructors::buildGlass(float x,float y,float z)
{
	GlassShape *glass;
	if(z==0)
	{
		glass= new GlassShape(elements, Point(2.7+x,0+y,0.3+z),0.1,1.4,2.7);
		glass= new GlassShape(elements, Point(-1.3+x,0+y,0.3+z),0.1,1.4,2.7);
	}
	else{
		glass= new GlassShape(elements, Point(2.7+x,0+y,0+z+0.5),0.1,5.7,2.2);
	}
}
void Constructors::buildGlasses(int floor)
{
	if (floor==1)
	{
		buildGlass(0,0,(floor-1)*3);
	}
	else
	{
		buildGlass(0,0,(floor-1)*3+0.3);
	}
}

void Constructors::buildWall(float x,float y,float z)
{
	WallShape *wall;
	if(z==0)
	{
		//front walls
		wall= new WallShape(elements, Point(8.7+x,3+y,0.3+z),0.2,2.7,2.7);
		wall= new WallShape(elements, Point(5.7+x,3+y,0.3+z),0.2,2.7,2.7);
		wall= new WallShape(elements, Point(-3.3+x,3+y,0.3+z),0.2,2.7,2.7);
		wall= new WallShape(elements, Point(-6.3+x,3+y,0.3+z),0.2,2.7,2.7);
		wall= new WallShape(elements, Point(2.9+x,2.7+y,0.3+z),2.7,0.2,2.7);
		wall= new WallShape(elements, Point(-2.9+x,2.7+y,0.3+z),2.7,0.2,2.7);
		//left walls
		wall= new WallShape(elements, Point(8.8+x,2.7+y,0.3+z),2.7,0.2,2.7);

		//right walls
		wall= new WallShape(elements, Point(-8.8+x,2.7+y,0.3+z),2.7,0.2,2.7);
		wall= new WallShape(elements, Point(-8.8+x,0+y,0.3+z),3,0.2,2.7);
		wall= new WallShape(elements, Point(-8.8+x,-3+y,0.3+z),3,0.2,1);
		wall= new WallShape(elements, Point(-8.8+x,-3+y,1+z),0.8,0.2,1.7);
	}
	else
	{//front walls
		wall= new WallShape(elements, Point(8.7+x,3+y,0+z),0.2,2.7,2.7);
		wall= new WallShape(elements, Point(5.7+x,3+y,0+z),0.2,2.7,1);
		wall= new WallShape(elements, Point(5.7+x,3+y,0+z+1),0.2,1,1.7);
		wall= new WallShape(elements, Point(3.7+x,3+y,0+z+1),0.2,0.7,1.7);
		wall= new WallShape(elements, Point(4.7+x,3+y,0+z+2),0.2,1,0.7);
		//middle wall
		wall= new WallShape(elements, Point(2.7+x,0+y,0+z),0.2,5.7,0.5);

		wall= new WallShape(elements, Point(-3.3+x,3+y,0+z),0.2,2.7,1);
		wall= new WallShape(elements, Point(-3.3+x,3+y,0+z+1),0.2,1,1.7);
		wall= new WallShape(elements, Point(-5.3+x,3+y,0+z+1),0.2,0.7,1.7);
		wall= new WallShape(elements, Point(-4.3+x,3+y,0+z+2),0.2,1,0.7);
		wall= new WallShape(elements, Point(-6.3+x,3+y,0+z),0.2,2.7,2.7);
		wall= new WallShape(elements, Point(3+x,2.7+y,0+z),2.7,0.2,2.7);
		wall= new WallShape(elements, Point(-3+x,2.7+y,0+z),2.7,0.2,2.7);
		//left walls
		wall= new WallShape(elements, Point(8.8+x,2.7+y,0+z),2.7,0.2,2.7);
		//right walls
		wall= new WallShape(elements, Point(-8.8+x,2.7+y,0+z),2.7,0.2,2.7);
		wall= new WallShape(elements, Point(-8.8+x,0+y,0+z),3,0.2,2.7);
		wall= new WallShape(elements, Point(-8.8+x,-3+y,0+z),3,0.2,1);
		wall= new WallShape(elements, Point(-8.8+x,-3+y,1+z),0.8,0.2,1.7);
	}
}
void Constructors::buildWalls(int floor)
{
	if (floor==1)
	{
		buildWall(0,0,(floor-1)*3);
	}
	else
	{
		buildWall(0,0,(floor-1)*3+0.3);
	}
}

void Constructors::buildExotericWall()
{
	glColor3ub(255, 255, 255);
	//left wall
	ExotericWallShape *wall= new ExotericWallShape(elements, Point(10.0,4.0,0),20.0,0.2,1.5);
	//back wall
	wall= new ExotericWallShape(elements, Point(10,-16.0,0),0.2,20.2,1.5);
	//right wall
	wall= new ExotericWallShape(elements, Point(-10.0,4.0,0),20,0.2,1.5);
	//front left wall
	wall= new ExotericWallShape(elements, Point(10,4.0,0),0.2,8,1.5);
	//front right wall
	wall= new ExotericWallShape(elements, Point(-2.0,4.0,0),0.2,8,1.5);
}

//Construct the building
void Constructors::Construct(void)
{
	int size = elements.size();
	if (size==0)
	{
		buildExotericWall();
		buildPillars(1);
		buildBeams(1);
		buildBeams(2);
		buildCeilings(1);
		buildPillars(2);
		buildBeams(3);
		buildCeilings(2);
		buildPillars(3);
		buildBeams(4);
		buildCeilings(3);
		buildPillars(4);
		buildBeams(5);
		buildCeilings(4);
		buildPillars(5);
		buildBeams(6);
		buildCeilings(5);
		buildPillars(6);
		buildBeams(7);
		buildCeilings(6);
		buildPillars(7);
		buildBeams(8);
		buildCeilings(7);
		buildPillars(8);
		buildBeams(9);
		buildCeilings(8);
		buildWalls(1);
		buildWalls(2);
		buildWalls(3);
		buildWalls(4);
		buildWalls(5);
		buildWalls(6);
		buildWalls(7);
		buildWalls(8);
		buildGlasses(1);
		buildGlasses(2);
		buildGlasses(3);
		buildGlasses(4);
		buildGlasses(5);
		buildGlasses(6);
		buildGlasses(7);
		buildGlasses(8);
	}
	size = elements.size();
	for (int i = 0;i<size;i++)
	{
		elements[i]->DrawShape();
	}
}
//Elements.cpp


float CubeShape::scale=1;
float CubeShape::transx=0;
float CubeShape::transy=0;
float CubeShape::transz=0;

void CubeShape::initCoords()
{
	//left face
	coords[0][0]=p.x;
	coords[0][1]=p.y;
	coords[0][2]=p.z;
	coords[1][0]=p.x;
	coords[1][1]=p.y-length;
	coords[1][2]=p.z;
	coords[2][0]=p.x;
	coords[2][1]=p.y-length;
	coords[2][2]=p.z+height;     
	coords[3][0]=p.x;
	coords[3][1]=p.y;
	coords[3][2]=p.z+height;
	//right face
	coords[4][0]=p.x-width;
	coords[4][1]=p.y;
	coords[4][2]=p.z;
	coords[5][0]=p.x-width;
	coords[5][1]=p.y-length;
	coords[5][2]=p.z;
	coords[6][0]=p.x-width;
	coords[6][1]=p.y-length;
	coords[6][2]=p.z+height;
	coords[7][0]=p.x-width;
	coords[7][1]=p.y;
	coords[7][2]=p.z+height;
	//top face
	coords[8][0]=p.x-width;
	coords[8][1]=p.y-length;
	coords[8][2]=p.z+height;
	coords[9][0]=p.x;
	coords[9][1]=p.y-length;
	coords[9][2]=p.z+height;
	coords[10][0]=p.x;
	coords[10][1]=p.y;
	coords[10][2]=p.z+height;     
	coords[11][0]=p.x-width;
	coords[11][1]=p.y;
	coords[11][2]=p.z+height;
	//low face
	coords[12][0]=p.x-width;
	coords[12][1]=p.y-length;
	coords[12][2]=p.z;
	coords[13][0]=p.x;
	coords[13][1]=p.y-length;
	coords[13][2]=p.z;
	coords[14][0]=p.x;
	coords[14][1]=p.y;
	coords[14][2]=p.z;     
	coords[15][0]=p.x-width;
	coords[15][1]=p.y;
	coords[15][2]=p.z;
	//front face
	coords[16][0]=p.x;
	coords[16][1]=p.y;
	coords[16][2]=p.z;
	coords[17][0]=p.x-width;
	coords[17][1]=p.y;
	coords[17][2]=p.z;
	coords[18][0]=p.x-width;
	coords[18][1]=p.y;
	coords[18][2]=p.z+height;     
	coords[19][0]=p.x;
	coords[19][1]=p.y;
	coords[19][2]=p.z+height;
	//back face
	coords[20][0]=p.x;
	coords[20][1]=p.y-length;
	coords[20][2]=p.z;
	coords[21][0]=p.x-width;
	coords[21][1]=p.y-length;
	coords[21][2]=p.z;
	coords[22][0]=p.x-width;
	coords[22][1]=p.y-length;
	coords[22][2]=p.z+height;     
	coords[23][0]=p.x;
	coords[23][1]=p.y-length;
	coords[23][2]=p.z+height;
}
CubeShape::CubeShape(Point p,float length,float width,float height)
{
	this->p=p;
	this->length=length;
	this->width = width;
	this->height = height;
	this->animateNum = currentAnimateNum;
	currentAnimateNum++;
	if (currentAnimateNum>maxAnimateNum)
	{
		maxAnimateNum = currentAnimateNum;
	}
	initCoords();
}
void CubeShape::DrawShape()
{
	//if (this->animateNum >startAnimateNum)
	//{
	//return ;
	//}
	glBegin(GL_QUADS);
	int  k;
	float *v;
	glNormal3f(1, 0, 0);//left face
	for (k = 0; k <=3; k++)
	{
		v = coords[k];
		glVertex3f(v[0]* CubeShape::scale-CubeShape::transx, v[1]* CubeShape::scale-CubeShape::transy, v[2]* CubeShape::scale-CubeShape::transz);
	}
	glNormal3f(-1, 0, 0);//right face
	for (k = 4; k <=7; k++)
	{
		v = coords[k];
		glVertex3f(v[0]* CubeShape::scale-CubeShape::transx, v[1]* CubeShape::scale-CubeShape::transy, v[2]* CubeShape::scale-CubeShape::transz);
	}
	glNormal3f(0, 0, 1);//top face
	for (k = 8; k <=11; k++)
	{
		v = coords[k];
		glVertex3f(v[0]* CubeShape::scale-CubeShape::transx, v[1]* CubeShape::scale-CubeShape::transy, v[2]* CubeShape::scale-CubeShape::transz);
	}
	glNormal3f(0, 0, -1);//back face
	for (k = 12; k <=15; k++)
	{
		v = coords[k];
		glVertex3f(v[0]*CubeShape::scale-CubeShape::transx, v[1]*CubeShape::scale-CubeShape::transy, v[2]*CubeShape::scale-CubeShape::transz);
	}
	glNormal3f(0, 1, 0);//front face
	for (k = 16; k <=19; k++)
	{
		v = coords[k];
		glVertex3f(v[0]*CubeShape::scale-CubeShape::transx, v[1]*CubeShape::scale-CubeShape::transy, v[2]*CubeShape::scale-CubeShape::transz);
	}
	glNormal3f(0, -1, 0);//back face
	for (k = 20; k <=23; k++)
	{
		v = coords[k];
		glVertex3f(v[0]*CubeShape::scale-CubeShape::transx, v[1]*CubeShape::scale-CubeShape::transy, v[2]*CubeShape::scale-CubeShape::transz);
	}
	glEnd();
};

PillarShape::PillarShape(std::vector<CubeShape*> &elements,Point p,float length,float width,float height):CubeShape(p,length, width, height)
{ 
	elements.push_back(this);
}
void PillarShape::DrawShape()
{
	glColor3ub(54,54,54);
	CubeShape::DrawShape();
}

WallShape::WallShape(std::vector<CubeShape*> &elements,Point p,float length,float width,float height):CubeShape(p,length, width, height)
{ 
	elements.push_back(this);
}
void WallShape::DrawShape()
{
	glColor3ub(209, 103, 23);
	CubeShape::DrawShape();
}
ExotericWallShape::ExotericWallShape(std::vector<CubeShape*> &elements,Point p,float length,float width,float height):CubeShape(p,length, width, height)
{ 
	elements.push_back(this);
}
void ExotericWallShape::DrawShape()
{
	glColor3ub(255, 255, 255);
	CubeShape::DrawShape();
}
BeamShape::BeamShape(std::vector<CubeShape*> &elements,Point p,float length,float width,float height):CubeShape(p,length, width, height)
{ 
	elements.push_back(this);
}
void BeamShape::DrawShape()
{
	glColor3ub(54,54,54);
	CubeShape::DrawShape();
}
CeilingShape::CeilingShape(std::vector<CubeShape*> &elements,Point p,float length,float width,float height):CubeShape(p,length, width, height)
{ 
	elements.push_back(this);
}
void CeilingShape::DrawShape()
{  
	glColor3ub(54,54,54);
	CubeShape::DrawShape();
}
GlassShape::GlassShape(std::vector<CubeShape*> &elements,Point p,float length,float width,float height):CubeShape(p,length, width, height)
{ 
	elements.push_back(this);
}
void GlassShape::DrawShape()
{  
	//color，the last para is alpha value，
	//set the transparency to 0.4.
	glColor4f(0.0f,0.0f,1.0f,0.4f);
	//Enable the transparency
	glEnable (GL_BLEND);
	glDepthMask (GL_FALSE);
	glBlendFunc (GL_SRC_ALPHA, GL_ONE);
	CubeShape::DrawShape();
	glDepthMask (GL_TRUE);
	glDisable (GL_BLEND);
}

//GLFrame.cpp

//======================================================================
//
//  @file  Framework.cpp
//
//  项目描述： OpenGL程序框架演示
//  文件描述:  键盘类和程序框架类
//  适用平台： Windows98/2000/NT/XP/WIN7
// 
//  你必须在你的继承类中完成以下函数的实现
//                   
//  GLApplication * GLApplication::Create(const char * class_name)  
//  创建你的子类的一个实例  
//                   
//  bool Init();              
//  执行所有的初始化工作，如果成功函数返回true       
//                   
//  void Uninit();             
//  执行所有的卸载工作          
//                   
//  void Update(DWORD milliseconds);          
//  执行所有的更新操作，传入的参数为两次操作经过的时间，以毫秒为单位
//                   
//  void Draw();               
//  执行所有的绘制操作
//                   
//======================================================================
#include "stdafx.h"

#include <gl/gl.h>              // 包含OpenGL头文件
#include <gl/glu.h>   
#define MAX_LOADSTRING 100
// 全局变量:
HINSTANCE hInst;        // 当前实例
TCHAR szTitle[MAX_LOADSTRING];     // 标题栏文本
TCHAR szWindowClass[MAX_LOADSTRING];   // 主窗口类名

// 主程序入口
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	int ret = -1;
	GLApplication * appl = GLApplication::Create(L"OpenGL");   // 创建程序类
	if (appl != 0)
	{
		ret = appl->Main(hInstance, hPrevInstance, lpCmdLine, nCmdShow);// 执行程序主循环
		delete appl;             // 删除程序类（在继承类中，使用GL_Example * example = new GL_Example(class_name);分配了一块内存）
	}
	else
	{                 // 创建程序出错
		MessageBoxA(HWND_DESKTOP, "创建程序出错", "Error", MB_OK | MB_ICONEXCLAMATION);
	}
	return ret;
}                  
// 处理窗口消息
LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	LONG user_data = GetWindowLong(hWnd, GWL_USERDATA);     // 返回用户自定义的32位的程序附加值
	if (user_data == 0)
	{ 
		/// 如果程序第一次运行
		if (uMsg == WM_CREATE)           // 处理窗口创建消息
		{                
			/// 返回窗口结构的指针，它保存刚创建的程序实例的类
			CREATESTRUCT * creation = reinterpret_cast<CREATESTRUCT *>(lParam);
			/// 获得程序实例的指针
			GLApplication * appl = reinterpret_cast<GLApplication *>(creation->lpCreateParams);
			/// 保存程序实例的指针为用户自定义的程序附加值
			SetWindowLong(hWnd, GWL_USERDATA, reinterpret_cast<LONG>(appl));
			appl->m_IsVisible = true;         // 设置程序可见
			return 0;             // 返回
		}
	}
	else
	{ /// 如果不是第一次窗口，返回程序实例的指针
		GLApplication * appl = reinterpret_cast<GLApplication *>(user_data);
		return appl->Message(hWnd, uMsg, wParam, lParam);    // 调用程序实例自己的消息处理函数
	}
	return DefWindowProc(hWnd, uMsg, wParam, lParam);     // 调用默认的窗口消息处理函数
}

// 构造函数
GLApplication::GLApplication(const wchar_t * class_name)     
{
	m_ClassName = class_name;           // 保存类名
	m_IsProgramLooping = true;           // 设置程序循环为true
	m_CreateFullScreen = true;           // 使用全屏模式
	m_IsVisible = false;            // 不可见
	m_ResizeDraw = false;            // 在窗口改变大小的时候，不可绘制
	m_LastTickCount = 0;
}
void GLApplication::ToggleFullscreen()         // 切换 全屏/窗口模式
{
	PostMessage(m_Window, WM_TOGGLEFULLSCREEN, 0, 0);     // 发送自定的切换消息
}
void GLApplication::TerminateApplication()        // 结束程序
{
	PostMessage(m_Window, WM_QUIT, 0, 0);        // 发送退出消息
	m_IsProgramLooping = false;           // 停止程序循环
}
void GLApplication::ReDraw()
{
	m_Window.ReshapeGL();         // 重新设置窗口的大小
	Draw();             // 重新绘制
	m_Window.SwapBuffers();         // 交换前后帧缓存
}
// “关于”框的消息处理程序。
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;
	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}
// 消息循环
LRESULT GLApplication::Message(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	switch (uMsg)              // 处理不同的Windows消息
	{
	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// 分析菜单选择:
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			TerminateApplication();          // 调用TerminateApplication函数
			return 0;      
		}
		break;
	case WM_SYSCOMMAND:            // 截获系统命令
		switch (wParam)            
		{
		case SC_SCREENSAVE:          // 截获屏幕保护启动命令
		case SC_MONITORPOWER:         // 截获显示其省电模式启动命令
			return 0;           // 不启用这两个命令
			break;
		}
		break;               // 退出
	case WM_CLOSE:             // 关闭窗口
		TerminateApplication();          // 调用TerminateApplication函数
		return 0;             
		break;
	case WM_EXITMENULOOP:
	case WM_EXITSIZEMOVE:
		m_LastTickCount = GetTickCount();       // 更新计数器的值
		return 0;
		break;
	case WM_MOVE:
		m_Window.SetPosX(LOWORD(lParam));       // 更新鼠标的坐标
		m_Window.SetPosY(HIWORD(lParam));       
		return 0;
		break;
	case WM_PAINT:
		if (m_ResizeDraw == true)         // 如果需要重绘
		{
			m_Window.ReshapeGL();         // 重新设置窗口的大小
			Draw();             // 重新绘制
			m_Window.SwapBuffers();         // 交换前后帧缓存
		}
		break;
	case WM_SIZING:             // 窗口正在改变大小
		{
			RECT * rect = (RECT *)lParam;
			m_Window.SetWidth(rect->right - rect->left);    // 设置窗口宽度
			m_Window.SetHeight(rect->bottom - rect->top);    // 设置窗口高度
			return TRUE;
		}
		break;
	case WM_SIZE:             // 窗口改变大小后
		switch (wParam)            // 处理不同的窗口状态
		{
		case SIZE_MINIMIZED:         // 是否最小化?
			m_IsVisible = false;        // 如果是，则设置不可见
			return 0;           
			break;
		case SIZE_MAXIMIZED:         // 窗口是否最大化?
		case SIZE_RESTORED:          // 窗口被还原?
			m_IsVisible = true;         // 设置为可见
			m_Window.SetWidth(LOWORD(lParam));     // 设置窗口宽度
			m_Window.SetHeight(HIWORD(lParam));     // 设置窗口高度
			m_Window.ReshapeGL();        // 改变窗口大小
			m_LastTickCount = GetTickCount();     // 更新计数器的值
			return 0;           
			break;
		}
		break;               
	case WM_KEYDOWN:            // 更新按键信息
		m_Keys.SetPressed(wParam);         
		return 0;             
		break;
	case WM_KEYUP:             // 更新释放键信息
		m_Keys.SetReleased(wParam);         
		return 0;             
		break;
	case WM_TOGGLEFULLSCREEN:          // 切换 全屏/窗口模式
		//  m_Window.SetFullScreen(!m_Window.GetFullScreen());
		//if(!m_Window.GetFullScreen())
		//ShowCursor(true);
		//else
		//ShowCursor(false);
		m_CreateFullScreen = !m_CreateFullScreen;
		PostMessage(hWnd, WM_QUIT, 0, 0);
		break;  
	case WM_LBUTTONDOWN:
		m_Keys.SetPressed(uMsg);         
		return 0;    
		break;
	case WM_LBUTTONUP:
		m_Keys.SetReleased(uMsg-1);         
		return 0;    
		break;
	}
	return DefWindowProc(hWnd, uMsg, wParam, lParam);     // 调用默认的窗口消息处理函数
}

// 程序的主循环
int GLApplication::Main(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	LoadString(hInstance, IDC_BUILDING, szWindowClass, MAX_LOADSTRING);
	/// 注册一个窗口
	WNDCLASSEX windowClass;            // 窗口类
	ZeroMemory(&windowClass, sizeof(WNDCLASSEX));      // 清空结构为0
	windowClass.cbSize   = sizeof(WNDCLASSEX);     // 窗口结构的大小
	windowClass.style   = CS_HREDRAW | CS_VREDRAW | CS_OWNDC; // 设置窗口类型为，移动时重画，并为窗口取得DC
	windowClass.lpfnWndProc  = (WNDPROC)(WindowProc);    // WndProc处理消息
	windowClass.hInstance  = hInstance;       // 设置实例
	windowClass.hbrBackground = (HBRUSH)(COLOR_APPWORKSPACE);   // 设置背景
	windowClass.hCursor   = LoadCursor(NULL, IDC_ARROW);   // 载入光标
	windowClass.lpszClassName = m_ClassName;       // 设置类名
	windowClass.lpszMenuName = MAKEINTRESOURCE(IDC_BUILDING);
	if (RegisterClassEx(&windowClass) == 0)        // 尝试注册窗口类
	{                 // NOTE: Failure, Should Never Happen
		MessageBoxA(HWND_DESKTOP, "注册窗口失败!", "Error", MB_OK | MB_ICONEXCLAMATION);
		return -1;              // 退出并返回FALSE
	}
	// 询问是否在全屏状态下运行?
	if (MessageBoxA(HWND_DESKTOP, "Do you want to run with full screen?", " Whether full screen mode?", MB_YESNO | MB_ICONQUESTION) == IDNO)
	{
		m_CreateFullScreen = false;          // m_CreateFullScreen记录当前的显示模式为窗口
	}
	//ScreenDlg sd;
	//sd.SetupWindow(&m_Window);
	//m_CreateFullScreen = m_Window.GetFullScreen();
	while (m_IsProgramLooping)           // 循环直道WM_QUIT退出程序
	{                 
		/// 创建一个窗口
		if (m_Window.Create(L"Building", m_ClassName,m_CreateFullScreen, hInstance, this) == true)
		{ 
			/// 如果初始化失败，则退出
			if (Init() == false)         // 调用自定义的初始化函数
			{               // 失败
				TerminateApplication();         // 关闭窗口退出程序
			}
			else              // 成功开始消息循环
			{               
				MSG msg;            // Window消息结构 
				bool isMessagePumpActive = true;      // 当消息不为空时，处理消息循环
				m_LastTickCount = GetTickCount();      // 返回当前的计时器的值
				m_Keys.Clear();           // 清空所有的按键信息
				while (isMessagePumpActive == true)      // 当消息不为空时，处理消息循环
				{              // 成功创建窗口，检测窗口消息
					if (PeekMessage(&msg, m_Window, 0, 0, PM_REMOVE) != 0)
					{             
						/// 检测是否为WM_QUIT消息
						if (msg.message != WM_QUIT)      
						{
							DispatchMessage(&msg);      // 如果不是发送消息到消息回调函数中处理
						}
						else           
						{
							isMessagePumpActive = false;    // 如果是，则退出
						}
					}
					/// 如果没有消息
					else            
					{
						if (m_IsVisible == false)      // 如果窗口不可见
						{
							WaitMessage();        // 暂停程序，等待消息
						}
						else           // 如果窗口可见
						{            // 执行消息循环
							DWORD tickCount = GetTickCount();   // 返回计时器的当前值
							Update(tickCount - m_LastTickCount);  // 调用用户自定义的更新函数
							m_LastTickCount = tickCount;    // 重新设置上一次，计数器的值
							Draw();          // 调用用户自定义的绘制函数
							m_Window.SwapBuffers();      // 交换前后帧缓存
						}
					}
				}              // 如果isMessagePumpActive == true，则循环
			}               
			// 程序结束
			Uninit();            // 用户自定义的卸载函数
			m_Window.Destroy();           // 删除窗口
		}
		else               // 如果创建窗口失败
		{                
			MessageBoxA(HWND_DESKTOP, "创建OpenGL窗口错误", "Error", MB_OK | MB_ICONEXCLAMATION);
			m_IsProgramLooping = false;         // 停止程序循环
		}
	}                 
	UnregisterClass(m_ClassName, hInstance);       // 取消注册的窗口
	return 0;
}
//GLWindow.cpp

//========================================================
//
//  @file  GLWindows.h
//
//  项目描述： OpenGL程序框架演示
//  文件描述:  程序窗口类
//  适用平台： Windows98/2000/NT/XP/WIN7
//    
//========================================================
#include "stdafx.h"
       // 包含GLwindows.h头文件
#include <gl/gl.h>              // 包含OpenGL头文件
#include <gl/glu.h>              

// 构造函数
GLWindow::GLWindow()             
{
	m_WindowPosX = 0;            // 窗口的左上角的X位置
	m_WindowPosY = 0;            // 窗口的左上角的Y位置
	m_WindowWidth = 840;            // 窗口的宽度
	m_WindowHeight = 680;            // 窗口的高度
	m_ScreenWidth = 1024;            // 全屏的宽度
	m_ScreenHeight = 768;            // 全屏的高度
	m_BitsPerPixel = 16;            // 颜色位深
	m_IsFullScreen = false;           // 不使用全屏
	m_hWnd = 0;
	m_hDC = 0;
	m_hRC = 0;
}
// 返回窗口的大小
int GLWindow::GetWidth()
{
	if (m_IsFullScreen == true)
	{
		return m_ScreenWidth;
	}
	else
	{
		return m_WindowWidth;
	}
}
int GLWindow::GetHeight()
{
	if (m_IsFullScreen == true)
	{
		return m_ScreenHeight;
	}
	else
	{
		return m_WindowHeight;
	}
}
// 设置窗口的大小
void GLWindow::SetWidth(int width)
{
	if (m_IsFullScreen == true)
	{
		m_ScreenWidth = width;
	}
	else
	{
		m_WindowWidth = width;
	}
}
void GLWindow::SetHeight(int height)
{
	if (m_IsFullScreen == true)
	{
		m_ScreenHeight = height;
	}
	else
	{
		m_WindowHeight = height;
	}
}
// 返回窗口左上角的位置
int GLWindow::GetPosX()
{
	if (m_IsFullScreen == false)
	{
		return m_WindowPosX;
	}
	return 0;
}
int GLWindow::GetPosY()
{
	if (m_IsFullScreen == false)
	{
		return m_WindowPosY;
	}
	return 0;
}
// 设置窗口左上角的位置
void GLWindow::SetPosX(int x)
{
	if (m_IsFullScreen == false)
	{
		m_WindowPosX = x;
	}
}
void GLWindow::SetPosY(int y)
{
	if (m_IsFullScreen == false)
	{
		m_WindowPosY = y;
	}
}
// 当窗口大小改变时，通知OpenGL调整大小
void GLWindow::ReshapeGL()
{
	GLsizei width = GetWidth();
	GLsizei height = GetHeight();
	glViewport(0, 0, width, height);         // 重新设置视口
	//glMatrixMode(GL_PROJECTION);         
	//glLoadIdentity();             
	//gluPerspective(45.0f, (GLfloat)width / (GLfloat)height, 1.0f, 100.0f); 
	//glMatrixMode(GL_MODELVIEW);           
	//glLoadIdentity();             
}
// 改变窗口的显示设置
bool GLWindow::ChangeScreenSetting()
{
	DEVMODE dmScreenSettings;           // 设备模式
	ZeroMemory(&dmScreenSettings, sizeof(DEVMODE));      // 清零结构
	dmScreenSettings.dmSize   = sizeof(DEVMODE);     // 结构大小
	dmScreenSettings.dmPelsWidth = GetWidth();      // 设置宽度
	dmScreenSettings.dmPelsHeight = GetHeight();      // 设置高度
	dmScreenSettings.dmBitsPerPel = m_BitsPerPixel;     // 设置位深度
	//dmScreenSettings.dmDisplayFrequency = 75;                           // 设置屏幕刷新率
	dmScreenSettings.dmFields  = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT ;//| DM_DISPLAYFREQUENCY;
	/// 改变显示模式
	if (ChangeDisplaySettings(&dmScreenSettings, 0) != DISP_CHANGE_SUCCESSFUL)
	{
		return false;             // 如果失败，返回false
	}
	return true;              // 成功返回
}
// 创建windows窗口
bool GLWindow::Create(const wchar_t * window_title,const wchar_t * class_name,bool fullscreen, HINSTANCE h_instance, LPVOID lpParam)
{
	m_IsFullScreen = fullscreen;
	int nX=0;
	int nY=0;
	PIXELFORMATDESCRIPTOR pfd =           // 设置像素描述结构
	{
		sizeof(PIXELFORMATDESCRIPTOR),         // 像素描述结构的大小 
		1,                // 版本号
		PFD_DRAW_TO_WINDOW |           // 缓存区的输出显示在一个窗口中
		PFD_SUPPORT_OPENGL |           // 缓存区支持OpenGL绘图
		PFD_STEREO   |           // 颜色缓存区是立体缓存
		PFD_DOUBLEBUFFER,            // 颜色缓存区是双缓存
		PFD_TYPE_RGBA,             // 使用RGBA颜色格式
		m_BitsPerPixel,             // 颜色缓存区中颜色值所占的位深
		0, 0, 0, 0, 0, 0,            // 使用默认的颜色设置
		0,                // 无Alpha缓存
		0,                // 颜色缓存区中alpha成分的移位计数
		0,                // 无累计缓存区
		0, 0, 0, 0,              // 累计缓存区无移位
		32,                // 32位深度缓存
		0,                // 无蒙版缓存
		0,                // 无辅助缓存区
		PFD_MAIN_PLANE,             // 必须为PFD_MAIN_PLANE，设置为主绘图层
		0,                // 表示OpenGL实现所支持的上层或下层平面的数量
		0, 0, 0               // 过时，已不再使用
	};
	DWORD windowStyle = WS_OVERLAPPEDWINDOW & ~WS_SIZEBOX & ~WS_MAXIMIZEBOX; // 定义我们窗口类型，使用常规设定，去掉最大化按钮，并不能改变窗体大小
	DWORD windowExtendedStyle = WS_EX_APPWINDOW;      
	if (m_IsFullScreen == true)           // 如果为全屏模式，尝试转化为全屏模式
	{
		if (ChangeScreenSetting() == false)
		{                // 全屏模式转换失败，弹出对话框提示，并尝试窗口模式
			MessageBoxA(HWND_DESKTOP, "模式转换失败.\n在窗口模式下运行.", "Error", MB_OK | MB_ICONEXCLAMATION);
			m_IsFullScreen = false;          // 设置为窗口模式
		}
		else               // 如果为窗口模式
		{
			ShowCursor(false);           // 隐藏鼠标
			windowStyle = WS_POPUP;          // 设置窗口模式为顶层窗口
			windowExtendedStyle |= WS_EX_TOPMOST;      
		}                
	}
	/// 调整我们窗口的大小，使其客户区的大小为我们设置的大小
	RECT windowRect = {GetPosX(), GetPosY(), GetPosX() + GetWidth(), GetPosY() + GetHeight()};
	if (m_IsFullScreen == false)          // 在窗口模式下使用
	{ 
		windowExtendedStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE; // 使窗口具有3D外观
		int wid = GetSystemMetrics(SM_CXSCREEN);  // 获取当前屏幕宽
		int hei = GetSystemMetrics(SM_CYSCREEN);  // 获取当前屏幕高
		nX = (wid - GetWidth()) / 2;                    // 计算窗口居中用
		nY = (hei - GetHeight()) / 2;   
		/// 调整我们窗口的大小，使其客户区的大小为我们设置的大小
		AdjustWindowRectEx(&windowRect, windowStyle, 0, windowExtendedStyle);
		/// 判断窗口的左上角是否隐藏在桌面外
		if (windowRect.left < 0)          // 如果窗口X坐标为负，移动坐标到0处，并调整窗口的位置
		{
			windowRect.right -= windowRect.left;      
			windowRect.left = 0;          
		}
		if (windowRect.top < 0)           // 如果窗口Y坐标为负，移动坐标到0处，并调整窗口的位置
		{
			windowRect.bottom -= windowRect.top;      
			windowRect.top = 0;           
		}
	}
	/// 创建窗口
	m_hWnd = CreateWindowEx(windowExtendedStyle,      // 窗口的扩展风格
		class_name,         // 窗口的类名
		window_title,        // 窗口标题
		windowStyle,        // 窗口的风格
		nX,nY,                                      // 窗口的左上角位置
		windowRect.right - windowRect.left,   // 窗口的宽度
		windowRect.bottom - windowRect.top,   // 窗口的高度
		HWND_DESKTOP,        // 窗口的父窗口为桌面
		0,           // 无菜单
		h_instance,         // 传入窗口的实例句柄
		lpParam);         // 传入程序类参数
	while (m_hWnd != 0)             // 窗口是否创建成功
	{
		m_hDC = GetDC(m_hWnd);           // 返回窗口的设备描述表
		if (m_hDC == 0)             // 如果为空
		{                // 失败
			break;              
		}
		GLuint PixelFormat = ChoosePixelFormat(m_hDC, &pfd);   // 查找一个兼容的像素格式
		if (PixelFormat == 0)           // 如果没找到
		{                // 失败
			break;              
		}
		if (SetPixelFormat(m_hDC, PixelFormat, &pfd) == false)   // 设置像素格式
		{                // 失败
			break;              
		}
		m_hRC = wglCreateContext(m_hDC);        // 创建OpenGL的渲染描述表
		if (m_hRC == 0)             // 如果为空
		{                // 失败
			break;              
		}
		if (wglMakeCurrent(m_hDC, m_hRC) == false)      // 设置当前的OpenGL的渲染对象为当前的窗口
		{                // 失败
			break;              
		}

		ShowWindow(m_hWnd, SW_NORMAL);         // 显示窗口
		ReshapeGL();             // 告诉OpenGL调整窗口大小
		return true;             // 成功返回
	}                 
	Destroy();               // 释放资源
	return false;              // 返回失败
}
// 删除OpenGL窗口
void GLWindow::Destroy()         
{
	if (m_hWnd != 0)          // 窗口句柄是否存在
	{
		if (m_hDC != 0)          // 窗口设备描述表是否存在
		{
			wglMakeCurrent(m_hDC, 0);      // 设置当前的OpenGL的渲染对象为空NULL
			if (m_hRC != 0)         // OpenGL的渲染描述表是否存在
			{
				wglDeleteContext(m_hRC);     // 释放OpenGL的渲染描述表
				m_hRC = 0;         // 设置OpenGL的渲染描述表为0
			}
			ReleaseDC(m_hWnd, m_hDC);      // 释放窗口的设备描述表
			m_hDC = 0;          // 设置窗口的设备描述表为0
		}
		DestroyWindow(m_hWnd);        // 删除窗口
		m_hWnd = 0;           // 设置窗口句柄为0
	}
	if (m_IsFullScreen)          // 如果为全屏模式，在程序结束后，变换到窗口模式，并显示鼠标
	{
		ChangeDisplaySettings(NULL, 0);      // 变换到窗口模式
		ShowCursor(true);         // 显示鼠标
	}
}
//image.cpp

// This code lets your read in SGI .rgb files.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef SEEK_SET
#  define SEEK_SET 0
#endif

typedef unsigned int GLuint;  // Should be 32-bit value.
typedef int GLint;  //Should be 32-bit value.
typedef struct _rawImageRec {
	unsigned short imagic;
	unsigned short type;
	unsigned short dim;
	unsigned short sizeX, sizeY, sizeZ;
	unsigned long min, max;
	unsigned long wasteBytes;
	char name[80];
	unsigned long colorMap;
	FILE *file;
	unsigned char *tmp, *tmpR, *tmpG, *tmpB;
	unsigned long rleEnd;
	GLuint *rowStart;
	GLint *rowSize;
} rawImageRec;

static void ConvertShort(unsigned short *array, unsigned int length)
{
	unsigned short b1, b2;
	unsigned char *ptr;
	ptr = (unsigned char *)array;
	while (length--) {
		b1 = *ptr++;
		b2 = *ptr++;
		*array++ = (b1 << 8) | (b2);
	}
}
static void ConvertUint(GLuint *array, unsigned int length)
{
	unsigned int b1, b2, b3, b4;
	unsigned char *ptr;
	ptr = (unsigned char *)array;
	while (length--) {
		b1 = *ptr++;
		b2 = *ptr++;
		b3 = *ptr++;
		b4 = *ptr++;
		*array++ = (b1 << 24) | (b2 << 16) | (b3 << 8) | (b4);
	}
}
static rawImageRec *RawImageOpen(char *fileName)
{
	union {
		int testWord;
		char testByte[4];
	} endianTest;
	rawImageRec *raw;
	int swapFlag;
	int x;
	endianTest.testWord = 1;
	if (endianTest.testByte[0] == 1) {
		swapFlag = 1;
	} else {
		swapFlag = 0;
	}
	raw = (rawImageRec *)malloc(sizeof(rawImageRec));
	if (raw == NULL) {
		fprintf(stderr, "Out of memory!\n");
		exit(1);
	}
	if ((raw->file = fopen(fileName, "rb")) == NULL) {
		perror(fileName);
		exit(1);
	}
	fread(raw, 1, 12, raw->file);
	if (swapFlag) {
		ConvertShort(&raw->imagic, 6);
	}
	raw->tmp = (unsigned char *)malloc(raw->sizeX*256);
	raw->tmpR = (unsigned char *)malloc(raw->sizeX*256);
	raw->tmpG = (unsigned char *)malloc(raw->sizeX*256);
	raw->tmpB = (unsigned char *)malloc(raw->sizeX*256);
	if (raw->tmp == NULL || raw->tmpR == NULL || raw->tmpG == NULL ||
		raw->tmpB == NULL) {
			fprintf(stderr, "Out of memory!\n");
			exit(1);
	}
	if ((raw->type & 0xFF00) == 0x0100) {
		x = raw->sizeY * raw->sizeZ * (int) sizeof(GLuint);
		raw->rowStart = (GLuint *)malloc(x);
		raw->rowSize = (GLint *)malloc(x);
		if (raw->rowStart == NULL || raw->rowSize == NULL) {
			fprintf(stderr, "Out of memory!\n");
			exit(1);
		}
		raw->rleEnd = 512 + (2 * x);
		fseek(raw->file, 512, SEEK_SET);
		fread(raw->rowStart, 1, x, raw->file);
		fread(raw->rowSize, 1, x, raw->file);
		if (swapFlag) {
			ConvertUint(raw->rowStart,
				(unsigned int) (x/sizeof(GLuint)));
			ConvertUint((GLuint *)raw->rowSize,
				(unsigned int) (x/sizeof(GLint)));
		}
	}
	return raw;
}
static void RawImageClose(rawImageRec *raw)
{
	fclose(raw->file);
	free(raw->tmp);
	free(raw->tmpR);
	free(raw->tmpG);
	free(raw->tmpB);
	free(raw);
}
static void RawImageGetRow(rawImageRec *raw, unsigned char *buf, int y, int z)
{
	unsigned char *iPtr, *oPtr, pixel;
	int count;
	if ((raw->type & 0xFF00) == 0x0100) {
		fseek(raw->file, (long) raw->rowStart[y+z*raw->sizeY], SEEK_SET);
		fread(raw->tmp, 1, (unsigned int)raw->rowSize[y+z*raw->sizeY],
			raw->file);
		iPtr = raw->tmp;
		oPtr = buf;
		for (;;) {
			pixel = *iPtr++;
			count = (int)(pixel & 0x7F);
			if (!count) {
				return;
			}
			if (pixel & 0x80) {
				while (count--) {
					*oPtr++ = *iPtr++;
				}
			} else {
				pixel = *iPtr++;
				while (count--) {
					*oPtr++ = pixel;
				}
			}
		}
	} else {
		fseek(raw->file, 512+(y*raw->sizeX)+(z*raw->sizeX*raw->sizeY),
			SEEK_SET);
		fread(buf, 1, raw->sizeX, raw->file);
	}
}
static void RawImageGetData(rawImageRec *raw, RGBImageRec *final)
{
	unsigned char *ptr;
	int i, j;
	final->data = (unsigned char *)malloc((raw->sizeX+1)*(raw->sizeY+1)*4);
	if (final->data == NULL) {
		fprintf(stderr, "Out of memory!\n");
		exit(1);
	}
	ptr = final->data;
	for (i = 0; i < (int)(raw->sizeY); i++) {
		RawImageGetRow(raw, raw->tmpR, i, 0);
		RawImageGetRow(raw, raw->tmpG, i, 1);
		RawImageGetRow(raw, raw->tmpB, i, 2);
		for (j = 0; j < (int)(raw->sizeX); j++) {
			*ptr++ = *(raw->tmpR + j);
			*ptr++ = *(raw->tmpG + j);
			*ptr++ = *(raw->tmpB + j);
		}
	}
}
RGBImageRec *RGBImageLoad(char *fileName)
{
	rawImageRec *raw;
	RGBImageRec *final;
	raw = RawImageOpen(fileName);
	final = (RGBImageRec *)malloc(sizeof(RGBImageRec));
	if (final == NULL) {
		fprintf(stderr, "Out of memory!\n");
		exit(1);
	}
	final->sizeX = raw->sizeX;
	final->sizeY = raw->sizeY;
	RawImageGetData(raw, final);
	RawImageClose(raw);
	return final;
}
