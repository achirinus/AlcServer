#pragma once

#include <tchar.h>
#include <windows.h>
#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"
#include "AlcDefines.h"

struct AlcWindow
{
	HWND m_hWnd;
	u16* m_sWndClassName;
	HINSTANCE m_hInstance;
};

struct AlcApp;

namespace Alc
{
	bool CreateAlcWindow(AlcWindow* InWindow, const TCHAR* sTitle, int nXPos, int nYPos, int nWidth, int nHeight);
	void ShowWindow(AlcWindow* InWindow);
	bool UpdateWindow();
	void ClearWindow(AlcWindow* InWindow, const float* aClearColor);
	void DestroyWindow(AlcWindow* InWindow);
	void SwapChain();

	void ImGuiInit(AlcWindow* InWindow);
	void ImGuiNewFrame();
	void ImGuiRender();
	void ImGuiShowdown();
	void ShowImGui();
}


