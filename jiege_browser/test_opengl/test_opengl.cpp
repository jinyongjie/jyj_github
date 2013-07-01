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



//����GLWindow.hͷ�ļ�
// ���������
class Keys              
{
public:
	// ���캯��
	Keys() { Clear(); }
	// ������еİ�����Ϣ
	void Clear() { ZeroMemory(&m_KeyDown, sizeof(m_KeyDown)); }
	// �ж�ĳ�����Ƿ���
	bool IsPressed(unsigned int key) { return (key < MAX_KEYS) ? (m_KeyDown[key] == true) : false; }
	// ����ĳ����������
	void SetPressed(unsigned int key) { if (key < MAX_KEYS) m_KeyDown[key] = true; }
	// ����ĳ�������ͷ�
	void SetReleased(unsigned int key) { if (key < MAX_KEYS) m_KeyDown[key] = false; }
private:
	static const unsigned int MAX_KEYS = 1024;
	bool m_KeyDown[MAX_KEYS];         //����1024��������״̬
};

// windows������
class GLWindow               
{
public:
	// ��Ҫ�Ĵ��ں��� 
	// ���캯��
	GLWindow();
	// ����windows����
	bool Create(const wchar_t * window_title, const wchar_t * class_name, bool fullscreen,HINSTANCE h_instance, LPVOID lpParam);
	// ɾ��OpenGL����
	void Destroy();
	// �ı䴰�ڵ���ʾ����
	bool ChangeScreenSetting();
	// �����ڴ�С�ı�ʱ��֪ͨOpenGL������С
	void ReshapeGL();
	// Swap Buffers (Double Buffering)
	void SwapBuffers() { ::SwapBuffers(m_hDC); }
	// ���溯��Ϊ����/���ش����������
	// ���ô������Ͻǵ�λ��
	void SetPosX(int x);
	void SetPosX(unsigned short x) { SetPosX((signed short)x); }  
	void SetPosY(int y);
	void SetPosY(unsigned short y) { SetPosY((signed short)y); }  
	// ���ش��ڵĴ�С
	int  GetWidth();
	int  GetHeight();
	// ���ô��ڵĴ�С
	void SetWidth(int width);
	void SetHeight(int height);
	// ���ش������Ͻǵ�λ��
	int  GetPosX();
	int  GetPosY();
	// ���ô��ڵ���ɫλ��
	void SetHiColor() { m_BitsPerPixel = 16; }
	void SetTrueColor() { m_BitsPerPixel = 32; }
	//void SetFullScreen(bool mode) { m_IsFullScreen = mode; }
	//bool GetFullScreen() { return m_IsFullScreen; }
	// �����������������GL_Window m_Window�������m_Window��Ϊ���ھ��ʹ��
	operator HWND() { return m_hWnd; }
private:
	HWND m_hWnd;              // ���ھ��
	HDC  m_hDC;              // �豸������
	HGLRC m_hRC;              // OpenGL��Ⱦ������
	int  m_WindowPosX;            // ���ڵ����Ͻǵ�Xλ��
	int  m_WindowPosY;            // ���ڵ����Ͻǵ�Yλ��
	int  m_WindowWidth;            // ���ڵĿ��
	int  m_WindowHeight;            // ���ڵĸ߶�
	int  m_ScreenWidth;            // ȫ���Ŀ��
	int  m_ScreenHeight;            // ȫ���ĸ߶�
	int  m_BitsPerPixel;            // ��ɫλ��
	bool m_IsFullScreen;            // �Ƿ�ȫ��
};

