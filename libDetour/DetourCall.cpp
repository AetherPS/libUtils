#include "stdafx.h"
#include "BaseDetour.h"
#include "DetourCall.h"
#include "hde64.h"
#include "Resolver.h"

void DetourCall::Create(void* address, void* destination)
{
	if (address == 0 || destination == nullptr)
	{
		Logger::Error("[DetourCall] create: \"address\" or \"destination\" NULL (%llX -> %llX)", address, destination);
		return;
	}

	if (*(uint8_t*)(address) != 0xE8)
	{
		Logger::Error("[DetourCall] Create: Instruction not a call instruction. The Call Detour is to be used to replace a relative call.");
		return;
	}

	Address = address;

	// save the original bytes
	sceKernelMprotect(reinterpret_cast<void*>(Address), JUMP_32SIZE, SCE_KERNEL_PROT_CPU_ALL);
	OriginalBytes = std::make_unique<uint8_t[]>(JUMP_32SIZE);
	memcpy(OriginalBytes.get(), address, JUMP_32SIZE);

#ifdef _DEBUG
	Logger::Info("[DetourCall] create: original bytes saved");
	hexdump(OriginalBytes.get(), JUMP_32SIZE, true);
#endif

	// Save the original function call.
	StubPtr = (void*)((uint64_t)Address + CALL_32SIZE + *(uint32_t*)((uint64_t)Address + 1));

	Logger::Info("[DetourCall] create: Original absolute location -> %llX", StubPtr);

	// create the trampoline and call it
	auto res = sceKernelMmap((void*)address, JUMP_64SIZE, SCE_KERNEL_PROT_CPU_ALL, 0x1000 | 0x2, -1, 0, (void**)&TrampolinePtr);
	if (res < 0 || TrampolinePtr == nullptr)
	{
		Logger::Error("[DetourCall] create: an error has occurred allocating trampoline: %X, %llX", res, TrampolinePtr);
		return;
	}

	// Make sure we can make the jump.
	if (!IsRelativelyClose((void*)((uint64_t)address + JUMP_32SIZE), TrampolinePtr))
	{
		Logger::Error("[DetourCall] create: Trampoline is not relatively close we can not write a jump 32.");

		sceKernelMunmap(TrampolinePtr, JUMP_64SIZE);

		return;
	}

	Logger::Info("[DetourCall] create: writing trampoline jumps");

	WriteJump64(TrampolinePtr, destination);
	WriteCall32(address, TrampolinePtr);

#ifdef _DEBUG
	Logger::Info("[DetourCall] create: jump set");

	Logger::Info("[DetourCall] Trampoline:");
	hexdump(TrampolinePtr, JUMP_64SIZE, true);

	Logger::Info("[DetourCall] Detour:");
	hexdump(address, JUMP_32SIZE, true);
#endif

	Logger::Success("[DetourCall] Detour written from 0x%llX -> 0x%llX", address, destination);
	DetourSet = true;
}

void DetourCall::Restore()
{
	if (DetourSet)
	{
		Logger::Info("[DetourCall] Original Bytes:");
		hexdump(OriginalBytes.get(), JUMP_32SIZE, false);

		Logger::Info("[DetourCall] Restoring original function bytes.");
		sceKernelMprotect(reinterpret_cast<void*>(Address), JUMP_32SIZE, SCE_KERNEL_PROT_CPU_ALL);
		memcpy(reinterpret_cast<void*>(Address), OriginalBytes.get(), JUMP_32SIZE);

		Logger::Info("[DetourCall] Address Bytes:");
		hexdump(Address, JUMP_32SIZE, false);

		sceKernelMunmap(TrampolinePtr, JUMP_64SIZE);

		Logger::Success("[DetourCall] Detour Removed.");
		DetourSet = false;
	}
	else
	{
		Logger::Warn("[DetourCall] restore: cannot restore detour that was not set!");
	}
}