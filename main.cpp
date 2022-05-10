#include <windows.h>
#include <winuser.h>
#include <wincon.h>
#include <iostream>
#include <vector>
#include <thread>
//#include <gdiplus.h>
//#include <gdiplusbitmap.h>

#include "Constants.h"

#define SQRT_2 1.41421356
#define TG_80deg 1.3962634

#define W_HEIGHT 600
#define W_WIDTH 800
#define X_CENTER W_WIDTH / 2
#define Y_CENTER W_HEIGHT / 2

#define R0 float(W_WIDTH) / (2 * TG_80deg)

#define WHITE RGBQUAD({ BYTE(255), BYTE(255), BYTE(255) })
#define BLACK RGBQUAD({ BYTE(0), BYTE(0), BYTE(0) })
#define BACKGROUND WHITE
//#define RED RED

using namespace std;
//using namespace system

struct POINT3D// : POINT
{
    LONG x, y, z;
    void move(const POINT3D& v)
    {
        x += v.x;
        y += v.y;
        z += v.z;
    }
};

struct Line
{
    POINT3D p[2];
    RGBQUAD color;
    Line(const POINT3D& p1, const POINT3D& p2, const RGBQUAD& _color)
    {
        p[0] = p1;
        p[1] = p2;
        color = _color;
    }
    void shift(const POINT3D& v)
    {
        p[0].move(v);
        p[1].move(v);
    }
};

struct Triangle
{
    POINT3D p[3];
    RGBQUAD color;
    Triangle(const POINT3D& p1, const POINT3D& p2, const POINT3D& p3, const RGBQUAD& _color)
    {
        p[0] = p1;
        p[1] = p2;
        p[2] = p3;
        color = _color;
    }
    void shift(const POINT3D& v)
    {
        p[0].move(v);
        p[1].move(v);
        p[2].move(v);
    }
};

//typedef vector<Triangle> Figure;
struct Figure// : vector<Triangle>
{
    vector<Triangle> tngls;
    Figure(const initializer_list<Triangle>& _tngls)
    {
        tngls = _tngls;
    }
    void shift(const POINT3D& v)
    {
        for (auto& t : tngls)
        {
            t.shift(v);
        }
    }
};

struct Matrix
{
    RGBQUAD* p;
    Matrix()
    {
        p = new RGBQUAD[W_WIDTH * W_HEIGHT];
        for (UINT i = 0; i < W_HEIGHT * W_WIDTH; ++i)
        {
            p[i] = BACKGROUND;
        }
    }
    void clear()
    {
        /*for (UINT i = 0; i < W_WIDTH * W_HEIGHT; ++i)
            p[i] = BACKGROUND;*/
        fill(&p[0], &p[W_WIDTH * W_HEIGHT], BACKGROUND);
    }
    void setPixel(UINT x, UINT y, const RGBQUAD& color)
    {
        if (x < W_WIDTH && y < W_HEIGHT)
        {
            p[y * W_WIDTH + x] = color;
            //cout << x << ' ' << y << endl;
        }
    }
    Matrix& operator=(const Matrix& m)
    {
        Matrix nm;
        for (UINT i = 0; i < W_HEIGHT * W_HEIGHT; ++i)
        {
            nm.p[i] = m.p[i];
        }
        return nm;
    }
};

