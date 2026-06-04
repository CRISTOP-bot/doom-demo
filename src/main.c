#include <windows.h>
#include <stdint.h>
#include <stdio.h>
#include "game_shared.h"

typedef void (*game_reset_fn)(GameState *);
typedef void (*game_tick_fn)(GameState *, const InputState *);
typedef void (*game_render_fn)(GameState *, uint32_t *, float *);

static uint32_t pixels[SCREEN_W * SCREEN_H];
static float zbuffer[SCREEN_W];
static GameState game;

static HINSTANCE g_hInst;
static HWND g_hwnd;
static BITMAPINFO bmi;

static int mouse_locked = 1;
static HMODULE game_dll;
static game_reset_fn game_reset_ptr;
static game_tick_fn game_tick_ptr;
static game_render_fn game_render_ptr;

static int prev_space = 0;
static int prev_e = 0;
static int prev_tab = 0;
static int prev_r = 0;

static void set_cursor_visible(int visible) {
    if (visible) {
        while (ShowCursor(TRUE) < 0) {}
    } else {
        while (ShowCursor(FALSE) >= 0) {}
    }
}

static void update_mouse_lock(HWND hwnd) {
    RECT rc;
    POINT tl, br, center;

    GetClientRect(hwnd, &rc);

    tl.x = rc.left;
    tl.y = rc.top;
    br.x = rc.right;
    br.y = rc.bottom;

    ClientToScreen(hwnd, &tl);
    ClientToScreen(hwnd, &br);

    center.x = (tl.x + br.x) / 2;
    center.y = (tl.y + br.y) / 2;

    if (mouse_locked) {
        RECT clip = { tl.x, tl.y, br.x, br.y };
        ClipCursor(&clip);
        SetCursorPos(center.x, center.y);
        set_cursor_visible(0);
    } else {
        ClipCursor(NULL);
        set_cursor_visible(1);
    }

    game.mouse_locked = mouse_locked;
}

static void toggle_mouse_lock(HWND hwnd) {
    mouse_locked = !mouse_locked;
    update_mouse_lock(hwnd);
}

static int key_down(int vk_or_char) {
    return (GetAsyncKeyState(vk_or_char) & 0x8000) != 0;
}

static void capture_input(HWND hwnd, InputState *in) {
    ZeroMemory(in, sizeof(*in));

    in->w = key_down('W');
    in->a = key_down('A');
    in->s = key_down('S');
    in->d = key_down('D');
    in->left  = key_down(VK_LEFT);
    in->right = key_down(VK_RIGHT);

    int sp = key_down(VK_SPACE);
    int e  = key_down('E');
    int t  = key_down(VK_TAB);
    int r  = key_down('R');

    in->space = sp && !prev_space;
    in->e     = e && !prev_e;
    in->tab   = t && !prev_tab;
    in->r     = r && !prev_r;

    prev_space = sp;
    prev_e = e;
    prev_tab = t;
    prev_r = r;

    if (in->tab) {
        toggle_mouse_lock(hwnd);
    }

    if (mouse_locked) {
        RECT rc;
        POINT tl, br, center, cur;

        GetClientRect(hwnd, &rc);
        tl.x = rc.left;
        tl.y = rc.top;
        br.x = rc.right;
        br.y = rc.bottom;

        ClientToScreen(hwnd, &tl);
        ClientToScreen(hwnd, &br);

        center.x = (tl.x + br.x) / 2;
        center.y = (tl.y + br.y) / 2;

        GetCursorPos(&cur);
        in->mouse_dx = cur.x - center.x;
        SetCursorPos(center.x, center.y);
    } else {
        in->mouse_dx = 0;
    }

    if (!mouse_locked) {
        in->left = 0;
        in->right = 0;
    }
}

static void present(HDC hdc, RECT rc) {
    StretchDIBits(
        hdc,
        0, 0, rc.right - rc.left, rc.bottom - rc.top,
        0, 0, SCREEN_W, SCREEN_H,
        pixels,
        &bmi,
        DIB_RGB_COLORS,
        SRCCOPY
    );
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_DESTROY:
            ClipCursor(NULL);
            PostQuitMessage(0);
            return 0;

        case WM_SETFOCUS:
            if (mouse_locked) update_mouse_lock(hwnd);
            return 0;

        case WM_KILLFOCUS:
            ClipCursor(NULL);
            set_cursor_visible(1);
            return 0;

        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);
            RECT rc;
            GetClientRect(hwnd, &rc);
            present(hdc, rc);
            EndPaint(hwnd, &ps);
            return 0;
        }
    }

    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

static int load_game_dll(void) {
    game_dll = LoadLibraryA("game_dll.dll");
    if (!game_dll) {
        MessageBoxA(NULL, "No se pudo cargar game_dll.dll", "Error", MB_ICONERROR);
        return 0;
    }

    game_reset_ptr = (game_reset_fn)GetProcAddress(game_dll, "game_reset");
    game_tick_ptr = (game_tick_fn)GetProcAddress(game_dll, "game_tick");
    game_render_ptr = (game_render_fn)GetProcAddress(game_dll, "game_render");

    if (!game_reset_ptr || !game_tick_ptr || !game_render_ptr) {
        MessageBoxA(NULL, "La DLL no tiene las funciones necesarias.", "Error", MB_ICONERROR);
        return 0;
    }

    return 1;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    (void)hPrevInstance;
    (void)lpCmdLine;

    g_hInst = hInstance;

    const char CLASS_NAME[] = "MiniDoomPlusClass";

    WNDCLASS wc = {0};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);

    if (!RegisterClass(&wc)) {
        MessageBoxA(NULL, "No se pudo registrar la clase de ventana.", "Error", MB_ICONERROR);
        return 1;
    }

    g_hwnd = CreateWindowEx(
        0,
        CLASS_NAME,
        "Mini DOOM Plus - Mejorado",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT,
        960, 640,
        NULL,
        NULL,
        hInstance,
        NULL
    );

    if (!g_hwnd) {
        MessageBoxA(NULL, "No se pudo crear la ventana.", "Error", MB_ICONERROR);
        return 1;
    }

    ShowWindow(g_hwnd, nCmdShow);

    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = SCREEN_W;
    bmi.bmiHeader.biHeight = -SCREEN_H;
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;

    if (!load_game_dll()) {
        return 1;
    }

    game_reset_ptr(&game);
    update_mouse_lock(g_hwnd);

    MSG msg;
    int running = 1;

    while (running) {
        while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
            if (msg.message == WM_QUIT) running = 0;
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        InputState input;
        capture_input(g_hwnd, &input);

        game_tick_ptr(&game, &input);
        game_render_ptr(&game, pixels, zbuffer);

        HDC hdc = GetDC(g_hwnd);
        RECT rc;
        GetClientRect(g_hwnd, &rc);
        present(hdc, rc);
        ReleaseDC(g_hwnd, hdc);

        Sleep(1);
    }

    if (game_dll) FreeLibrary(game_dll);
    ClipCursor(NULL);
    set_cursor_visible(1);

    return 0;
}