// �����ĳ����࣬�̳�����������OpenGL����
class GLApplication           
{
public:
	// ����һ��ȫ�ֵ�Create����������������뱻�̳���ʵ��
	static GLApplication * Create(const wchar_t * class_name); //�������Լ�������
	// ����������
	virtual ~GLApplication() {};
protected:
	// ����ĺ������뱻�̳���ʵ�֣���ɻ�����OpenGL��Ⱦ����
	virtual bool Init() = 0;       //OpenGL�ĳ�ʼ�� 
	virtual void Uninit() = 0;       //OpenGL��ж��
	virtual void Update(DWORD milliseconds) = 0;    //ִ��OpenGL����ĸ���
	virtual void Draw() = 0;         //����OpenGL����
	// ͨ�õĺ���
	void ToggleFullscreen();         //�л�ȫ��/����ģʽ
	void TerminateApplication();        //��������
	void ResizeDraw(bool enable) { m_ResizeDraw = enable; } //�����ڴ��ڸı��С��ʱ�򣬿��Ի���
	Keys m_Keys;            //������
	// ���캯��
	GLApplication(const wchar_t * class_name); 
	void ReDraw();
private:
	// �������ѭ��
	friend int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow);
	int  Main(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow);
	// ��Ϣ����ص�����
	friend LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT Message(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	friend INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);

	static const UINT WM_TOGGLEFULLSCREEN = (WM_USER + 1);  //�Զ�����Ϣ�����л�����ģʽ��ʱ����
	GLWindow m_Window;          //Window��
	const wchar_t* m_ClassName;         //������
	bool  m_IsProgramLooping;        //����ѭ����ǣ����Ϊfalse�����˳�����
	bool        m_CreateFullScreen;                             //��Ϊtrue���򴴽�ȫ��ģʽ
	bool  m_IsVisible;         //�����Ƿ�ɼ�
	bool  m_ResizeDraw;         //�Ƿ��ڸı��Сʱ�������˻��ƺ���
	DWORD  m_LastTickCount;        //��һ�μ�ʱ����ֵ
};

          //���������Ŀ����
