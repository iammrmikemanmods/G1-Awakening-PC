// SPDX-License-Identifier: MIT
// Windows-only opening helper. This does not run or replace the game runtime.
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#include <mfplay.h>
#include <shellapi.h>
#include <atomic>
#include <new>
#include "startup_contract.h"

namespace {
constexpr UINT video_ended = WM_APP + 10, video_error = WM_APP + 11;
constexpr UINT_PTR poll_timer = 1;
struct Callback final : IMFPMediaPlayerCallback {
    std::atomic<ULONG> references{1};
    HWND window;
    explicit Callback(HWND h) : window(h) {}
    STDMETHODIMP QueryInterface(REFIID id, void** out) override {
        if (!out) return E_POINTER;
        *out = nullptr;
        if (id != __uuidof(IUnknown) && id != __uuidof(IMFPMediaPlayerCallback)) return E_NOINTERFACE;
        *out = static_cast<IMFPMediaPlayerCallback*>(this); AddRef(); return S_OK;
    }
    STDMETHODIMP_(ULONG) AddRef() override { return ++references; }
    STDMETHODIMP_(ULONG) Release() override {
        const ULONG remaining = --references;
        if (!remaining) delete this;
        return remaining;
    }
    STDMETHODIMP_(void) OnMediaPlayerEvent(MFP_EVENT_HEADER* event) override {
        if (!event || FAILED(event->hrEvent)) PostMessageW(window, video_error, 0, 0);
        else if (event->eEventType == MFP_EVENT_TYPE_PLAYBACK_ENDED) PostMessageW(window, video_ended, 0, 0);
    }
};
struct State {
    HWND window = nullptr;
    IMFPMediaPlayer* player = nullptr;
    HBITMAP image = nullptr;
    const wchar_t* marker = nullptr;
    ULONGLONG started = 0;
    bool ended = false, ready = false, error = false;
    int result = 0;
};
State* state(HWND window) {
    return reinterpret_cast<State*>(GetWindowLongPtrW(window, GWLP_USERDATA));
}
bool menu_ready(const wchar_t* path) {
    if (!path || !*path) return false;
    HANDLE file = CreateFileW(path, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
                              nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (file == INVALID_HANDLE_VALUE) return false;
    LARGE_INTEGER size{};
    char data[64]{};
    DWORD received = 0;
    const bool valid_size = GetFileSizeEx(file, &size) && size.QuadPart == static_cast<LONGLONG>(g1::ready_marker.size());
    const bool read = valid_size && ReadFile(file, data, static_cast<DWORD>(sizeof(data)), &received, nullptr);
    CloseHandle(file);
    return read && g1::valid_marker(std::string_view(data, received));
}
void stop_video(State& s) {
    if (s.player) { s.player->Shutdown(); s.player->Release(); s.player = nullptr; }
}
void finish_video(State& s) {
    if (!s.ended) { s.ended = true; stop_video(s); InvalidateRect(s.window, nullptr, TRUE); }
}
void advance(State& s) {
    s.ready = s.ready || menu_ready(s.marker);
    const auto next = g1::phase(s.ended, s.ready, s.error, GetTickCount64() - s.started);
    if (next == g1::Phase::complete) DestroyWindow(s.window);
    else if (next == g1::Phase::failed_open) {
        s.result = s.error ? 3 : 4;
        DestroyWindow(s.window); // The separately running game must never be held hostage by this helper.
    }
}
void paint_loading(State& s, HDC dc, const RECT& bounds) {
    FillRect(dc, &bounds, reinterpret_cast<HBRUSH>(GetStockObject(BLACK_BRUSH)));
    BITMAP bitmap{};
    if (s.image && GetObjectW(s.image, static_cast<int>(sizeof(bitmap)), &bitmap) == static_cast<int>(sizeof(bitmap))) {
        const auto r = g1::fit_image(bounds.right - bounds.left, bounds.bottom - bounds.top,
                                    bitmap.bmWidth, bitmap.bmHeight);
        HDC source = CreateCompatibleDC(dc);
        if (source) {
            HGDIOBJ old = SelectObject(source, s.image);
            SetStretchBltMode(dc, HALFTONE); SetBrushOrgEx(dc, 0, 0, nullptr);
            StretchBlt(dc, bounds.left + r.x, bounds.top + r.y, r.width, r.height,
                       source, 0, 0, bitmap.bmWidth, bitmap.bmHeight, SRCCOPY);
            SelectObject(source, old); DeleteDC(source);
        }
    }
    SetBkMode(dc, TRANSPARENT); SetTextColor(dc, RGB(225,230,238));
    const int scaled = static_cast<int>((bounds.bottom - bounds.top) / 36);
    HFONT font = CreateFontW(-(scaled < 18 ? 18 : scaled), 0, 0, 0, FW_BOLD,
        FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
        CLEARTYPE_QUALITY, DEFAULT_PITCH | FF_SWISS, L"Segoe UI");
    HGDIOBJ old_font = font ? SelectObject(dc, font) : nullptr;
    RECT caption = bounds; caption.top = bounds.top + (bounds.bottom - bounds.top) * 5 / 6;
    DrawTextW(dc, L"LOADING G1 AWAKENING", -1, &caption, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    if (font) { SelectObject(dc, old_font); DeleteObject(font); }
}
LRESULT CALLBACK window_proc(HWND window, UINT message, WPARAM wp, LPARAM lp) {
    State* s = state(window);
    switch (message) {
    case WM_NCCREATE: {
        auto* create = reinterpret_cast<CREATESTRUCTW*>(lp);
        SetWindowLongPtrW(window, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(create->lpCreateParams));
        return TRUE;
    }
    case WM_KEYDOWN:
        if (s && (wp == VK_ESCAPE || wp == VK_RETURN || wp == VK_SPACE)) {
            finish_video(*s); advance(*s); return 0;
        }
        break;
    case WM_LBUTTONDOWN:
    case video_ended:
        if (s) { finish_video(*s); advance(*s); } return 0;
    case video_error:
        if (s) { s->error = true; advance(*s); } return 0;
    case WM_TIMER:
        if (s && wp == poll_timer) advance(*s);
        return 0;
    case WM_SIZE:
        if (s && s->player) s->player->UpdateVideo();
        InvalidateRect(window, nullptr, FALSE); return 0;
    case WM_PAINT: {
        PAINTSTRUCT ps{}; HDC dc = BeginPaint(window, &ps);
        if (s && s->player && !s->ended) s->player->UpdateVideo();
        else if (s) { RECT r{}; GetClientRect(window, &r); paint_loading(*s, dc, r); }
        EndPaint(window, &ps); return 0;
    }
    case WM_ERASEBKGND: return 1;
    case WM_CLOSE:
        if (s) s->result = 5;
        DestroyWindow(window); return 0;
    case WM_DESTROY:
        KillTimer(window, poll_timer); PostQuitMessage(0); return 0;
    }
    return DefWindowProcW(window, message, wp, lp);
}
} // namespace

int WINAPI wWinMain(HINSTANCE instance, HINSTANCE, PWSTR, int) {
    int argc = 0;
    LPWSTR* argv = CommandLineToArgvW(GetCommandLineW(), &argc);
    if (!argv || argc != 4) { if (argv) LocalFree(argv); return 2; }
    const HRESULT com = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
    if (FAILED(com)) { LocalFree(argv); return 2; }
    SetProcessDPIAware();
    WNDCLASSW cls{};
    cls.lpfnWndProc = window_proc; cls.hInstance = instance;
    cls.lpszClassName = L"G1AwakeningOpeningV1";
    cls.hCursor = LoadCursorW(nullptr, IDC_ARROW);
    if (!RegisterClassW(&cls)) { CoUninitialize(); LocalFree(argv); return 2; }
    State s{};
    s.marker = argv[3]; s.started = GetTickCount64();
    s.image = static_cast<HBITMAP>(LoadImageW(nullptr, argv[2], IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION));
    s.window = CreateWindowExW(WS_EX_TOPMOST | WS_EX_TOOLWINDOW, cls.lpszClassName, L"G1 Awakening",
        WS_POPUP | WS_VISIBLE, 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN),
        nullptr, nullptr, instance, &s);
    Callback* callback = s.window ? new (std::nothrow) Callback(s.window) : nullptr;
    if (!s.window || !callback) s.result = 2;
    else {
        const HRESULT start = MFPCreateMediaPlayer(argv[1], TRUE, MFP_OPTION_NONE, callback, s.window, &s.player);
        if (FAILED(start)) s.result = 3;
        else if (!SetTimer(s.window, poll_timer, 50, nullptr)) s.result = 4;
        else {
            ShowWindow(s.window, SW_SHOW); SetForegroundWindow(s.window); SetFocus(s.window);
            MSG message{};
            BOOL result = 0;
            while ((result = GetMessageW(&message, nullptr, 0, 0)) > 0) {
                TranslateMessage(&message); DispatchMessageW(&message);
            }
            if (result == -1) s.result = 2;
        }
    }
    stop_video(s);
    if (callback) callback->Release();
    if (s.window && IsWindow(s.window)) DestroyWindow(s.window);
    if (s.image) DeleteObject(s.image);
    UnregisterClassW(cls.lpszClassName, instance);
    CoUninitialize(); LocalFree(argv);
    return s.result;
}
