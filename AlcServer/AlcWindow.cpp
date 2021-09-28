#include <d3d11.h>

#include "AlcWindow.h"

#include "AlcLog.h"
#include "AlcString.h"
#include "AlcApp.h"
#include "AlcMemory.h"

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
			UINT nWidth = (UINT)LOWORD(lParam);
			UINT nHeight = (UINT)HIWORD(lParam);
			CleanupRenderTarget();
			g_pSwapChain->ResizeBuffers(0, nWidth, nHeight, DXGI_FORMAT_UNKNOWN, 0);
			CreateRenderTarget();

			/*if (ImGui::GetCurrentContext())
			{
				ImGuiViewport* MainViewport = ImGui::GetMainViewport();
				MainViewport->WorkSize = ImVec2((float)nWidth, (float)nHeight);
			}*/
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
		InWindow->m_sWndClassName = Alc::CopyString((u16*)wClass.lpszClassName);
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
		const float aClear[] = { 1.f, 1.f, 1.f, 1.f };
		if (aClearColor)
		{
			g_pDeviceContext->ClearRenderTargetView(g_pRenderTargetView, aClearColor);
		}
		else
		{
			g_pDeviceContext->ClearRenderTargetView(g_pRenderTargetView, aClear);
		}
	}

	void DestroyWindow(AlcWindow* InWindow)
	{
		CleanupDevice();

		::DestroyWindow(InWindow->m_hWnd);
		::UnregisterClass((LPCWSTR)InWindow->m_sWndClassName, InWindow->m_hInstance);
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

	void ImGuiNewFrame()
	{
		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();
	}

	void ImGuiRender()
	{
		ImGui::Render();
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	}

	void ImGuiShowdown()
	{
		ImGui_ImplDX11_Shutdown();
		ImGui_ImplWin32_Shutdown();
		ImGui::DestroyContext();
	}

	static void ShowMemoryOverlay(bool* pOpen)
	{
		static int corner = 3;
		ImGuiIO& io = ImGui::GetIO();
		ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize |
			ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav |
			ImGuiWindowFlags_Popup | ImGuiWindowFlags_NoMove;
		if (corner != -1)
		{
			const float PAD = 10.0f;
			const ImGuiViewport* viewport = ImGui::GetMainViewport();
			ImVec2 work_pos = viewport->WorkPos; // Use work area to avoid menu-bar/task-bar, if any!
			ImVec2 work_size = viewport->WorkSize;
			ImVec2 window_pos, window_pos_pivot;
			window_pos.x = (corner & 1) ? (work_pos.x + work_size.x - PAD) : (work_pos.x + PAD);
			window_pos.y = (corner & 2) ? (work_pos.y + work_size.y - PAD) : (work_pos.y + PAD);
			window_pos_pivot.x = (corner & 1) ? 1.0f : 0.0f;
			window_pos_pivot.y = (corner & 2) ? 1.0f : 0.0f;
			ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always, window_pos_pivot);
		}
		ImGui::SetNextWindowBgAlpha(0.35f); // Transparent background

		if (*pOpen)
		{
			//ImGui::OpenPopup("Memory");
		}

		ImGui::BeginGroup();
		AlcMemoryArena& FrameArena = Alc::GetArena(AlcArena_Frame);
		ImGui::Text("Frame Arena:");
		ImGui::Indent();
		ImGui::Text("Total: %d bytes", FrameArena.m_nSize);
		ImGui::Text("Used: %d bytes", FrameArena.m_nCursor);
		ImGui::Text("Free: %d bytes", FrameArena.m_nSize - FrameArena.m_nCursor);
		ImGui::Unindent();
		ImGui::Separator();

		AlcMemoryArena& PermanentArena = Alc::GetArena(AlcArena_Permanent);
		ImGui::Text("Permanent Arena:");
		ImGui::Indent();
		ImGui::Text("Total: %d bytes", PermanentArena.m_nSize);
		ImGui::Text("Used: %d bytes", PermanentArena.m_nCursor);
		ImGui::Text("Free: %d bytes", PermanentArena.m_nSize - PermanentArena.m_nCursor);
		ImGui::Unindent();
		ImGui::Separator();

		AlcMemoryArena& TempArena = Alc::GetArena(AlcArena_Temp);
		ImGui::Text("Temp Arena:");
		ImGui::Indent();
		ImGui::Text("Total: %d bytes", TempArena.m_nSize);
		ImGui::Text("Used: %d bytes", TempArena.m_nCursor);
		ImGui::Text("Free: %d bytes", TempArena.m_nSize - TempArena.m_nCursor);
		ImGui::Unindent();

		ImGui::EndGroup();
		
	}

	enum AccountsTableColumn
	{
		AccountsTableColumn_Email,
		AccountsTableColumn_Pass,
		AccountsTableColumn_First,
		AccountsTableColumn_Last,
	};

	void ShowImGui()
	{
		AlcApp* pApp = Alc::GetApp();

		static float f = 0.f;
		static int nCounter = 0;

		ImGuiWindowFlags WindowFlags = 0;
		WindowFlags |= ImGuiWindowFlags_NoDecoration;
		WindowFlags |= ImGuiWindowFlags_MenuBar;
		WindowFlags |= ImGuiWindowFlags_NoMove;
		WindowFlags |= ImGuiWindowFlags_NoFocusOnAppearing;
		const ImGuiViewport* pMainViewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(pMainViewport->WorkPos, ImGuiCond_Always);
		ImGui::SetNextWindowSize(pMainViewport->WorkSize, ImGuiCond_Always);

		ImGui::Begin("Hello, world!", nullptr, WindowFlags);

		if (ImGui::Button("Load Accounts"))
		{
			Alc::LoadAccounts();
		}

		ImGui::Separator();

		static ImGuiTableFlags tableFlags =
			 ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders
			| ImGuiTableFlags_ScrollX | ImGuiTableFlags_ScrollY
			| ImGuiTableFlags_SizingFixedFit;

		if (pApp->m_nNumAccounts)
		{
			if (ImGui::BeginTable("AccountsTable", 4, tableFlags))
			{
				ImGui::TableSetupColumn("Email", ImGuiTableColumnFlags_DefaultSort, 0.f, AccountsTableColumn_Email);
				ImGui::TableSetupColumn("Password", ImGuiTableColumnFlags_DefaultSort, 0.f, AccountsTableColumn_Pass);
				ImGui::TableSetupColumn("First Name", ImGuiTableColumnFlags_DefaultSort, 0.f, AccountsTableColumn_First);
				ImGui::TableSetupColumn("Last Name", ImGuiTableColumnFlags_DefaultSort, 0.f, AccountsTableColumn_Last);

				for (s32 i = 0; i < pApp->m_nNumAccounts; i++)
				{
					ImGui::TableNextRow();
					AlcAccount& TempAccount = pApp->m_aAccounts[i];
					{
						s32 nCount = Alc::GetStringSize(TempAccount.m_sEmail);
						char* sSmall = ALC_PUSH_FRAME_ARRAY(char, nCount + 1);
						Alc::ToSmallString(TempAccount.m_sEmail, (u8*)sSmall);
						
						if (ImGui::TableSetColumnIndex(0))
						{
							ImGui::TextUnformatted(sSmall);
						}
					}

					{
						s32 nCount = Alc::GetStringSize(TempAccount.m_sPass);
						char* sSmall = ALC_PUSH_FRAME_ARRAY(char, nCount + 1);
						Alc::ToSmallString(TempAccount.m_sPass, (u8*)sSmall);
						if (ImGui::TableSetColumnIndex(1))
						{
							ImGui::TextUnformatted(sSmall);
						}
					}

					{
						s32 nCount = Alc::GetStringSize(TempAccount.m_sFirstName);
						char* sSmall = ALC_PUSH_FRAME_ARRAY(char, nCount + 1);
						Alc::ToSmallString(TempAccount.m_sFirstName, (u8*)sSmall);
						if (ImGui::TableSetColumnIndex(2))
						{
							ImGui::TextUnformatted(sSmall);
						}
					}

					{
						s32 nCount = Alc::GetStringSize(TempAccount.m_sLastName);
						char* sSmall = ALC_PUSH_FRAME_ARRAY(char, nCount + 1);
						Alc::ToSmallString(TempAccount.m_sLastName, (u8*)sSmall);
						if (ImGui::TableSetColumnIndex(3))
						{
							ImGui::TextUnformatted(sSmall);
						}
					}
				}

				ImGui::EndTable();
			}
		}
		if (pApp->m_bShowMemoryOverlay)
		{
			ShowMemoryOverlay(&pApp->m_bShowMemoryOverlay);
		}
		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("Tools"))
			{
				ImGui::MenuItem("Memory overlay", NULL, &pApp->m_bShowMemoryOverlay);
				ImGui::EndMenu();
			}
		}
		ImGui::EndMenuBar();

		ImGui::End();
	}
}