#define srandom srand
// ��GL_Application������һ������
class Buildings : GLApplication
{
public:
	~Buildings(void);
public:
	bool Init();          //< ִ�����еĳ�ʼ������������ɹ���������true
	void Uninit();          //< ִ�����е�ж�ع���
	void Update(DWORD milliseconds);       //< ִ�����еĸ��²���������Ĳ���Ϊ���β���������ʱ�䣬�Ժ���Ϊ��λ
	void Draw();            //< ִ�����еĻ��Ʋ���
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
	friend class GLApplication;        //< ����Ϊ����һ����Ԫ�࣬�����������������ʵ����������GL_Application * GL_Application::Create(const char * class_name)
	Buildings(const wchar_t * class_name);      //< ���캯��
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
//  ��Ŀ������ OpenGL��������ʾ
//  �ļ�����:  ������ͳ�������
//  ����ƽ̨�� Windows98/2000/NT/XP/WIN7
// 
//  ���������ļ̳�����������º�����ʵ��
//                   
//  GLApplication * GLApplication::Create(const char * class_name)  
//  ������������һ��ʵ��  
//                   
//  bool Init();              
//  ִ�����еĳ�ʼ������������ɹ���������true       
//                   
//  void Uninit();             
//  ִ�����е�ж�ع���          
//                   
//  void Update(DWORD milliseconds);          
//  ִ�����еĸ��²���������Ĳ���Ϊ���β���������ʱ�䣬�Ժ���Ϊ��λ
//                   
//  void Draw();               
//  ִ�����еĻ��Ʋ���
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
//  ��Ŀ������ OpenGL��������ʾ
//  �ļ�����:  ���򴰿��� 
//  ����ƽ̨�� Windows98/2000/NT/XP/WIN7
// 
//   
//========================================================
#ifndef __GLWINDOW_H__
#define __GLWINDOW_H__
#include <windows.h>                                             // ����windows.hͷ�ļ�
#pragma warning(disable: 4311)                                  // ��ֹ��������������ת���ľ���
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

// stdafx.h : ��׼ϵͳ�����ļ��İ����ļ���
// ���Ǿ���ʹ�õ��������ĵ�
// �ض�����Ŀ�İ����ļ�
//
#pragma once
#include "targetver.h"
#define WIN32_LEAN_AND_MEAN             // �� Windows ͷ���ų�����ʹ�õ�����
// Windows ͷ�ļ�:
#include <windows.h>
// C ����ʱͷ�ļ�
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>

// TODO: �ڴ˴����ó�����Ҫ������ͷ�ļ�
//targetver.h

#pragma once
// ���º궨��Ҫ������ƽ̨��Ҫ������ƽ̨
// �Ǿ�������Ӧ�ó������蹦�ܵ� Windows��Internet Explorer �Ȳ�Ʒ��
// ����汾��ͨ����ָ���汾�����Ͱ汾��ƽ̨���������п��õĹ��ܣ������
// ����������
// �������Ҫ��Ե�������ָ���汾��ƽ̨�����޸����ж��塣
// �йز�ͬƽ̨��Ӧֵ��������Ϣ����ο� MSDN��
#ifndef WINVER                          // ָ��Ҫ������ƽ̨�� Windows Vista��
#define WINVER 0x0600           // ����ֵ����Ϊ��Ӧ��ֵ���������� Windows �������汾��
#endif
#ifndef _WIN32_WINNT            // ָ��Ҫ������ƽ̨�� Windows Vista��
#define _WIN32_WINNT 0x0600     // ����ֵ����Ϊ��Ӧ��ֵ���������� Windows �������汾��
#endif
#ifndef _WIN32_WINDOWS          // ָ��Ҫ������ƽ̨�� Windows 98��
#define _WIN32_WINDOWS 0x0410 // ����ֵ����Ϊ�ʵ���ֵ���������� Windows Me ����߰汾��
#endif
#ifndef _WIN32_IE                       // ָ��Ҫ������ƽ̨�� Internet Explorer 7.0��
#define _WIN32_IE 0x0700        // ����ֵ����Ϊ��Ӧ��ֵ���������� IE �������汾��
#endif
//Buildings.cpp

#include "StdAfx.h"


#include <gl\gl.h>            // ����OpenGLͷ�ļ�
#include <gl\glu.h>            
#pragma comment(lib, "opengl32.lib")       //����OpenGL���ӿ��ļ�
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

// ����һ�������ʵ��
GLApplication * GLApplication::Create(const wchar_t * class_name)
{
	Buildings * buildings = new Buildings(class_name);
	return reinterpret_cast<GLApplication *>(buildings);
}
Buildings::~Buildings(void)
{
}
// ���캯��
Buildings::Buildings(const wchar_t * class_name) : GLApplication(class_name)
{
	/// ��ʼ���û��Զ���ĳ������
}
// ��ʼ��OpenGL
bool Buildings::Init()         
{
	// �û��Զ���ĳ�ʼ������
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
	ResizeDraw(true);           // �ı�OpenGL���ڴ�С��ֱ�ӵ�������ĺ���
	return true;            //�ɹ�����
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
// �û��Զ����ж�غ���
void Buildings::Uninit()         
{
	// �û��Զ����ж�ع���
	///......
	///......
}

// ������º���
void Buildings::Update(DWORD milliseconds)      
{
	if (m_Keys.IsPressed(VK_ESCAPE) == true)     // ��ESC�˳�
	{
		TerminateApplication();         
	}
	if (m_Keys.IsPressed(VK_F1) == true)      // ��F1�л�����/ȫ��ģʽ
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
	// �õ���ǰ���λ��
	GetCursorPos(&mousePos);      
	ShowCursor(TRUE);
	// ������û���ƶ�,���ø���
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
	//glTexCoord2f(GLfloat s, GLfloat t)����ԭ��
	//������������GLfloat s, GLfloat t
	//��һ����������X���ꡣ 0.0f ���������ࡣ 0.5f ��������е㣬 1.0f ��������Ҳࡣ
	//�ڶ�����������Y���ꡣ 0.0f ������ĵײ��� 0.5f ��������е㣬 1.0f ������Ķ�����
	//һ����ȫ������ĸ����������ֱ���(0.0f, 0.0f)��(0.0f, 1.0f)��(1.0f, 1.0f)��(1.0f, 0.0f)���ֱ��Ӧ���¡����ϡ����ϡ����½ǡ�
	//������ô��Ϊ������һ������һ��ճ���������ϣ���ʾ�������ʾ�ǲ�����ȫ���ĸ��������������
	//������ʾ3��2�и�λͼ���������������(0.0f, 0.0f)��(0.0f, 3.0f)��(2.0f, 3.0f)��(2.0f, 0.0f)
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
	// �õ���ǰ���λ��
	GetCursorPos(&_mousePos); 
	Constructors *constructors = new Constructors();
	constructors->Construct();
	LightFunction();
}
// ���ƺ���
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
	glFlush();             //< ǿ��ִ�����е�OpenGL����
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
	//color��the last para is alpha value��
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
//  ��Ŀ������ OpenGL��������ʾ
//  �ļ�����:  ������ͳ�������
//  ����ƽ̨�� Windows98/2000/NT/XP/WIN7
// 
//  ���������ļ̳�����������º�����ʵ��
//                   
//  GLApplication * GLApplication::Create(const char * class_name)  
//  ������������һ��ʵ��  
//                   
//  bool Init();              
//  ִ�����еĳ�ʼ������������ɹ���������true       
//                   
//  void Uninit();             
//  ִ�����е�ж�ع���          
//                   
//  void Update(DWORD milliseconds);          
//  ִ�����еĸ��²���������Ĳ���Ϊ���β���������ʱ�䣬�Ժ���Ϊ��λ
//                   
//  void Draw();               
//  ִ�����еĻ��Ʋ���
//                   
//======================================================================
#include "stdafx.h"

#include <gl/gl.h>              // ����OpenGLͷ�ļ�
#include <gl/glu.h>   
#define MAX_LOADSTRING 100
// ȫ�ֱ���:
HINSTANCE hInst;        // ��ǰʵ��
TCHAR szTitle[MAX_LOADSTRING];     // �������ı�
TCHAR szWindowClass[MAX_LOADSTRING];   // ����������

// ���������
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	int ret = -1;
	GLApplication * appl = GLApplication::Create(L"OpenGL");   // ����������
	if (appl != 0)
	{
		ret = appl->Main(hInstance, hPrevInstance, lpCmdLine, nCmdShow);// ִ�г�����ѭ��
		delete appl;             // ɾ�������ࣨ�ڼ̳����У�ʹ��GL_Example * example = new GL_Example(class_name);������һ���ڴ棩
	}
	else
	{                 // �����������
		MessageBoxA(HWND_DESKTOP, "�����������", "Error", MB_OK | MB_ICONEXCLAMATION);
	}
	return ret;
}                  
// ��������Ϣ
LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	LONG user_data = GetWindowLong(hWnd, GWL_USERDATA);     // �����û��Զ����32λ�ĳ��򸽼�ֵ
	if (user_data == 0)
	{ 
		/// ��������һ������
		if (uMsg == WM_CREATE)           // �����ڴ�����Ϣ
		{                
			/// ���ش��ڽṹ��ָ�룬������մ����ĳ���ʵ������
			CREATESTRUCT * creation = reinterpret_cast<CREATESTRUCT *>(lParam);
			/// ��ó���ʵ����ָ��
			GLApplication * appl = reinterpret_cast<GLApplication *>(creation->lpCreateParams);
			/// �������ʵ����ָ��Ϊ�û��Զ���ĳ��򸽼�ֵ
			SetWindowLong(hWnd, GWL_USERDATA, reinterpret_cast<LONG>(appl));
			appl->m_IsVisible = true;         // ���ó���ɼ�
			return 0;             // ����
		}
	}
	else
	{ /// ������ǵ�һ�δ��ڣ����س���ʵ����ָ��
		GLApplication * appl = reinterpret_cast<GLApplication *>(user_data);
		return appl->Message(hWnd, uMsg, wParam, lParam);    // ���ó���ʵ���Լ�����Ϣ������
	}
	return DefWindowProc(hWnd, uMsg, wParam, lParam);     // ����Ĭ�ϵĴ�����Ϣ������
}

