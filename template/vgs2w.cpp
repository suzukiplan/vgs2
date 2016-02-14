/* (C)2015, SUZUKI PLAN.
 *----------------------------------------------------------------------------
 * Description: VGS mk-II SR - emulator
 *    Platform: Windows
 *      Author: Yoji Suzuki (SUZUKI PLAN)
 *----------------------------------------------------------------------------
 */
#include <Windows.h>
#include <process.h>
#include <io.h>
#include <d3d9.h>
#include <d2d1.h>
#include "vgs2.h"
#include "vgsdec.h"
#include "vgsspu.h"

void* _psg;

/*
 *----------------------------------------------------------------------------
 * macros
 *----------------------------------------------------------------------------
 */
#define VXSIZE 320
#define VYSIZE 400

/*
 *----------------------------------------------------------------------------
 * file static variables
 *----------------------------------------------------------------------------
 */
static LPDIRECT3D9 _lpD3D;
static LPDIRECT3DDEVICE9 _lpDev;
static LPDIRECT3DSURFACE9 _lpBuf;
static BOOL isHEL = FALSE;

static ID2D1Factory* _lpD2Fact;
static ID2D1HwndRenderTarget* _lpD2RT;
static ID2D1Bitmap* _lpD2Buf;
static LPVOID _lpD2BufPtr;
static UINT _iD2BufPitch;
static ID2D1BitmapBrush* _lpD2Brush;
static D2D1_RECT_F _rtD2RectF;
static D2D1_RECT_U _rtD2RectU;

static RECT _rect = {0, 0, VXSIZE, VYSIZE};
static const RECT _rectConst = {0, 0, VXSIZE, VYSIZE};
static int need_restore = 0;
static HWND hwndMain;
static int fs_nCmdShow;
static int fs_width, fs_height;

static BOOL _useD2D = FALSE;
static LPDIRECT3D9(WINAPI* fpDirect3DCreate9)(UINT);
static HRESULT(WINAPI* fpD2D1CreateFactory)(D2D1_FACTORY_TYPE, REFIID, D2D1_FACTORY_OPTIONS*, ID2D1Factory**);

/*
 *----------------------------------------------------------------------------
 * Function prototypes
 *----------------------------------------------------------------------------
 */

void putlog(const char*, int, const char*, ...);
static int SetWinCenter(HWND hWnd);
static LRESULT CALLBACK MainWndProc(HWND, UINT, UINT, LONG);
static int loadrom(const char* fname);

static int d3d_init(HWND, const RECT*);
static int d3d_init9(HWND, const RECT*);
static int d2d_init(HWND hWnd, const RECT*);
static void term();
static void vtrans(int, int*);

struct binrec {
    char name[16];
    int size;
    char* data;
};

/*
 *----------------------------------------------------------------------------
 * entry point
 *----------------------------------------------------------------------------
 */
