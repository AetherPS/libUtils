#include "stdafx.h"
#include "BaseDetour.h"
#include "Detour32.h"
#include "hde64.h"
#include "Resolver.h"

void Detour32::Create(void* address, void* destination)
{
	if (address == 0 || destination == nullptr)
	{
		Logger::Error("[Detour32] create: \"address\" or \"destination\" NULL (%llX -> %llX)", address, destination);
		return;
	}

	// calculate the necessary instructions size to detour
	auto instructionLength = 0;
	while (instructionLength < JUMP_32SIZE)
	{
		auto ptr = reinterpret_cast<uint64_t>(address) + instructionLength;

		hde64s hs;
		instructionLength += hde64_disasm(reinterpret_cast<void*>(ptr), &hs);

		if (hs.flags & F_ERROR)
		{
			Logger::Error("[Detour32] HDE has run into an error dissasembling %llX!", ptr);
			return;
		}
	}

	Logger::Info("[Detour32] create: instructionLength = %i", instructionLength);

	Address = address;

	// save the original instructions
	OriginalSize = instructionLength;
	OriginalBytes = std::make_unique<uint8_t[]>(OriginalSize);
	memcpy(OriginalBytes.get(), address, OriginalSize);

#ifdef _DEBUG
	Logger::Info("[Detour32] create: original bytes saved");
	hexdump(OriginalBytes.get(), OriginalSize, true);
#endif

	// create the trampoline and jump to it
	auto res = sceKernelMmap((void*)address, JUMP_64SIZE, SCE_KERNEL_PROT_CPU_ALL, 0x1000 | 0x2, -1, 0, (void**)&TrampolinePtr);
	if (res < 0 || TrampolinePtr == nullptr)
	{
		Logger::Error("[Detour32] create: an error has occurred allocating trampoline: %X, %llX", res, TrampolinePtr);
		return;
	}

	// Make sure we can make the jump.
	if (!IsRelativelyClose((void*)((uint64_t)address + JUMP_32SIZE), TrampolinePtr))
	{
		Logger::Error("[Detour32] create: Trampoline is not relatively close we can not write a jump 32.");

		sceKernelMunmap(TrampolinePtr, JUMP_64SIZE);

		return;
	}

	Logger::Info("[Detour32] create: writing trampoline jumps");

	// allocate the stub
	StubSize = instructionLength + JUMP_64SIZE;
	res = sceKernelMmap(0, StubSize, SCE_KERNEL_PROT_CPU_ALL, 0x1000 | 0x2, -1, 0, (void**)&StubPtr);
	if (res < 0 || StubPtr == nullptr)
	{
		Logger::Error("[Detour32] create: sceKernelMmap returned error = %X", res);
		return;
	}

	// write original instructions to the stub
	memcpy(StubPtr, Address, instructionLength);

	// write the jump back to main function to the stub
	auto stubJump = reinterpret_cast<uint64_t>(StubPtr) + instructionLength;
	auto stubReturn = reinterpret_cast<uint64_t>(Address) + instructionLength;

	Logger::Info("[Detour32] create: writing detour jumps");

	WriteJump64(reinterpret_cast<void*>(stubJump), reinterpret_cast<void*>(stubReturn));
	WriteJump64(TrampolinePtr, destination);
	WriteJump32(address, TrampolinePtr);

#ifdef _DEBUG
	Logger::Info("[Detour32] create: jump set");

	Logger::Info("[Detour32] Trampoline:");
	hexdump(TrampolinePtr, JUMP_64SIZE, true);

	Logger::Info("[Detour32] Detour:");
	hexdump(address, JUMP_32SIZE, true);
#endif

	Logger::Success("[Detour32] Detour written from 0x%llX -> 0x%llX", address, destination);
	DetourSet = true;
}

void Detour32::Restore()
{
	if (DetourSet)
	{
		Logger::Info("[Detour32] Restoring original function bytes.");
		sceKernelMprotect(reinterpret_cast<void*>(Address), OriginalSize, SCE_KERNEL_PROT_CPU_ALL);
		memcpy(reinterpret_cast<void*>(Address), OriginalBytes.get(), OriginalSize);

		sceKernelMunmap(TrampolinePtr, JUMP_64SIZE);
		sceKernelMunmap(StubPtr, StubSize);

		Logger::Success("[Detour32] Detour Removed.");
		DetourSet = false;
	}
	else
	{
		Logger::Warn("[Detour32] restore: cannot restore detour that was not set!");
	}
}