// ���캯��
GLApplication::GLApplication(const wchar_t * class_name)     
{
	m_ClassName = class_name;           // ��������
	m_IsProgramLooping = true;           // ���ó���ѭ��Ϊtrue
	m_CreateFullScreen = true;           // ʹ��ȫ��ģʽ
	m_IsVisible = false;            // ���ɼ�
	m_ResizeDraw = false;            // �ڴ��ڸı��С��ʱ�򣬲��ɻ���
	m_LastTickCount = 0;
}
void GLApplication::ToggleFullscreen()         // �л� ȫ��/����ģʽ
{
	PostMessage(m_Window, WM_TOGGLEFULLSCREEN, 0, 0);     // �����Զ����л���Ϣ
}
void GLApplication::TerminateApplication()        // ��������
{
	PostMessage(m_Window, WM_QUIT, 0, 0);        // �����˳���Ϣ
	m_IsProgramLooping = false;           // ֹͣ����ѭ��
}
void GLApplication::ReDraw()
{
	m_Window.ReshapeGL();         // �������ô��ڵĴ�С
	Draw();             // ���»���
	m_Window.SwapBuffers();         // ����ǰ��֡����
}
// �����ڡ������Ϣ�������
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
// ��Ϣѭ��
LRESULT GLApplication::Message(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	switch (uMsg)              // ����ͬ��Windows��Ϣ
	{
	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// �����˵�ѡ��:
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			TerminateApplication();          // ����TerminateApplication����
			return 0;      
		}
		break;
	case WM_SYSCOMMAND:            // �ػ�ϵͳ����
		switch (wParam)            
		{
		case SC_SCREENSAVE:          // �ػ���Ļ������������
		case SC_MONITORPOWER:         // �ػ���ʾ��ʡ��ģʽ��������
			return 0;           // ����������������
			break;
		}
		break;               // �˳�
	case WM_CLOSE:             // �رմ���
		TerminateApplication();          // ����TerminateApplication����
		return 0;             
		break;
	case WM_EXITMENULOOP:
	case WM_EXITSIZEMOVE:
		m_LastTickCount = GetTickCount();       // ���¼�������ֵ
		return 0;
		break;
	case WM_MOVE:
		m_Window.SetPosX(LOWORD(lParam));       // ������������
		m_Window.SetPosY(HIWORD(lParam));       
		return 0;
		break;
	case WM_PAINT:
		if (m_ResizeDraw == true)         // �����Ҫ�ػ�
		{
			m_Window.ReshapeGL();         // �������ô��ڵĴ�С
			Draw();             // ���»���
			m_Window.SwapBuffers();         // ����ǰ��֡����
		}
		break;
	case WM_SIZING:             // �������ڸı��С
		{
			RECT * rect = (RECT *)lParam;
			m_Window.SetWidth(rect->right - rect->left);    // ���ô��ڿ��
			m_Window.SetHeight(rect->bottom - rect->top);    // ���ô��ڸ߶�
			return TRUE;
		}
		break;
	case WM_SIZE:             // ���ڸı��С��
		switch (wParam)            // ����ͬ�Ĵ���״̬
		{
		case SIZE_MINIMIZED:         // �Ƿ���С��?
			m_IsVisible = false;        // ����ǣ������ò��ɼ�
			return 0;           
			break;
		case SIZE_MAXIMIZED:         // �����Ƿ����?
		case SIZE_RESTORED:          // ���ڱ���ԭ?
			m_IsVisible = true;         // ����Ϊ�ɼ�
			m_Window.SetWidth(LOWORD(lParam));     // ���ô��ڿ��
			m_Window.SetHeight(HIWORD(lParam));     // ���ô��ڸ߶�
			m_Window.ReshapeGL();        // �ı䴰�ڴ�С
			m_LastTickCount = GetTickCount();     // ���¼�������ֵ
			return 0;           
			break;
		}
		break;               
	case WM_KEYDOWN:            // ���°�����Ϣ
		m_Keys.SetPressed(wParam);         
		return 0;             
		break;
	case WM_KEYUP:             // �����ͷż���Ϣ
		m_Keys.SetReleased(wParam);         
		return 0;             
		break;
	case WM_TOGGLEFULLSCREEN:          // �л� ȫ��/����ģʽ
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
	return DefWindowProc(hWnd, uMsg, wParam, lParam);     // ����Ĭ�ϵĴ�����Ϣ������
}