extern "C" int __stdcall WinMain(HINSTANCE hIns, HINSTANCE hPIns, LPSTR lpCmd, int nCmdShow)
{
    HWND hwnd;
    WNDCLASS wndclass;
    MSG msg;
    D3DLOCKED_RECT lrect;
    DWORD tCount;
    DWORD tCount2;
    HRESULT res;
    int i;
    int cn, pn, bn;
    char path[256];
    void* spu;

    CreateDirectory("DATA", NULL);
    SetCurrentDirectory("DATA");
    DeleteFile("LOG.TXT");
    putlog(NULL, 0, "Executes \"%s\" on Windows", APPNAME);

    /* initialize vgs-bgm-decoder */
    _psg = vgsdec_create_context();
    if (NULL == _psg) {
        putlog(__FILE__, __LINE__, "vgs-bgm-decoder error.");
        MessageBox(NULL, "Could not initialize vgs-bgm-decoder.", "ERROR", MB_OK | MB_ICONERROR);
        return FALSE;
    }

    /* load romdata */
    if (loadrom("..\\ROMDATA.BIN")) {
        putlog(__FILE__, __LINE__, "loadrom error.");
        MessageBox(NULL, "Could not load rom data.", "ERROR", MB_OK | MB_ICONERROR);
        return FALSE;
    }

    /* extract to the slots */
    cn = 0;
    pn = 0;
    bn = 0;
    for (i = 0; i < 256; i++) {
        sprintf(path, "GSLOT%03d.CHR", i);
        if (0 == gload(i, path)) cn++;
        sprintf(path, "ESLOT%03d.PCM", i);
        if (0 == eload(i, path)) pn++;
        sprintf(path, "BSLOT%03d.BGM", i);
        if (0 == bload(i, path)) bn++;
    }
    putlog(__FILE__, __LINE__, "Data has extracted. (CHR=%d, PCM=%d, BGM=%d)", cn, pn, bn);

    /* disable IME */
    if (!ImmDisableIME(GetCurrentThreadId())) {
        putlog(__FILE__, __LINE__, "ImmDisableIME error. (%d)", GetLastError());
    }

    /* regist window class */
    memset(&wndclass, 0, sizeof(wndclass));
    wndclass.hCursor = LoadCursor(NULL, IDC_HAND);
    wndclass.hIcon = LoadIcon(hIns, MAKEINTRESOURCE(1000));
    wndclass.lpszMenuName = NULL;
    wndclass.lpszClassName = APPNAME;
    wndclass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    wndclass.hInstance = hIns;
    wndclass.style = CS_BYTEALIGNCLIENT | CS_VREDRAW | CS_HREDRAW;
    wndclass.lpfnWndProc = (WNDPROC)MainWndProc;
    wndclass.cbClsExtra = 0;
    wndclass.cbWndExtra = 0;
    if (!RegisterClass(&wndclass)) {
        putlog(__FILE__, __LINE__, "RegisterClass error. (%d)", GetLastError());
        return FALSE;
    }

    /* create window */
    AdjustWindowRect(&_rect, WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX, FALSE);
    fs_width = _rect.right - _rect.left;
    fs_height = _rect.bottom - _rect.top;
    hwnd = CreateWindowEx(0, APPNAME, APPNAME, WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX, CW_USEDEFAULT, CW_USEDEFAULT, fs_width, fs_height,
                          (HWND)NULL, (HMENU)NULL, hIns, (LPSTR)NULL);
    if (NULL == hwnd) {
        putlog(__FILE__, __LINE__, "CreateWindowEx error. (%d)", GetLastError());
        return FALSE;
    }
    hwndMain = hwnd;

    /* visible window */
    ValidateRect(hwnd, 0);
    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);
    fs_nCmdShow = nCmdShow;

    /* initialize graphics */
    if (d3d_init(hwnd, &_rectConst)) {
        putlog(__FILE__, __LINE__, "d3d_init error.");
        term();
        return FALSE;
    }

    /* initialize sounds */
    spu = vgsspu_init(sndbuf);
    if (NULL == spu) {
        putlog(__FILE__, __LINE__, "init_sound error.");
        term();
        return FALSE;
    }

    /* Initialize AP */
    if (vgs2_init()) {
        putlog(__FILE__, __LINE__, "vge_init error.");
        term();
        vgsspu_end(spu);
        vgsdec_release_context(_psg);
        return FALSE;
    }

    tCount = timeGetTime();

    /* Main loop */
    while (TRUE) {
        if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE)) {
            if (msg.message == WM_QUIT) {
                break;
            }
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        if (need_restore) {
            Sleep(50);
            continue;
        }
        /* start drawing */
        if (_useD2D) {
            _lpD2RT->BeginDraw();
        } else {
            if (FAILED(res = _lpDev->BeginScene())) {
                putlog(__FILE__, __LINE__, "BeginScene failed: %x", res);
                need_restore = 1;
                continue;
            }
        }
        /* call AP procedure */
        if (_pause) {
            if (vgs2_pause()) {
                break;
            }
        } else {
            if (vgs2_loop()) {
                break;
            }
        }
        if (_useD2D) {
            vtrans(_iD2BufPitch, (int*)_lpD2BufPtr);
            res = _lpD2Buf->CopyFromMemory(&_rtD2RectU, _lpD2BufPtr, _iD2BufPitch);
            if (FAILED(res)) {
                putlog(__FILE__, __LINE__, "CopyFromMemory failed: %x", res);
                break;
            }
            _lpD2Brush->SetBitmap(_lpD2Buf);
            _lpD2RT->FillRectangle(&_rtD2RectF, _lpD2Brush);
            if (FAILED(res = _lpD2RT->EndDraw())) {
                putlog(__FILE__, __LINE__, "EndDraw failed: %x", res);
                need_restore = 1;
                continue;
            }
        } else {
            if (FAILED(res = _lpBuf->LockRect(&lrect, (LPRECT)&_rectConst, D3DLOCK_DISCARD))) {
                putlog(__FILE__, __LINE__, "LockRect failed: %x", res);
                need_restore = 1;
                continue;
            }
            vtrans(lrect.Pitch, (int*)lrect.pBits);
            if (FAILED(res = _lpBuf->UnlockRect())) {
                putlog(__FILE__, __LINE__, "UnlockRect failed: %x", res);
                need_restore = 1;
                continue;
            }
            if (FAILED(res = _lpDev->EndScene())) {
                putlog(__FILE__, __LINE__, "EndScene failed: %x", res);
                need_restore = 1;
                continue;
            }
            res = _lpDev->Present(NULL, NULL, NULL, NULL);
            if (FAILED(res)) {
                putlog(__FILE__, __LINE__, "Present failed: %x", res);
                need_restore = 1;
                continue;
            }
        }
        if (_touch.s) {
            _touch.s++;
        }
        /* wait (HEL only) */
        if (isHEL) {
            tCount2 = timeGetTime();
            if (tCount2 - tCount < 16) {
                Sleep(16 - (tCount2 - tCount));
            }
            tCount = timeGetTime();
        }
    }

    vgs2_term();

    term();
    vgsspu_end(spu);
    vgsdec_release_context(_psg);
    putlog(__FILE__, __LINE__, "Exit program.");
    return TRUE;
}