class Draw
{
public:
    HDC hdc;
    Matrix matrix;
    //vector<thread*> tds;
public:
    Draw(HDC& _hdc)//, Matrix& m)
    {
        hdc = _hdc;
        //matrix = m;
    }
    void line(const Line& l)
    {
        INT x0, y0, x1, y1;

        double zoom0 = double(1) + double(l.p[0].z) / R0;
        double zoom1 = double(1) + double(l.p[1].z) / R0;

        INT xGap0 = W_WIDTH * INT((1 - 1 / zoom0) / 2) + W_WIDTH / 2;
        INT yGap0 = W_HEIGHT * INT((1 - 1 / zoom0) / 2) + W_HEIGHT / 2;
        INT xGap1 = W_WIDTH * INT((1 - 1 / zoom1) / 2) + W_WIDTH / 2;
        INT yGap1 = W_HEIGHT * INT((1 - 1 / zoom1) / 2) + W_HEIGHT / 2;

        x0 = xGap0 + l.p[0].x / zoom0;
        y0 = yGap0 + l.p[0].y / zoom0;
        x1 = xGap1 + l.p[1].x / zoom1;
        y1 = yGap1 + l.p[1].y / zoom1;


        INT dx = x0 - x1;
        INT dy = y0 - y1;
        INT begY;
        INT minX;

        double tan = (double)dy / (double)dx;
        if (abs(tan) <= 1)
        {
            if (dx < 0)
            {
                dx = -dx;
                dy = -dy;
                begY = y0;
                minX = x0;
            }
            else
            {
                begY = y1;
                minX = x1;
            }
            for (UINT x = 0; x < dx; ++x)
            {
                matrix.setPixel(x + minX, begY + x * tan, l.color);
            }
        }
        else
        {
            if (dy < 0)
            {
                dx = -dx;
                dy = -dy;
                begY = y0;
                minX = x0;
            }
            else
            {
                begY = y1;
                minX = x1;
            }
            for (UINT y = 0; y < dy; ++y)
            {
                //SetPixel(hdc, minX + y / tan, begY + y, l.color);
                matrix.setPixel(minX + INT(y / tan), begY + y, l.color);
            }
        }
    }
    void triangle(const Triangle& t)
    {
        line(Line(t.p[0], t.p[1], t.color));
        line(Line(t.p[1], t.p[2], t.color));
        line(Line(t.p[2], t.p[0], t.color));
    }
    void figure(const Figure& f)
    {
        for (Triangle t : f.tngls)
            triangle(t);
    }
    /*void byMatrix(HDC _hdc)
    {
        /*UINT x = 0, y = 0;
        for (auto row : matrix.p)
        {
            for (auto pxl : row)
                SetPixel(hdc, x++, y, pxl);
            ++y;
        }*/
        /*for (UINT y = 0; y < W_HEIGHT; ++y)
        {
            //cout << y << endl;
            for (UINT x = 0; x < W_WIDTH; ++x)
            {
                //cout << x << endl;
                SetPixel(_hdc, x, y, matrix.p[y][x]);
            }
        }
    }*/
    void byMatrix()
    {
        /*UINT x = 0, y = 0;
        for (auto row : matrix.p)
        {
            for (auto pxl : row)
                SetPixel(hdc, x++, y, pxl);
            ++y;
        }*/
        for (UINT y = 0; y < W_HEIGHT; ++y)
        {
            //cout << yd << endl;
            //tds.push_back(new thread(&Draw::OneLineOnDC, this, y));
            //tds.back()->join();
            OneLineOnDC(y);
        }
        //for (auto td : tds)
            //td->join();
    }
private:
    void OneLineOnDC(const UINT l)
    {
        for (UINT x = 0; x < W_WIDTH; ++x)
        {
            //cout << x << endl;
            //for (UINT i = 0; i < W_WIDTH * W_HEIGHT; ++i)
            //tds.push_back(thread(SetPixel, ref(hdc), x, y, RGB(matrix.p[y * W_WIDTH + x].rgbRed, matrix.p[y * W_WIDTH + x].rgbGreen, matrix.p[y * W_WIDTH + x].rgbBlue)));
            //tds.back().join();
            UINT y = l * W_WIDTH;
            SetPixelV(hdc, x, l, RGB(matrix.p[y + x].rgbRed, matrix.p[y + x].rgbGreen, matrix.p[y + x].rgbBlue));
        }
    }
};

