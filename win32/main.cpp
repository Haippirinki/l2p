#define WIN32_LEAN_AND_MEAN 1
#include <Windows.h>
#include "glcorearb.h"
#include "wglext.h"
#include "GL_3_3.h"

#include <common/Application.h>

static int64_t s_frequency = 0;
static Application* s_application = nullptr;

static int64_t getMicroseconds()
{
	int64_t counter;
	QueryPerformanceCounter((LARGE_INTEGER*)&counter);
	return counter * 1000 / (s_frequency / 1000);
}

static LRESULT CALLBACK windowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	LRESULT lResult = 0;
	RECT rect;

	switch(uMsg)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	case WM_LBUTTONDOWN:
		SetCapture(hWnd);
		GetClientRect(hWnd, &rect);
		s_application->mouseDown((float)(LOWORD(lParam) - rect.left), (float)(HIWORD(lParam) - rect.top));
		break;

	case WM_LBUTTONUP:
		ReleaseCapture();
		GetClientRect(hWnd, &rect);
		s_application->mouseUp((float)(LOWORD(lParam) - rect.left), (float)(HIWORD(lParam) - rect.top));
		break;

	case WM_MOUSEMOVE:
		GetClientRect(hWnd, &rect);
		s_application->mouseMove((float)(LOWORD(lParam) - rect.left), (float)(HIWORD(lParam) - rect.top));
		break;

	default:
		lResult = DefWindowProc(hWnd, uMsg, wParam, lParam);
		break;
	}

	return lResult;
}

int main(int argc, char** argv)
{
	const char* name = "l2p";

	WNDCLASSEX wc { 0 };
	wc.cbSize = sizeof(wc);
	wc.lpfnWndProc = windowProc;
	wc.hInstance = GetModuleHandle(nullptr);
	wc.hCursor = LoadCursor(0, IDC_ARROW);
	wc.lpszClassName = name;

	if(RegisterClassEx(&wc) == 0)
	{
		return 1;
	}

	DWORD style = WS_OVERLAPPEDWINDOW | WS_VISIBLE;

	RECT rect;
	rect.left = 0;
	rect.right = 1280;
	rect.top = 0;
	rect.bottom = 720;
	AdjustWindowRect(&rect, style, FALSE);

	HWND hWnd = CreateWindowEx(0, name, 0, style, CW_USEDEFAULT, CW_USEDEFAULT, rect.right - rect.left, rect.bottom - rect.top, 0, 0, GetModuleHandle(nullptr), 0);

	if(!hWnd)
	{
		return 1;
	}

	SetWindowText(hWnd, name);

	HDC hDC = GetDC(hWnd);

	PIXELFORMATDESCRIPTOR pfd { 0 };
	pfd.nSize = sizeof(pfd);
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pfd.cColorBits = 24;

	GLuint pixelFormat = ChoosePixelFormat(hDC, &pfd);
	SetPixelFormat(hDC, pixelFormat, &pfd);

	HGLRC hInitialRC = wglCreateContext(hDC);
	wglMakeCurrent(hDC, hInitialRC);

	PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB = (PFNWGLCREATECONTEXTATTRIBSARBPROC)wglGetProcAddress("wglCreateContextAttribsARB");

	const int contextAttributes[] =
	{
		WGL_CONTEXT_MAJOR_VERSION_ARB, 3,
		WGL_CONTEXT_MINOR_VERSION_ARB, 3,
		WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
		0
	};

	HGLRC hRC = wglCreateContextAttribsARB(hDC, 0, contextAttributes);
	wglMakeCurrent(hDC, hRC);

	wglDeleteContext(hInitialRC);

	setup_GL_3_3();

	PFNWGLSWAPINTERVALEXTPROC wglSwapIntervalEXT = (PFNWGLSWAPINTERVALEXTPROC)wglGetProcAddress("wglSwapIntervalEXT");
	wglSwapIntervalEXT(1);

	glEnable(GL_FRAMEBUFFER_SRGB);

	s_application = createApplication();

	s_application->init();

	QueryPerformanceFrequency((LARGE_INTEGER*)&s_frequency);

	int64_t startTime = getMicroseconds();
	int64_t lastUpdateTime = startTime;

	bool shouldExit = false;
	while(!shouldExit)
	{
		GetClientRect(hWnd, &rect);
		int width = rect.right - rect.left;
		int height = rect.bottom - rect.top;

		int64_t time = getMicroseconds();

		s_application->update((float)width, (float)height, (time - startTime) * (1.0 / 1000000.0), (time - lastUpdateTime) * (1.0 / 1000000.0));

		lastUpdateTime = time;

		s_application->render(width, height);

		SwapBuffers(hDC);

		MSG msg;
		while(PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
		{
			if(msg.message == WM_QUIT)
			{
				shouldExit = true;
				break;
			}

			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	delete s_application;

	wglMakeCurrent(0, 0);
	wglDeleteContext(hRC);

	DestroyWindow(hWnd);

	return 0;
}