/*
 *----------------------------------------------------------------------------
 * output a log record
 *----------------------------------------------------------------------------
 */
void putlog(const char* fn, int ln, const char* msg, ...)
{
    FILE* fp;
    va_list args;
    time_t t1;
    struct tm* t2;

    if (NULL == (fp = fopen("LOG.TXT", "a"))) {
        return;
    }
    t1 = time(NULL);
    t2 = localtime(&t1);
    fprintf(fp, "%04d/%02d/%02d %02d:%02d:%02d ", t2->tm_year + 1900, t2->tm_mon + 1, t2->tm_mday, t2->tm_hour, t2->tm_min, t2->tm_sec);
    va_start(args, msg);
    vfprintf(fp, msg, args);
    va_end(args);
    if (fn)
        fprintf(fp, " <%s:%d>\n", fn, ln);
    else
        fprintf(fp, "\n");
    fclose(fp);
}

/*
 *----------------------------------------------------------------------------
 * get center position
 *----------------------------------------------------------------------------
 */
static int SetWinCenter(HWND hWnd)
{
    HWND hDeskWnd;
    RECT deskrc, rc;
    int x, y;

    hDeskWnd = GetDesktopWindow();
    GetWindowRect(hDeskWnd, (LPRECT)&deskrc);
    GetWindowRect(hWnd, (LPRECT)&rc);
    x = (deskrc.right - (rc.right - rc.left)) / 2;
    y = (deskrc.bottom - (rc.bottom - rc.top)) / 2;
    SetWindowPos(hWnd, HWND_TOP, x, y, (rc.right - rc.left), (rc.bottom - rc.top), SWP_SHOWWINDOW);
    return 0;
}

/*
 *----------------------------------------------------------------------------
 * window procedure
 *----------------------------------------------------------------------------
 */