LRESULT WINAPI mainProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
        /*case WM_KEYDOWN:
        {
            if (wParam == VK_CONTROL)
        }*/
    case WM_CLOSE:
    {
        PostQuitMessage(0);
    }
    /*case WM_PAINT: {
        PAINTSTRUCT paintStruct = { 0 };
        HDC hDC = BeginPaint(hWnd, &paintStruct);

        EndPaint(hWnd, &paintStruct);
    }*/
    break;
    }
    return 0;
}

class MainWindow
{
public:
    HWND hwnd;
    HDC hdc;
    MainWindow()
    {
        hwnd = CreateWindowEx(0, WC_DIALOG, "Main Window", WS_VISIBLE | WS_SYSMENU,
            CW_USEDEFAULT, CW_USEDEFAULT, W_WIDTH, W_HEIGHT, 0, 0, 0, 0);
        SetWindowLong(hwnd, 4, (LRESULT)mainProc);

        hdc = GetDC(hwnd);
    }
};

void messageThread(MSG& msg, HWND& hwnd)
{
    while (GetMessage(&msg, hwnd, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}

int main()
{
    MainWindow mainwindow;
    MSG msg;

    //Matrix display;
    Draw draw(mainwindow.hdc);// , display);

    /*COLORREF bitMassive[W_WIDTH * W_HEIGHT];
    HBITMAP bitMap = CreateBitmap(W_WIDTH, W_HEIGHT, 1, sizeof(COLORREF), bitMassive);
    auto hMemDC = CreateCompatibleDC(mainwindow.hdc);
    BitBlt(mainwindow.hdc, 0, 0, W_WIDTH, W_HEIGHT, hMemDC, W_WIDTH, W_HEIGHT, SRCCOPY);*/

    //POINT3D p = { 0, 0, 0 };


    RGBQUAD someClr = { 200, 200, 100 };
    Figure cube = { Triangle({100, 100, 100}, {100, 200, 100}, {200, 200, 100}, someClr),//�����
                    Triangle({100, 100, 100}, {200, 100, 100}, {200, 200, 100}, someClr),

                    Triangle({100, 100, 200}, {100, 200, 200}, {200, 200, 200}, someClr),//���
                    Triangle({100, 100, 200}, {200, 100, 200}, {200, 200, 200}, someClr),

                    Triangle({100, 100, 100}, {100, 100, 200}, {100, 200, 200}, someClr),//����
                    Triangle({100, 100, 100}, {100, 200, 100}, {100, 200, 200}, someClr),

                    Triangle({200, 100, 100}, {200, 100, 200}, {200, 200, 200}, someClr),//�����
                    Triangle({200, 100, 100}, {200, 200, 100}, {200, 200, 200}, someClr),

                    Triangle({100, 100, 100}, {200, 100, 100}, {100, 100, 200}, someClr),//����
                    Triangle({100, 100, 200}, {200, 100, 200}, {200, 100, 100}, someClr),

                    Triangle({100, 200, 100}, {200, 200, 100}, {100, 200, 200}, someClr),//���
                    Triangle({100, 200, 200}, {200, 200, 200}, {200, 200, 100}, someClr) };
    Line line({ 0, 0, 200 }, { 400, 400, 0 }, BLACK);
    //draw.line(line);

    //draw.triangle(Triangle({ 100, 100 }, { 200, 150 }, { 150, 300 }, BLACK));
    //draw.line(line);
    //draw.figure(cube);

    //thread messageThread(messageThread, ref(msg), ref(hwnd));
    //messageThread.detach();
    GetMessage(&msg, 0, 0, 0);
    POINT cursor = msg.pt;

    COLORREF* mass = new COLORREF[W_WIDTH * W_HEIGHT];
    for (UINT i = 0; i < W_WIDTH * W_HEIGHT; ++i)
        mass[i] = RGB(100, 0, 200);

    BITMAPINFO bif;
    ZeroMemory(&bif, sizeof(BITMAPINFO));

    bif.bmiHeader.biSize = sizeof(bif);
    bif.bmiHeader.biHeight = -W_HEIGHT;
    bif.bmiHeader.biWidth = W_WIDTH;
    bif.bmiHeader.biSizeImage = (bif.bmiHeader.biWidth) * bif.bmiHeader.biHeight;
    bif.bmiHeader.biPlanes = 1;
    bif.bmiHeader.biBitCount = sizeof(RGBQUAD) * 8;

    PAINTSTRUCT ps;
    HBITMAP hBitmap;
    BITMAP bm;
    HDC hDC;
    HDC hMemDC;

    while (GetMessage(&msg, 0, 0, 0))
    {
        cube.shift({ msg.pt.x - cursor.x, msg.pt.y - cursor.y, 0 });
        if (msg.message == WM_KEYDOWN && msg.wParam == VK_UP)
        {
            cube.shift({ 0, 0, -10 });
        }
        if (msg.message == WM_KEYDOWN && msg.wParam == VK_DOWN)
        {
            cube.shift({ 0, 0, 10 });
        }
        if (msg.message == WM_KEYDOWN && msg.wParam == VK_LEFT)
        {
            cube.shift({ 10, 0, 0 });
        }
        if (msg.message == WM_KEYDOWN && msg.wParam == VK_RIGHT)
        {
            cube.shift({ -10, 0, 0 });
        }
        cursor = msg.pt;

        draw.matrix.clear();
        draw.figure(cube);
        //draw.byMatrix();

        /*HDC hDC;
        PAINTSTRUCT ps;
        HBRUSH brush;
        hDC = BeginPaint(mainwindow.hwnd, &ps);
        for (int y = 0; y < W_HEIGHT; y+=10)
            for (int x = 0; x < W_WIDTH; x+=10)
            {
                brush = CreateSolidBrush(COLORREF(RGB(100, 100, 100)));
                SelectObject(hDC, brush);

                Rectangle(hDC, x, y, x+10, y+10);
            }
        EndPaint(mainwindow.hwnd, &ps);*/
        //line.shift({ 10, 10, 100 });
        //draw.line(line);

        /*HDC newHDC = CreateCompatibleDC(mainwindow.hdc);
        draw.byMatrix(newHDC);
        if (!BitBlt(mainwindow.hdc, 0, 0, W_WIDTH, W_HEIGHT, newHDC, 0, 0, SRCCOPY))
            cout << "!BitBlt" << endl;


        //UpdateWindow(mainwindow.hwnd);
        if (!RedrawWindow(mainwindow.hwnd, 0, 0, RDW_INTERNALPAINT))
            cout << "!rdrwwnd" << endl;

        DeleteDC(newHDC);

        //cout << draw.matrix.p[0][0] << endl;
            //cout << "BACKGROUND" << endl;
        /*for (UINT i = 0; i < W_HEIGHT; ++i)
        {
            for (UINT j = 0; j < W_WIDTH; ++j)
                cout << draw.matrix.p[i][j] << ' ';
            cout << endl;
        }*/
        /*HDC hdcbmp = CreateCompatibleDC(mainwindow.hdc);

        HBITMAP bmp = CreateCompatibleBitmap(mainwindow.hdc, W_WIDTH, W_HEIGHT);//CreateBitmap(W_WIDTH, W_HEIGHT, 1, sizeof(COLORREF), *draw.matrix.p);
        SetDIBits(hdcbmp, bmp, mass, )

        if (!bmp)
            cout << "no bmp" << endl;
        BITMAP Bitmap;
        /*if (!GetObject(bmp, sizeof(BITMAP), (LPSTR)&Bitmap))
            cout << "!getobject" << endl;*/
            /*if (!SelectObject(hdcbmp, bmp))
                cout << "!selectobject" << endl;
            if (!BitBlt(mainwindow.hdc, 0, 0, Bitmap.bmWidth, Bitmap.bmHeight, hdcbmp, 0, 0, SRCCOPY))
                cout << "!bitblt" << endl;
            ReleaseDC(mainwindow.hwnd, mainwindow.hdc);

            DeleteDC(hdcbmp);*/

            //SetDIBitsToDevice(mainwindow.hdc, 0, 0, W_WIDTH, W_HEIGHT, )

            /*hDC = BeginPaint(mainwindow.hwnd, &ps);
            hMemDC = CreateCompatibleDC(hDC);
            hBitmap = CreateBitmap(W_WIDTH, W_HEIGHT, 1, sizeof(RGBQUAD) * 8, draw.matrix.p);
            GetObject(hBitmap, sizeof(BITMAP), &bm);
            SelectObject(hMemDC, hBitmap);
            BitBlt(hDC, 0, 0, bm.bmWidth, bm.bmHeight, hMemDC, 0, 0, SRCCOPY);
            DeleteDC(hMemDC);
            ReleaseDC(mainwindow.hwnd, hDC);
            //DeleteObject(hBitmap);
            EndPaint(mainwindow.hwnd, &ps);*/

            //RECT* r = new RECT({ 0, 0, W_WIDTH, W_HEIGHT });
            //PAINTSTRUCT paintStruct = { 0 };
            //HDC hDC = BeginPaint(mainwindow.hwnd, &paintStruct);
            //InvalidateRect(mainwindow.hwnd, NULL, 1);
            //EndPaint(mainwindow.hwnd, &paintStruct);
            //ValidateRect(mainwindow.hwnd, NULL);

            //auto nHDC = GetDC(mainwindow.hwnd);
            //PAINTSTRUCT ps;
            //HDC nHDC = BeginPaint(mainwindow.hwnd, &ps);*/

        RECT rect = { 0, 0, W_WIDTH, W_HEIGHT };
        GetClientRect(mainwindow.hwnd, &rect);
        auto hdc = BeginPaint(mainwindow.hwnd, &ps);
        SetBkColor(hdc, 0xEECCCC);
        // �������� ������ ��������� ��� ������� ������������
        auto hCmpDC = CreateCompatibleDC(hdc);
        auto hBmp = CreateCompatibleBitmap(hdc, W_WIDTH, W_HEIGHT);
        SelectObject(hCmpDC, hBmp);
        // �������� ������� ������
        LOGBRUSH br;
        br.lbStyle = BS_SOLID;
        br.lbColor = 0xEECCCC;
        HBRUSH brush;
        brush = CreateBrushIndirect(&br);
        FillRect(hCmpDC, &rect, brush);
        DeleteObject(brush);
        // ���������
        SetDIBitsToDevice(mainwindow.hdc, 0, 0, W_WIDTH, W_HEIGHT, 0, 0, 0, W_HEIGHT, draw.matrix.p, &bif, DIB_PAL_COLORS);
        // ����� �� �����
        SetStretchBltMode(hdc, COLORONCOLOR);
        BitBlt(hdc, 0, 0, W_WIDTH, W_HEIGHT,
            hCmpDC, 0, 0, SRCCOPY);
        DeleteDC(hCmpDC);
        DeleteObject(hBmp);
        hCmpDC = NULL;
        EndPaint(mainwindow.hwnd, &ps);

        //InvalidateRect(mainwindow.hwnd, NULL, 1);
        //cout << SetDIBitsToDevice(mainwindow.hdc, 0, 0, W_WIDTH, W_HEIGHT, 0, 0, 0, W_HEIGHT, draw.matrix.p, &bif, DIB_PAL_COLORS) << endl;
        //UpdateWindow(mainwindow.hwnd);
        //ReleaseDC(mainwindow.hwnd, nHDC);
        //UpdateWindow(mainwindow.hwnd);

        TranslateMessage(&msg);
        DispatchMessage(&msg);

        //cout << "newDC" << endl;
    }

    return 0;
}