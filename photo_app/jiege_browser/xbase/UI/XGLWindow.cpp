#include "StdAfx.h"
#include "XGLWindow.h"

#include <gl/GL.h>
#include <gl/GLU.h>

CXGLWindow::CXGLWindow(void)
{
}

CXGLWindow::~CXGLWindow(void)
{
}

HRESULT CXGLWindow::Draw( HDC hdc )
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear Screen And Depth Buffer
	glLoadIdentity();
	// EXAMPLE OPENGL CODE START
	////////////////////////////////////////////////////////
	//////////////////////////////////////////////////////////////////////////////
	static GLfloat xrot; // X Rotation
	static GLfloat yrot; // Y Rotation
	static GLfloat zrot; // Z Rotation
	glPushMatrix(); // Push Matrix Onto Stack (Copy The Current Matrix)
	glLoadIdentity(); // Reset The Current Modelview Matrix
	glTranslatef(0.0f,0.0f,-6.0f); // Move Left 1.5 Units And Into The Screen 6.0
	glRotatef(xrot,1.0f,0.0f,0.0f);
	glRotatef(yrot,0.0f,1.0f,0.0f);
	glRotatef(zrot,0.0f,0.0f,1.0f);
	glBegin(GL_QUADS);
	// Front Face
	glColor3f(1.0f,0.0f,0.0f);glVertex3f(-1.0f, -1.0f, 1.0f);
	glColor3f(0.0f,1.0f,0.0f);glVertex3f( 1.0f, -1.0f, 1.0f);
	glColor3f(0.0f,1.0f,0.0f);glVertex3f( 1.0f, 1.0f, 1.0f);
	glColor3f(0.0f,0.0f,1.0f);glVertex3f(-1.0f, 1.0f, 1.0f);
	// Back Face
	glColor3f(1.0f,0.0f,0.0f);glVertex3f(-1.0f, -1.0f, -1.0f);
	glColor3f(0.0f,1.0f,0.0f);glVertex3f(-1.0f, 1.0f, -1.0f);
	glColor3f(0.0f,1.0f,0.0f);glVertex3f( 1.0f, 1.0f, -1.0f);
	glColor3f(0.0f,0.0f,1.0f);glVertex3f( 1.0f, -1.0f, -1.0f);
	// Top Face
	glColor3f(0.0f,1.0f,0.0f);glVertex3f(-1.0f, 1.0f, -1.0f);
	glColor3f(0.0f,0.0f,1.0f);glVertex3f(-1.0f, 1.0f, 1.0f);
	glColor3f(0.0f,1.0f,0.0f);glVertex3f( 1.0f, 1.0f, 1.0f);
	glColor3f(0.0f,1.0f,0.0f);glVertex3f( 1.0f, 1.0f, -1.0f);
	// Bottom Face
	glColor3f(1.0f,0.0f,0.0f);glVertex3f(-1.0f, -1.0f, -1.0f);
	glColor3f(0.0f,0.0f,1.0f);glVertex3f( 1.0f, -1.0f, -1.0f);
	glColor3f(0.0f,1.0f,0.0f);glVertex3f( 1.0f, -1.0f, 1.0f);
	glColor3f(1.0f,0.0f,0.0f);glVertex3f(-1.0f, -1.0f, 1.0f);
	// Right face
	glColor3f(0.0f,0.0f,1.0f);glVertex3f( 1.0f, -1.0f, -1.0f);
	glColor3f(0.0f,1.0f,0.0f);glVertex3f( 1.0f, 1.0f, -1.0f);
	glColor3f(0.0f,1.0f,0.0f);glVertex3f( 1.0f, 1.0f, 1.0f);
	glColor3f(0.0f,1.0f,0.0f);glVertex3f( 1.0f, -1.0f, 1.0f);
	// Left Face
	glColor3f(1.0f,0.0f,0.0f);glVertex3f(-1.0f, -1.0f, -1.0f);
	glColor3f(1.0f,0.0f,0.0f);glVertex3f(-1.0f, -1.0f, 1.0f);
	glColor3f(0.0f,0.0f,1.0f);glVertex3f(-1.0f, 1.0f, 1.0f);
	glColor3f(0.0f,1.0f,0.0f);glVertex3f(-1.0f, 1.0f, -1.0f);
	glEnd();
	glPopMatrix(); // Pop Matrix Off The Stack
	xrot+=1.3f;
	yrot+=1.2f;
	zrot+=1.4f; // Decrease The Rotation Variable For The Quad
	//////////////////////////////////////////////////////////////////////////////
	// EXAMPLE OPENGL CODE END
	//////////////////////////////////////////////////////////
	// Swap our scene to the front
	SwapBuffers(hdc);

	return S_OK;
}


int CXGLWindow::InitGL()
{
	glShadeModel(GL_SMOOTH); // Enable Smooth Shading
	glClearColor(0.0f, 0.0f, 0.0f, 0.5f); // Black Background
	glClearDepth(1.0f); // Depth Buffer Setup
	glEnable(GL_DEPTH_TEST); // Enables Depth Testing
	glDepthFunc(GL_LEQUAL); // The Type Of Depth Testing To Do
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST); // Really Nice Perspective Calculations
	glEnable(GL_TEXTURE_2D); // Enable Texture Mapping
	
	PIXELFORMATDESCRIPTOR pfd = 
	{
		sizeof(PIXELFORMATDESCRIPTOR),
		1,
		PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL|PFD_DOUBLEBUFFER,
		PFD_TYPE_RGBA,
		24,
		0,0,0,0,0,0,
		0,0,0,
		0,0,0,0,
		32,
		0,
		0,
		PFD_MAIN_PLANE,
		0,
		0,0,0
	};
	HDC hdc = GetDC();
	int nPixelFormat = ::ChoosePixelFormat(hdc,&pfd);
	::SetPixelFormat(hdc,nPixelFormat,&pfd);
	HGLRC hrc = ::wglCreateContext(hdc);
	::wglMakeCurrent(hdc,hrc);
	
	return TRUE; // Initialization Went OK
}


LRESULT CXGLWindow::OnCreate(UINT, WPARAM, LPARAM, BOOL&)
{
	InitGL();

	return S_OK;
}
LRESULT CXGLWindow::OnDestroy(UINT, WPARAM, LPARAM, BOOL&)
{
	return S_OK;
}
LRESULT CXGLWindow::OnPaint( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	HDC hdc = GetDC();
	Draw(hdc);
	return S_OK;
}
LRESULT CXGLWindow::OnSize( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	int cx = LOWORD(lParam);
	int cy = HIWORD(lParam);
	glViewport(0,0,cx,cy); // Reset The Current Viewport
	glMatrixMode(GL_PROJECTION); // Select The Projection Matrix
	glLoadIdentity(); // Reset The Projection Matrix
	gluPerspective(45.0f,(GLfloat)cx/(GLfloat)cy,0.1f,2000.0f); // Calculate The Aspect Ratio Of The Window
	glMatrixMode(GL_MODELVIEW); // Select The Modelview Matrix
	glLoadIdentity(); // Reset The Modelview Matrix
	//后面有中文注释的可以不加
	glShadeModel(GL_SMOOTH);    // 启用阴影平滑
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f); //设置背景
	glClearDepth(1.0f);      // 设置深度缓存
	glEnable(GL_DEPTH_TEST);    // 启用深度测试
	glDepthFunc(GL_LEQUAL);     //所作深度测试的类型
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST); // 真正精细的透视修正

	return S_OK;
}
LRESULT CXGLWindow::OnEraseBkgrnd( UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	return S_OK;
}

// BMPLoader.cpp