// �������ѭ��
int GLApplication::Main(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	LoadString(hInstance, IDC_BUILDING, szWindowClass, MAX_LOADSTRING);
	/// ע��һ������
	WNDCLASSEX windowClass;            // ������
	ZeroMemory(&windowClass, sizeof(WNDCLASSEX));      // ��սṹΪ0
	windowClass.cbSize   = sizeof(WNDCLASSEX);     // ���ڽṹ�Ĵ�С
	windowClass.style   = CS_HREDRAW | CS_VREDRAW | CS_OWNDC; // ���ô�������Ϊ���ƶ�ʱ�ػ�����Ϊ����ȡ��DC
	windowClass.lpfnWndProc  = (WNDPROC)(WindowProc);    // WndProc������Ϣ
	windowClass.hInstance  = hInstance;       // ����ʵ��
	windowClass.hbrBackground = (HBRUSH)(COLOR_APPWORKSPACE);   // ���ñ���
	windowClass.hCursor   = LoadCursor(NULL, IDC_ARROW);   // ������
	windowClass.lpszClassName = m_ClassName;       // ��������
	windowClass.lpszMenuName = MAKEINTRESOURCE(IDC_BUILDING);
	if (RegisterClassEx(&windowClass) == 0)        // ����ע�ᴰ����
	{                 // NOTE: Failure, Should Never Happen
		MessageBoxA(HWND_DESKTOP, "ע�ᴰ��ʧ��!", "Error", MB_OK | MB_ICONEXCLAMATION);
		return -1;              // �˳�������FALSE
	}
	// ѯ���Ƿ���ȫ��״̬������?
	if (MessageBoxA(HWND_DESKTOP, "Do you want to run with full screen?", " Whether full screen mode?", MB_YESNO | MB_ICONQUESTION) == IDNO)
	{
		m_CreateFullScreen = false;          // m_CreateFullScreen��¼��ǰ����ʾģʽΪ����
	}
	//ScreenDlg sd;
	//sd.SetupWindow(&m_Window);
	//m_CreateFullScreen = m_Window.GetFullScreen();
	while (m_IsProgramLooping)           // ѭ��ֱ��WM_QUIT�˳�����
	{                 
		/// ����һ������
		if (m_Window.Create(L"Building", m_ClassName,m_CreateFullScreen, hInstance, this) == true)
		{ 
			/// �����ʼ��ʧ�ܣ����˳�
			if (Init() == false)         // �����Զ���ĳ�ʼ������
			{               // ʧ��
				TerminateApplication();         // �رմ����˳�����
			}
			else              // �ɹ���ʼ��Ϣѭ��
			{               
				MSG msg;            // Window��Ϣ�ṹ 
				bool isMessagePumpActive = true;      // ����Ϣ��Ϊ��ʱ��������Ϣѭ��
				m_LastTickCount = GetTickCount();      // ���ص�ǰ�ļ�ʱ����ֵ
				m_Keys.Clear();           // ������еİ�����Ϣ
				while (isMessagePumpActive == true)      // ����Ϣ��Ϊ��ʱ��������Ϣѭ��
				{              // �ɹ��������ڣ���ⴰ����Ϣ
					if (PeekMessage(&msg, m_Window, 0, 0, PM_REMOVE) != 0)
					{             
						/// ����Ƿ�ΪWM_QUIT��Ϣ
						if (msg.message != WM_QUIT)      
						{
							DispatchMessage(&msg);      // ������Ƿ�����Ϣ����Ϣ�ص������д���
						}
						else           
						{
							isMessagePumpActive = false;    // ����ǣ����˳�
						}
					}
					/// ���û����Ϣ
					else            
					{
						if (m_IsVisible == false)      // ������ڲ��ɼ�
						{
							WaitMessage();        // ��ͣ���򣬵ȴ���Ϣ
						}
						else           // ������ڿɼ�
						{            // ִ����Ϣѭ��
							DWORD tickCount = GetTickCount();   // ���ؼ�ʱ���ĵ�ǰֵ
							Update(tickCount - m_LastTickCount);  // �����û��Զ���ĸ��º���
							m_LastTickCount = tickCount;    // ����������һ�Σ���������ֵ
							Draw();          // �����û��Զ���Ļ��ƺ���
							m_Window.SwapBuffers();      // ����ǰ��֡����
						}
					}
				}              // ���isMessagePumpActive == true����ѭ��
			}               
			// �������
			Uninit();            // �û��Զ����ж�غ���
			m_Window.Destroy();           // ɾ������
		}
		else               // �����������ʧ��
		{                
			MessageBoxA(HWND_DESKTOP, "����OpenGL���ڴ���", "Error", MB_OK | MB_ICONEXCLAMATION);
			m_IsProgramLooping = false;         // ֹͣ����ѭ��
		}
	}                 
	UnregisterClass(m_ClassName, hInstance);       // ȡ��ע��Ĵ���
	return 0;
}
//GLWindow.cpp