static LRESULT CALLBACK MainWndProc(HWND hWnd, UINT msg, UINT wParam, LONG lParam)
{
    RECT r;
    TRACKMOUSEEVENT tme;
    int mx;
    int my;

    switch (msg) {
        case WM_SYSCOMMAND:
            switch (wParam) {
                case SC_SCREENSAVE:
                case SC_MONITORPOWER:
                    return 0L;
            }
            return DefWindowProc(hWnd, msg, wParam, lParam);
        case WM_KILLFOCUS:
            vgs2_setPause(1);
            return DefWindowProc(hWnd, msg, wParam, lParam);
        case WM_SETFOCUS:
            return DefWindowProc(hWnd, msg, wParam, lParam);
        case WM_KEYUP:
            if (VK_ESCAPE == wParam) {
                vgs2_setPause(1);
            }
            return DefWindowProc(hWnd, msg, wParam, lParam);
        case WM_SIZE:
            if (need_restore) {
                gterm();
                if (d3d_init(hwndMain, &_rectConst)) {
                    putlog(__FILE__, __LINE__, "Failed restoring graphics.");
                    PostQuitMessage(0);
                } else {
                    putlog(__FILE__, __LINE__, "Restored graphics.");
                    ShowWindow(hwndMain, fs_nCmdShow);
                    UpdateWindow(hwndMain);
                    need_restore = 0;
                    SetWindowPos(hwndMain, HWND_TOP, 0, 0, fs_width, fs_height, 0);
                }
            }
            break;
        case WM_DESTROY:
            PostQuitMessage(0);
            break;
        case WM_MOUSEMOVE:
            mx = LOWORD(lParam);
            my = HIWORD(lParam);
            if (GetClientRect(hWnd, &r)) {
                if (r.right && r.bottom) {
                    _touch.px = _touch.cx;
                    _touch.py = _touch.cy;
                    _touch.cx = (mx * (XSIZE * 100) / r.right) / 100;
                    _touch.cy = (my * (YSIZE * 100) / r.bottom) / 100;
                }
            }
            if (_touch.s && _touch.px && _touch.py) {
                _touch.dx = _touch.cx - _touch.px;
                _touch.dy = _touch.cy - _touch.py;
            } else {
                _touch.dx = 0;
                _touch.dy = 0;
            }
            break;
        case WM_MOUSELEAVE:
            memset(&_touch, 0, sizeof(_touch));
            break;
        case WM_LBUTTONDOWN:
            tme.cbSize = sizeof(tme);
            tme.dwFlags = TME_LEAVE;
            tme.hwndTrack = hWnd;
            TrackMouseEvent(&tme);
            InvalidateRect(hWnd, NULL, FALSE);
            UpdateWindow(hWnd);
            _touch.s = 1;
            break;
        case WM_RBUTTONUP:
            vgs2_setPause(1);
            break;
        case WM_LBUTTONUP:
            _touch.s = 0;
            break;
        default:
            return DefWindowProc(hWnd, msg, wParam, lParam);
    }
    return 0L;
}

/*
 *----------------------------------------------------------------------------
 * initialize graphics Direct3D
 *----------------------------------------------------------------------------
 */
static int d3d_init(HWND hWnd, const RECT* rect)
{
    if (_useD2D) {
        return d2d_init(hWnd, rect);
    }
    return d3d_init9(hWnd, rect);
}

/*
 *----------------------------------------------------------------------------
 * initialize Direct2D
 *----------------------------------------------------------------------------
 */
