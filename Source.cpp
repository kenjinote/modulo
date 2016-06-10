#pragma comment(linker,"\"/manifestdependency:type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

#ifdef _WIN64
#ifdef _DEBUG
#pragma comment(lib, "x64\\Debug\\WinNTL-9_8_1.lib")
#else
#pragma comment(lib, "x64\\Release\\WinNTL-9_8_1.lib")
#endif
#else
#ifdef _DEBUG
#pragma comment(lib, "Debug\\WinNTL-9_8_1.lib")
#else
#pragma comment(lib, "Release\\WinNTL-9_8_1.lib")
#endif
#endif

#include "WinNTL-9_8_1\include\NTL\ZZ.h"
#include <windows.h>
#include <sstream>

using namespace NTL;
using namespace std;

TCHAR szClassName[] = TEXT("Window");

string ZZToString(const ZZ &z)
{
	stringstream buffer;
	buffer << z;
	return buffer.str();
}

void hsv2rgb(double h, double s, double v, double*r, double*g, double*b)
{
	while (h >= 360)h -= 360;
	const int i = (int)(h / 60.0);
	const double f = (h / 60.0) - i;
	const double p1 = v * (1 - s);
	const double p2 = v * (1 - s*f);
	const double p3 = v * (1 - s*(1 - f));
	switch (i)
	{
	case 0: *r = v; *g = p3; *b = p1; break;
	case 1: *r = p2; *g = v; *b = p1; break;
	case 2: *r = p1; *g = v; *b = p3; break;
	case 3: *r = p1; *g = p2; *b = v; break;
	case 4: *r = p3; *g = p1; *b = v; break;
	case 5: *r = v; *g = p1; *b = p2; break;
	}
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	static HWND hEdit1;
	static HWND hButton1;
	static HWND hButton2;
	static HWND hButton3;
	static HWND hButton4;
	static HWND hButton5;
	static ZZ n;
	static enum
	{
		IDADD,
		IDSUB,
		IDMUL,
		IDDIV,
		IDPOW,
	} Mode;

	switch (msg)
	{
	case WM_CREATE:
		hEdit1 = CreateWindowEx(WS_EX_CLIENTEDGE, TEXT("EDIT"), TEXT("15"),
			WS_CHILD | WS_VISIBLE | WS_TABSTOP | ES_NUMBER | ES_AUTOHSCROLL,
			0, 0, 0, 0, hWnd, 0, ((LPCREATESTRUCT)lParam)->hInstance, 0);
		hButton1 = CreateWindow(TEXT("BUTTON"), TEXT("加法"), WS_VISIBLE | WS_CHILD, 0, 0, 0, 0, hWnd, (HMENU)100, ((LPCREATESTRUCT)lParam)->hInstance, 0);
		hButton2 = CreateWindow(TEXT("BUTTON"), TEXT("減法"), WS_VISIBLE | WS_CHILD, 0, 0, 0, 0, hWnd, (HMENU)101, ((LPCREATESTRUCT)lParam)->hInstance, 0);
		hButton3 = CreateWindow(TEXT("BUTTON"), TEXT("乗法"), WS_VISIBLE | WS_CHILD, 0, 0, 0, 0, hWnd, (HMENU)102, ((LPCREATESTRUCT)lParam)->hInstance, 0);
		hButton4 = CreateWindow(TEXT("BUTTON"), TEXT("除法"), WS_VISIBLE | WS_CHILD, 0, 0, 0, 0, hWnd, (HMENU)103, ((LPCREATESTRUCT)lParam)->hInstance, 0);
		hButton5 = CreateWindow(TEXT("BUTTON"), TEXT("累乗"), WS_VISIBLE | WS_CHILD, 0, 0, 0, 0, hWnd, (HMENU)104, ((LPCREATESTRUCT)lParam)->hInstance, 0);
		break;
	case WM_SIZE:
		MoveWindow(hEdit1, 10, 10, 128, 32, TRUE);
		MoveWindow(hButton1, 128 + 20, 10, 64, 32, TRUE);
		MoveWindow(hButton2, 128 + 20 + 74 * 1, 10, 64, 32, TRUE);
		MoveWindow(hButton3, 128 + 20 + 74 * 2, 10, 64, 32, TRUE);
		MoveWindow(hButton4, 128 + 20 + 74 * 3, 10, 64, 32, TRUE);
		MoveWindow(hButton5, 128 + 20 + 74 * 4, 10, 64, 32, TRUE);
		break;
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
		if (n != 0)
		{
			SIZE size;
			GetTextExtentPoint32(hdc, TEXT("8888888888"), (int)GetWindowTextLength(hEdit1) + 1, &size);
			for (ZZ x = to_ZZ(0); x <= n; ++x)
			{
				for (ZZ y = to_ZZ(0); y <= n; ++y)
				{
					string str;
					ZZ mod;
					switch (Mode)
					{
					case IDADD:
						mod = (x + y) % n;
						break;
					case IDSUB:
						mod = (x - y) % n;
						break;
					case IDMUL:
						mod = MulMod(x, y, n);
						break;
					case IDDIV:
						if (y != 0)
						{
							mod = (x / y) % n;
						}
						else
						{
							mod = to_ZZ(0);
						}
						break;
					case IDPOW:
						{
							mod = to_ZZ(1);
							for (ZZ i = to_ZZ(0); i < y; ++i)
							{
								mod *= x;
							}
							mod %= n;
						}
						break;
					}

					double r, g, b;
					double c;

					long l;
					conv(l, n);
					long lmod;
					conv(lmod, mod);

					c = 360.0 * lmod / l;

					hsv2rgb(c, 0.5, 1.0, &r, &g, &b);

					COLORREF color = RGB(255*r,255*g,255*b);

					str = ZZToString(mod);

					int xx, yy;
					conv(xx, x);
					conv(yy, y);

					SetBkColor(hdc, color);
					TextOutA(hdc, xx * size.cx + 10, yy * 22 + 50, str.c_str(), (int)str.length());
				}
			}
		}
		EndPaint(hWnd, &ps);
	}
	break;
	case WM_COMMAND:
		if (LOWORD(wParam) >= 100 && LOWORD(wParam) <= 104)
		{
			const int len = GetWindowTextLengthA(hEdit1) + 1;
			LPSTR str = (LPSTR)GlobalAlloc(GMEM_FIXED, len);
			GetWindowTextA(hEdit1, str, len);
			conv(n, str);
			GlobalFree(str);

			switch (LOWORD(wParam))
			{
			case 100:
				Mode = IDADD;
				break;
			case 101:
				Mode = IDSUB;
				break;
			case 102:
				Mode = IDMUL;
				break;
			case 103:
				Mode = IDDIV;
				break;
			case 104:
				Mode = IDPOW;
				break;
			default:
				break;
			}
			InvalidateRect(hWnd, 0, 1);
		}
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, msg, wParam, lParam);
	}
	return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPreInst, LPSTR pCmdLine, int nCmdShow)
{
	MSG msg;
	WNDCLASS wndclass = {
		CS_HREDRAW | CS_VREDRAW,
		WndProc,
		0,
		0,
		hInstance,
		0,
		LoadCursor(0,IDC_ARROW),
		(HBRUSH)(COLOR_WINDOW + 1),
		0,
		szClassName
	};
	RegisterClass(&wndclass);
	HWND hWnd = CreateWindow(
		szClassName,
		TEXT("NTLを使ったサンプルプログラム"),
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		0,
		CW_USEDEFAULT,
		0,
		0,
		0,
		hInstance,
		0
	);
	ShowWindow(hWnd, SW_SHOWDEFAULT);
	UpdateWindow(hWnd);
	while (GetMessage(&msg, 0, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return (int)msg.wParam;
}
