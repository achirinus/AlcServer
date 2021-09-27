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
#include "AlcWindow.h"
#include "AlcString.h"

void TestDB();
void TestAlcWindow();

int main()
{
	const wchar_t* sTest1 = L"Test1";
	int nSize = Alc::GetStringSize(sTest1);
	wchar_t* sCopyTest = Alc::CopyString(sTest1);

	TestAlcWindow();

    return 0;
}

void TestAlcWindow()
{
	AlcWindow* MainWindow = new AlcWindow;
	if (!Alc::CreateAlcWindow(MainWindow, _T("Test Window"), 100, 100, 1280, 800))
	{
		LogError("Failed to create Window");
		return;
	}
	Alc::ShowWindow(MainWindow);

	Alc::ImGuiInit(MainWindow);

	bool bShowDemo = true;
	bool bShowAnother = false;
	ImVec4 clearColor = ImVec4(0.45f, 0.55f, 0.60f, 1.0f);

	bool bDone = false;

	while (!bDone)
	{
		if (!Alc::UpdateWindow())
		{
			bDone = true;
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
		Alc::ClearWindow(MainWindow, aClearColoraplha);
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

		Alc::SwapChain();
	}

	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	Alc::DestroyWindow(MainWindow);
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

