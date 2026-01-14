#include "stdafx.h"
#include "Detour.h"
#include "DetourManager.h"

DetourManager::~DetourManager()
{
	Logger::Info("[DetourManager] Cleaning up!");

	// Clean up all the detours.
	for (const auto& detour : DetourList)
	{
		Logger::Info("[DetourManager] Removing Detour %s", detour.first.c_str());
		delete detour.second;
	}
}
