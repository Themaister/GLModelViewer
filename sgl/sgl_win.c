/* 
 * Copyright (c) 2011, Hans-Kristian Arntzen <maister@archlinux.us>
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#define SGL_EXPOSE_INTERNAL
#include "sgl.h"

#include <GL/gl.h>
#include <stdio.h>
#include <stdbool.h>

static HWND g_hwnd;
static HGLRC g_hrc;
static HDC g_hdc;

static WNDCLASSEXA g_wndclass;

static bool g_quit;
static bool g_inited;

static bool g_resized;
static unsigned g_resize_width;
static unsigned g_resize_height;

static bool g_fullscreen;

static bool g_ctx_modern;
static unsigned g_gl_major;
static unsigned g_gl_minor;

static void setup_pixel_format(HDC hdc)
{
   static PIXELFORMATDESCRIPTOR pfd = {
      .nSize = sizeof(PIXELFORMATDESCRIPTOR),
      .nVersion = 1,
      .dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,
      .iPixelType = PFD_TYPE_RGBA,
      .cColorBits = 32,
      .cDepthBits = 24,
      .cStencilBits = 8,
      .iLayerType = PFD_MAIN_PLANE,
   };

   int num_pixel_format = ChoosePixelFormat(hdc, &pfd);
   SetPixelFormat(hdc, num_pixel_format, &pfd);
}

static void create_gl_context(HWND hwnd)
{
   g_hdc = GetDC(hwnd);
   if (!g_hdc)
   {
      g_quit = true;
      return;
   }

   setup_pixel_format(g_hdc);
   g_hrc = wglCreateContext(g_hdc);
   if (!g_hrc)
   {
      g_quit = true;
      return;
   }

   if (wglMakeCurrent(g_hdc, g_hrc) != TRUE)
   {
      g_quit = true;
      wglDeleteContext(g_hrc);
   }

   // Take it to the limit! :D
   if (g_ctx_modern)
   {
      fprintf(stderr, "[SGL]: Creating modern OpenGL %u.%u context!\n", g_gl_major, g_gl_minor);
      typedef HGLRC (*ContextProc)(HDC, HGLRC, const int *);
      ContextProc proc = (ContextProc)sgl_get_proc_address("wglCreateContextAttribsARB");
      if (!proc)
      {
         fprintf(stderr, "[SGL]: Cannot find wglCreateContextAttribsARB. Forced to use legacy context.\n");
         return;
      }

#define WGL_CONTEXT_MAJOR_VERSION_ARB           0x2091
#define WGL_CONTEXT_MINOR_VERSION_ARB           0x2092
#define WGL_CONTEXT_FLAGS_ARB                   0x2094
#define WGL_CONTEXT_PROFILE_MASK_ARB            0x9126
#define WGL_CONTEXT_CORE_PROFILE_BIT_ARB        0x0001

      const int attribs[] = {
         WGL_CONTEXT_MAJOR_VERSION_ARB, g_gl_major,
         WGL_CONTEXT_MINOR_VERSION_ARB, g_gl_minor,
         WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
         0
      };

      HGLRC new_ctx = proc(g_hdc, NULL, attribs);
      wglMakeCurrent(NULL, NULL);
      wglDeleteContext(g_hrc);
      wglMakeCurrent(g_hdc, new_ctx);
      g_hrc = new_ctx;
   }

   int ver_major, ver_minor;
   glGetIntegerv(GL_MAJOR_VERSION, &ver_major);
   glGetIntegerv(GL_MINOR_VERSION, &ver_minor);
   fprintf(stderr, "[SGL]: Got OpenGL version: %u.%u\n", ver_major, ver_minor);
}


static LRESULT CALLBACK WndProc(HWND hwnd, UINT message,
      WPARAM wparam, LPARAM lparam)
{
   switch (message)
   {
      case WM_SYSCOMMAND:
         // Prevent screensavers, etc, while running :)
         switch (wparam)
         {
            case SC_SCREENSAVE:
            case SC_MONITORPOWER:
               return 0;
         }
         break;

      case WM_CREATE:
         create_gl_context(hwnd);
         return 0;

      case WM_CLOSE:
      case WM_DESTROY:
         if (!g_quit)
         {
            wglMakeCurrent(g_hdc, NULL);
            wglDeleteContext(g_hrc);
            g_quit = true;
         }
         return 0;

      case WM_QUIT:
         g_quit = true;
         return 0;

      case WM_SIZE:
         // Do not send resize message if we minimize ...
         if (wparam != SIZE_MAXHIDE && wparam != SIZE_MINIMIZED)
         {
            g_resize_width = LOWORD(lparam);
            g_resize_height = HIWORD(lparam);
            g_resized = true;
         }
         return 0;
   }

   return DefWindowProc(hwnd, message, wparam, lparam);
}

static bool set_fullscreen(unsigned width, unsigned height)
{
   DEVMODE devmode = {
      .dmSize = sizeof(DEVMODE),
      .dmPelsWidth = width,
      .dmPelsHeight = height,
      .dmFields = DM_PELSWIDTH | DM_PELSHEIGHT,
   };

   return ChangeDisplaySettings(&devmode, CDS_FULLSCREEN) == DISP_CHANGE_SUCCESSFUL;
}

int sgl_init(const struct sgl_context_options *opts)
{
   if (g_inited)
      return SGL_ERROR;

   g_quit = false;
   g_resized = false;

   g_ctx_modern = opts->context.style == SGL_CONTEXT_MODERN;
   g_gl_major = opts->context.major;
   g_gl_minor = opts->context.minor;

   memset(&g_wndclass, 0, sizeof(g_wndclass));
   g_wndclass.cbSize = sizeof(g_wndclass);
   g_wndclass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
   g_wndclass.lpfnWndProc = WndProc;
   g_wndclass.hInstance = GetModuleHandle(NULL);
   g_wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
   g_wndclass.lpszClassName = "SGL Window";

   if (!RegisterClassExA(&g_wndclass))
      return SGL_ERROR;

   unsigned width = opts->res.width;
   unsigned height = opts->res.height;
   DWORD style = 0;
   RECT rect;
   GetClientRect(GetDesktopWindow(), &rect);

   switch (opts->screen_type)
   {
      case SGL_SCREEN_WINDOWED:
      {
         RECT rect = {
            .right = width,
            .bottom = height,
         };
         style = WS_OVERLAPPEDWINDOW;
         AdjustWindowRect(&rect, style, FALSE);
         width = rect.right - rect.left;
         height = rect.bottom - rect.top;
         break;
      }

      case SGL_SCREEN_FULLSCREEN:
         g_fullscreen = true;
         style = WS_POPUP | WS_VISIBLE;

         // Recover, just use windowed fullscreen instead.
         if (!set_fullscreen(width, height))
         {
            g_fullscreen = false;
            width = rect.right - rect.left;
            height = rect.bottom - rect.top;
         }
         break;

      case SGL_SCREEN_WINDOWED_FULLSCREEN:
         style = WS_POPUP | WS_VISIBLE;
         width = rect.right - rect.left;
         height = rect.bottom - rect.top;
         break;

      default:
         UnregisterClassA("SGL Window", GetModuleHandle(NULL));
         return false;
   }

   g_hwnd = CreateWindowExA(0, "SGL Window", opts->title ? opts->title : "SGL Window",
         style,
         CW_USEDEFAULT, CW_USEDEFAULT, width, height,
         NULL, NULL, NULL, NULL);

   if (!g_hwnd)
   {
      UnregisterClassA("SGL Window", GetModuleHandle(NULL));
      return SGL_ERROR;
   }

   if (opts->screen_type == SGL_SCREEN_WINDOWED)
   {
      ShowWindow(g_hwnd, SW_RESTORE);
      UpdateWindow(g_hwnd);
      SetForegroundWindow(g_hwnd);
      SetFocus(g_hwnd);
   }
   else
      ShowCursor(FALSE);

   sgl_set_swap_interval(opts->swap_interval);

   g_inited = true;

   return SGL_OK;
}

void sgl_deinit(void)
{
   g_inited = false;

   DestroyWindow(g_hwnd);
   UnregisterClassA("SGL Window", GetModuleHandle(NULL));

   if (g_fullscreen)
      ChangeDisplaySettings(NULL, 0);
   g_fullscreen = false;
}

void sgl_set_window_title(const char *title)
{
   SetWindowTextA(g_hwnd, title);
}

int sgl_check_resize(unsigned *width, unsigned *height)
{
   if (g_resized)
   {
      *width = g_resize_width;
      *height = g_resize_height;
      g_resized = false;
      return SGL_TRUE;
   }
   else
      return SGL_FALSE;
}

void sgl_set_swap_interval(unsigned interval)
{
   static BOOL (*swap_interval)(int) = NULL;
   if (!swap_interval)
      swap_interval = (BOOL (*)(int))sgl_get_proc_address("wglSwapIntervalEXT");

   if (swap_interval)
      swap_interval(interval);
}

void sgl_swap_buffers(void)
{
   SwapBuffers(g_hdc);

   MSG msg;
   while (PeekMessage(&msg, g_hwnd, 0, 0, PM_REMOVE))
   {
      TranslateMessage(&msg);
      DispatchMessage(&msg);
   }
}

int sgl_has_focus(void)
{
   return GetFocus() == g_hwnd;
}

int sgl_is_alive(void)
{
   return !g_quit;
}

sgl_function_t sgl_get_proc_address(const char *sym)
{
   return (sgl_function_t)wglGetProcAddress(sym);
}

void sgl_get_handles(struct sgl_handles *handles)
{
   handles->hwnd = g_hwnd;
   handles->hglrc = g_hrc;
   handles->hdc = g_hdc;
}

void sgl_set_input_callbacks(const struct sgl_input_callbacks *cbs)
{
   (void)cbs;
}

void sgl_set_mouse_mode(int capture, int relative, int visible)
{
   (void)capture;
   (void)relative;
   (void)visible;
}

