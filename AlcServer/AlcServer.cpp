// AlcServer.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include "AlcSql.h"
#include "AlcLog.h"
#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"
#include <d3d11.h>
#include <windows.h>
#include <tchar.h>

void TestDB();
void TestImGui();

static ID3D11Device* g_pDevice = NULL;
static ID3D11DeviceContext* g_pDeviceContext = NULL;
static IDXGISwapChain* g_pSwapChain = NULL;
static ID3D11RenderTargetView* g_pRenderTargetView = NULL;

bool CreateDevice(HWND hWnd);
void CleanupDevice();
void CreateRenderTarget();
void CleanupRenderTarget();

LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

int main()
{
	TestImGui();

    return 0;
}

void TestImGui()
{
	WNDCLASSEX wClass = { sizeof(WNDCLASSEX), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(NULL), NULL, NULL, NULL, NULL, _T("ImGuiExample"), NULL };
	::RegisterClassEx(&wClass);
	HWND hWnd = ::CreateWindow(wClass.lpszClassName, _T("Imgui dx11"), WS_OVERLAPPEDWINDOW, 100, 100, 1280, 800, NULL, NULL, wClass.hInstance, NULL);

	if (!CreateDevice(hWnd))
	{
		CleanupDevice();
		::UnregisterClass(wClass.lpszClassName, wClass.hInstance);
		LogError("Failed to create D3D Device");
		return;
	}

	::ShowWindow(hWnd, SW_SHOWDEFAULT);
	::UpdateWindow(hWnd);

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();

	ImGui::StyleColorsDark();
	ImGui_ImplWin32_Init(hWnd);
	ImGui_ImplDX11_Init(g_pDevice, g_pDeviceContext);

	bool bShowDemo = true;
	bool bShowAnother = false;
	ImVec4 clearColor = ImVec4(0.45f, 0.55f, 0.60f, 1.0f);

	bool bDone = false;

	while (!bDone)
	{
		MSG wndMsg;
		while (::PeekMessage(&wndMsg, NULL, 0U, 0U, PM_REMOVE))
		{
			::TranslateMessage(&wndMsg);
			::DispatchMessage(&wndMsg);
			if (wndMsg.message == WM_QUIT) bDone = true;
		}

		if (bDone) break;

		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();

		if (bShowDemo)
		{
			ImGui::ShowDemoWindow(&bShowDemo);
		}

		{
			static float f = 0.f;
			static int nCounter = 0;

			ImGui::Begin("Hello, world!");

			ImGui::Text("This is some text");
			ImGui::Checkbox("Demo Window", &bShowDemo);
			ImGui::Checkbox("Another Window", &bShowAnother);

			ImGui::SliderFloat("float", &f, 0.f, 1.f);
			ImGui::ColorEdit3("Clear Color", (float*)&clearColor);

			if (ImGui::Button("Button"))
			{
				nCounter++;
			}

			ImGui::SameLine();
			ImGui::Text("Counter = %d", nCounter);

			ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
			ImGui::End();
		}

		if (bShowAnother)
		{
			ImGui::Begin("Another Window", &bShowAnother);
			ImGui::Text("Hello from another window");
			if (ImGui::Button("Close Me"))
			{
				bShowAnother = false;
			}
			ImGui::End();
		}

		ImGui::Render();
		const float aClearColoraplha[4] = { clearColor.x * clearColor.w, clearColor.y * clearColor.w, clearColor.z * clearColor.w, clearColor.w };
		g_pDeviceContext->OMSetRenderTargets(1, &g_pRenderTargetView, NULL);
		g_pDeviceContext->ClearRenderTargetView(g_pRenderTargetView, aClearColoraplha);
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

		g_pSwapChain->Present(1, 0);
	}

	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
	CleanupDevice();

	::DestroyWindow(hWnd);
	::UnregisterClass(wClass.lpszClassName, wClass.hInstance);

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

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

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

void TestDB()
{
	AlcStatement TestRead;
	if (CreateSqlStatement(&TestRead, (wchar_t*)L"SELECT * FROM Users"))
	{
		int nID;
		wchar_t sEmail[256];
		wchar_t sPass[256];
		wchar_t sFirstName[256];
		wchar_t sSecondName[256];
		while (IsSqlStatementValid(&TestRead))
		{
			int nCol = 1;
			if (!GetIntFromStatement(&TestRead, nCol++, &nID))
			{
				LogError("Failed to get ID");
				continue;
			}
			if (!GetStringFromStatement(&TestRead, nCol++, sEmail, 256))
			{
				LogError("Failed to get Email");
				continue;
			}
			if (!GetStringFromStatement(&TestRead, nCol++, sPass, 256))
			{
				LogError("Failed to get Pass");
				continue;
			}
			if (!GetStringFromStatement(&TestRead, nCol++, sFirstName, 256))
			{
				LogError("Failed to get First Name");
				continue;
			}
			if (!GetStringFromStatement(&TestRead, nCol++, sSecondName, 256))
			{
				LogError("Failed to get Last Name");
				continue;
			}
			LogInfo("Id: %d", nID);
			LogInfo("Email: %ls", sEmail);
			LogInfo("Pass: %ls", sPass);
			LogInfo("First Name: %ls", sFirstName);
			LogInfo("Last Name: %ls", sSecondName);
		}
		CloseSqlStatement(&TestRead);
	}
}

