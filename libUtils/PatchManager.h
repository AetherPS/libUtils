#pragma once
#include "Patcher.h"

class PatchManager
{
public:
	PatchManager();
	~PatchManager();

	void AddPatch(std::string name, uint64_t address, const void* data, size_t length, bool setNow = true);
	void EnablePatch(std::string name);
	void DisablePatch(std::string name);

private:
	std::map<std::string, Patcher*> PatchList;
};