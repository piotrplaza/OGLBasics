#include <cassert>
#include <string>
#include <vector>
#include <stdexcept>
#include <iostream>

#include <windows.h>

#include <GL/glew.h>
#include <gl/gl.h>

#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shaders.hpp"
#include "tools.hpp"

const bool fullScreen = false;
const int width = 600;
const int height = 600;

shaders::ProgramId program;

struct
{
	glm::mat4 mv { 1.0f };
	glm::mat4 p { 1.0f };

	glm::mat4 getMVP() const
	{
		return p * mv;
	}
} mvp;

const std::vector<glm::vec3> vertices = {
	{-1, -1, 0},
	{1, -1, 0},
	{-1, 1, 0},
	{1, 1, 0}
};

const std::vector<unsigned> indices = {
	0, 1, 2,
	1, 2, 3
};

void Initialize()
{
	const GLenum glewInitResult = glewInit();
	assert(GLEW_OK == glewInitResult);

	glEnable(GL_DEPTH_TEST);
	glClearColor(0, 0, 0, 1);

	GLuint vertexBuffer;
	glGenBuffers(1, &vertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(vertices.front()), vertices.data(), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
	glEnableVertexAttribArray(0);

	GLuint elementBuffer;
	glGenBuffers(1, &elementBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(indices.front()), indices.data(), GL_STATIC_DRAW);

	program = shaders::LinkProgram(shaders::CompileShaders("shaders/basic.vs", "shaders/basic.fs"),
		{ {0, "bPos"} });
}

int fractalType = 0;
int coloringType = 0;
int iterations = 1;
float zoom = 0.5f;
glm::vec2 translation = {0.0f, 0.0f};
glm::vec2 juliaC = { -0.1, 0.65 };

void RenderScene()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(program);
	glUniformMatrix4fv(glGetUniformLocation(program, "mvp"), 1, GL_FALSE,
		glm::value_ptr(mvp.getMVP()));
	glUniform1i(glGetUniformLocation(program, "fractalType"), fractalType);
	glUniform1i(glGetUniformLocation(program, "coloringType"), coloringType);
	glUniform1i(glGetUniformLocation(program, "iterations"), iterations);
	glUniform1f(glGetUniformLocation(program, "zoom"), zoom);
	glUniform2f(glGetUniformLocation(program, "translation"), translation.x, translation.y);
	glUniform2f(glGetUniformLocation(program, "juliaC"), juliaC.x, juliaC.y);

	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, nullptr);
}

float angleX = 0.0f;
float angleY = 0.0f;

void PrepareFrame()
{
	mvp.mv = glm::translate(glm::mat4( 1.0f ), { 0.0f, 0.0f, -2.0f });
	mvp.mv = glm::rotate(mvp.mv, angleX, { 1.0f, 0.0f, 0.0f });
	mvp.mv = glm::rotate(mvp.mv, angleY, { 0.0f, 1.0f, 0.0f });

	RenderScene();
}

void ChangeSize(int w, int h)
{
	glViewport(0, 0, w, h);

	mvp.p = glm::perspective(70.0f, (float)w / h, 1.0f, 1000.0f);
}

void HandleKeyboard(bool const * const keys)
{
	const float angleDelta = 0.01f;
	const float zoomFactor = 1.01f;
	const float translationFactor = 0.01f;
	const float juliaCDelta = 0.00001f;

	if (keys[VK_LEFT])
	{
		angleY -= angleDelta;
	}
	if (keys[VK_RIGHT])
	{
		angleY += angleDelta;
	}
	if (keys[VK_UP])
	{
		angleX -= angleDelta;
	}
	if (keys[VK_DOWN])
	{
		angleX += angleDelta;
	}
	if (keys[(int)'1'])
	{
		fractalType = 0;
		std::cout << "fractalType: " << fractalType << std::endl;
	}
	if (keys[(int)'2'])
	{
		fractalType = 1;
		std::cout << "fractalType: " << fractalType << std::endl;
	}
	if (keys[(int)'0'])
	{
		coloringType = 0;
		std::cout << "coloringType: " << coloringType << std::endl;
	}
	if (keys[(int)'9'])
	{
		coloringType = 1;
		std::cout << "coloringType: " << coloringType << std::endl;
	}
	if (keys[(int)'F'])
	{
		juliaC.x -= juliaCDelta;
		std::cout << "juliaC.x: " << juliaC.x << std::endl;
	}
	if (keys[(int)'H'])
	{
		juliaC.x += juliaCDelta;
		std::cout << "juliaC.x: " << juliaC.x << std::endl;
	}
	if (keys[(int)'G'])
	{
		juliaC.y -= juliaCDelta;
		std::cout << "juliaC.y: " << juliaC.y << std::endl;
	}
	if (keys[(int)'T'])
	{
		juliaC.y += juliaCDelta;
		std::cout << "juliaC.y: " << juliaC.y << std::endl;
	}
	if (keys[(int)'W'])
	{
		++iterations;
		std::cout << "iterations: " << iterations << std::endl;
	}
	if (keys[(int)'Q'] && iterations > 1)
	{
		--iterations;
		std::cout << "iterations: " << iterations << std::endl;
	}
	if (keys[(int)'A'])
	{
		zoom *= zoomFactor;
		std::cout << "zoom: " << zoom << std::endl;
	}
	if (keys[(int)'Z'])
	{
		zoom /= zoomFactor;
		std::cout << "zoom: " << zoom << std::endl;
	}
	if (keys[(int)'J'])
	{
		translation.x -= translationFactor / zoom;
		std::cout << "translation: " << translation.x << ";" << translation.y << std::endl;
	}
	if (keys[(int)'L'])
	{
		translation.x += translationFactor / zoom;
		std::cout << "translation: " << translation.x << ";" << translation.y << std::endl;
	}
	if (keys[(int)'K'])
	{
		translation.y -= translationFactor / zoom;
		std::cout << "translation: " << translation.x << ";" << translation.y << std::endl;
	}
	if (keys[(int)'I'])
	{
		translation.y += translationFactor / zoom;
		std::cout << "translation: " << translation.x << ";" << translation.y << std::endl;
	}
	if (keys[VK_SPACE])
	{
		zoom = 0.5f;
		translation = { 0.0f, 0.0f };
		std::cout << "zoom: " << zoom << std::endl;
		std::cout << "translation: " << translation.x << ";" << translation.y << std::endl;
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
