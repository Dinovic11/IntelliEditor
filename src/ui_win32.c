// Prototype Win32 UI: contrôle d'édition multiligne lié à editor_api
#include <windows.h>
#include <stdio.h>
#include "editor_api.h"

#define ID_EDIT 1001

static EditorHandle *g_editor = NULL;
static HWND g_edit = NULL;

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    case WM_CREATE: {
        g_editor = editor_create(512);
        if (g_editor) {
            editor_set_text(g_editor, "IntelliEditor Prototype Win32\r\n\r\nCeci est une démo utilisant editor_api.");
        }

        // Create multiline edit control
        g_edit = CreateWindowA("EDIT", NULL,
                               WS_CHILD | WS_VISIBLE | WS_VSCROLL | ES_MULTILINE | ES_AUTOVSCROLL | ES_AUTOHSCROLL,
                               0, 0, 0, 0,
                               hWnd, (HMENU)ID_EDIT, GetModuleHandleA(NULL), NULL);
        if (g_edit && g_editor) {
            char buf[4096] = {0};
            editor_to_string(g_editor, buf, sizeof(buf));
            SetWindowTextA(g_edit, buf);
        }
        return 0;
    }
    case WM_SIZE: {
        if (g_edit) {
            RECT rc;
            GetClientRect(hWnd, &rc);
            SetWindowPos(g_edit, NULL, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top, SWP_NOZORDER);
        }
        return 0;
    }
    case WM_COMMAND: {
        if (LOWORD(wParam) == ID_EDIT && HIWORD(wParam) == EN_CHANGE) {
            // Update editor buffer when edit changes
            if (g_edit && g_editor) {
                int len = GetWindowTextLengthA(g_edit);
                if (len > 0) {
                    char *buf = (char *)malloc((size_t)len + 1);
                    if (buf) {
                        GetWindowTextA(g_edit, buf, len + 1);
                        editor_set_text(g_editor, buf);
                        free(buf);
                    }
                } else {
                    editor_set_text(g_editor, "");
                }
            }
        }
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
        fprintf(stderr, "Erreur : impossible d'enregistrer la classe de fenêtre\n");
        return 1;
    }

    HWND hwnd = CreateWindowA(className, "IntelliEditor - Prototype Win32", WS_OVERLAPPEDWINDOW,
                              CW_USEDEFAULT, CW_USEDEFAULT, 640, 480,
                              NULL, NULL, hInst, NULL);
    if (!hwnd) {
        fprintf(stderr, "Erreur : impossible de créer la fenêtre\n");
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