static int d2d_init(HWND hWnd, const RECT* rect)
{
    static BOOL loaded = FALSE;
    static HMODULE hm3;
    static HMODULE hm2;
    HRESULT hres;
    int width;
    int height;

    putlog(__FILE__, __LINE__, "Initializing the Direct2D End-User runtime...");

    _rtD2RectF.left = (FLOAT)rect->left;
    _rtD2RectF.right = (FLOAT)rect->right;
    _rtD2RectF.top = (FLOAT)rect->top;
    _rtD2RectF.bottom = (FLOAT)rect->bottom;

    _rtD2RectU.left = (UINT)rect->left;
    _rtD2RectU.right = (UINT)rect->right;
    _rtD2RectU.top = (UINT)rect->top;
    _rtD2RectU.bottom = (UINT)rect->bottom;

    width = rect->right - rect->left;
    height = rect->bottom - rect->top;

    if (NULL == hm2) {
        hm2 = LoadLibrary("d2d1.dll");
        if (NULL == hm2) {
            putlog(__FILE__, __LINE__, "LoadLibrary(d2d1.dll) failed: %d", GetLastError());
            MessageBox(hWnd, "Direct2D End-User Runtime was not installed.", "ERROR", MB_OK | MB_ICONERROR);
            return -1;
        }
        putlog(__FILE__, __LINE__, "Loaded d2d1.dll");
        fpD2D1CreateFactory = (HRESULT(WINAPI*)(D2D1_FACTORY_TYPE, REFIID, D2D1_FACTORY_OPTIONS*, ID2D1Factory**))GetProcAddress(hm2, "D2D1CreateFactory");
        if (NULL == fpD2D1CreateFactory) {
            putlog(__FILE__, __LINE__, "GetProcAddress(D2D1CreateFactory) failed: %d", GetLastError());
            MessageBox(hWnd, "Direct2D End-User Runtime was not installed.", "ERROR", MB_OK | MB_ICONERROR);
            return -1;
        }
    }

    hres = fpD2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, __uuidof(ID2D1Factory), NULL, &_lpD2Fact);
    if (FAILED(hres)) {
        putlog(__FILE__, __LINE__, "D2D1CreateFactory failed: 0x%08x", hres);
        MessageBox(hWnd, "Could not initialized Direct2D.", "ERROR", MB_OK | MB_ICONERROR);
        return -1;
    }

    const D2D1_SIZE_U size = D2D1::SizeU(width, height);
    const D2D1_PIXEL_FORMAT pixelFormat = D2D1::PixelFormat(DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_IGNORE);
    D2D1_RENDER_TARGET_PROPERTIES props = D2D1::RenderTargetProperties();
    props.pixelFormat = pixelFormat;
    hres = _lpD2Fact->CreateHwndRenderTarget(props, D2D1::HwndRenderTargetProperties(hWnd, size), &_lpD2RT);
    if (FAILED(hres)) {
        putlog(__FILE__, __LINE__, "CreateHwndRenderTarget failed: 0x%08x", hres);
        MessageBox(hWnd, "Could not initialized Direct2D.", "ERROR", MB_OK | MB_ICONERROR);
        return -1;
    }

    _iD2BufPitch = width * 4;
    if (NULL == _lpD2BufPtr) {
        _lpD2BufPtr = malloc(_iD2BufPitch * height);
        if (NULL == _lpD2BufPtr) {
            putlog(__FILE__, __LINE__, "malloc failed: %d", GetLastError());
            MessageBox(hWnd, "No memory!", "ERROR", MB_OK | MB_ICONERROR);
            return -1;
        }
        memset(_lpD2BufPtr, 0, _iD2BufPitch * height);
    }

    D2D1_BITMAP_PROPERTIES bprop = {pixelFormat, 96.0f, 96.0f};
    _lpD2Fact->GetDesktopDpi(&bprop.dpiX, &bprop.dpiY);

    hres = _lpD2RT->CreateBitmap(size, _lpD2BufPtr, _iD2BufPitch, &bprop, &_lpD2Buf);
    if (FAILED(hres)) {
        putlog(__FILE__, __LINE__, "CreateBitmap failed: 0x%08x", hres);
        MessageBox(hWnd, "Could not initialized Direct2D.", "ERROR", MB_OK | MB_ICONERROR);
        return -1;
    }

    hres = _lpD2RT->CreateBitmapBrush(_lpD2Buf, &_lpD2Brush);
    if (FAILED(hres)) {
        putlog(__FILE__, __LINE__, "CreateBitmapBrush failed: 0x%08x", hres);
        MessageBox(hWnd, "Could not initialized Direct2D.", "ERROR", MB_OK | MB_ICONERROR);
        return -1;
    }

    return 0;
}

/*
 *----------------------------------------------------------------------------
 * initialize Direct3D
 *----------------------------------------------------------------------------
 */
