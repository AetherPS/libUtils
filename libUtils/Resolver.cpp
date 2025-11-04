#include "stdafx.h"
#include "Resolver.h"

uint64_t BaseAddress = 0;
uint64_t TextSegmentSize = 0;

uint64_t ResolveAddress(uint64_t offset)
{
	if (BaseAddress == 0)
	{
		SceKernelModuleInfo info;
		info.size = sizeof(SceKernelModuleInfo);
		sceKernelGetModuleInfo(0, &info);
		BaseAddress = (uint64_t)info.segmentInfo[0].address;
		TextSegmentSize = info.segmentInfo[0].size;
	}

	return BaseAddress + offset;
}

void* ResolveAddressv(uint64_t offset)
{
	return (void*)ResolveAddress(offset);
}

bool IsInTextSeg(uint64_t addr)
{
	if (BaseAddress == 0 || TextSegmentSize == 0)
	{
		SceKernelModuleInfo info;
		info.size = sizeof(SceKernelModuleInfo);
		sceKernelGetModuleInfo(0, &info);
		BaseAddress = (uint64_t)info.segmentInfo[0].address;
		TextSegmentSize = info.segmentInfo[0].size;
	}

	return (addr > BaseAddress) && (addr < (BaseAddress + TextSegmentSize));
}

bool IsMemoryMapped(uint64_t address)
{
	SceKernelVirtualQueryInfo info;
	return sceKernelVirtualQuery((void*)address, 0, &info, sizeof(info)) == 0;
}