#include <windows.h>
#include <d2d1.h>
#include <ctime>
#include <cmath>

#pragma comment(lib, "d2d1.lib")

#define SafeRelease(p) if(p != NULL) p->Release(); p = NULL
#define aleft -1
#define aright +1
#define aup -g_units
#define adown +g_units

#define TIMER_ID 1234

constexpr int g_unitsX = 128;
constexpr int g_unitsY = 64;
constexpr float g_tickTime = 100;

//should be other files, but i'm lazy
namespace logic {
    bool field[g_unitsX * g_unitsY];
    bool fieldOld[g_unitsX * g_unitsY];
    bool isPause = false;
    bool isFlame = false;
    bool isSpeedUp = false;

    int timeStamp = 0;;// = clock();
    int cursorPosX = 0,
        cursorPosY = 0;

    int getElementId(int x, int y) {
        if (x < 0) {
            x += g_unitsX;
        }
        else if (x >= g_unitsX) {
            x -= g_unitsX;
        }

        if (y < 0) {
            y += g_unitsY;
        }
        else if (y >= g_unitsY) {
            y -= g_unitsY;
        }

        return x + y * g_unitsX;
    }

    bool gameRule(bool state, int alives) {
        if (state) {
            if (2 > alives || alives > 3)
                return false;
        }
        else {
            if (alives == 3)
                return true;
        }
        return state;
    }

    void tick(HWND hWnd) {
        if (isPause)
            return;

        for (int iy = 0; iy < g_unitsY; iy++) {
            for (int ix = 0; ix < g_unitsX; ix++) {

                int alives = 0;
                if (fieldOld[getElementId(ix + 1, iy)]) alives++;
                if (fieldOld[getElementId(ix, iy + 1)]) alives++;
                if (fieldOld[getElementId(ix + 1, iy + 1)]) alives++;

                if (fieldOld[getElementId(ix - 1, iy)]) alives++;
                if (fieldOld[getElementId(ix, iy - 1)]) alives++;
                if (fieldOld[getElementId(ix - 1, iy - 1)]) alives++;

                if (fieldOld[getElementId(ix + 1, iy - 1)]) alives++;
                if (fieldOld[getElementId(ix - 1, iy + 1)]) alives++;

                field[getElementId(ix, iy)] = gameRule(field[getElementId(ix, iy)], alives);

            }
        }

        InvalidateRect(hWnd, 0, false);
        memcpy(fieldOld, field, g_unitsX * g_unitsY * sizeof(bool));
    }
}

namespace r {
    int wndSizeX, wndSizeY;

    ID2D1HwndRenderTarget* pRenderTarget = NULL;
    ID2D1SolidColorBrush* pWhiteBrush = NULL;
    ID2D1SolidColorBrush* pGrayBrush = NULL;
    ID2D1SolidColorBrush* pRedBrush = NULL;
    ID2D1RadialGradientBrush* pSpeedBrush = NULL;
    ID2D1RadialGradientBrush* pIceBrush = NULL;
    ID2D1RadialGradientBrush* pFlameBrush = NULL;
    ID2D1Factory* pFactory = NULL;