//========================================================
//
//  @file  GLWindows.h
//
//  ��Ŀ������ OpenGL��������ʾ
//  �ļ�����:  ���򴰿���
//  ����ƽ̨�� Windows98/2000/NT/XP/WIN7
//    
//========================================================
#include "stdafx.h"
       // ����GLwindows.hͷ�ļ�
#include <gl/gl.h>              // ����OpenGLͷ�ļ�
#include <gl/glu.h>              

// ���캯��
GLWindow::GLWindow()             
{
	m_WindowPosX = 0;            // ���ڵ����Ͻǵ�Xλ��
	m_WindowPosY = 0;            // ���ڵ����Ͻǵ�Yλ��
	m_WindowWidth = 840;            // ���ڵĿ��
	m_WindowHeight = 680;            // ���ڵĸ߶�
	m_ScreenWidth = 1024;            // ȫ���Ŀ��
	m_ScreenHeight = 768;            // ȫ���ĸ߶�
	m_BitsPerPixel = 16;            // ��ɫλ��
	m_IsFullScreen = false;           // ��ʹ��ȫ��
	m_hWnd = 0;
	m_hDC = 0;
	m_hRC = 0;
}
// ���ش��ڵĴ�С
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
// ���ô��ڵĴ�С
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
// ���ش������Ͻǵ�λ��
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
// ���ô������Ͻǵ�λ��
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
// �����ڴ�С�ı�ʱ��֪ͨOpenGL������С
void GLWindow::ReshapeGL()
{
	GLsizei width = GetWidth();
	GLsizei height = GetHeight();
	glViewport(0, 0, width, height);         // ���������ӿ�
	//glMatrixMode(GL_PROJECTION);         
	//glLoadIdentity();             
	//gluPerspective(45.0f, (GLfloat)width / (GLfloat)height, 1.0f, 100.0f); 
	//glMatrixMode(GL_MODELVIEW);           
	//glLoadIdentity();             
}
// �ı䴰�ڵ���ʾ����
bool GLWindow::ChangeScreenSetting()
{
	DEVMODE dmScreenSettings;           // �豸ģʽ
	ZeroMemory(&dmScreenSettings, sizeof(DEVMODE));      // ����ṹ
	dmScreenSettings.dmSize   = sizeof(DEVMODE);     // �ṹ��С
	dmScreenSettings.dmPelsWidth = GetWidth();      // ���ÿ��
	dmScreenSettings.dmPelsHeight = GetHeight();      // ���ø߶�
	dmScreenSettings.dmBitsPerPel = m_BitsPerPixel;     // ����λ���
	//dmScreenSettings.dmDisplayFrequency = 75;                           // ������Ļˢ����
	dmScreenSettings.dmFields  = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT ;//| DM_DISPLAYFREQUENCY;
	/// �ı���ʾģʽ
	if (ChangeDisplaySettings(&dmScreenSettings, 0) != DISP_CHANGE_SUCCESSFUL)
	{
		return false;             // ���ʧ�ܣ�����false
	}
	return true;              // �ɹ�����
}
// ����windows����
bool GLWindow::Create(const wchar_t * window_title,const wchar_t * class_name,bool fullscreen, HINSTANCE h_instance, LPVOID lpParam)
{
	m_IsFullScreen = fullscreen;
	int nX=0;
	int nY=0;
	PIXELFORMATDESCRIPTOR pfd =           // �������������ṹ
	{
		sizeof(PIXELFORMATDESCRIPTOR),         // ���������ṹ�Ĵ�С 
		1,                // �汾��
		PFD_DRAW_TO_WINDOW |           // �������������ʾ��һ��������
		PFD_SUPPORT_OPENGL |           // ������֧��OpenGL��ͼ
		PFD_STEREO   |           // ��ɫ�����������建��
		PFD_DOUBLEBUFFER,            // ��ɫ��������˫����
		PFD_TYPE_RGBA,             // ʹ��RGBA��ɫ��ʽ
		m_BitsPerPixel,             // ��ɫ����������ɫֵ��ռ��λ��
		0, 0, 0, 0, 0, 0,            // ʹ��Ĭ�ϵ���ɫ����
		0,                // ��Alpha����
		0,                // ��ɫ��������alpha�ɷֵ���λ����
		0,                // ���ۼƻ�����
		0, 0, 0, 0,              // �ۼƻ���������λ
		32,                // 32λ��Ȼ���
		0,                // ���ɰ滺��
		0,                // �޸���������
		PFD_MAIN_PLANE,             // ����ΪPFD_MAIN_PLANE������Ϊ����ͼ��
		0,                // ��ʾOpenGLʵ����֧�ֵ��ϲ���²�ƽ�������
		0, 0, 0               // ��ʱ���Ѳ���ʹ��
	};
	DWORD windowStyle = WS_OVERLAPPEDWINDOW & ~WS_SIZEBOX & ~WS_MAXIMIZEBOX; // �������Ǵ������ͣ�ʹ�ó����趨��ȥ����󻯰�ť�������ܸı䴰���С
	DWORD windowExtendedStyle = WS_EX_APPWINDOW;      
	if (m_IsFullScreen == true)           // ���Ϊȫ��ģʽ������ת��Ϊȫ��ģʽ
	{
		if (ChangeScreenSetting() == false)
		{                // ȫ��ģʽת��ʧ�ܣ������Ի�����ʾ�������Դ���ģʽ
			MessageBoxA(HWND_DESKTOP, "ģʽת��ʧ��.\n�ڴ���ģʽ������.", "Error", MB_OK | MB_ICONEXCLAMATION);
			m_IsFullScreen = false;          // ����Ϊ����ģʽ
		}
		else               // ���Ϊ����ģʽ
		{
			ShowCursor(false);           // �������
			windowStyle = WS_POPUP;          // ���ô���ģʽΪ���㴰��
			windowExtendedStyle |= WS_EX_TOPMOST;      
		}                
	}
	/// �������Ǵ��ڵĴ�С��ʹ��ͻ����Ĵ�СΪ�������õĴ�С
	RECT windowRect = {GetPosX(), GetPosY(), GetPosX() + GetWidth(), GetPosY() + GetHeight()};
	if (m_IsFullScreen == false)          // �ڴ���ģʽ��ʹ��
	{ 
		windowExtendedStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE; // ʹ���ھ���3D���
		int wid = GetSystemMetrics(SM_CXSCREEN);  // ��ȡ��ǰ��Ļ��
		int hei = GetSystemMetrics(SM_CYSCREEN);  // ��ȡ��ǰ��Ļ��
		nX = (wid - GetWidth()) / 2;                    // ���㴰�ھ�����
		nY = (hei - GetHeight()) / 2;   
		/// �������Ǵ��ڵĴ�С��ʹ��ͻ����Ĵ�СΪ�������õĴ�С
		AdjustWindowRectEx(&windowRect, windowStyle, 0, windowExtendedStyle);
		/// �жϴ��ڵ����Ͻ��Ƿ�������������
		if (windowRect.left < 0)          // �������X����Ϊ�����ƶ����굽0�������������ڵ�λ��
		{
			windowRect.right -= windowRect.left;      
			windowRect.left = 0;          
		}
		if (windowRect.top < 0)           // �������Y����Ϊ�����ƶ����굽0�������������ڵ�λ��
		{
			windowRect.bottom -= windowRect.top;      
			windowRect.top = 0;           
		}
	}
	/// ��������
	m_hWnd = CreateWindowEx(windowExtendedStyle,      // ���ڵ���չ���
		class_name,         // ���ڵ�����
		window_title,        // ���ڱ���
		windowStyle,        // ���ڵķ��
		nX,nY,                                      // ���ڵ����Ͻ�λ��
		windowRect.right - windowRect.left,   // ���ڵĿ��
		windowRect.bottom - windowRect.top,   // ���ڵĸ߶�
		HWND_DESKTOP,        // ���ڵĸ�����Ϊ����
		0,           // �޲˵�
		h_instance,         // ���봰�ڵ�ʵ�����
		lpParam);         // ������������
	while (m_hWnd != 0)             // �����Ƿ񴴽��ɹ�
	{
		m_hDC = GetDC(m_hWnd);           // ���ش��ڵ��豸������
		if (m_hDC == 0)             // ���Ϊ��
		{                // ʧ��
			break;              
		}
		GLuint PixelFormat = ChoosePixelFormat(m_hDC, &pfd);   // ����һ�����ݵ����ظ�ʽ
		if (PixelFormat == 0)           // ���û�ҵ�
		{                // ʧ��
			break;              
		}
		if (SetPixelFormat(m_hDC, PixelFormat, &pfd) == false)   // �������ظ�ʽ
		{                // ʧ��
			break;              
		}
		m_hRC = wglCreateContext(m_hDC);        // ����OpenGL����Ⱦ������
		if (m_hRC == 0)             // ���Ϊ��
		{                // ʧ��
			break;              
		}
		if (wglMakeCurrent(m_hDC, m_hRC) == false)      // ���õ�ǰ��OpenGL����Ⱦ����Ϊ��ǰ�Ĵ���
		{                // ʧ��
			break;              
		}

		ShowWindow(m_hWnd, SW_NORMAL);         // ��ʾ����
		ReshapeGL();             // ����OpenGL�������ڴ�С
		return true;             // �ɹ�����
	}                 
	Destroy();               // �ͷ���Դ
	return false;              // ����ʧ��
}
// ɾ��OpenGL����
void GLWindow::Destroy()         
{
	if (m_hWnd != 0)          // ���ھ���Ƿ����
	{
		if (m_hDC != 0)          // �����豸�������Ƿ����
		{
			wglMakeCurrent(m_hDC, 0);      // ���õ�ǰ��OpenGL����Ⱦ����Ϊ��NULL
			if (m_hRC != 0)         // OpenGL����Ⱦ�������Ƿ����
			{
				wglDeleteContext(m_hRC);     // �ͷ�OpenGL����Ⱦ������
				m_hRC = 0;         // ����OpenGL����Ⱦ������Ϊ0
			}
			ReleaseDC(m_hWnd, m_hDC);      // �ͷŴ��ڵ��豸������
			m_hDC = 0;          // ���ô��ڵ��豸������Ϊ0
		}
		DestroyWindow(m_hWnd);        // ɾ������
		m_hWnd = 0;           // ���ô��ھ��Ϊ0
	}
	if (m_IsFullScreen)          // ���Ϊȫ��ģʽ���ڳ�������󣬱任������ģʽ������ʾ���
	{
		ChangeDisplaySettings(NULL, 0);      // �任������ģʽ
		ShowCursor(true);         // ��ʾ���
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
