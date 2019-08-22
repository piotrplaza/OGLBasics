#include <cassert>
#include <string>
#include <vector>
#include <stdexcept>
#include <iostream>

#include <windows.h>

#include <GL/glew.h>
#include <gl/gl.h>

#include <glm/vec3.hpp>

#include "shaders.hpp"
#include "tools.hpp"

const bool fullScreen = false;
const int width = 600;
const int height = 600;

shaders::ProgramId program;

void Initialize()
{
	const GLenum glewInitResult = glewInit();
	assert(GLEW_OK == glewInitResult);

	glEnable(GL_DEPTH_TEST);
	glClearColor(0, 0, 0, 1);

	program = shaders::LinkProgram(shaders::CompileShaders("shaders/basic.vs", "shaders/basic.fs"),
		{ {0, "bPos"}, {1, "bCol"} });
}

glm::vec3 v2 = { 0, 1, 0 };

void RenderScene()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	const static std::vector<glm::vec3> c = {
		{1, 0, 0},
		{0, 1, 0},
		{0, 0, 1}
	};

	std::vector<glm::vec3> v = {
		{-1, -1, 0},
		{1, -1, 0},
		v2
	};

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, v.data());
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, c.data());

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	glUseProgram(program);
	glDrawArrays(GL_TRIANGLES, 0, v.size());
}

void PrepareFrame()
{
	RenderScene();
}

void ChangeSize(int w, int h)
{
	glViewport(0, 0, w, h);
}

void HandleKeyboard(bool const * const keys)
{
	const float delta = 0.01f;

	if (keys[VK_LEFT])
	{
		v2.x -= delta;
	}
	if (keys[VK_RIGHT])
	{
		v2.x += delta;
	}
	if (keys[VK_DOWN])
	{
		v2.y -= delta;
	}
	if (keys[VK_UP])
	{
		v2.y += delta;
	}
}

void SetDCPixelFormat(HDC hDC)
{
	const PIXELFORMATDESCRIPTOR pfd =
	{
		sizeof(PIXELFORMATDESCRIPTOR),
		1,
		PFD_DRAW_TO_WINDOW |
		PFD_SUPPORT_OPENGL |
		PFD_DOUBLEBUFFER,
		PFD_TYPE_RGBA,
		24,
		0, 0, 0, 0, 0, 0,
		0, 0,
		0, 0, 0, 0,
		32,
		0,
		0,
		PFD_MAIN_PLANE,
		0,
		0, 0, 0
	};
	const int pixelFormt = ChoosePixelFormat(hDC, &pfd);
	SetPixelFormat(hDC, pixelFormt, &pfd);
}

static bool keys[256];
static bool quit;
static HDC hDC;

LRESULT CALLBACK WndProc(HWND hWnd,
						 UINT message,
						 WPARAM wParam,
						 LPARAM lParam)
{
	static HGLRC hRC;

	switch(message)
	{
		case WM_CREATE:
		{
			hDC = GetDC(hWnd);
			SetDCPixelFormat(hDC);
			hRC = wglCreateContext(hDC);
			wglMakeCurrent(hDC, hRC);
			try
			{
				Initialize();
			}
			catch (const std::runtime_error & error)
			{
				MessageBox(nullptr, error.what(), "Runtime error",
					MB_OK | MB_ICONEXCLAMATION);
				ExitProcess(0);
			}
		}
			break;
		case WM_DESTROY:
			wglMakeCurrent(hDC, nullptr);
			wglDeleteContext(hRC);
			PostQuitMessage(0);
			quit = true;
			break;
		case WM_SIZE:
			ChangeSize(LOWORD(lParam), HIWORD(lParam));
			break;
		case WM_KEYDOWN:
			keys[wParam] = true;
			break;
		case WM_KEYUP:
			keys[wParam] = false;
			break;
		case WM_RBUTTONDOWN:
			break;
		case WM_RBUTTONUP:
			break;
		case WM_LBUTTONDOWN:
			break;
		case WM_LBUTTONUP:
			break;
		case WM_MBUTTONDOWN:
			break;
		case WM_MBUTTONUP:
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0l;
}

int APIENTRY WinMain(HINSTANCE hInstance,
					 HINSTANCE hPrevInstance,
					 LPSTR lpCmdLine,
					 int nCmdShow)
{
	const LPCTSTR lpszAppName = "OpenGL window";
	const int winPosX = 10, winPosY = 10;

	WNDCLASS wc;

	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = (WNDPROC)WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hIcon = nullptr;
	wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wc.hbrBackground = nullptr;
	wc.lpszMenuName = nullptr;
	wc.lpszClassName = lpszAppName;
	
	if(!RegisterClass(&wc))
	{
		MessageBox(nullptr, "Window class registration failed.", "Error",
			MB_OK | MB_ICONEXCLAMATION);
		return false;
	}

	HWND hWnd = CreateWindow(
		lpszAppName,
		lpszAppName,
		fullScreen ? WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_POPUP | WS_MAXIMIZE : 
		WS_OVERLAPPEDWINDOW,
		winPosX, winPosY,
		width, height,
		nullptr,
		nullptr,
		hInstance,
		nullptr);

	if(!hWnd)
	{
		MessageBox(nullptr, "Window creation failed.", "Error",
			MB_OK | MB_ICONEXCLAMATION);
		return false;
	}

	tools::RedirectIOToConsole(650, 10);
	ShowWindow(hWnd, SW_SHOW);
	
	MSG msg{};

	while (!keys[VK_ESCAPE] && !quit)
	{
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			HandleKeyboard(keys);
			PrepareFrame();
			SwapBuffers(hDC);
		}
	}
	
	return (int)msg.wParam;
}
