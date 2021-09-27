#include <d3d11.h>
#include <windows.h>
#include <tchar.h>
#include "AlcWindow.h"
#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"
#include "AlcLog.h"
#include "AlcString.h"

static ID3D11Device* g_pDevice = NULL;
static ID3D11DeviceContext* g_pDeviceContext = NULL;
static IDXGISwapChain* g_pSwapChain = NULL;
static ID3D11RenderTargetView* g_pRenderTargetView = NULL;

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

void CreateRenderTarget()
{
	ID3D11Texture2D* pBackBuffer;
	g_pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
	g_pDevice->CreateRenderTargetView(pBackBuffer, NULL, &g_pRenderTargetView);
	pBackBuffer->Release();
}

void CleanupRenderTarget()
{
	if (g_pRenderTargetView)
	{
		g_pRenderTargetView->Release();
		g_pRenderTargetView = NULL;
	}
}


bool CreateDevice(HWND hWnd)
{
	DXGI_SWAP_CHAIN_DESC sd;
	ZeroMemory(&sd, sizeof(sd));

	sd.BufferCount = 2;
	sd.BufferDesc.Width = 0;
	sd.BufferDesc.Height = 0;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.OutputWindow = hWnd;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.Windowed = TRUE;
	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	UINT nCreateFlags = 0;

	D3D_FEATURE_LEVEL featureLevel;
	const D3D_FEATURE_LEVEL featureLevelArray[2] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0 };

	if (D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, nCreateFlags, featureLevelArray, 2, D3D11_SDK_VERSION, &sd, &g_pSwapChain, &g_pDevice, &featureLevel, &g_pDeviceContext) != S_OK)
	{
		return false;
	}

	CreateRenderTarget();
	return true;
}

void CleanupDevice()
{
	CleanupRenderTarget();
	if (g_pSwapChain)
	{
		g_pSwapChain->Release();
		g_pSwapChain = NULL;
	}
	if (g_pDeviceContext)
	{
		g_pDeviceContext->Release();
		g_pDeviceContext = NULL;
	}
	if (g_pDevice)
	{
		g_pDevice->Release();
		g_pDevice = NULL;
	}
}

LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam)) return true;

	switch (msg)
	{
	case WM_SIZE:
	{
		if (g_pDevice != NULL && wParam != SIZE_MINIMIZED)
		{
			CleanupRenderTarget();
			g_pSwapChain->ResizeBuffers(0, (UINT)LOWORD(lParam), (UINT)HIWORD(lParam), DXGI_FORMAT_UNKNOWN, 0);
			CreateRenderTarget();
		}
		return 0;
	}break;
	case WM_SYSCOMMAND:
	{
		if ((wParam & 0xfff0) == SC_KEYMENU) return 0; // Disable ALT application menu
	}break;
	case WM_DESTROY:
	{
		::PostQuitMessage(0);
	}break;
	}
	return ::DefWindowProc(hWnd, msg, wParam, lParam);
}

namespace Alc
{
	bool CreateAlcWindow(AlcWindow* InWindow, const TCHAR* sTitle, int nXPos, int nYPos, int nWidth, int nHeight)
	{
		if (!InWindow) return false;

		WNDCLASSEX wClass = { sizeof(WNDCLASSEX), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(NULL), NULL, NULL, NULL, NULL, _T("ImGuiExample"), NULL };
		::RegisterClassEx(&wClass);
		HWND hWnd = ::CreateWindow(wClass.lpszClassName, sTitle, WS_OVERLAPPEDWINDOW, nXPos, nYPos, nWidth, nHeight, NULL, NULL, wClass.hInstance, NULL);

		if (!CreateDevice(hWnd))
		{
			CleanupDevice();
			::UnregisterClass(wClass.lpszClassName, wClass.hInstance);
			LogError("Failed to create D3D Device");
			return false;
		}
		InWindow->m_hWnd = hWnd;
		InWindow->m_sWndClassName = Alc::CopyString(wClass.lpszClassName);
		InWindow->m_hInstance = wClass.hInstance;
		return true;
	}

	void ShowWindow(AlcWindow* InWindow)
	{
		if (!InWindow) return;
		::ShowWindow(InWindow->m_hWnd, SW_SHOWDEFAULT);
		::UpdateWindow(InWindow->m_hWnd);
	}

	bool UpdateWindow()
	{
		MSG wndMsg;
		while (::PeekMessage(&wndMsg, NULL, 0U, 0U, PM_REMOVE))
		{
			::TranslateMessage(&wndMsg);
			::DispatchMessage(&wndMsg);
			if (wndMsg.message == WM_QUIT) return false;
		}
		return true;
	}

	void ClearWindow(AlcWindow* InWindow, const float* aClearColor)
	{
		g_pDeviceContext->OMSetRenderTargets(1, &g_pRenderTargetView, NULL);
		g_pDeviceContext->ClearRenderTargetView(g_pRenderTargetView, aClearColor);
	}

	void DestroyWindow(AlcWindow* InWindow)
	{
		CleanupDevice();

		::DestroyWindow(InWindow->m_hWnd);
		::UnregisterClass(InWindow->m_sWndClassName, InWindow->m_hInstance);
	}

	void SwapChain()
	{
		g_pSwapChain->Present(1, 0);
	}

	void ImGuiInit(AlcWindow* InWindow)
	{
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO();

		ImGui::StyleColorsDark();
		ImGui_ImplWin32_Init(InWindow->m_hWnd);
		ImGui_ImplDX11_Init(g_pDevice, g_pDeviceContext);
	}

}