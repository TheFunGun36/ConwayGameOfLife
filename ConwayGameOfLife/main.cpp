#include <windows.h>
#include <d2d1.h>
#include <ctime>
#include <cmath>

#include "Constants.h"
#include "GameLogic.h"

#pragma comment(lib, "d2d1.lib")

#define SafeRelease(p) if(p != NULL) p->Release(); p = NULL
#define aleft -1
#define aright +1
#define aup -g_units
#define adown +g_units

#define TIMER_ID 1234

namespace r
{
	int wndSizeX, wndSizeY;

	ID2D1HwndRenderTarget* pRenderTarget = NULL;
	ID2D1SolidColorBrush* pWhiteBrush = NULL;
	ID2D1SolidColorBrush* pGrayBrush = NULL;
	ID2D1SolidColorBrush* pRedBrush = NULL;
	ID2D1RadialGradientBrush* pSpeedBrush = NULL;
	ID2D1RadialGradientBrush* pIceBrush = NULL;
	ID2D1RadialGradientBrush* pFlameBrush = NULL;
	ID2D1Factory* pFactory = NULL;



	void CreateResources(HWND hWnd)
	{
		RECT rc;
		GetClientRect(hWnd, &rc);

		D2D1_SIZE_U size = D2D1::SizeU(rc.right - rc.left, rc.bottom - rc.top);
		wndSizeX = rc.right - rc.left;
		wndSizeY = rc.bottom - rc.top;

		if (FAILED(pFactory->CreateHwndRenderTarget(D2D1::RenderTargetProperties(),
			D2D1::HwndRenderTargetProperties(hWnd, size),
			&pRenderTarget)))
			DestroyWindow(hWnd);

		if (pWhiteBrush == NULL)
			if (FAILED(pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(0.8f, 0.8f, 0.8f), &pWhiteBrush)))
				DestroyWindow(hWnd);