static int d3d_init9(HWND hWnd, const RECT* rect)
{
    D3DDISPLAYMODE dm;
    D3DPRESENT_PARAMETERS dprm;
    HRESULT res;
    static HMODULE hm;

    putlog(__FILE__, __LINE__, "Initializing the DirectX9 End-User runtime...");

    if (NULL == hm) {
        hm = LoadLibrary("d3d9.dll");
        if (NULL == hm) {
            putlog(__FILE__, __LINE__, "LoadLibrary(d3d9.dll) failed: %d", GetLastError());
            MessageBox(hWnd, "DirectX9 End-User Runtime was not installed.", "ERROR", MB_OK | MB_ICONERROR);
            return -1;
        }
        putlog(__FILE__, __LINE__, "Loaded d3d9.dll");
        fpDirect3DCreate9 = (LPDIRECT3D9(WINAPI*)(UINT))GetProcAddress(hm, "Direct3DCreate9");
        if (NULL == fpDirect3DCreate9) {
            putlog(__FILE__, __LINE__, "GetProcAddress(Direct3DCreate9) failed: %d", GetLastError());
            MessageBox(hWnd, "DirectX9 End-User Runtime was not installed.", "ERROR", MB_OK | MB_ICONERROR);
            return -1;
        }
    }

    if (NULL == (_lpD3D = fpDirect3DCreate9(32))) {
        putlog(__FILE__, __LINE__, "Direct3DCreate9 failed: %d", GetLastError());
        return -1;
    }

    _lpD3D->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &dm);

    memset(&dprm, 0, sizeof(dprm));
    dprm.Windowed = TRUE;
    dprm.FullScreen_RefreshRateInHz = 0;
    dprm.BackBufferWidth = rect->right - rect->left;
    dprm.BackBufferHeight = rect->bottom - rect->top;
    dprm.BackBufferCount = 1;
    dprm.SwapEffect = D3DSWAPEFFECT_COPY;
    dprm.BackBufferFormat = dm.Format;
    dprm.EnableAutoDepthStencil = TRUE;
    dprm.AutoDepthStencilFormat = D3DFMT_D16;
    dprm.PresentationInterval = D3DPRESENT_INTERVAL_DEFAULT;
    dprm.Flags = D3DPRESENTFLAG_LOCKABLE_BACKBUFFER;

    do {
        res = _lpD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &dprm, &_lpDev);
        if (!FAILED(res)) {
            putlog(__FILE__, __LINE__, "Using: HAL+SWV");
            break;
        }
        putlog(__FILE__, __LINE__, "CreateDevice(HAL+SWV) failed: %x", res);
        res = _lpD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_REF, hWnd, D3DCREATE_HARDWARE_VERTEXPROCESSING, &dprm, &_lpDev);
        if (!FAILED(res)) {
            putlog(__FILE__, __LINE__, "Using: REF+SWV");
            isHEL = TRUE;
            break;
        }
        putlog(__FILE__, __LINE__, "CreateDevice(REF+SWV) failed: %x", res);
    } while (0);

    if (FAILED(res)) {
        return -1;
    }

    res = _lpDev->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &_lpBuf);
    if (FAILED(res)) {
        putlog(__FILE__, __LINE__, "GetBackBuffer failed: %x", res);
        return -1;
    }

    return 0;
}

/*
 *----------------------------------------------------------------------------
 * release DirectGraphic objects
 *----------------------------------------------------------------------------
 */
static void term()
{
    if (_useD2D) {
        /* release Direct2D */
        if (_lpD2Buf) {
            _lpD2Buf->Release();
            _lpD2Buf = NULL;
        }
        if (_lpD2RT) {
            _lpD2RT->Release();
            _lpD2RT = NULL;
        }
        if (_lpD2Fact) {
            _lpD2Fact->Release();
            _lpD2Fact = NULL;
        }
    } else {
        /* release Direct3D(9) */
        if (_lpDev) {
            _lpDev->Release();
            _lpDev = NULL;
        }
        if (_lpD3D) {
            _lpD3D->Release();
            _lpD3D = NULL;
        }
    }
}

/*
 *----------------------------------------------------------------------------
 * VRAM to surface
 *----------------------------------------------------------------------------
 */
