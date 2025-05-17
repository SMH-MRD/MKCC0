#pragma once

#include <windows.h>
#include <string>

class CSubPanel {
public:
    CSubPanel(HINSTANCE hInstance, HWND hParent) {
        const wchar_t CLASS_NAME[] = L"SubWindowClass";

        WNDCLASS wc = { };
        wc.lpfnWndProc = CSubPanel::WndProc;
        wc.hInstance = hInstance;
        wc.lpszClassName = CLASS_NAME;

        RegisterClass(&wc);

        hWnd = CreateWindowEx(
            0,                              // Optional window styles
            CLASS_NAME,                     // Window class
            L"サブウィンドウ",              // Window text
            WS_OVERLAPPEDWINDOW,            // Window style

            CW_USEDEFAULT, CW_USEDEFAULT, 300, 200,
            hParent, nullptr, hInstance, nullptr
        );

        if (hWnd) {
            ShowWindow(hWnd, SW_SHOW);
        }
    }

    static LRESULT CALLBACK WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
        switch (uMsg) {
        case WM_DESTROY:
            // PostQuitMessage(0);
            return 0;
        case WM_CLOSE:
            DestroyWindow(hwnd);
            return 0;
        }
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }

private:
    HWND hWnd;
};

