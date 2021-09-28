// AlcServer.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
#include "AlcSql.h"
#include "AlcLog.h"
#include "AlcWindow.h"
#include "AlcString.h"
#include "AlcApp.h"
#include "AlcDefines.h"
#include "AlcMemory.h"

void TestAlcWindow();

int main()
{
	Alc::InitMemory();
	TestAlcWindow();

    return 0;
}

void TestAlcWindow()
{
	AlcWindow* MainWindow = ALC_PUSH_PERMANENT(AlcWindow);
	if (!Alc::CreateAlcWindow(MainWindow, _T("Test Window"), 100, 100, 1280, 800))
	{
		LogError("Failed to create Window");
		return;
	}
	Alc::ShowWindow(MainWindow);

	Alc::ImGuiInit(MainWindow);

	bool bDone = false;

	while (!bDone)
	{
		if (!Alc::UpdateWindow())
		{
			bDone = true;
		}

		if (bDone) break;

		Alc::ImGuiNewFrame();

		Alc::ShowImGui();
		ImGui::ShowDemoWindow(nullptr);
		
		Alc::ClearWindow(MainWindow, nullptr);

		Alc::ImGuiRender();
		Alc::SwapChain();
		Alc::ClearArena(AlcArena_Frame);
	}

	Alc::ImGuiShowdown();
	Alc::DestroyWindow(MainWindow);
}
