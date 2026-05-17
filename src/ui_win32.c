#include <windows.h>
#include <stdio.h>
#include "editor_api.h"

static EditorHandle *g_editor = NULL;

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    case WM_CREATE: {
        g_editor = editor_create(512);
        if (g_editor) {
            editor_set_text(g_editor, "IntelliEditor Win32 Prototype\n\nThis is a demo using editor_api.");
        }
        return 0;
    }
    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);
        char buf[4096] = {0};
        if (g_editor) editor_to_string(g_editor, buf, sizeof(buf));
        DrawTextA(hdc, buf, -1, &ps.rcPaint, DT_LEFT | DT_WORDBREAK);
        EndPaint(hWnd, &ps);
        return 0;
    }
    case WM_DESTROY:
        if (g_editor) editor_destroy(g_editor);
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProcA(hWnd, msg, wParam, lParam);
}

int main(void) {
    const char *className = "IntelliEditorDemoClass";
    HINSTANCE hInst = GetModuleHandleA(NULL);

    WNDCLASSA wc = {0};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInst;
    wc.lpszClassName = className;

    if (!RegisterClassA(&wc)) {
        fprintf(stderr, "Failed to register window class\n");
        return 1;
    }

    HWND hwnd = CreateWindowA(className, "IntelliEditor - Prototype UI", WS_OVERLAPPEDWINDOW,
                              CW_USEDEFAULT, CW_USEDEFAULT, 640, 480,
                              NULL, NULL, hInst, NULL);
    if (!hwnd) {
        fprintf(stderr, "Failed to create window\n");
        return 1;
    }

    ShowWindow(hwnd, SW_SHOWDEFAULT);
    UpdateWindow(hwnd);

    MSG msg;
    while (GetMessageA(&msg, NULL, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessageA(&msg);
    }

    return (int)msg.wParam;
}