    void CreateResources(HWND hWnd) {
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

    void DestroyResources() {
        SafeRelease(r::pRenderTarget);
        SafeRelease(r::pWhiteBrush);
        SafeRelease(r::pGrayBrush);
        SafeRelease(r::pRedBrush);
        SafeRelease(r::pIceBrush);
        SafeRelease(r::pFlameBrush);
        SafeRelease(r::pSpeedBrush);
    }

    void BeginDraw(HWND hWnd) {
        if (pRenderTarget == NULL) {
            CreateResources(hWnd);
        }

        pRenderTarget->BeginDraw();
    }
    void EndDraw(HWND hWnd) {
        HRESULT hr = pRenderTarget->EndDraw();
        if (FAILED(hr) || hr == D2DERR_RECREATE_TARGET) {

            DestroyWindow(hWnd);
        }
    }
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR lpCmdLine, int nCmdShow) {
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
    while (ret > 0) {
        ret = GetMessage(&msg, NULL, 0, 0);
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    KillTimer(hWnd, TIMER_ID);

    return 0;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    static PAINTSTRUCT ps;
    switch (msg) {

    case WM_KEYDOWN:
        switch (wParam) {
        case 'S':
            if (!logic::isPause)
                logic::isSpeedUp = !logic::isSpeedUp;
            break;

        case VK_SPACE: [[fallthrough]]
        case 'P':
            logic::isPause = !logic::isPause;
            logic::isSpeedUp = false;
            break;

        case VK_ESCAPE: [[fallthrough]]
        case 'F':
            logic::isFlame = true;
            memset(logic::fieldOld, false, g_unitsX * g_unitsY * sizeof(bool));
            memset(logic::field, false, g_unitsX * g_unitsY * sizeof(bool));
            break;
        }
        InvalidateRect(hWnd, 0, false);
        break;

    case WM_KEYUP:
        if (wParam == 'F' || wParam == VK_ESCAPE) {
            logic::isFlame = false;
        }
        InvalidateRect(hWnd, 0, false);
        break;

    case WM_MOUSEMOVE:
        logic::cursorPosX = LOWORD(lParam);
        logic::cursorPosY = HIWORD(lParam);
        InvalidateRect(hWnd, 0, false);
        break;

    case WM_SIZE:
        r::DestroyResources();
        InvalidateRect(hWnd, 0, false);
        break;

    case WM_LBUTTONDOWN:
    {
        if (!logic::isFlame) {
            int xStep = r::wndSizeX / g_unitsX,
                yStep = r::wndSizeY / g_unitsY;

            int id = logic::getElementId(logic::cursorPosX / xStep, logic::cursorPosY / yStep);

            bool val = !logic::field[id];
            logic::field[id] = val;
            logic::fieldOld[id] = val;
        }
        InvalidateRect(hWnd, 0, false);
    }
    break;

    case WM_TIMER:
        if (logic::isSpeedUp) {
            if (clock() - logic::timeStamp > g_tickTime / 4) {
                logic::timeStamp = clock();
                logic::tick(hWnd);
                InvalidateRect(hWnd, 0, false);
            }
        }
        else {
            if (clock() - logic::timeStamp > g_tickTime) {
                logic::timeStamp = clock();
                logic::tick(hWnd);
                InvalidateRect(hWnd, 0, false);
            }
        }
        break;

    case WM_CREATE:
        if (FAILED(D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &r::pFactory)))
            DestroyWindow(hWnd);
        r::CreateResources(hWnd);


        //memset(logic::field, true, sizeof(bool) * g_unitsX * g_unitsY);
        logic::field[logic::getElementId(2, 0)] = true;
        logic::field[logic::getElementId(3, 1)] = true;
        logic::field[logic::getElementId(1, 2)] = true;
        logic::field[logic::getElementId(2, 2)] = true;
        logic::field[logic::getElementId(3, 2)] = true;


        memcpy(logic::fieldOld, logic::field, g_unitsX * g_unitsY * sizeof(bool));
        break;

    case WM_PAINT:
    {
        if (logic::isSpeedUp) {
            if (clock() - logic::timeStamp > g_tickTime / 3) {
                logic::timeStamp = clock();
                logic::tick(hWnd);
            }
        }
        else {
            if (clock() - logic::timeStamp > g_tickTime) {
                logic::timeStamp = clock();
                logic::tick(hWnd);
            }
        }

        BeginPaint(hWnd, &ps);

        r::BeginDraw(hWnd);

        r::pRenderTarget->Clear(D2D1::ColorF(0.2f, 0.2f, 0.2f));

        float xStep = floor(r::wndSizeX / g_unitsX),
            yStep = floor(r::wndSizeY / g_unitsY);

        for (float i = xStep; i < r::wndSizeX; i += xStep) {
            r::pRenderTarget->DrawLine(D2D1::Point2F(i, 0), D2D1::Point2F(i, r::wndSizeY), r::pGrayBrush);
        }
        for (float i = yStep; i < r::wndSizeY; i += yStep) {
            r::pRenderTarget->DrawLine(D2D1::Point2F(0, i), D2D1::Point2F(r::wndSizeX, i), r::pGrayBrush);
        }

        for (int iy = 0; iy < g_unitsY; iy++) {
            for (int ix = 0; ix < g_unitsX; ix++) {
                if (logic::field[logic::getElementId(ix, iy)]) {
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
        rect.left = int(logic::cursorPosX / xStep) * xStep;
        rect.right = rect.left + xStep;
        rect.top = int(logic::cursorPosY / yStep) * yStep;
        rect.bottom = rect.top + yStep;
        r::pRenderTarget->DrawRectangle(rect, r::pRedBrush, 2.0f);

        if (logic::isPause) {
            r::pRenderTarget->FillRectangle(D2D1::RectF(0.0f, 0.0f, r::wndSizeX, r::wndSizeY), r::pIceBrush);
        }
        else if (logic::isSpeedUp) {
            r::pRenderTarget->FillRectangle(D2D1::RectF(0.0f, 0.0f, r::wndSizeX, r::wndSizeY), r::pSpeedBrush);
        }
        if (logic::isFlame) {
            r::pRenderTarget->FillRectangle(D2D1::RectF(0.0f, 0.0f, r::wndSizeX, r::wndSizeY), r::pFlameBrush);
        }


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