static void vtrans(int pitch, int* ptr)
{
    register short vx, vy;
    register int p;
    pitch >>= 2;

    p = 0;
    if (_interlace) {
        for (vy = 0; vy < VYSIZE; vy += 2) {
            for (vx = 0; vx < VXSIZE; vx += 2) {
                if (_vram.sp[p]) {
                    ptr[vy * pitch + vx] = _PAL[_vram.sp[p]];
                    ptr[vy * pitch + vx + 1] = _PAL[_vram.sp[p]];
                    ptr[(vy + 1) * pitch + vx] = 0;
                    ptr[(vy + 1) * pitch + vx + 1] = 0;
                    _vram.sp[p] = 0;
                } else {
                    ptr[vy * pitch + vx] = _PAL[_vram.bg[p]];
                    ptr[vy * pitch + vx + 1] = _PAL[_vram.bg[p]];
                    ptr[(vy + 1) * pitch + vx] = 0;
                    ptr[(vy + 1) * pitch + vx + 1] = 0;
                }
                p++;
            }
        }
    } else {
        for (vy = 0; vy < VYSIZE; vy += 2) {
            for (vx = 0; vx < VXSIZE; vx += 2) {
                if (_vram.sp[p]) {
                    ptr[vy * pitch + vx] = _PAL[_vram.sp[p]];
                    ptr[vy * pitch + vx + 1] = _PAL[_vram.sp[p]];
                    ptr[(vy + 1) * pitch + vx] = _PAL[_vram.sp[p]];
                    ptr[(vy + 1) * pitch + vx + 1] = _PAL[_vram.sp[p]];
                    _vram.sp[p] = 0;
                } else {
                    ptr[vy * pitch + vx] = _PAL[_vram.bg[p]];
                    ptr[vy * pitch + vx + 1] = _PAL[_vram.bg[p]];
                    ptr[(vy + 1) * pitch + vx] = _PAL[_vram.bg[p]];
                    ptr[(vy + 1) * pitch + vx + 1] = _PAL[_vram.bg[p]];
                }
                p++;
            }
        }
    }
}

/*
 *----------------------------------------------------------------------------
 * Load ROMDATA.BIN
 *----------------------------------------------------------------------------
 */
int loadrom(const char* fname)
{
    FILE* fp;
    unsigned char s[4];
    int i;
    int j;

    /* read size */
    if (NULL == (fp = fopen(fname, "rb"))) {
        return -1;
    }
    if (4 != fread(&BN, 1, sizeof(BN), fp)) {
        fclose(fp);
        return -1;
    }

    /* allocate buffer */
    if (BR) free(BR);
    if (NULL == (BR = (struct _BINREC*)malloc(BN * sizeof(struct _BINREC)))) {
        fclose(fp);
        return -1;
    }
    memset(BR, 0, BN * sizeof(struct binrec));

    /* load name data */
    for (i = 0; i < BN; i++) {
        if (16 != fread(BR[i].name, 1, 16, fp)) {
            fclose(fp);
            return -1;
        }
        for (j = 0; '\0' != BR[i].name[j]; j++) {
            if (15 == j) {
                fclose(fp);
                return -1;
            }
            BR[i].name[j] ^= 0xAA;
        }
    }

    /* load main data */
    for (i = 0; i < BN; i++) {
        if (4 != fread(s, 1, 4, fp)) {
            fclose(fp);
            return -1;
        }
        BR[i].size = s[0];
        BR[i].size <<= 8;
        BR[i].size |= s[1];
        BR[i].size <<= 8;
        BR[i].size |= s[2];
        BR[i].size <<= 8;
        BR[i].size |= s[3];
        if (NULL == (BR[i].data = (char*)malloc(BR[i].size))) {
            fclose(fp);
            return -1;
        }
        if (BR[i].size != (int)fread(BR[i].data, 1, BR[i].size, fp)) {
            fclose(fp);
            return -1;
        }
    }

    fclose(fp);
    return 0;
}

/*
 *----------------------------------------------------------------------------
 * make pallet table (n/a)
 *----------------------------------------------------------------------------
 */
void make_pallet()
{
}

/*
 *----------------------------------------------------------------------------
 * open file
 *----------------------------------------------------------------------------
 */
FILE* vgs2_fopen(const char* n, const char* p)
{
    return fopen(n, p);
}

/*
 *----------------------------------------------------------------------------
 * show ads
 *----------------------------------------------------------------------------
 */
void vgs2_showAds()
{
}

/*
 *----------------------------------------------------------------------------
 * delete ads
 *----------------------------------------------------------------------------
 */
void vgs2_deleteAds()
{
}