		if (pGrayBrush == NULL)
			if (FAILED(pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(0.5f, 0.5f, 0.5f), &pGrayBrush)))
				DestroyWindow(hWnd);

		if (pRedBrush == NULL)
			if (FAILED(pRenderTarget->CreateSolidColorBrush(D2D1::ColorF(0.7f, 0.2f, 0.2f), &pRedBrush)))
				DestroyWindow(hWnd);

		ID2D1GradientStopCollection* stopCollection = NULL;

		D2D1_GRADIENT_STOP gradientStop[2];
		gradientStop[0].color = D2D1::ColorF(0.6f, 0.7f, 0.8f, 0.3f);
		gradientStop[0].position = 0.0f;
		gradientStop[1].color = D2D1::ColorF(0.2f, 0.7f, 0.8f, 0.2f);
		gradientStop[1].position = 1.0f;


		if (FAILED(pRenderTarget->CreateGradientStopCollection(
			gradientStop,
			2,
			D2D1_GAMMA_2_2,
			D2D1_EXTEND_MODE_CLAMP,
			&stopCollection)))
			DestroyWindow(hWnd);

		if (pIceBrush == NULL)
			if (FAILED(pRenderTarget->CreateRadialGradientBrush(
				D2D1::RadialGradientBrushProperties(
					D2D1::Point2F(wndSizeX / 2, wndSizeY / 2),
					D2D1::Point2F(),
					wndSizeX / 2,
					wndSizeY / 2),
				stopCollection,
				&pIceBrush)))
				DestroyWindow(hWnd);

		SafeRelease(stopCollection);
		gradientStop[0].color = D2D1::ColorF(0.9f, 0.8f, 0.8f, 0.4f);
		gradientStop[0].position = 0.0f;
		gradientStop[1].color = D2D1::ColorF(0.9f, 0.2f, 0.2f, 0.2f);
		gradientStop[1].position = 1.0f;

		if (FAILED(pRenderTarget->CreateGradientStopCollection(
			gradientStop,
			2,
			D2D1_GAMMA_2_2,
			D2D1_EXTEND_MODE_CLAMP,
			&stopCollection)))
			DestroyWindow(hWnd);

		if (pFlameBrush == NULL)
			if (FAILED(pRenderTarget->CreateRadialGradientBrush(
				D2D1::RadialGradientBrushProperties(
					D2D1::Point2F(wndSizeX / 2, wndSizeY / 2),
					D2D1::Point2F(),
					wndSizeX / 2,
					wndSizeY / 2),
				stopCollection,
				&pFlameBrush)))
				DestroyWindow(hWnd);

		SafeRelease(stopCollection);
		gradientStop[0].color = D2D1::ColorF(0.4f, 0.8f, 0.4f, 0.3f);
		gradientStop[0].position = 0.0f;
		gradientStop[1].color = D2D1::ColorF(0.1f, 1.0f, 0.1f, 0.2f);
		gradientStop[1].position = 1.0f;

		if (FAILED(pRenderTarget->CreateGradientStopCollection(
			gradientStop,
			2,
			D2D1_GAMMA_2_2,
			D2D1_EXTEND_MODE_CLAMP,
			&stopCollection)))
			DestroyWindow(hWnd);

		if (pSpeedBrush == NULL)
			if (FAILED(pRenderTarget->CreateRadialGradientBrush(
				D2D1::RadialGradientBrushProperties(
					D2D1::Point2F(wndSizeX / 2, wndSizeY / 2),
					D2D1::Point2F(),
					wndSizeX / 2,
					wndSizeY / 2),
				stopCollection,
				&pSpeedBrush)))
				DestroyWindow(hWnd);

		SafeRelease(stopCollection);

		//CalculateLayout();
	}

	void DestroyResources()
	{
		SafeRelease(r::pRenderTarget);
		SafeRelease(r::pWhiteBrush);
		SafeRelease(r::pGrayBrush);
		SafeRelease(r::pRedBrush);
		SafeRelease(r::pIceBrush);
		SafeRelease(r::pFlameBrush);
		SafeRelease(r::pSpeedBrush);
	}

	void BeginDraw(HWND hWnd)
	{
		if (pRenderTarget == NULL)
		{
			CreateResources(hWnd);
		}

		pRenderTarget->BeginDraw();
	}
	void EndDraw(HWND hWnd)
	{
		HRESULT hr = pRenderTarget->EndDraw();
		if (FAILED(hr) || hr == D2DERR_RECREATE_TARGET)
		{

			DestroyWindow(hWnd);
		}
	}
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR lpCmdLine, int nCmdShow)
{
	WNDCLASSEX wcex;
	ZeroMemory(&wcex, sizeof(wcex));
	wcex.cbSize = sizeof(wcex);
	wcex.style = HS_VERTICAL | HS_HORIZONTAL;
	wcex.lpfnWndProc = WndProc;
	wcex.hInstance = hInst;
	wcex.hCursor = LoadCursor(hInst, IDC_ARROW);
	wcex.lpszClassName = L"CLA";
	wcex.hIcon = LoadIcon(hInst, IDI_APPLICATION);
	wcex.hIconSm = LoadIcon(hInst, IDI_APPLICATION);
	wcex.hbrBackground = (HBRUSH)GetStockObject(GRAY_BRUSH);

	RegisterClassEx(&wcex);

	HWND hWnd = CreateWindow(L"CLA", L"Conway Game Of Life", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, NULL, NULL, hInst, NULL);

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	MSG msg;

	SetTimer(hWnd, TIMER_ID, 16, NULL);

	BOOL ret = 1;
	while (ret > 0)
	{
		ret = GetMessage(&msg, NULL, 0, 0);
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	KillTimer(hWnd, TIMER_ID);

	return 0;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	static GameLogic gl;
	static PAINTSTRUCT ps;

	switch (msg)
	{
	case WM_KEYDOWN:
		switch (wParam)
		{
		case 'S':
			if (!gl.isPause)
				gl.isSpeedUp = !gl.isSpeedUp;
			break;

		case VK_SPACE: [[fallthrough]]
		case 'P':
			gl.isPause = !gl.isPause;
			gl.isSpeedUp = false;
			break;

		case VK_ESCAPE: [[fallthrough]]
		case 'F':
			gl.isFlame = true;
			gl.cleanField();
			break;
		}
		InvalidateRect(hWnd, 0, false);
		break;

	case WM_KEYUP:
		if (wParam == 'F' || wParam == VK_ESCAPE)
			gl.isFlame = false;
		InvalidateRect(hWnd, 0, false);
		break;

	case WM_MOUSEMOVE:
		gl.cursorPosX = LOWORD(lParam);
		gl.cursorPosY = HIWORD(lParam);
		InvalidateRect(hWnd, 0, false);
		break;

	case WM_SIZE:
		r::DestroyResources();
		InvalidateRect(hWnd, 0, false);
		break;

	case WM_LBUTTONDOWN:
	{
		if (!gl.isFlame)
		{
			float xStep = float(r::wndSizeX) / g_unitsX,
				yStep = float(r::wndSizeY) / g_unitsY;

			gl.invertElement(gl.cursorPosX / xStep, gl.cursorPosY / yStep);
		}

		InvalidateRect(hWnd, 0, false);
	}
	break;

	case WM_TIMER:
		if (gl.isSpeedUp)
		{
			if (clock() - gl.timeStamp > g_tickTime / 4)
			{
				gl.timeStamp = clock();
				gl.tick();
				InvalidateRect(hWnd, 0, false);
			}
		}
		else
		{
			if (clock() - gl.timeStamp > g_tickTime)
			{
				gl.timeStamp = clock();
				gl.tick();
				InvalidateRect(hWnd, 0, false);
			}
		}
		break;

	case WM_CREATE:
		if (FAILED(D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &r::pFactory)))
			DestroyWindow(hWnd);
		r::CreateResources(hWnd);

		gl.cleanField();

		gl.invertElement(2, 0);
		gl.invertElement(3, 1);
		gl.invertElement(1, 2);
		gl.invertElement(2, 2);
		gl.invertElement(3, 2);
		break;

	case WM_PAINT:
	{
		if (clock() - gl.timeStamp > g_tickTime / ((gl.isSpeedUp) ? 3 : 1))
		{
			gl.timeStamp = clock();
			gl.tick();
		}

		BeginPaint(hWnd, &ps);
		r::BeginDraw(hWnd);
		r::pRenderTarget->Clear(D2D1::ColorF(0.2f, 0.2f, 0.2f));

		float xStep = float(r::wndSizeX) / g_unitsX,
			yStep = float(r::wndSizeY) / g_unitsY;

		for (float i = xStep; i < r::wndSizeX; i += xStep)
			r::pRenderTarget->DrawLine(D2D1::Point2F(i, 0), D2D1::Point2F(i, r::wndSizeY), r::pGrayBrush);

		for (float i = yStep; i < r::wndSizeY; i += yStep)
			r::pRenderTarget->DrawLine(D2D1::Point2F(0, i), D2D1::Point2F(r::wndSizeX, i), r::pGrayBrush);

		for (int iy = 0; iy < g_unitsY; iy++)
		{
			for (int ix = 0; ix < g_unitsX; ix++)
			{
				if (gl.getElement(ix, iy))
				{
					D2D1_RECT_F rect;
					rect.left = ix * xStep;
					rect.right = rect.left + xStep;
					rect.top = iy * yStep;
					rect.bottom = rect.top + yStep;
					r::pRenderTarget->FillRectangle(rect, r::pWhiteBrush);
				}
			}
		}

		D2D1_RECT_F rect;
		rect.left = int(gl.cursorPosX / xStep) * xStep;
		rect.right = rect.left + xStep;
		rect.top = int(gl.cursorPosY / yStep) * yStep;
		rect.bottom = rect.top + yStep;
		r::pRenderTarget->DrawRectangle(rect, r::pRedBrush, 2.0f);

		if (gl.isPause)
			r::pRenderTarget->FillRectangle(D2D1::RectF(0.0f, 0.0f, r::wndSizeX, r::wndSizeY), r::pIceBrush);
		else if (gl.isSpeedUp)
			r::pRenderTarget->FillRectangle(D2D1::RectF(0.0f, 0.0f, r::wndSizeX, r::wndSizeY), r::pSpeedBrush);
		if (gl.isFlame)
			r::pRenderTarget->FillRectangle(D2D1::RectF(0.0f, 0.0f, r::wndSizeX, r::wndSizeY), r::pFlameBrush);

		r::EndDraw(hWnd);

		EndPaint(hWnd, &ps);
	}
	break;

	case WM_DESTROY:
		r::DestroyResources();
		SafeRelease(r::pFactory);
		PostQuitMessage(0);
		break;

	default:
		return DefWindowProc(hWnd, msg, wParam, lParam);
	}
	return 0;
}