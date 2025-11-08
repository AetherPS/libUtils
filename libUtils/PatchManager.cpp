#include "stdafx.h"
#include "PatchManager.h"
#include "Patcher.h"

PatchManager::PatchManager()
{
}

PatchManager::~PatchManager()
{
	// Clean up all the patches.
	for (const auto& patch : PatchList)
	{
		delete patch.second;
	}
}

void PatchManager::AddPatch(std::string name, uint64_t address, const void* data, size_t length, bool setNow)
{
	PatchList.insert(std::make_pair(name, new Patcher(address, data, length, setNow)));
}

void PatchManager::EnablePatch(std::string name)
{
	PatchList[name]->SetPatch();
}

void PatchManager::DisablePatch(std::string name)
{
	PatchList[name]->RestorePatch();
}