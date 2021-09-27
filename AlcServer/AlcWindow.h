#pragma once

struct AlcWindow
{
	HWND m_hWnd;
	LPCTSTR m_sWndClassName;
	HINSTANCE m_hInstance;
};

namespace Alc
{
	bool CreateAlcWindow(AlcWindow* InWindow, const TCHAR* sTitle, int nXPos, int nYPos, int nWidth, int nHeight);
	void ShowWindow(AlcWindow* InWindow);
	bool UpdateWindow();
	void ClearWindow(AlcWindow* InWindow, const float* aClearColor);
	void DestroyWindow(AlcWindow* InWindow);
	void SwapChain();

	void ImGuiInit(AlcWindow* InWindow);
}


