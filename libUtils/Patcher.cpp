#include "stdafx.h"
#include "Patcher.h"

void Patcher::SetPatch()
{
	if (PatchData)
	{
		//Set protection to all
		sceKernelMprotect((void*)Address, Length, SCE_KERNEL_PROT_CPU_ALL);

		//Write original Data back.
		memcpy((void*)Address, PatchData.get(), Length);

		Logger::Success("[Patcher] SetPatch: Patch (0x%llX) set!\n", Address);
	}
	else
	{
		Logger::Warn("[Patcher] Patch not installed.\n");
	}
}

void Patcher::RestorePatch()
{
	if (OriginalData)
	{
		//Set protection to all
		sceKernelMprotect((void*)Address, Length, SCE_KERNEL_PROT_CPU_ALL);

		//Write original Data back.
		memcpy((void*)Address, OriginalData.get(), Length);

		Logger::Success("[Patcher] RestorePatch: Patch (0x%llX) restored!\n", Address);
	}
	else
	{
		Logger::Warn("[Patcher] Patch not installed.\n");
	}
}

Patcher::Patcher(uint64_t Address, const void* Data, size_t Length, bool setNow)
{
	//Backup Params.
	this->Address = Address;
	this->Length = Length;

	//Set protection to all
	sceKernelMprotect((void*)Address, Length, SCE_KERNEL_PROT_CPU_ALL);

	//Backup data.
	OriginalData = std::make_unique<uint8_t[]>(Length);
	memcpy(OriginalData.get(), (void*)Address, Length);
	PatchData = std::make_unique<uint8_t[]>(Length);
	memcpy(PatchData.get(), Data, Length);

	//Write Patch.
	if (setNow)
		memcpy((void*)Address, Data, Length);

	Logger::Success("[Patcher] InstallPatch: Patch (0x%llX) written!\n", Address);
}

Patcher::~Patcher()
{
	RestorePatch();
}