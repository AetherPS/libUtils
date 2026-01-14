#include "stdafx.h"
#include "BaseDetour.h"
#include "Detour64.h"
#include "hde64.h"

void Detour64::Create(void* address, void* destination)
{
	if (address == 0 || destination == nullptr)
	{
		Logger::Error("[Detour64] create: \"address\" or \"destination\" NULL (%llX -> %llX)", address, destination);
		return;
	}

	// calculate the necessary instructions size to detour
	auto instructionLength = 0;
	while (instructionLength < JUMP_64SIZE)
	{
		auto ptr = reinterpret_cast<uint64_t>(address) + instructionLength;

		hde64s hs;
		instructionLength += hde64_disasm(reinterpret_cast<void*>(ptr), &hs);

		if (hs.flags & F_ERROR)
		{
			Logger::Error("[Detour64] HDE has run into an error dissasembling %llX!", ptr);
			return;
		}
	}

	Logger::Info("[Detour64] create: instructionLength = %i", instructionLength);

	Address = address;

	// save the original instructions
	OriginalSize = instructionLength;
	OriginalBytes = std::make_unique<uint8_t[]>(OriginalSize);
	memcpy(OriginalBytes.get(), address, OriginalSize);

#ifdef _DEBUG
	Logger::Info("[Detour64] create: original bytes saved");
	hexdump(OriginalBytes.get(), OriginalSize, true);
#endif

	// allocate the stub
	StubSize = instructionLength + JUMP_64SIZE;
	auto res = sceKernelMmap(0, StubSize, SCE_KERNEL_PROT_CPU_ALL, 0x1000 | 0x2, -1, 0, &StubPtr);
	if (res < 0 || StubPtr == nullptr)
	{
		Logger::Error("[Detour64] create: sceKernelMmap returned error = %X", res);
		return;
	}

	// write original instructions to the stub
	memcpy(StubPtr, Address, instructionLength);

	// write the jump back to main function to the stub
	auto stub_jump = reinterpret_cast<uint64_t>(StubPtr) + instructionLength;
	auto stub_return = reinterpret_cast<uint64_t>(Address) + instructionLength;

	Logger::Info("[Detour64] create: writing detour jumps");

	WriteJump64(reinterpret_cast<void*>(stub_jump), reinterpret_cast<void*>(stub_return));
	WriteJump64(Address, destination);

#ifdef _DEBUG
	Logger::Info("[Detour64] create: detour set");

	Logger::Info("[Detour64] Stub:");
	hexdump(StubPtr, StubSize, true);

	Logger::Info("[Detour64] Detour:");
	hexdump(address, JUMP_64SIZE, true);
#endif
	Logger::Success("[Detour64] Detour written from 0x%llX -> 0x%llX", address, destination);
	DetourSet = true;
}

void Detour64::Restore()
{
	if (DetourSet)
	{
		Logger::Info("[Detour64] Restoring original function bytes.");
		sceKernelMprotect(reinterpret_cast<void*>(Address), OriginalSize, SCE_KERNEL_PROT_CPU_ALL);
		memcpy(reinterpret_cast<void*>(Address), OriginalBytes.get(), OriginalSize);

		sceKernelMunmap(StubPtr, StubSize);

		StubPtr = nullptr;
		StubSize = 0;
		OriginalSize = 0;
		Address = 0;

		Logger::Success("[Detour64] Detour Removed.");
		DetourSet = false;
	}
	else
	{
		Logger::Warn("[Detour64] restore: cannot restore detour that was not set!");
	}
}